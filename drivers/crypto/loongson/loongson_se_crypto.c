/*
 *   Driver for loongson SE module using the kernel asynchronous crypto api.
 *
 *   Copyright 2023 Loongson Technology, Inc.
 */
#define pr_fmt(fmt)     KBUILD_MODNAME ": " fmt

#include <linux/bitfield.h>
#include <linux/crypto.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/ftrace.h>
#include <linux/iopoll.h>

#include <crypto/engine.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/skcipher.h>
#include <crypto/internal/rng.h>
#include <crypto/sm3.h>
#include <crypto/sm4.h>
#include <crypto/rng.h>

#define SE_MAILBOX_S			0x0
#define SE_MAILBOX_L			0x20
#define SE_S2LINT_STAT			0x88
#define SE_S2LINT_EN			0x8c
#define SE_S2LINT_SET			0x90
#define SE_S2LINT_CL			0x94
#define SE_L2SINT_STAT			0x98
#define SE_L2SINT_EN			0x9c
#define SE_L2SINT_SET			0xa0
#define SE_L2SINT_CL			0xa4

/* INT bit definition */
#define SE_INT_SETUP			BIT(0)
#define SE_INT_RNG				BIT(1)
#define SE_INT_SM2				BIT(2)
#define SE_INT_SM3				BIT(3)
#define SE_INT_SM4				BIT(4)
#define SE_INT_ALL				(SE_INT_SETUP | \
								 SE_INT_SM2 | \
								 SE_INT_SM3 | \
								 SE_INT_SM4 | \
								 SE_INT_RNG)

#define SE_CMD_SETUP_SIGN		BIT(31)
#define SE_CMD_SETUP(n)			(SE_CMD_SETUP_SIGN | n)
#define SE_CMD_START			SE_CMD_SETUP(0)
#define SE_CMD_STOP				SE_CMD_SETUP(1)
#define SE_CMD_GETVER			SE_CMD_SETUP(2)
#define SE_CMD_SETBUF			SE_CMD_SETUP(3)
#define SE_CMD_ALG_MSG			SE_CMD_SETUP(4)

#define SE_CMD_RNG			0x50
#define SE_CMD_SM4_ENCRY	0x61
#define SE_CMD_SM4_DECRY	0x62
#define SE_CMD_SM3			0x63
#define SE_CMD_SM2_SIGN		0x74
#define SE_CMD_SM2_VSIGN	0x75

#define SE_ALG_TYPE_SM2		0
#define SE_ALG_TYPE_SM3		1
#define SE_ALG_TYPE_SM4		2
#define SE_ALG_TYPE_RNG		3

#define SE_DATA_SIZE		0x400000

/* SM3 definitions */
#define SE_SM3_DATA_SIZE	0x100000

#define SE_SM3_BUFLEN			0x100

#define SE_HASH_FLAGS_INIT		BIT(0)
#define SE_HASH_FLAGS_FINUP		BIT(1)

#define SE_HASH_OP_UPDATE		1
#define SE_HASH_OP_FINAL		2

/* SM4 definitions */
#define SE_SM4_DATA_SIZE		0x100000
#define SE_SM4_DATA_ALIGN		0x40ULL
#define SE_SM4_DATA_ALIGN_MASK	(SE_SM4_DATA_ALIGN - 1)
#define SE_SKCIPHER_OP_ENCRY	1
#define SE_SKCIPHER_OP_DECRY	2

/* RNG definitions */
#define SE_RNG_DATA_SIZE		PAGE_SIZE
#define SE_RNG_DATA_ALIGN		0x4ULL

#define MODULE_NAME   "Loongson SE"

/* The crypto framework makes it hard to avoid this global. */
static struct device *se_dev;

struct loongson_alg_common {
	u32 type;
	u32 data_size;
	bool need_engine;
	union {
		struct ahash_alg ahash;
		struct skcipher_alg skcipher;
		struct akcipher_alg akcipher;
		struct rng_alg rng;
	} u;
};

struct se_alg_engine {
	struct list_head engine_list;
	struct list_head finish_list;
	struct loongson_alg_common *alg;
	struct crypto_engine *engine;

	/* TODO There is a probability of problems in multithreaded environments */
	/* Working request */
	void *rctx;
	int cmd_ret;
	u32 cmd;

	/* Buffer information */
	u32 buffer_size;
	u32 buffer_cnt;
	void *msg_buffer;
	dma_addr_t msg_addr;
	void *in_buffer;
	dma_addr_t in_addr;
	void *out_buffer;
	dma_addr_t out_addr;
	void *key_buffer;
	dma_addr_t key_addr;

	spinlock_t msg_lock;
};

struct loongson_hash_ctx {
	struct crypto_engine_ctx enginectx;
	struct se_alg_engine *sae;
};

struct loongson_hash_request_ctx {
	unsigned long op;
	unsigned long flags;
	struct ahash_request *req;

	int buf_size;
	int buf_cnt;

	struct scatterlist *sg;
	int nents;
	int sg_bytes;

	u8 cache[SE_SM3_BUFLEN] __aligned(sizeof(u32));
	int cache_len;
};

struct loongson_skcipher_ctx {
	struct crypto_engine_ctx enginectx;
	struct se_alg_engine *sae;
	int keylen;
	u32 key[SM4_KEY_SIZE / sizeof(u32)];
};

struct loongson_skcipher_request_ctx {
	unsigned long op;
	struct skcipher_request *req;

	struct scatterlist *src;
	struct scatterlist *dst;
	int in_nents;
	int out_nents;
	u32 rest_bytes;
	u32 copyed_bytes;
};

struct loongson_rng_ctx {
	struct se_alg_engine *sae;
};

struct loongson_se {
	void __iomem *base;
	spinlock_t cmd_lock;
	struct list_head alg_engine;
	struct list_head finish_engine;
	u32 version;

	/* Interaction memory */
	void *data_buffer;
	dma_addr_t data_addr;
	u32 data_free;
	u32 data_used;

	/* Synchronous CMD */
	struct completion cmd_completion;
	struct completion rng_completion;

	/* Memory copy task */
	struct tasklet_struct task;
};

struct se_cmd {
	u32 cmd;
	u32 info[7];
};

struct se_res {
	u32 cmd;
	u32 cmd_ret;
	u32 info[6];
};

struct se_alg_req {
	u32 len;
	u32 in_off;
	u32 out_off;
	u32 key_off;
	u32 info[3];
};

struct se_alg_res {
	u32 cmd_ret;
	u32 info[6];
};

struct se_alg_msg {
	u32 cmd;
	union {
		struct se_alg_req req;
		struct se_alg_res res;
	} u;
};

struct se_mailbox_data {
	u32 int_bit;
	union {
		u32 mailbox[8];
		struct se_cmd gcmd;
		struct se_res res;
	} u;
};

static int loongson_sm4_cipher_once(struct se_alg_engine *sae,
		struct skcipher_request *req, u32 op);

static inline u32 se_readl(u64 addr)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;

	return readl(se->base + addr);
}

static inline void se_writel(u32 val, u64 addr)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;

	writel(val, se->base + addr);
}

static void se_enable_int(u32 int_bit)
{
	u32 tmp;

	if (!int_bit)
		return;

	tmp = se_readl(SE_S2LINT_EN);
	tmp |= int_bit;
	se_writel(tmp, SE_S2LINT_EN);
}

static void se_disable_int(u32 int_bit)
{
	u32 tmp;

	if (!int_bit)
		return;

	tmp = se_readl(SE_S2LINT_EN);
	tmp &= ~(int_bit);
	se_writel(tmp, SE_S2LINT_EN);
}

static int se_send_requeset(struct loongson_se *se,
		struct se_mailbox_data *req)
{
	unsigned long flag;
	u32 status;
	int err = 0;
	int i;

	if (!se || !req)
		return -EINVAL;

	if (se_readl(SE_L2SINT_STAT) ||
			!(se_readl(SE_L2SINT_EN) & req->int_bit))
		return -EBUSY;

	spin_lock_irqsave(&se->cmd_lock, flag);

	for (i = 0; i < ARRAY_SIZE(req->u.mailbox); i++)
		se_writel(req->u.mailbox[i], SE_MAILBOX_S + i * 4);

	se_writel(req->int_bit, SE_L2SINT_SET);

	err = readl_relaxed_poll_timeout_atomic(se->base + SE_L2SINT_STAT, status,
				!(status & req->int_bit), 10, 10000);

	spin_unlock_irqrestore(&se->cmd_lock, flag);

	return err;
}

static int se_get_response(struct loongson_se *se,
		struct se_mailbox_data *res)
{
	unsigned long flag;
	int i;

	if (!se || !res)
		return -EINVAL;

	if ((se_readl(SE_S2LINT_STAT) & res->int_bit) == 0)
		return -EBUSY;

	spin_lock_irqsave(&se->cmd_lock, flag);

	for (i = 0; i < ARRAY_SIZE(res->u.mailbox); i++)
		res->u.mailbox[i] = se_readl(SE_MAILBOX_L + i * 4);

	se_writel(res->int_bit, SE_S2LINT_CL);

	spin_unlock_irqrestore(&se->cmd_lock, flag);

	return 0;
}

static int se_send_alg_requeset(struct loongson_se *se, int int_bit)
{
	u32 status;
	int err = 0;

	if ((se_readl(SE_L2SINT_STAT) & int_bit) ||
			!(se_readl(SE_L2SINT_EN) & int_bit))
		return -EBUSY;

	se_writel(int_bit, SE_L2SINT_SET);

	err = readl_relaxed_poll_timeout_atomic(se->base + SE_L2SINT_STAT, status,
				!(status & int_bit), 10, 10000);

	return err;
}

static int loongson_se_get_res(u32 int_bit, u32 cmd,
		struct se_mailbox_data *res)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;
	int err = 0;

	res->int_bit = int_bit;

	/* Get response already clear status */
	if (se_get_response(se, res)) {
		pr_err("Int 0x%x get response fail.\n", int_bit);
		return -EFAULT;
	}

	/* Check response */
	if (res->u.res.cmd == cmd)
		err = 0;
	else {
		pr_err("Response cmd is 0x%x, not expect cmd 0x%x.\n",
				res->u.res.cmd, cmd);
		err = -EFAULT;
	}

	return err;
}

static int se_send_genl_cmd(struct loongson_se *se, struct se_mailbox_data *req,
		struct se_mailbox_data *res, int retry)
{
	int err = 0, cnt = 0;

try_again:
	if (cnt++ >= retry)
		goto out;

	pr_debug("%d time send cmd 0x%x\n", cnt, req->u.gcmd.cmd);

	err = se_send_requeset(se, req);
	if (err)
		goto try_again;

	if (!wait_for_completion_timeout(&se->cmd_completion,
			msecs_to_jiffies(0x1000)))
		goto try_again;

	err = loongson_se_get_res(req->int_bit, req->u.gcmd.cmd, res);
	if (err || res->u.res.cmd_ret)
		goto try_again;

out:
	se_enable_int(req->int_bit);

	return err;
}

static int se_send_sm3_cmd(struct loongson_se *se,
		struct se_alg_engine *sae, int retry)
{
	struct se_alg_msg *alg_msg = (struct se_alg_msg *)sae->msg_buffer;
	unsigned long flag;
	int err = 0;

	if (!alg_msg)
		return -EINVAL;

	spin_lock_irqsave(&sae->msg_lock, flag);

	alg_msg->cmd = SE_CMD_SM3;
	alg_msg->u.req.len = sae->buffer_cnt;
	alg_msg->u.req.in_off = sae->in_addr - se->data_addr;
	alg_msg->u.req.out_off = sae->out_addr - se->data_addr;

	sae->cmd = SE_CMD_SM3;

try_again:
	pr_debug("SM3 CMD data offset is 0x%x, data length is %d\n",
			alg_msg->u.req.in_off, alg_msg->u.req.len);

	if (!retry--)
		goto out;

	err = se_send_alg_requeset(se, SE_INT_SM3);
	if (err) {
		udelay(5);
		goto try_again;
	}

out:
	spin_unlock_irqrestore(&sae->msg_lock, flag);
	return err;
}

static int se_send_sm4_cmd(struct loongson_se *se,
		struct se_alg_engine *sae, u32 op, int retry)
{
	struct se_alg_msg *alg_msg = (struct se_alg_msg *)sae->msg_buffer;
	unsigned long flag;
	int err = 0;

	if (!sae->buffer_cnt)
		return 0;

	if (!alg_msg)
		return -EINVAL;

	spin_lock_irqsave(&sae->msg_lock, flag);

	if (op == SE_SKCIPHER_OP_ENCRY)
		alg_msg->cmd = SE_CMD_SM4_ENCRY;
	else
		alg_msg->cmd = SE_CMD_SM4_DECRY;

	sae->cmd = alg_msg->cmd;

	/*
	 * Buffer_cnt should not greater than buffer_size,
	 * so it is safe to round up.
	 */
	alg_msg->u.req.len = round_up(sae->buffer_cnt, SE_SM4_DATA_ALIGN);
	alg_msg->u.req.in_off = sae->in_addr - se->data_addr;
	alg_msg->u.req.out_off = sae->out_addr - se->data_addr;
	alg_msg->u.req.key_off = sae->key_addr - se->data_addr;

try_again:
	pr_debug("SM4 CMD %u key offset is 0x%x, "
			"data offset is 0x%x, data length is %d\n",
			alg_msg->cmd, alg_msg->u.req.key_off,
			alg_msg->u.req.in_off, alg_msg->u.req.len);

	if (!retry--)
		goto out;

	err = se_send_alg_requeset(se, SE_INT_SM4);
	if (err) {
		udelay(5);
		goto try_again;
	}

out:
	spin_unlock_irqrestore(&sae->msg_lock, flag);
	return err;
}

static int se_send_rng_cmd(struct loongson_se *se,
		struct se_alg_engine *sae, int retry)
{
	struct se_alg_msg *alg_msg = (struct se_alg_msg *)sae->msg_buffer;
	unsigned long flag;
	int err = 0;

	if (!sae->buffer_cnt)
		return 0;

	if (!alg_msg)
		return -EINVAL;

	spin_lock_irqsave(&sae->msg_lock, flag);

	alg_msg->cmd = SE_CMD_RNG;
	alg_msg->u.req.len = round_up(sae->buffer_cnt, SE_RNG_DATA_ALIGN);
	alg_msg->u.req.out_off = sae->out_addr - se->data_addr;

	sae->cmd = SE_CMD_RNG;

try_again:
	pr_debug("RNG CMD data offset is 0x%x, data length is %d\n",
			alg_msg->u.req.out_off, alg_msg->u.req.len);

	if (!retry--)
		goto out;

	err = se_send_alg_requeset(se, SE_INT_RNG);
	if (err) {
		udelay(5);
		goto try_again;
	}

out:
	spin_unlock_irqrestore(&sae->msg_lock, flag);
	return err;
}

static struct se_alg_engine *se_find_engine(struct loongson_se *se,
												u32 type)
{
	struct se_alg_engine *sae;
	struct se_alg_engine *next;

	list_for_each_entry_safe(sae, next, &se->alg_engine, engine_list) {
		if (sae->alg->type == type)
			return sae;
	}

	return NULL;
}

static void loongson_sm3_finish_req(struct se_alg_engine *sae)
{
	struct loongson_hash_request_ctx *rctx =
		(struct loongson_hash_request_ctx *)sae->rctx;

	pr_debug("%s op %lu ret %d\n", __func__, rctx->op, sae->cmd_ret);

	if (!rctx || !rctx->req)
		return;

	/* The buffer is cleared regardless of whether the final cmd is successful*/
	if (rctx->op == SE_HASH_OP_FINAL || rctx->flags & SE_HASH_FLAGS_FINUP) {
		if (!sae->cmd_ret)
			memcpy(rctx->req->result, sae->out_buffer, SM3_DIGEST_SIZE);

		memset(sae->in_buffer, 0, sae->buffer_cnt);
		sae->buffer_cnt = 0;
		rctx->buf_cnt = 0;
		rctx->flags &= ~SE_HASH_FLAGS_FINUP;
		sae->rctx = NULL;
	}

	crypto_finalize_hash_request(sae->engine, rctx->req, sae->cmd_ret);
	list_del(&sae->finish_list);
	se_enable_int(SE_INT_SM3);
}

static void loongson_sm4_finish_task(struct se_alg_engine *sae)
{
	struct loongson_skcipher_request_ctx *rctx =
		(struct loongson_skcipher_request_ctx *)sae->rctx;
	int err = 0;

	if (!rctx || !rctx->req)
		return;

	pr_debug("%s op %lu ret %d\n", __func__, rctx->op, sae->cmd_ret);

	if (sae->cmd_ret || !sae->buffer_cnt) {
		err = sae->cmd_ret;
		sae->buffer_cnt = 0;
		goto out;
	}

	if (sg_pcopy_from_buffer(rctx->dst, rctx->out_nents, sae->out_buffer,
			sae->buffer_cnt, rctx->copyed_bytes) != sae->buffer_cnt) {
		pr_err("SM4 finish copy failed!\n");
		err = -EFAULT;
	}

	rctx->rest_bytes -= sae->buffer_cnt;
	rctx->copyed_bytes += sae->buffer_cnt;
	sae->buffer_cnt = 0;

	if (rctx->rest_bytes && !err) {
		err = loongson_sm4_cipher_once(sae, rctx->req, rctx->op);
		if (!err) {
			list_del(&sae->finish_list);
			se_enable_int(SE_INT_SM4);
			return;
		}
	}

out:
	crypto_finalize_skcipher_request(sae->engine, rctx->req, err);
	sae->rctx = NULL;
	list_del(&sae->finish_list);
	se_enable_int(SE_INT_SM4);
}

static void loongson_sm4_finish_req(struct se_alg_engine *sae)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;

	tasklet_schedule(&se->task);
}

static void loongson_se_finish_req(struct loongson_se *se)
{
	struct se_alg_engine *sae;
	struct se_alg_engine *next;

	list_for_each_entry_safe(sae, next, &se->finish_engine, finish_list) {
		if (sae->rctx) {
			switch (sae->alg->type) {
			case SE_ALG_TYPE_SM3:
				loongson_sm3_finish_req(sae);
				break;

			case SE_ALG_TYPE_SM4:
				loongson_sm4_finish_req(sae);
				break;

			case SE_ALG_TYPE_RNG:
				complete(&se->rng_completion);
				list_del(&sae->finish_list);
				break;

			default:
				pr_err("%s Unrecognized ALG %d\n", __func__, sae->alg->type);
				break;
			}
		} else {
			pr_err("%s Cannot find %d request\n", __func__, sae->alg->type);
			list_del(&sae->finish_list);
		}
	}
}

/*-------------------- SM3 functions -----------------------------------------*/
static int loongson_sm3_enqueue(struct ahash_request *req, unsigned int op)
{
	struct loongson_hash_request_ctx *rctx = ahash_request_ctx(req);
	struct loongson_hash_ctx *ctx = crypto_tfm_ctx(req->base.tfm);

	rctx->op = op;

	return crypto_transfer_hash_request_to_engine(ctx->sae->engine, req);
}

static int loongson_sm3_final_req(struct ahash_request *req)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;
	struct loongson_hash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(req));
	struct loongson_hash_request_ctx *rctx = ahash_request_ctx(req);
	struct se_alg_engine *sae = ctx->sae;
	int err = 0;

	/* Clean cache */
	if (rctx->cache_len) {
		memcpy(sae->in_buffer + sae->buffer_cnt,
			rctx->cache, rctx->cache_len);
		sae->buffer_cnt += rctx->cache_len;
		memset(rctx->cache, 0, rctx->cache_len);
		rctx->cache_len = 0;
	}

	err = se_send_sm3_cmd(se, sae, 5);
	if (err)
		return err;

	return -EINPROGRESS;
}

static int loongson_sm3_update_req(struct ahash_request *req)
{
	struct loongson_hash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(req));
	struct loongson_hash_request_ctx *rctx = ahash_request_ctx(req);
	struct se_alg_engine *sae = ctx->sae;
	size_t bytes;
	int err = 0;

	if (rctx->sg_bytes + sae->buffer_cnt > sae->buffer_size) {
		pr_err("SM3 data overflow! Max size is 0x%x Bytes\n", sae->buffer_size);
		err = -ENOMEM;
		rctx->flags |= SE_HASH_FLAGS_FINUP;
		goto out;
	}

	if (rctx->cache_len) {
		memcpy(sae->in_buffer + sae->buffer_cnt,
			rctx->cache, rctx->cache_len);
		sae->buffer_cnt += rctx->cache_len;
		memset(rctx->cache, 0, rctx->cache_len);
		rctx->cache_len = 0;
	}

	if (rctx->sg && rctx->nents) {
		bytes = sg_pcopy_to_buffer(rctx->sg, rctx->nents,
			sae->in_buffer + sae->buffer_cnt, rctx->sg_bytes, 0);
		if (bytes != rctx->sg_bytes) {
			err = -EFAULT;
			rctx->flags |= SE_HASH_FLAGS_FINUP;
		}
		sae->buffer_cnt += rctx->sg_bytes;
		rctx->sg = NULL;
		rctx->nents = 0;
		rctx->sg_bytes = 0;
	}

out:
	return err;
}

static int loongson_sm3_one_request(struct crypto_engine *engine, void *areq)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;
	struct ahash_request *req = container_of(areq, struct ahash_request, base);
	struct loongson_hash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(req));
	struct loongson_hash_request_ctx *rctx = ahash_request_ctx(req);
	struct se_alg_engine *sae = ctx->sae;
	int err = 0;

	if (!sae)
		return -ENODEV;

	if (sae->rctx && sae->rctx != rctx) {
		/* Should wait last request done */
		crypto_finalize_hash_request(sae->engine, req, -EINPROGRESS);
		err = loongson_sm3_enqueue(req, rctx->op);
		pr_debug("You may block here ...\n");
		return err == -EINPROGRESS ? 0 : err;
	}

	sae->rctx = rctx;
	rctx->req = req;

	if (rctx->op == SE_HASH_OP_UPDATE)
		err = loongson_sm3_update_req(req);

	if (rctx->op == SE_HASH_OP_FINAL ||
			(rctx->flags & SE_HASH_FLAGS_FINUP && !err))
		err = loongson_sm3_final_req(req);

	pr_debug("%s op %lu flags 0x%lx err %d\n",
			__func__, rctx->op, rctx->flags, err);

	if (err != -EINPROGRESS) {
		sae->cmd_ret = err;
		list_add_tail(&sae->finish_list, &se->finish_engine);
		loongson_se_finish_req(se);
	}

	return 0;
}

static int loongson_sm3_final(struct ahash_request *req)
{
	return loongson_sm3_enqueue(req, SE_HASH_OP_FINAL);
}

static int loongson_sm3_update(struct ahash_request *req)
{
	struct loongson_hash_request_ctx *rctx = ahash_request_ctx(req);
	size_t bytes;

	/*
	 * NOTE:
	 * return -EINPROGRESS upper layer will wait
	 * return other ecodes to inform upper layer request failed
	 * return 0 upper layer will go
	 */
	if (!req->nbytes)
		return 0;

	rctx->buf_cnt += req->nbytes;

	if (req->nbytes <= SE_SM3_BUFLEN - rctx->cache_len) {
		bytes = sg_pcopy_to_buffer(req->src, sg_nents(req->src), rctx->cache,
				req->nbytes, 0);
		if (bytes != req->nbytes)
			return -EFAULT;
		rctx->cache_len += bytes;
		return 0;
	}

	rctx->sg = req->src;
	rctx->nents = sg_nents_for_len(req->src, req->nbytes);
	rctx->sg_bytes = req->nbytes;

	return loongson_sm3_enqueue(req, SE_HASH_OP_UPDATE);
}

static int loongson_sm3_init(struct ahash_request *req)
{
	struct loongson_hash_ctx *ctx = crypto_ahash_ctx(crypto_ahash_reqtfm(req));
	struct loongson_hash_request_ctx *rctx = ahash_request_ctx(req);

	memset(rctx, 0, sizeof(*rctx));

	rctx->buf_size = ctx->sae->buffer_size;

	return 0;
}

static int loongson_sm3_finup(struct ahash_request *req)
{
	struct loongson_hash_request_ctx *rctx = ahash_request_ctx(req);
	int err;

	/* The engine only allows one request to be pushed in */
	rctx->flags |= SE_HASH_FLAGS_FINUP;

	err = loongson_sm3_update(req);
	if (err)
		return err;

	err = loongson_sm3_final(req);

	return err;
}

static int loongson_sm3_digest(struct ahash_request *req)
{
	return loongson_sm3_init(req) ? : loongson_sm3_finup(req);
}

static int loongson_se_ahash_init(struct crypto_tfm *tfm)
{
	struct loongson_hash_ctx *tfm_ctx = crypto_tfm_ctx(tfm);
	struct loongson_se *se = dev_get_drvdata(se_dev);

	crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
				 sizeof(struct loongson_hash_request_ctx));
	memset(tfm_ctx, 0, sizeof(*tfm_ctx));

	tfm_ctx->sae = se_find_engine(se, SE_ALG_TYPE_SM3);
	if (unlikely(!tfm_ctx->sae))
		return -ENODEV;

	tfm_ctx->enginectx.op.do_one_request = loongson_sm3_one_request;
	tfm_ctx->enginectx.op.prepare_request = NULL;
	tfm_ctx->enginectx.op.unprepare_request = NULL;

	return 0;
}

static void loongson_se_ahash_exit(struct crypto_tfm *tfm)
{
	struct loongson_hash_ctx *tfm_ctx = crypto_tfm_ctx(tfm);
	struct loongson_hash_request_ctx *rctx;
	struct se_alg_engine *sae = tfm_ctx->sae;

	if (sae && sae->rctx)
		rctx = (struct loongson_hash_request_ctx *)sae->rctx;
	else
		return;

	if (rctx->req && rctx->req->base.tfm == tfm) {
		/* Last request blocked, remove it */
		crypto_finalize_hash_request(sae->engine, rctx->req, -EFAULT);
		sae->rctx = NULL;
		sae->cmd = 0;
		sae->cmd_ret = 0;
		sae->buffer_cnt = 0;
	}
}

/*-------------------- SM4 functions -----------------------------------------*/
static int loongson_sm4_enqueue(struct skcipher_request *req, unsigned int op)
{
	struct loongson_skcipher_ctx *ctx = crypto_skcipher_ctx(
					crypto_skcipher_reqtfm(req));
	struct loongson_skcipher_request_ctx *rctx = skcipher_request_ctx(req);
	struct se_alg_engine *sae = ctx->sae;

	rctx->op = op;

	return crypto_transfer_skcipher_request_to_engine(sae->engine, req);
}

static int loongson_sm4_cipher_once(struct se_alg_engine *sae,
		struct skcipher_request *req, u32 op)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;
	struct loongson_skcipher_request_ctx *rctx = skcipher_request_ctx(req);
	size_t nbytes;

	if (sae->buffer_cnt)
		return -EINPROGRESS;

	nbytes = min(rctx->rest_bytes, sae->buffer_size);
	if (!nbytes)
		return 0;

	if (sg_pcopy_to_buffer(rctx->src, rctx->in_nents, sae->in_buffer,
				nbytes, rctx->copyed_bytes) != nbytes) {
		pr_err("SM4 copy sg to buffer failed!\n");
		return -EFAULT;
	}

	sae->buffer_cnt = nbytes;

	return se_send_sm4_cmd(se, sae, op, 5);
}

static int loongson_sm4_one_request(struct crypto_engine *engine, void *areq)
{
	struct skcipher_request *req = container_of(areq,
					struct skcipher_request, base);
	struct loongson_skcipher_ctx *ctx = crypto_skcipher_ctx(
					crypto_skcipher_reqtfm(req));
	struct loongson_skcipher_request_ctx *rctx = skcipher_request_ctx(req);
	struct se_alg_engine *sae = ctx->sae;

	if (!sae)
		return -ENODEV;

	if (sae->rctx && sae->rctx != rctx) {
		int err;
		/* Should wait last request done */
		crypto_finalize_skcipher_request(sae->engine, req, -EINPROGRESS);
		err = loongson_sm4_enqueue(req, rctx->op);
		return err == -EINPROGRESS ? 0 : err;
	}

	sae->rctx = rctx;
	rctx->req = req;
	rctx->src = req->src;
	rctx->dst = req->dst;
	rctx->in_nents = sg_nents_for_len(req->src, req->cryptlen);
	rctx->out_nents = sg_nents_for_len(req->dst, req->cryptlen);
	rctx->copyed_bytes = 0;
	rctx->rest_bytes = req->cryptlen;

	memcpy(sae->key_buffer, ctx->key, ctx->keylen);

	return loongson_sm4_cipher_once(sae, req, rctx->op);
}

static int loongson_ecb_sm4_setkey(struct crypto_skcipher *cipher,
		const u8 *key, unsigned int len)
{
	struct crypto_tfm *tfm = crypto_skcipher_tfm(cipher);
	struct loongson_skcipher_ctx *ctx = crypto_tfm_ctx(tfm);

	if (unlikely(len > SM4_KEY_SIZE)) {
		pr_err("SM4 key length overflow!\n");
		return -EFAULT;
	}

	memcpy(ctx->key, key, len);
	ctx->keylen = len;

	return 0;
}

static int loongson_ecb_sm4_encrypt(struct skcipher_request *req)
{
	return loongson_sm4_enqueue(req, SE_SKCIPHER_OP_ENCRY);
}

static int loongson_ecb_sm4_decrypt(struct skcipher_request *req)
{
	return loongson_sm4_enqueue(req, SE_SKCIPHER_OP_DECRY);
}

static int loongson_se_skcipher_init(struct crypto_tfm *tfm)
{
	struct loongson_skcipher_ctx *tfm_ctx = crypto_tfm_ctx(tfm);
	struct loongson_se *se = dev_get_drvdata(se_dev);

	crypto_skcipher_set_reqsize(__crypto_skcipher_cast(tfm),
				 sizeof(struct loongson_skcipher_request_ctx));
	memset(tfm_ctx, 0, sizeof(*tfm_ctx));

	tfm_ctx->sae = se_find_engine(se, SE_ALG_TYPE_SM4);
	if (unlikely(!tfm_ctx->sae))
		return -ENODEV;

	tfm_ctx->enginectx.op.do_one_request = loongson_sm4_one_request;
	tfm_ctx->enginectx.op.prepare_request = NULL;
	tfm_ctx->enginectx.op.unprepare_request = NULL;

	return 0;
}

static void loongson_se_skcipher_exit(struct crypto_tfm *tfm)
{
	struct loongson_skcipher_ctx *tfm_ctx = crypto_tfm_ctx(tfm);
	struct loongson_skcipher_request_ctx *rctx;
	struct se_alg_engine *sae = tfm_ctx->sae;

	if (sae && sae->rctx)
		rctx = (struct loongson_skcipher_request_ctx *)sae->rctx;
	else
		return;

	if (rctx->req && rctx->req->base.tfm == tfm) {
		/* Last request blocked, remove it */
		crypto_finalize_skcipher_request(sae->engine, rctx->req, -EFAULT);
		sae->rctx = NULL;
		sae->cmd = 0;
		sae->cmd_ret = 0;
		sae->buffer_cnt = 0;
	}
}

/*-------------------- RNG functions -----------------------------------------*/
static int loongson_rng_seed(struct crypto_rng *tfm, const u8 *seed,
			 unsigned int slen)
{
	return 0;
}

static int loongson_rng_generate(struct crypto_rng *tfm,
			     const u8 *src, unsigned int slen,
			     u8 *dstn, unsigned int dlen)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;
	struct loongson_rng_ctx *ctx = crypto_rng_ctx(tfm);
	struct se_alg_engine *sae = ctx->sae;
	int err;

	if (sae->buffer_cnt)
		return -EBUSY;

	sae->rctx = ctx;

	do {
		sae->buffer_cnt = min(dlen, sae->buffer_size);
		err = se_send_rng_cmd(se, sae, 5);
		if (err)
			return err;

		wait_for_completion_interruptible(&se->rng_completion);

		if (sae->cmd_ret)
			return -EFAULT;

		memcpy(dstn, sae->out_buffer, sae->buffer_cnt);
		dlen -= sae->buffer_cnt;
		dstn += sae->buffer_cnt;
		se_enable_int(SE_INT_RNG);
	} while (dlen > 0);

	sae->buffer_cnt = 0;
	sae->rctx = NULL;

	return 0;
}

static int loongson_se_rng_init(struct crypto_tfm *tfm)
{
	struct loongson_se *se = (struct loongson_se *)se_dev->driver_data;
	struct loongson_rng_ctx *ctx = crypto_tfm_ctx(tfm);

	ctx->sae = se_find_engine(se, SE_ALG_TYPE_RNG);
	if (!ctx->sae) {
		pr_err("%s Can not find TYPE %d engine\n",
				__func__, SE_ALG_TYPE_SM3);
		return -EFAULT;
	}

	return 0;
}

static int init_crypto_hw(struct loongson_se *se)
{
	struct se_mailbox_data req = {0};
	struct se_mailbox_data res = {0};
	struct device *dev = se_dev;
	int err, retry = 5;
	u64 size;

	/* TODO Use platform data */
	size = SE_DATA_SIZE;

	if (size & (size - 1))
		size = roundup_pow_of_two(size);

	se_enable_int(SE_INT_ALL);

	/* Start engine */
	memset(&req, 0, sizeof(struct se_mailbox_data));
	memset(&res, 0, sizeof(struct se_mailbox_data));
	req.int_bit = SE_INT_SETUP;
	req.u.gcmd.cmd = SE_CMD_START;
	err = se_send_genl_cmd(se, &req, &res, retry);
	if (err)
		return err;

	/* Get Version */
	memset(&req, 0, sizeof(struct se_mailbox_data));
	memset(&res, 0, sizeof(struct se_mailbox_data));
	req.int_bit = SE_INT_SETUP;
	req.u.gcmd.cmd = SE_CMD_GETVER;
	err = se_send_genl_cmd(se, &req, &res, retry);
	if (err)
		return err;

	se->version = res.u.res.info[0];

	/* Setup data buffer */
	se->data_buffer = dmam_alloc_coherent(dev, size,
			&se->data_addr, GFP_KERNEL);
	if (!se->data_buffer)
		return -ENOMEM;

	memset(se->data_buffer, 0, size);

	memset(&req, 0, sizeof(struct se_mailbox_data));
	memset(&res, 0, sizeof(struct se_mailbox_data));
	req.int_bit = SE_INT_SETUP;
	req.u.gcmd.cmd = SE_CMD_SETBUF;
	/* MMAP */
	req.u.gcmd.info[0] = (se->data_addr & 0xffffffff) | 0x80;
	req.u.gcmd.info[1] = se->data_addr >> 32;
	/* MASK */
	req.u.gcmd.info[2] = ~(size - 1);
	req.u.gcmd.info[3] = 0xffffffff;

	pr_debug("Set win mmap 0x%llx, mask 0x%llx\n",
			((u64)req.u.gcmd.info[1] << 32) | req.u.gcmd.info[0],
			((u64)req.u.gcmd.info[3] << 32) | req.u.gcmd.info[2]);

	err = se_send_genl_cmd(se, &req, &res, retry);
	if (err)
		return err;

	se->data_free = size;
	se->data_used = 0;

	return err;
}

static void loongson_se_disable_hw(struct loongson_se *se)
{
	struct se_mailbox_data req = {0};
	struct se_mailbox_data res = {0};
	int retry = 5;

	/* Stop engine */
	req.int_bit = SE_INT_SETUP;
	req.u.gcmd.cmd = SE_CMD_STOP;
	se_send_genl_cmd(se, &req, &res, retry);

	se_disable_int(SE_INT_ALL);
}

static void loongson_se_task_routine(unsigned long data)
{
	struct loongson_se *se = (struct loongson_se *)data;
	struct se_alg_engine *sae;
	struct se_alg_engine *next;

	list_for_each_entry_safe(sae, next, &se->finish_engine, finish_list) {
		if (sae->rctx) {
			switch (sae->alg->type) {
			case SE_ALG_TYPE_SM4:
				loongson_sm4_finish_task(sae);
				break;

			default:
				pr_debug("%s Unrecognized ALG %d\n", __func__, sae->alg->type);
				break;
			}
		} else
			list_del(&sae->finish_list);
	}
}

static void loongson_se_update_irq(struct loongson_se *se,
		struct se_alg_engine *sae, u32 int_bit, u32 *int_status)
{
	struct se_alg_msg *alg_msg;
	unsigned long flag;

	if (!sae) {
		pr_debug("Can not find INT 0x%x engine!\n", int_bit);
		return;
	}

	alg_msg = (struct se_alg_msg *)sae->msg_buffer;
	if (!alg_msg) {
		pr_debug("Can not find INT 0x%x Message buffer!\n", int_bit);
		return;
	}

	spin_lock_irqsave(&sae->msg_lock, flag);

	if (alg_msg->cmd == sae->cmd)
		sae->cmd_ret = alg_msg->u.res.cmd_ret;
	else {
		sae->cmd_ret = -EFAULT;
		pr_err("INT 0x%x response CMD is 0x%x, expect 0x%x!\n",
				int_bit, alg_msg->cmd, sae->cmd);
	}

	memset(alg_msg, 0, sizeof(struct se_alg_msg));
	list_add_tail(&sae->finish_list, &se->finish_engine);
	*int_status &= ~int_bit;
	se_writel(int_bit, SE_S2LINT_CL);
	spin_unlock_irqrestore(&sae->msg_lock, flag);
}

static irqreturn_t loongson_se_irq(int irq, void *dev_id)
{
	struct loongson_se *se = (struct loongson_se *)dev_id;
	struct se_alg_engine *sae;
	u32 int_status;

	int_status = se_readl(SE_S2LINT_STAT);

	pr_debug("%s int status is 0x%x\n", __func__, int_status);

	se_disable_int(int_status);

	if (int_status & SE_INT_SETUP) {
		complete(&se->cmd_completion);
		int_status &= ~SE_INT_SETUP;
	}

	if (int_status & SE_INT_RNG) {
		sae = se_find_engine(se, SE_ALG_TYPE_RNG);
		loongson_se_update_irq(se, sae, SE_INT_RNG, &int_status);
	}

	if (int_status & SE_INT_SM3) {
		sae = se_find_engine(se, SE_ALG_TYPE_SM3);
		loongson_se_update_irq(se, sae, SE_INT_SM3, &int_status);
	}

	if (int_status & SE_INT_SM4) {
		sae = se_find_engine(se, SE_ALG_TYPE_SM4);
		loongson_se_update_irq(se, sae, SE_INT_SM4, &int_status);
	}

	loongson_se_finish_req(se);
	se_enable_int(int_status);

	return IRQ_HANDLED;
}

/*------------------- Algorithm definitions ----------------------------------*/
static struct loongson_alg_common se_alg[] = {
	{
		.type = SE_ALG_TYPE_SM3,
		.data_size = SE_SM3_DATA_SIZE,
		.need_engine = true,
		.u.ahash = {
			.init = loongson_sm3_init,
			.update = loongson_sm3_update,
			.final = loongson_sm3_final,
			.digest = loongson_sm3_digest,
			.halg.digestsize = SM3_DIGEST_SIZE,
			.halg.statesize = sizeof(struct loongson_hash_request_ctx),
			.halg.base = {
				.cra_name = "sm3",
				.cra_driver_name = "loongson-se-sm3",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC,
				.cra_blocksize = SM3_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct loongson_hash_ctx),
				.cra_alignmask = 0,
				.cra_module = THIS_MODULE,
				.cra_init = loongson_se_ahash_init,
				.cra_exit = loongson_se_ahash_exit,
			}
		}
	},
	{
		.type = SE_ALG_TYPE_SM4,
		.data_size = SE_SM4_DATA_SIZE,
		.need_engine = true,
		.u.skcipher = {
			.setkey = loongson_ecb_sm4_setkey,
			.encrypt = loongson_ecb_sm4_encrypt,
			.decrypt = loongson_ecb_sm4_decrypt,
			.min_keysize = SM4_KEY_SIZE,
			.max_keysize = SM4_KEY_SIZE,
			.base = {
				.cra_name = "ecb(sm4)",
				.cra_driver_name = "loongson-se-sm4",
				.cra_priority = 300,
				.cra_flags = CRYPTO_ALG_ASYNC,
				.cra_blocksize = SM4_BLOCK_SIZE,
				.cra_ctxsize = sizeof(struct loongson_skcipher_ctx),
				.cra_alignmask = 0,
				.cra_module = THIS_MODULE,
				.cra_init = loongson_se_skcipher_init,
				.cra_exit = loongson_se_skcipher_exit,
			}
		}
	},
	{
		.type = SE_ALG_TYPE_RNG,
		.data_size = SE_RNG_DATA_SIZE,
		.need_engine = false,
		.u.rng = {
			.generate	= loongson_rng_generate,
			.seed		= loongson_rng_seed,
			.seedsize	= 0,
			.base		= {
				.cra_name		= "stdrng",
				.cra_driver_name	= "loongson-se-rng",
				.cra_flags		= CRYPTO_ALG_TYPE_RNG,
				.cra_priority		= 300,
				.cra_ctxsize		= sizeof(struct loongson_rng_ctx),
				.cra_module		= THIS_MODULE,
				.cra_init		= loongson_se_rng_init,
			}
		}
	}
};

static int loongson_se_register_engine(struct se_alg_engine *sae, bool need)
{
	struct device *dev = se_dev;
	int err;

	if (!need)
		return 0;

	sae->engine = crypto_engine_alloc_init(dev, 1);
	if (!sae->engine)
		return -ENOMEM;

	err = crypto_engine_start(sae->engine);
	if (err)
		crypto_engine_exit(sae->engine);

	return err;
}

static int loongson_se_set_msg(struct loongson_se *se,
		struct se_alg_engine *sae)
{
	struct se_mailbox_data req = {0};
	struct se_mailbox_data res = {0};
	int err, msg_size;

	msg_size = sizeof(struct se_alg_msg);

	req.int_bit = SE_INT_SETUP;
	req.u.gcmd.cmd = SE_CMD_ALG_MSG;
	/* MSG off */
	req.u.gcmd.info[0] = sae->alg->type;
	req.u.gcmd.info[1] = sae->msg_addr - se->data_addr;
	req.u.gcmd.info[2] = msg_size;

	pr_debug("Set Alg %d msg off 0x%x, msg size %d\n", sae->alg->type,
			req.u.gcmd.info[1], req.u.gcmd.info[2]);

	err = se_send_genl_cmd(se, &req, &res, 5);
	if (res.u.res.cmd_ret)
		return res.u.res.cmd_ret;

	return err;
}

static int loongson_se_register_alg(struct loongson_se *se)
{
	struct se_alg_engine *sae;
	struct device *dev = se_dev;
	int err = 0;
	int msg_size, offset, i;

	msg_size = sizeof(struct se_alg_msg);

	for (i = 0; i < ARRAY_SIZE(se_alg); i++) {
		sae = devm_kzalloc(dev, sizeof(struct se_alg_engine), GFP_KERNEL);
		if (!sae) {
			err = -ENOMEM;
			break;
		}

		err = loongson_se_register_engine(sae, se_alg[i].need_engine);
		if (err)
			goto exit_engine;

		list_add_tail(&sae->engine_list, &se->alg_engine);
		spin_lock_init(&se->cmd_lock);
		sae->alg = &se_alg[i];

		/* Check buffer space */
		if (se->data_free < se_alg[i].data_size) {
			pr_err("Remain buffer size not enough\n");
			err = -ENOMEM;
			goto alg_err;
		}

		offset = 0;

		switch (se_alg[i].type) {
		case SE_ALG_TYPE_SM3:
			sae->in_buffer = se->data_buffer + se->data_used;
			sae->in_addr = se->data_addr + se->data_used;
			sae->buffer_size = sae->alg->data_size - SM3_DIGEST_SIZE - msg_size;
			offset += sae->buffer_size;

			sae->out_buffer = sae->in_buffer + offset;
			sae->out_addr = sae->in_addr + offset;
			offset += SM3_DIGEST_SIZE;

			sae->msg_buffer = sae->in_buffer + offset;
			sae->msg_addr = sae->in_addr + offset;

			err = loongson_se_set_msg(se, sae);
			if (err)
				goto alg_err;

			err = crypto_register_ahash(&se_alg[i].u.ahash);
			break;

		case SE_ALG_TYPE_SM4:
			sae->in_buffer = se->data_buffer + se->data_used;
			sae->in_addr = se->data_addr + se->data_used;
			/* Inplace encrypt and decrypt */
			sae->out_buffer = sae->in_buffer;
			sae->out_addr = sae->in_addr;
			sae->buffer_size = sae->alg->data_size - SM4_KEY_SIZE - msg_size;
			sae->buffer_size &= ~(SE_SM4_DATA_ALIGN_MASK);
			offset += sae->buffer_size;

			sae->key_buffer = sae->in_buffer + offset;
			sae->key_addr = sae->in_addr + offset;
			offset += SM4_KEY_SIZE;

			sae->msg_buffer = sae->in_buffer + offset;
			sae->msg_addr = sae->in_addr + offset;

			err = loongson_se_set_msg(se, sae);
			if (err)
				goto alg_err;

			err = crypto_register_skcipher(&se_alg[i].u.skcipher);
			break;

		case SE_ALG_TYPE_RNG:
			sae->out_buffer = se->data_buffer + se->data_used;
			sae->out_addr = se->data_addr + se->data_used;
			sae->buffer_size = sae->alg->data_size - msg_size;
			offset += sae->buffer_size;

			sae->msg_buffer = sae->out_buffer + offset;
			sae->msg_addr = sae->out_addr + offset;

			err = loongson_se_set_msg(se, sae);
			if (err)
				goto alg_err;

			err = crypto_register_rng(&se_alg[i].u.rng);
			break;

		default:
			pr_err("%s Unrecognized ALG %d\n", __func__, se_alg[i].type);
			err = -EINVAL;
			break;
		}

		if (err) {
			pr_err("Register type %d alg failed!\n",
				se_alg[i].type);
			goto alg_err;
		}

		se->data_free -= sae->alg->data_size;
		se->data_used += sae->alg->data_size;

		pr_debug("Alg type %d, data buffer free is 0x%x, data used is 0x%x\n",
				sae->alg->type, se->data_free, se->data_used);

		continue;

alg_err:
		list_del(&sae->engine_list);
		devm_kfree(dev, sae);
exit_engine:
		if (err == -ENOMEM)
			break;
		else
			continue;
	}

	return err;
}

static void loongson_unregister_alg(struct loongson_se *se)
{
	struct device *dev = se_dev;
	struct se_alg_engine *sae;
	struct se_alg_engine *next;

	list_for_each_entry_safe(sae, next, &se->alg_engine, engine_list) {
		switch (sae->alg->type) {
		case SE_ALG_TYPE_SM3:
			crypto_unregister_ahash(&sae->alg->u.ahash);
			break;

		case SE_ALG_TYPE_SM4:
			crypto_unregister_skcipher(&sae->alg->u.skcipher);
			break;

		case SE_ALG_TYPE_RNG:
			crypto_unregister_rng(&sae->alg->u.rng);
			break;

		default:
			pr_err("%s Unrecognized ALG %d\n", __func__, sae->alg->type);
			break;
		}

		if (sae->alg->need_engine && sae->engine)
			crypto_engine_exit(sae->engine);
		list_del(&sae->engine_list);
		devm_kfree(dev, sae);
	}

	se->data_free = SE_DATA_SIZE;
	se->data_used = 0;
}

static const struct of_device_id loongson_se_of_match[] = {
	{ .compatible = "loongson-se", },
	{}
};
MODULE_DEVICE_TABLE(of, loongson_se_of_match);

static int loongson_se_probe(struct platform_device *pdev)
{
	struct loongson_se *se;
	struct resource *res;
	struct device *dev = &pdev->dev;
	int irq, err;

	if (se_dev)
		return -ENODEV;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return -ENODEV;

	se = devm_kzalloc(dev, sizeof(struct loongson_se),
			  GFP_KERNEL);
	if (!se)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	se->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(se->base))
		return PTR_ERR(se->base);

	se_dev = &pdev->dev;
	platform_set_drvdata(pdev, se);
	dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));

	INIT_LIST_HEAD(&se->alg_engine);
	INIT_LIST_HEAD(&se->finish_engine);
	spin_lock_init(&se->cmd_lock);
	init_completion(&se->cmd_completion);
	init_completion(&se->rng_completion);
	tasklet_init(&se->task, loongson_se_task_routine, (unsigned long)se);

	err = devm_request_irq(dev, irq, loongson_se_irq, 0,
			       "loongson-crypto", se);
	if (err)
		goto disable_hw;

	err = init_crypto_hw(se);
	if (err)
		goto out;

	err = loongson_se_register_alg(se);
	if (err) {
		dev_err(dev, "Failed to register alg\n");
		goto disable_hw;
	}

	return 0;

disable_hw:
	loongson_se_disable_hw(se);
out:
	return err;
}

static int loongson_se_remove(struct platform_device *pdev)
{
	struct loongson_se *se = platform_get_drvdata(pdev);
	int irq = platform_get_irq(pdev, 0);

	loongson_se_disable_hw(se);
	tasklet_kill(&se->task);
	loongson_unregister_alg(se);
	devm_free_irq(&pdev->dev, irq, se);

	return 0;
}

static struct platform_driver loongson_se_driver = {
	.probe   = loongson_se_probe,
	.remove  = loongson_se_remove,
	.driver  = {
		.name  = "loongson-se",
		.owner = THIS_MODULE,
		.of_match_table = loongson_se_of_match,
	},
};

module_platform_driver(loongson_se_driver);

MODULE_AUTHOR("Yinggang Gu");
MODULE_DESCRIPTION("Loongson SE driver");
MODULE_LICENSE("GPL");
