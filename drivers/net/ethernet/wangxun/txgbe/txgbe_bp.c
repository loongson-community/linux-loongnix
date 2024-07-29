/*
 * WangXun 10 Gigabit PCI Express Linux driver
 * Copyright (c) 2015 - 2017 Beijing WangXun Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 */
#include "txgbe_bp.h"

void txgbe_bp_close_protect(struct txgbe_adapter *adapter)
{
	adapter->flags2 |= TXGBE_FLAG2_KR_PRO_DOWN;
	while (adapter->flags2 & TXGBE_FLAG2_KR_PRO_REINIT){
		msleep(100);
		printk("wait to reinited ok..%x\n",adapter->flags2);
	}
}

int txgbe_bp_mode_setting(struct txgbe_adapter *adapter)
{
	struct txgbe_hw *hw = &adapter->hw;

	/*default to open an73*/
	adapter->backplane_an = AUTO?1:0;
	adapter->an37 = AUTO?1:0;
	switch (adapter->backplane_mode) {
	case TXGBE_BP_M_KR:
		hw->subsystem_device_id = TXGBE_ID_WX1820_KR_KX_KX4;
		break;
	case TXGBE_BP_M_KX4:
		hw->subsystem_device_id = TXGBE_ID_WX1820_MAC_XAUI;
		break;
	case TXGBE_BP_M_KX:
		hw->subsystem_device_id = TXGBE_ID_WX1820_MAC_SGMII;
		break;
	case TXGBE_BP_M_SFI:
		hw->subsystem_device_id = TXGBE_ID_WX1820_SFP;
		break;
	default:
		break;
	}

	if (adapter->backplane_auto == TXGBE_BP_M_AUTO) {
		adapter->backplane_an = 1;
		adapter->an37 = 1;
	} else if (adapter->backplane_auto == TXGBE_BP_M_NAUTO) {
		adapter->backplane_an = 0;
		adapter->an37 = 0;
	}

	if (adapter->ffe_set == 0)
		return 0;

	if (KR_SET == 1) {
		adapter->ffe_main = KR_MAIN;
		adapter->ffe_pre = KR_PRE;
		adapter->ffe_post = KR_POST;
	} else if (!KR_SET && KX4_SET == 1) {
		adapter->ffe_main = KX4_MAIN;
		adapter->ffe_pre = KX4_PRE;
		adapter->ffe_post = KX4_POST;
	} else if (!KR_SET && !KX4_SET && KX_SET == 1) {
		adapter->ffe_main = KX_MAIN;
		adapter->ffe_pre = KX_PRE;
		adapter->ffe_post = KX_POST;
	} else if (!KR_SET && !KX4_SET && !KX_SET && SFI_SET == 1) {
		adapter->ffe_main = SFI_MAIN;
		adapter->ffe_pre = SFI_PRE;
		adapter->ffe_post = SFI_POST;
	}
	return 0;
}

void txgbe_bp_watchdog_event(struct txgbe_adapter *adapter)
{
	u32 value = 0;
	struct txgbe_hw *hw = &adapter->hw;
	struct net_device *netdev = adapter->netdev;
	
	/* only continue if link is down */
	if (netif_carrier_ok(netdev))
		return;

	if (KR_POLLING == 1) {
		value = txgbe_rd32_epcs(hw, 0x78002);
		value = value & 0x4;
		if (value == 0x4) {
			e_dev_info("Enter training\n");
			handle_bkp_an73_flow(0, adapter);
		}
	} else {
		if(adapter->flags2 & TXGBE_FLAG2_KR_TRAINING){
			e_dev_info("Enter training\n");
			handle_bkp_an73_flow(0, adapter);
			adapter->flags2 &= ~TXGBE_FLAG2_KR_TRAINING;
		}
	}
}

void txgbe_bp_down_event(struct txgbe_adapter *adapter)
{
	struct txgbe_hw *hw = &adapter->hw;
	u32 val = 0, val1 = 0;

	if (adapter->backplane_an == 0)
		return;

	switch (KR_RESTART_T_MODE) {
	case 1:
		txgbe_wr32_epcs(hw, TXGBE_VR_AN_KR_MODE_CL, 0x0000);
		txgbe_wr32_epcs(hw, TXGBE_SR_AN_MMD_CTL, 0x0000);
		txgbe_wr32_epcs(hw, 0x78001, 0x0000);
		msleep(1000);
		txgbe_set_link_to_kr(hw, 1);
		break;
	case 2:
		txgbe_wr32_epcs(hw, TXGBE_VR_AN_KR_MODE_CL, 0x0000);
		txgbe_wr32_epcs(hw, TXGBE_SR_AN_MMD_CTL, 0x0000);
		txgbe_wr32_epcs(hw, 0x78001, 0x0000);
		msleep(1050);
		txgbe_wr32_epcs(hw, TXGBE_VR_AN_KR_MODE_CL, 0x0001);
		txgbe_wr32_epcs(hw, TXGBE_SR_AN_MMD_CTL, 0x3200);
		txgbe_wr32_epcs(hw, 0x78001, 0x0007);
		break;
	default:
		if (AN73_TRAINNING_MODE == 1)
			msleep(100);
		else
			msleep(1000);
		val = txgbe_rd32_epcs(hw, 0x78002);
		val1 = txgbe_rd32_epcs(hw, TXGBE_SR_AN_MMD_CTL);
		if ((val & BIT(2)) == BIT(2)) {
			if (!(adapter->flags2 & TXGBE_FLAG2_KR_TRAINING))
				adapter->flags2 |= TXGBE_FLAG2_KR_TRAINING;
		} else {
			txgbe_wr32_epcs(hw, TXGBE_SR_AN_MMD_CTL, 0);
			txgbe_wr32_epcs(hw, 0x78002, 0x0000);
			txgbe_wr32_epcs(hw, TXGBE_SR_AN_MMD_CTL, 0x3000);
		}
		kr_dbg(KR_MODE, "0x78002 : %x - 0x70000 : %x\n", val, val1);
		kr_dbg(KR_MODE, "0x70012 : %x\n", txgbe_rd32_epcs(hw, 0x70012));
		break;
	}
}

/*Check Ethernet Backplane AN73 Base Page Ability
**return value:
**  -1 : none link mode matched, exit
**   0 : current link mode matched, wait AN73 to be completed
**   1 : current link mode not matched, set to matched link mode, re-start AN73 external
*/
int chk_bkp_an73_ability(bkpan73ability tBkpAn73Ability, bkpan73ability tLpBkpAn73Ability,
								struct txgbe_adapter *adapter)
{
	unsigned int comLinkAbility;

	kr_dbg(KR_MODE, "CheckBkpAn73Ability():\n");
	kr_dbg(KR_MODE, "------------------------\n");

	/*-- Check the common link ability and take action based on the result*/
	comLinkAbility = tBkpAn73Ability.linkAbility & tLpBkpAn73Ability.linkAbility;
	kr_dbg(KR_MODE, "comLinkAbility= 0x%x, linkAbility= 0x%x, lpLinkAbility= 0x%x\n",
	       comLinkAbility, tBkpAn73Ability.linkAbility, tLpBkpAn73Ability.linkAbility);

	/*only support kr*/
	if (comLinkAbility == 0){
		kr_dbg(KR_MODE, "WARNING: The Link Partner does not support any compatible speed mode!!!\n\n");
		return -1;
	} else if (comLinkAbility & 0x80) {
		if (tBkpAn73Ability.currentLinkMode == 0){
			kr_dbg(KR_MODE, "Link mode is matched with Link Partner: [LINK_KR].\n");
			return 0;
		} else {
			kr_dbg(KR_MODE, "Link mode is not matched with Link Partner: [LINK_KR].\n");
			kr_dbg(KR_MODE, "Set the local link mode to [LINK_KR] ...\n");
			return 1;
		}
	}

#if 0
	if (comLinkAbility == 0){
		kr_dbg(KR_MODE, "WARNING: The Link Partner does not support any compatible speed mode!!!\n\n");
		return -1;
	} else if (comLinkAbility & 0x80) {
		if (tBkpAn73Ability.currentLinkMode == 0){
			kr_dbg(KR_MODE, "Link mode is matched with Link Partner: [LINK_KR].\n");
			return 0;
		}else{
			kr_dbg(KR_MODE, "Link mode is not matched with Link Partner: [LINK_KR].\n");
			kr_dbg(KR_MODE, "Set the local link mode to [LINK_KR] ...\n");
			txgbe_set_link_to_kr(hw, 1);
			return 1;
		}
	} else if (comLinkAbility & 0x40) {
		if (tBkpAn73Ability.currentLinkMode == 0x10){
			kr_dbg(KR_MODE, "Link mode is matched with Link Partner: [LINK_KX4].\n");
			return 0;
		} else {
			kr_dbg(KR_MODE, "Link mode is not matched with Link Partner: [LINK_KX4].\n");
			kr_dbg(KR_MODE, "Set the local link mode to [LINK_KX4] ...\n");
			txgbe_set_link_to_kx4(hw, 1);
			return 1;
		}
	} else if (comLinkAbility & 0x20) {
		if (tBkpAn73Ability.currentLinkMode == 0x1){
			kr_dbg(KR_MODE, "Link mode is matched with Link Partner: [LINK_KX].\n");
			return 0;
		} else {
			kr_dbg(KR_MODE, "Link mode is not matched with Link Partner: [LINK_KX].\n");
			kr_dbg(KR_MODE, "Set the local link mode to [LINK_KX] ...\n");
			txgbe_set_link_to_kx(hw, 1, 1);
			return 1;
		}
	}
#endif
	return 0;
}


/*Get Ethernet Backplane AN73 Base Page Ability
**byLinkPartner:
**- 1: Get Link Partner Base Page
**- 2: Get Link Partner Next Page (only get NXP Ability Register 1 at the moment)
**- 0: Get Local Device Base Page
*/
int get_bkp_an73_ability(bkpan73ability *pt_bkp_an73_ability, unsigned char byLinkPartner,
			 struct txgbe_adapter *adapter)
{
	struct txgbe_hw *hw = &adapter->hw;
	unsigned int rdata;
	int status = 0;

	kr_dbg(KR_MODE, "byLinkPartner = %d\n", byLinkPartner);
	kr_dbg(KR_MODE, "----------------------------------------\n");

	if (byLinkPartner == 1) /*Link Partner Base Page*/
	{
		/*Read the link partner AN73 Base Page Ability Registers*/
		kr_dbg(KR_MODE, "Read the link partner AN73 Base Page Ability Registers...\n");
		rdata = txgbe_rd32_epcs(hw, TXGBE_SR_AN_MMD_LP_ABL1);
		kr_dbg(KR_MODE, "SR AN MMD LP Base Page Ability Register 1: 0x%x\n", rdata);
		pt_bkp_an73_ability->nextPage = (rdata >> 15) & 0x01;
		kr_dbg(KR_MODE, "  Next Page (bit15): %d\n", pt_bkp_an73_ability->nextPage);

		rdata = txgbe_rd32_epcs(hw, 0x70014);
		kr_dbg(KR_MODE, "SR AN MMD LP Base Page Ability Register 2: 0x%x\n", rdata);
		pt_bkp_an73_ability->linkAbility = rdata & 0xE0;
		kr_dbg(KR_MODE, "  Link Ability (bit[15:0]): 0x%x\n",
		       pt_bkp_an73_ability->linkAbility);
		kr_dbg(KR_MODE, "  (0x20- KX_ONLY, 0x40- KX4_ONLY, 0x60- KX4_KX\n");
		kr_dbg(KR_MODE, "   0x80- KR_ONLY, 0xA0- KR_KX, 0xC0- KR_KX4, 0xE0- KR_KX4_KX)\n");

		rdata = txgbe_rd32_epcs(hw, 0x70015);
		kr_dbg(KR_MODE, "SR AN MMD LP Base Page Ability Register 3: 0x%x\n", rdata);
		kr_dbg(KR_MODE, "  FEC Request (bit15): %d\n", ((rdata >> 15) & 0x01));
		kr_dbg(KR_MODE, "  FEC Enable  (bit14): %d\n", ((rdata >> 14) & 0x01));
		pt_bkp_an73_ability->fecAbility = (rdata >> 14) & 0x03;
	} else if (byLinkPartner == 2) {/*Link Partner Next Page*/
		/*Read the link partner AN73 Next Page Ability Registers*/
		kr_dbg(KR_MODE, "\nRead the link partner AN73 Next Page Ability Registers...\n");
		rdata = txgbe_rd32_epcs(hw, 0x70019);
		kr_dbg(KR_MODE, " SR AN MMD LP XNP Ability Register 1: 0x%x\n", rdata);
		pt_bkp_an73_ability->nextPage = (rdata >> 15) & 0x01;
		if (KR_MODE)e_dev_info("  Next Page (bit15): %d\n", pt_bkp_an73_ability->nextPage);
	} else {
		/*Read the local AN73 Base Page Ability Registers*/
		kr_dbg(KR_MODE, "\nRead the local AN73 Base Page Ability Registers...\n");
		rdata = txgbe_rd32_epcs(hw, TXGBE_SR_AN_MMD_ADV_REG1);
		kr_dbg(KR_MODE, "SR AN MMD Advertisement Register 1: 0x%x\n", rdata);
		pt_bkp_an73_ability->nextPage = (rdata >> 15) & 0x01;
		kr_dbg(KR_MODE, "  Next Page (bit15): %d\n", pt_bkp_an73_ability->nextPage);

		rdata = txgbe_rd32_epcs(hw, TXGBE_SR_AN_MMD_ADV_REG2);
		kr_dbg(KR_MODE, "SR AN MMD Advertisement Register 2: 0x%x\n", rdata);
		pt_bkp_an73_ability->linkAbility = rdata & 0xE0;
		kr_dbg(KR_MODE, "  Link Ability (bit[15:0]): 0x%x\n",
		       pt_bkp_an73_ability->linkAbility);
		kr_dbg(KR_MODE, "  (0x20- KX_ONLY, 0x40- KX4_ONLY, 0x60- KX4_KX\n");
		kr_dbg(KR_MODE, "   0x80- KR_ONLY, 0xA0- KR_KX, 0xC0- KR_KX4, 0xE0- KR_KX4_KX)\n");

		rdata = txgbe_rd32_epcs(hw, 0x70012);
		kr_dbg(KR_MODE, "SR AN MMD Advertisement Register 3: 0x%x\n", rdata);
		kr_dbg(KR_MODE, "  FEC Request (bit15): %d\n", ((rdata >> 15) & 0x01));
		kr_dbg(KR_MODE, "  FEC Enable  (bit14): %d\n", ((rdata >> 14) & 0x01));
		pt_bkp_an73_ability->fecAbility = (rdata >> 14) & 0x03;
	} /*if (byLinkPartner == 1) Link Partner Base Page*/

	return status;
}

/* DESCRIPTION: Set the source data fields[bitHigh:bitLow] with setValue
** INPUTS:      *src_data: Source data pointer
**              bitHigh: High bit position of the fields
**              bitLow : Low bit position of the fields
**              setValue: Set value of the fields
** OUTPUTS:     return the updated source data
*/
static void set_fields(
    unsigned int *src_data,
    unsigned int bitHigh,
    unsigned int bitLow,
    unsigned int setValue) 
{
	int i;

	if (bitHigh == bitLow) {
		if (setValue == 0)
			*src_data &= ~(1 << bitLow);
		else
			*src_data |= (1 << bitLow);
	} else {
		for (i = bitLow; i <= bitHigh; i++)
			*src_data &= ~(1 << i); 
		*src_data |= (setValue << bitLow);
	}
}

/*Clear Ethernet Backplane AN73 Interrupt status
**- intIndexHi  =0, only intIndex bit will be cleared
**- intIndexHi !=0, the [intIndexHi, intIndex] range will be cleared
*/
int clr_bkp_an73_int(unsigned int intIndex, unsigned int intIndexHi, struct txgbe_adapter * adapter)
{
	struct txgbe_hw *hw = &adapter->hw;
	unsigned int rdata, wdata;
	int status = 0;

	rdata = txgbe_rd32_epcs(hw, 0x78002);
	kr_dbg(KR_MODE, "[Before clear] Read VR AN MMD Interrupt Register: 0x%x\n", rdata);

	wdata = rdata;
	if (intIndexHi)
		set_fields(&wdata, intIndexHi, intIndex, 0);
	else
		set_fields(&wdata, intIndex, intIndex, 0);

	txgbe_wr32_epcs(hw, 0x78002, wdata);
	rdata = txgbe_rd32_epcs(hw, 0x78002);
	kr_dbg(KR_MODE, "[After clear] Read VR AN MMD Interrupt Register: 0x%x\n", rdata);

	return status;
}

void read_phy_lane_txeq(unsigned short lane, struct txgbe_adapter *adapter)
{
	struct txgbe_hw *hw = &adapter->hw;
	unsigned int addr, rdata;

	/*LANEN_DIG_ASIC_TX_ASIC_IN_1[11:6]: TX_MAIN_CURSOR*/
	addr  = 0x100E | (lane << 8);
	rdata = rd32_ephy(hw, addr);
	kr_dbg(KR_MODE, "PHY LANE%0d TX EQ Read Value:\n", lane);
	kr_dbg(KR_MODE, "  TX_MAIN_CURSOR: %d\n", ((rdata >> 6) & 0x3F));

	/*LANEN_DIG_ASIC_TX_ASIC_IN_2[5 :0]: TX_PRE_CURSOR*/
	/*LANEN_DIG_ASIC_TX_ASIC_IN_2[11:6]: TX_POST_CURSOR*/
	addr  = 0x100F | (lane << 8);
	rdata = rd32_ephy(hw, addr);
	kr_dbg(KR_MODE, "  TX_PRE_CURSOR : %d\n", (rdata & 0x3F));
	kr_dbg(KR_MODE, "  TX_POST_CURSOR: %d\n", ((rdata >> 6) & 0x3F));
	kr_dbg(KR_MODE, "**********************************************\n");
}


/*Enable Clause 72 KR training
**
**Note:
**<1>. The Clause 72 start-up protocol should be initiated when all pages are exchanged during Clause 73 auto-
**negotiation and when the auto-negotiation process is waiting for link status to be UP for 500 ms after 
**exchanging all the pages.
**
**<2>. The local device and link partner should be enabled the CL72 KR training
**with in 500ms
**
**enable:
**- bits[1:0] =2'b11: Enable the CL72 KR training
**- bits[1:0] =2'b01: Disable the CL72 KR training
*/
int en_cl72_krtr(unsigned int enable, struct txgbe_adapter *adapter)
{
	struct txgbe_hw *hw = &adapter->hw;
	unsigned int wdata = 0;
	u32 val;

	if (enable == 1) {
		kr_dbg(KR_MODE, "\nDisable Clause 72 KR Training ...\n");
		read_phy_lane_txeq(0, adapter);
	} else if (enable == 3) {
		kr_dbg(KR_MODE, "\nEnable Clause 72 KR Training ...\n");
		if (CL72_KRTR_PRBS_MODE_EN != 0xffff) {
			/*Set PRBS Timer Duration Control to maximum 6.7ms in VR_PMA_KRTR_PRBS_CTRL1 Register*/
			wdata = CL72_KRTR_PRBS_MODE_EN;
			txgbe_wr32_epcs(hw, 0x18005, wdata);
			/*Set PRBS Timer Duration Control to maximum 6.7ms in VR_PMA_KRTR_PRBS_CTRL1 Register*/
			wdata = 0xFFFF;
			txgbe_wr32_epcs(hw, 0x18004, wdata);

			/*Enable PRBS Mode to determine KR Training Status by setting Bit 0 of VR_PMA_KRTR_PRBS_CTRL0 Register*/
			wdata = 0;
			set_fields(&wdata, 0, 0, 1);
		}

		/*Enable PRBS31 as the KR Training Pattern by setting Bit 1 of VR_PMA_KRTR_PRBS_CTRL0 Register*/
		if (CL72_KRTR_PRBS31_EN == 1)
			set_fields(&wdata, 1, 1, 1);
		val = txgbe_rd32_epcs(hw, 0x18003);
		wdata |= val;
		txgbe_wr32_epcs(hw, 0x18003, wdata);
		read_phy_lane_txeq(0, adapter);
	}

	/*Enable the Clause 72 start-up protocol by setting Bit 1 of SR_PMA_KR_PMD_CTRL Register.
	**Restart the Clause 72 start-up protocol by setting Bit 0 of SR_PMA_KR_PMD_CTRL Register*/
	wdata = enable;
	txgbe_wr32_epcs(hw, 0x10096, wdata);
	return 0;
}

int chk_cl72_krtr_status(struct txgbe_adapter *adapter)
{
	struct txgbe_hw *hw = &adapter->hw;
	unsigned int rdata = 0, rdata1;
	int status = 0;

	status = read_poll_timeout(txgbe_rd32_epcs, rdata1, (rdata1 & 0x9), 1000,
				   400000, false, hw, 0x10097);
	if (!status) {
		//Get the latest received coefficient update or status
		rdata = txgbe_rd32_epcs(hw, 0x010098);
		kr_dbg(KR_MODE, "SR PMA MMD 10GBASE-KR LP Coefficient Update Register: 0x%x\n",
		       rdata);
		rdata = txgbe_rd32_epcs(hw, 0x010099);
		kr_dbg(KR_MODE, "SR PMA MMD 10GBASE-KR LP Coefficient Status Register: 0x%x\n",
		       rdata);
		rdata = txgbe_rd32_epcs(hw, 0x01009a);
		kr_dbg(KR_MODE, "SR PMA MMD 10GBASE-KR LD Coefficient Update: 0x%x\n", rdata);

		rdata = txgbe_rd32_epcs(hw, 0x01009b);
		kr_dbg(KR_MODE, " SR PMA MMD 10GBASE-KR LD Coefficient Status: 0x%x\n", rdata);

		rdata = txgbe_rd32_epcs(hw, 0x010097);
		kr_dbg(KR_MODE, "SR PMA MMD 10GBASE-KR Status Register: 0x%x\n", rdata);
		kr_dbg(KR_MODE, "  Training Failure         (bit3): %d\n", ((rdata >> 3) & 0x01));
		kr_dbg(KR_MODE, "  Start-Up Protocol Status (bit2): %d\n", ((rdata >> 2) & 0x01));
		kr_dbg(KR_MODE, "  Frame Lock               (bit1): %d\n", ((rdata >> 1) & 0x01));
		kr_dbg(KR_MODE, "  Receiver Status          (bit0): %d\n", ((rdata >> 0) & 0x01));

		/*If bit3 is set, Training is completed with failure*/
		if ((rdata1 >> 3) & 0x01) {
			kr_dbg(KR_MODE, "Training is completed with failure!!!\n");
			read_phy_lane_txeq(0, adapter);
			return status;
		}

		/*If bit0 is set, Receiver trained and ready to receive data*/
		if ((rdata1 >> 0) & 0x01) {
			kr_dbg(KR_MODE, "Receiver trained and ready to receive data ^_^\n");
			e_dev_info("Receiver ready.\n");
			read_phy_lane_txeq(0, adapter);
			return status;
		}
	}

	kr_dbg(KR_MODE, "ERROR: Check Clause 72 KR Training Complete Timeout!!!\n");

	return status;
}

int handle_bkp_an73_flow(unsigned char bp_link_mode, struct txgbe_adapter *adapter)
{
	bkpan73ability tBkpAn73Ability , tLpBkpAn73Ability ;
	u32 rdata = 0, rdata1 = 0, round = 1;
	struct txgbe_hw *hw = &adapter->hw;
	bool lpld_all_rd = false;
	unsigned int addr, data;
	int status = 0, k;

	tBkpAn73Ability.currentLinkMode = bp_link_mode;

	if (AN73_TRAINNING_MODE == 1) {
		round = 2;
		txgbe_wr32_epcs(hw, TXGBE_SR_AN_MMD_CTL, 0);
	}
	kr_dbg(KR_MODE, "HandleBkpAn73Flow().\n");
	kr_dbg(KR_MODE, "---------------------------------\n");

	/*1. Get the local AN73 Base Page Ability*/
	kr_dbg(KR_MODE, "<1>. Get the local AN73 Base Page Ability ...\n");
	get_bkp_an73_ability(&tBkpAn73Ability, 0, adapter);
	/*2. Check the AN73 Interrupt Status*/
	kr_dbg(KR_MODE, "<2>. Check the AN73 Interrupt Status ...\n");
	/*3.Clear the AN_PG_RCV interrupt*/
	clr_bkp_an73_int(2, 0x0, adapter);

	/*3.1. Get the link partner AN73 Base Page Ability*/
	kr_dbg(KR_MODE, "<3.1>. Get the link partner AN73 Base Page Ability ...\n");
	get_bkp_an73_ability(&tLpBkpAn73Ability, 1, adapter);

	/*3.2. Check the AN73 Link Ability with Link Partner*/
	kr_dbg(KR_MODE, "<3.2>. Check the AN73 Link Ability with Link Partner ...\n");
	kr_dbg(KR_MODE, "                 Local Link Ability: 0x%x\n", tBkpAn73Ability.linkAbility);
	kr_dbg(KR_MODE, "  Link Partner Link Ability: 0x%x\n", tLpBkpAn73Ability.linkAbility);

	chk_bkp_an73_ability(tBkpAn73Ability, tLpBkpAn73Ability, adapter);

	/*Check the FEC and KR Training for KR mode*/
	/* FEC handling */
	kr_dbg(KR_MODE, "<3.3>. Check the FEC for KR mode ...\n");
	tBkpAn73Ability.fecAbility = 0x3;
	tLpBkpAn73Ability.fecAbility = 0x3;
	if (((tBkpAn73Ability.fecAbility & tLpBkpAn73Ability.fecAbility) == 0x03)
		&& (KR_FEC == 1)) {
		e_dev_info("Enable KR FEC ...\n");
		//Write 1 to SR_PMA_KR_FEC_CTRL bit0 to enable the FEC
		data = 1;
		addr = 0x100ab; //SR_PMA_KR_FEC_CTRL 
		txgbe_wr32_epcs(hw, addr, data);
	} else {
		e_dev_info("KR FEC is disabled.\n");
	}
	kr_dbg(KR_MODE, "\n<3.4>. Check the CL72 KR Training for KR mode ...\n");

	for (k = 0; k < round; k++) {
		status |= en_cl72_krtr(3, adapter);
		kr_dbg(KR_MODE, "\nCheck the Clause 72 KR Training status ...\n");
		status |= chk_cl72_krtr_status(adapter);

		status = read_poll_timeout(txgbe_rd32_epcs, rdata, (rdata & 0x8000), 1000,
					   200000, false, hw, 0x10099);
		if (!status) {
			rdata1 = txgbe_rd32_epcs(hw, 0x1009b) & 0x8000;
			if (rdata1 == 0x8000)
				lpld_all_rd = true;
		}

		if (lpld_all_rd) {
			rdata = rd32_ephy(hw, 0x100E);
			rdata1 = rd32_ephy(hw, 0x100F);
			e_dev_info("Lp and Ld all Ready, FFE : %d-%d-%d.\n",
				   (rdata >> 6) & 0x3F, rdata1 & 0x3F, (rdata1 >> 6) & 0x3F);
			clr_bkp_an73_int(2, 0, adapter);
			clr_bkp_an73_int(1, 0, adapter);
			clr_bkp_an73_int(0, 0, adapter);
			status = read_poll_timeout(txgbe_rd32_epcs, rdata, (rdata & 0x1000), 1000,
						   100000, false, hw, 0x30020);
			if (!status)
				e_dev_info("INT_AN_INT_CMPLT =1, AN73 Done Success.\n");
			return 0;
		}
		clr_bkp_an73_int(2, 0, adapter);
		clr_bkp_an73_int(1, 0, adapter);
		clr_bkp_an73_int(0, 0, adapter);
	}
	e_dev_info("Trainning failure\n");

	if (AN73_TRAINNING_MODE == 0)
		status |= en_cl72_krtr(1, adapter);

	return status;
}
