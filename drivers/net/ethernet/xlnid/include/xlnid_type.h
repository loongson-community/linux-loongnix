/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2008 - 2022 Xel Technology. */

#ifndef _XLNID_TYPE_H_
#define _XLNID_TYPE_H_

/*
 * The following is a brief description of the error categories used by the
 * ERROR_REPORT* macros.
 *
 * - XLNID_ERROR_INVALID_STATE
 * This category is for errors which represent a serious failure state that is
 * unexpected, and could be potentially harmful to device operation. It should
 * not be used for errors relating to issues that can be worked around or
 * ignored.
 *
 * - XLNID_ERROR_POLLING
 * This category is for errors related to polling/timeout issues and should be
 * used in any case where the timeout occurred, or a failure to obtain a lock, or
 * failure to receive data within the time limit.
 *
 * - XLNID_ERROR_CAUTION
 * This category should be used for reporting issues that may be the cause of
 * other errors, such as temperature warnings. It should indicate an event which
 * could be serious, but hasn't necessarily caused problems yet.
 *
 * - XLNID_ERROR_SOFTWARE
 * This category is intended for errors due to software state preventing
 * something. The category is not intended for errors due to bad arguments, or
 * due to unsupported features. It should be used when a state occurs which
 * prevents action but is not a serious issue.
 *
 * - XLNID_ERROR_ARGUMENT
 * This category is for when a bad or invalid argument is passed. It should be
 * used whenever a function is called and error checking has detected the
 * argument is wrong or incorrect.
 *
 * - XLNID_ERROR_UNSUPPORTED
 * This category is for errors which are due to unsupported circumstances or
 * configuration issues. It should not be used when the issue is due to an
 * invalid argument, but for when something has occurred that is unsupported
 * (Ex: Flow control autonegotiation or an unsupported SFP+ module.)
 */

#include "xlnid_osdep.h"

/* Override this by setting IOMEM in your xlnid_osdep.h header */
#ifndef IOMEM
#define IOMEM
#endif

//#define XLNID_GE_SUPPORT          /* no phy */
//#define XLNID_GE_PHY_SUPPORT      /* phy */
//#define XLNID_XGE_SUPPORT
//#define XLNID_1000BASEX_SUPPORT
//#define XLNID_GEPHY_SUPPORT
//#define XLNID_RMII_MAC_SUPPORT


/* SKYLAKE registers */
#define SKYLAKE_TOP_BASE            0x00000000
#define SKYLAKE_CRG_BASE            0x00100000
#define SKYLAKE_PCIE_INTF_BASE      0x00200000
#define SKYLAKE_RX_CTL_BASE         0x00300000
#define SKYLAKE_TX_CTL_BASE         0x00400000
#define SKYLAKE_MAC_BASE            0x00500000
#define SKYLAKE_SPI_INTF_BASE       0x00600000
#define SKYLAKE_I2C_INTF_BASE       0x00700000
#define SKYLAKE_MDIO_MASTER_BASE    0x00800000
#define SKYLAKE_PHY_CTL_BASE        0x00900000
#define SKYLAKE_LINKSEC_BASE        0x00A00000
#define SKYLAKE_NCSI_BASE           0x00B00000
#define SKYLAKE_SMBUS_BASE          0x00C00000

/* TOP_MPI Registers */
#define SKYLAKE_TOP_VERSION         SKYLAKE_TOP_BASE + 0x00000
#define SKYLAKE_PTSW_MODE           SKYLAKE_TOP_BASE + 0x000c
#define SKYLAKE_TXBUFFER_MTI        SKYLAKE_TOP_BASE + 0x15c
#define SKYLAKE_LEDCTL		        SKYLAKE_TOP_BASE + 0x00600
#define SKYLAKE_TOP_MPI_LINK_MODE   SKYLAKE_TOP_BASE + 0x00700
#define SKYLAKE_PCS_LINK_STATUS     SKYLAKE_TOP_BASE + 0x00A90

/* CRG regeisters */
#define SKYLAKE_CRG_PLL_XGMAC_AUX_CFG   SKYLAKE_CRG_BASE + 0x0004
#define SKYLAKE_CRG_SYS_RST_N	        SKYLAKE_CRG_BASE + 0x0010
#define SKYLAKE_CRG_SDS_MEM_REQ         SKYLAKE_CRG_BASE + 0x001C
#define SKYLAKE_CRG_SDS_LANE_SEL        SKYLAKE_CRG_BASE + 0x0018
#define SKYLAKE_CRG_SDS_MEM_DATA	    SKYLAKE_CRG_BASE + 0x0020
#define SKYLAKE_CRG_SDS_MEM_ADDR	    SKYLAKE_CRG_BASE + 0x0024
#define SKYLAKE_CRG_SDS_IRST_POR_N      SKYLAKE_CRG_BASE + 0x0048
#define SKYLAKE_CRG_SDS_IPD_MULTI_SYNTH_N       SKYLAKE_CRG_BASE + 0x004C
#define SKYLAKE_CRG_SDS_IRST_MULTI_HARD_SYNTH_N SKYLAKE_CRG_BASE + 0x005C
#define SKYLAKE_CRG_LANE_IRST_PIPE_RST_N        SKYLAKE_CRG_BASE + 0x0064
#define SKYLAKE_CRG_LANE_IRST_MULTI_HARD_TXRX_N SKYLAKE_CRG_BASE + 0x0068
#define SKYLAKE_CRG_LANE_IPD_MULTI_TX_N         SKYLAKE_CRG_BASE + 0x006C
#define SKYLAKE_CRG_LANE_IPD_MULTI_RX_N         SKYLAKE_CRG_BASE + 0x0070
#define SKYLAKE_CRG_GMAC_LINK_RST_N	    SKYLAKE_CRG_BASE + 0x0204
#define SKYLAKE_CRG_XGMAC_LINK_RST_N	SKYLAKE_CRG_BASE + 0x0208
#define SKYLAKE_CRG_SGMII_LINK_RST_N	SKYLAKE_CRG_BASE + 0x020C

/* PCIE_INTF registers */
#define SKYLAKE_UL_RX_PKT_CNT           SKYLAKE_PCIE_INTF_BASE + 0x20000
#define SKYLAKE_ECO_RESET	            SKYLAKE_PCIE_INTF_BASE + 0x30000
#define SKYLAKE_RX_PD_FIFO_STATUS	    SKYLAKE_PCIE_INTF_BASE + 0x20154
#define SKYLAKE_PCIE_DMA_CHANNEL_BUSY	SKYLAKE_PCIE_INTF_BASE + 0x20140
#define SKYLAKE_PCIE_DWC_PLREG_CMD	    SKYLAKE_PCIE_INTF_BASE + 0x20200
#define SKYLAKE_PCIE_DWC_PLREG_WDATA	SKYLAKE_PCIE_INTF_BASE + 0x20204

/* Interrupt Registers */
#define SKYLAKE_EICR	        SKYLAKE_PCIE_INTF_BASE + 0x00800
#define SKYLAKE_EICS	        SKYLAKE_PCIE_INTF_BASE + 0x00808
#define SKYLAKE_EIMS	        SKYLAKE_PCIE_INTF_BASE + 0x00880
#define SKYLAKE_EIMC	        SKYLAKE_PCIE_INTF_BASE + 0x00888
#define SKYLAKE_EIAC	        SKYLAKE_PCIE_INTF_BASE + 0x00810
#define SKYLAKE_EIAM	        SKYLAKE_PCIE_INTF_BASE + 0x00890
#define SKYLAKE_EICR_EX(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x00A80 + (_i) * 4)
#define SKYLAKE_EICS_EX(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x00A90 + (_i) * 4)
#define SKYLAKE_EIMS_EX(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x00AA0 + (_i) * 4)
#define SKYLAKE_EIMC_EX(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x00AB0 + (_i) * 4)
#define SKYLAKE_EIAC_EX(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x00AC0 + (_i) * 4)
#define SKYLAKE_EIAM_EX(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x00AD0 + (_i) * 4)
#define SKYLAKE_EITR(_i)		(((_i) <= 23) ? (SKYLAKE_PCIE_INTF_BASE + 0x00820 + ((_i) * 4)) : \
				 (SKYLAKE_PCIE_INTF_BASE + 0x012300 + (((_i) - 24) * 4)))
#define SKYLAKE_IVAR(_i)		(SKYLAKE_PCIE_INTF_BASE + 0x00900 + ((_i) * 4)) /* 24 at 0x900-0x960 */
#define SKYLAKE_IVAR_MISC		SKYLAKE_PCIE_INTF_BASE + 0x00A00 /* misc MSI-X interrupt causes */
#define SKYLAKE_GPIE		    SKYLAKE_PCIE_INTF_BASE + 0x00898
#define SKYLAKE_TCPTIMER		SKYLAKE_PCIE_INTF_BASE + 0x0004C

/* Transmit DMA registers */
#define SKYLAKE_TDBAL(_i)		(SKYLAKE_PCIE_INTF_BASE + 0x06000 + ((_i) * 0x40)) /* 32 of them (0-31)*/
#define SKYLAKE_TDBAH(_i)		(SKYLAKE_PCIE_INTF_BASE + 0x06004 + ((_i) * 0x40))
#define SKYLAKE_TDLEN(_i)		(SKYLAKE_PCIE_INTF_BASE + 0x06008 + ((_i) * 0x40))
#define SKYLAKE_TDH(_i)		    (SKYLAKE_PCIE_INTF_BASE + 0x06010 + ((_i) * 0x40))
#define SKYLAKE_TDT(_i)		    (SKYLAKE_PCIE_INTF_BASE + 0x06018 + ((_i) * 0x40))
#define SKYLAKE_TXDCTL(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x06028 + ((_i) * 0x40))
#define SKYLAKE_TDWBAL(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x06038 + ((_i) * 0x40))
#define SKYLAKE_TDWBAH(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x0603C + ((_i) * 0x40))

/* Receive DMA Registers */
#define SKYLAKE_RDBAL(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x01000 + ((_i) * 0x40))
#define SKYLAKE_RDBAH(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x01004 + ((_i) * 0x40))
#define SKYLAKE_RDLEN(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x01008 + ((_i) * 0x40))
#define SKYLAKE_RDH(_i)         (SKYLAKE_PCIE_INTF_BASE + 0x01010 + ((_i) * 0x40))
#define SKYLAKE_SRRCTL(_i)      (SKYLAKE_PCIE_INTF_BASE + 0x01014 + ((_i) * 0x40))
#define SKYLAKE_RDT(_i)	        (SKYLAKE_PCIE_INTF_BASE + 0x01018 + ((_i) * 0x40))
#define SKYLAKE_RXDCTL(_i)	    (SKYLAKE_PCIE_INTF_BASE + 0x01028 + ((_i) * 0x40))
#define SKYLAKE_RDRXCTL		    (SKYLAKE_PCIE_INTF_BASE + 0x02F00)
#define SKYLAKE_RXCTRL		    (SKYLAKE_PCIE_INTF_BASE + 0x03000)

/* Receive Registers */
#define SKYLAKE_RXCSUM		    SKYLAKE_RX_CTL_BASE + 0x05000
#define SKYLAKE_MAXFRS		    SKYLAKE_RX_CTL_BASE + 0x04268
#define SKYLAKE_RXDCTL_VME      SKYLAKE_RX_CTL_BASE + 0x1028
#define SKYLAKE_RXBUF_MAP_SEL   SKYLAKE_RX_CTL_BASE + 0xF170
#define SKYLAKE_HLREG0		    SKYLAKE_RX_CTL_BASE + 0x04240
#define SKYLAKE_HLREG1		    SKYLAKE_RX_CTL_BASE + 0x04244

/* Multicast Table Array - 128 entries */
#define SKYLAKE_MTA(_i)		    (SKYLAKE_RX_CTL_BASE + 0x05200 + ((_i) * 4))
#define SKYLAKE_RAL(_i)		    (SKYLAKE_RX_CTL_BASE + 0x0A200 + ((_i) * 8))
#define SKYLAKE_RAH(_i)	        (SKYLAKE_RX_CTL_BASE + 0x0A204 + ((_i) * 8))

/* array of 4096 1-bit vlan filters */
#define SKYLAKE_VFTA(_i)		(SKYLAKE_RX_CTL_BASE + 0x0A000 + ((_i) * 4))
#define SKYLAKE_FCTRL		    SKYLAKE_RX_CTL_BASE + 0x05080
#define SKYLAKE_VLNCTRL		    SKYLAKE_RX_CTL_BASE +0x05088
#define SKYLAKE_MCSTCTRL		SKYLAKE_RX_CTL_BASE + 0x05090
#define SKYLAKE_MRQC		    SKYLAKE_RX_CTL_BASE + 0x0EC80
#define SKYLAKE_SAQF(_i)	    (SKYLAKE_RX_CTL_BASE + 0x0E000 + ((_i) * 4)) /* Source Address Queue Filter */
#define SKYLAKE_DAQF(_i)	    (SKYLAKE_RX_CTL_BASE + 0x0E200 + ((_i) * 4)) /* Dest. Address Queue Filter */
#define SKYLAKE_SDPQF(_i)	    (SKYLAKE_RX_CTL_BASE + 0x0E400 + ((_i) * 4)) /* Src Dest. Addr Queue Filter */
#define SKYLAKE_FTQF(_i)	    (SKYLAKE_RX_CTL_BASE + 0x0E600 + ((_i) * 4)) /* Five Tuple Queue Filter */
#define SKYLAKE_ETQF(_i)	    (SKYLAKE_RX_CTL_BASE + 0x05128 + ((_i) * 4)) /* EType Queue Filter */
#define SKYLAKE_ETQS(_i)	    (SKYLAKE_RX_CTL_BASE+ 0x0EC00 + ((_i) * 4)) /* EType Queue Select */
#define SKYLAKE_SYNQF	        SKYLAKE_RX_CTL_BASE + 0x0EC30 /* SYN Packet Queue Filter */
#define SKYLAKE_RQTC	        SKYLAKE_RX_CTL_BASE + 0x0EC70
#define SKYLAKE_MTQC	        SKYLAKE_RX_CTL_BASE + 0x08120

#define SKYLAKE_L34T_IMIR(_i)	(SKYLAKE_RX_CTL_BASE + 0x0E800 + ((_i) * 4)) /*128 of these (0-127)*/
#define SKYLAKE_RSSRK(_i)		(SKYLAKE_RX_CTL_BASE + 0x0EB80 + ((_i) * 4))  /* 10 of these (0-9) */
#define SKYLAKE_RETA(_i)		(SKYLAKE_RX_CTL_BASE + 0x0EB00 + ((_i) * 4))  /* 32 of these (0-31) */
#define SKYLAKE_IMIRVP		    SKYLAKE_RX_CTL_BASE + 0x0EC60
#define SKYLAKE_LLITHRESH		SKYLAKE_RX_CTL_BASE + 0x0EC90

/* Flow Director registers */
#define SKYLAKE_FDIRCTRL	    SKYLAKE_RX_CTL_BASE + 0x0EE00
#define SKYLAKE_FDIRHKEY	    SKYLAKE_RX_CTL_BASE + 0x0EE68
#define SKYLAKE_FDIRSKEY	    SKYLAKE_RX_CTL_BASE + 0x0EE6C
#define SKYLAKE_FDIRDIP4M	    SKYLAKE_RX_CTL_BASE + 0x0EE3C
#define SKYLAKE_FDIRSIP4M	    SKYLAKE_RX_CTL_BASE + 0x0EE40
#define SKYLAKE_FDIRTCPM	    SKYLAKE_RX_CTL_BASE + 0x0EE44
#define SKYLAKE_FDIRUDPM	    SKYLAKE_RX_CTL_BASE + 0x0EE48
#define SKYLAKE_FDIRSCTPM	    SKYLAKE_RX_CTL_BASE + 0x0EE78
#define SKYLAKE_FDIRIP6M	    SKYLAKE_RX_CTL_BASE + 0x0EE74
#define SKYLAKE_FDIRM	        SKYLAKE_RX_CTL_BASE + 0x0EE70

/* Flow Director Stats registers */
#define SKYLAKE_FDIRFREE	    SKYLAKE_RX_CTL_BASE + 0x0EE38
#define SKYLAKE_FDIRLEN	        SKYLAKE_RX_CTL_BASE + 0x0EE4C
#define SKYLAKE_FDIRUSTAT	    SKYLAKE_RX_CTL_BASE + 0x0EE50
#define SKYLAKE_FDIRFSTAT	    SKYLAKE_RX_CTL_BASE + 0x0EE54
#define SKYLAKE_FDIRMATCH	    SKYLAKE_RX_CTL_BASE + 0x0EE58
#define SKYLAKE_FDIRMISS	    SKYLAKE_RX_CTL_BASE + 0x0EE5C

/* Flow Director Programming registers */
#define SKYLAKE_FDIRSIPv6(_i)   (SKYLAKE_RX_CTL_BASE + 0x0EE0C + ((_i) * 4)) /* 3 of these (0-2) */
#define SKYLAKE_FDIRIPSA	    SKYLAKE_RX_CTL_BASE + 0x0EE18
#define SKYLAKE_FDIRIPDA	    SKYLAKE_RX_CTL_BASE + 0x0EE1C
#define SKYLAKE_FDIRPORT	    SKYLAKE_RX_CTL_BASE + 0x0EE20
#define SKYLAKE_FDIRVLAN	    SKYLAKE_RX_CTL_BASE + 0x0EE24
#define SKYLAKE_FDIRHASH	    SKYLAKE_RX_CTL_BASE + 0x0EE28
#define SKYLAKE_FDIRCMD	        SKYLAKE_RX_CTL_BASE + 0x0EE2C

/* Management */
#define SKYLAKE_MAVTV(_i)		(SKYLAKE_RX_CTL_BASE + 0x05010 + ((_i) * 4)) /* 8 of these (0-7) */
#define SKYLAKE_MFUTP(_i)		(SKYLAKE_RX_CTL_BASE + 0x05030 + ((_i) * 4)) /* 8 of these (0-7) */
#define SKYLAKE_MANC		    SKYLAKE_RX_CTL_BASE + 0x05820
#define SKYLAKE_MFVAL		    SKYLAKE_RX_CTL_BASE+0x05824
#define SKYLAKE_MANC2H		    SKYLAKE_RX_CTL_BASE + 0x05860
#define SKYLAKE_MDEF(_i)		(SKYLAKE_RX_CTL_BASE+ 0x05890 + ((_i) * 4)) /* 8 of these (0-7) */
#define SKYLAKE_MIPAF		    SKYLAKE_RX_CTL_BASE + 0x058B0
#define SKYLAKE_MMAL(_i)		(SKYLAKE_RX_CTL_BASE + 0x05910 + ((_i) * 8)) /* 4 of these (0-3) */
#define SKYLAKE_MMAH(_i)		(SKYLAKE_RX_CTL_BASE + 0x05914 + ((_i) * 8)) /* 4 of these (0-3) */
#define SKYLAKE_FTFT		    SKYLAKE_RX_CTL_BASE + 0x09400 /* 0x9400-0x97FC */
#define SKYLAKE_METF(_i)		(SKYLAKE_RX_CTL_BASE + 0x05190 + ((_i) * 4)) /* 4 of these (0-3) */
#define SKYLAKE_MDEF_EXT(_i)	(SKYLAKE_RX_CTL_BASE + 0x05160 + ((_i) * 4)) /* 8 of these (0-7) */

#define SKYLAKE_DMATXCTL		(SKYLAKE_TX_CTL_BASE + 0x04A80)

/* MAC regeisters */
#define XLNID_MAC_LAN_UNIT          0x10000
#define SKYLAKE_MCI_CMD_DATA        SKYLAKE_MAC_BASE + 0x0000
#define SKYLAKE_MCI_WDATA_I	        SKYLAKE_MAC_BASE + 0x0004
#define SKYLAKE_MCI_RDATA_O	        SKYLAKE_MAC_BASE + 0x0008
#define SKYLAKE_XGMAC_LOOPBACK_CTRL SKYLAKE_MAC_BASE + 0x0020

/* SPI_INTF registers */
#define SKYLAKE_EEC		        (SKYLAKE_SPI_INTF_BASE + 0x10010)
#define SKYLAKE_EERD		    (SKYLAKE_SPI_INTF_BASE + 0x10014)
#define SKYLAKE_EEWR		    (SKYLAKE_SPI_INTF_BASE + 0x10018)

/* I2C_INTF registers */
#define SKYLAKE_I2CCTL          (SKYLAKE_I2C_INTF_BASE + 0x00028)
#define SKYLAKE_I2CRD0          (SKYLAKE_I2C_INTF_BASE + 0x0002C)
#define SKYLAKE_I2CRD1          (SKYLAKE_I2C_INTF_BASE + 0x00038)
#define SKYLAKE_I2CWR0          (SKYLAKE_I2C_INTF_BASE + 0x00030)
#define SKYLAKE_I2CWR1          (SKYLAKE_I2C_INTF_BASE + 0x0003C)
#define SKYLAKE_I2CCONF         (SKYLAKE_I2C_INTF_BASE + 0x00034)

/* MDIO_MASTER registers */
#define SKYLAKE_MAS_TXBUS_DATA  (SKYLAKE_MDIO_MASTER_BASE + 0x00008)
#define SKYLAKE_MAS_TXBUS_CTL   (SKYLAKE_MDIO_MASTER_BASE + 0x0000C)
#define SKYLAKE_MAS_SEND_EN     (SKYLAKE_MDIO_MASTER_BASE + 0x00010)

/* PHY_CTL regeister */
#define SKYLAKE_PHY_PI_COFNIG   (SKYLAKE_PHY_CTL_BASE + 0x00)
#define SKYLAKE_SGM_MAC_COFNIG  (SKYLAKE_PHY_CTL_BASE + 0x10)
#define SKYLAKE_SGM_AN_COFNIG   (SKYLAKE_PHY_CTL_BASE + 0x14)

#define SKYLAKE_FLUSH_REG       SKYLAKE_TOP_VERSION


/* WESTLAKE registers */
#define WESTLAKE_CRG_BASE           0x00000000
#define WESTLAKE_ETH_PORT_BASE      0x00100000
#define WESTLAKE_MACSEC_BASE        0x00200000
#define WESTLAKE_RX_CTL_BASE        0x00300000
#define WESTLAKE_TX_CTL_BASE        0x00400000
#define WESTLAKE_SMBUS_BASE         0x00500000
#define WESTLAKE_DMA_CTL_BASE       0x04e8
#define WESTLAKE_ETH_COMMON_BASE    0x00700000
#define WESTLAKE_MEM_CTL_BASE       0x00800000
#define WESTLAKE_PCIE_CTL_BASE      0x00900000
#define WESTLAKE_LED_BASE           0x00A00000
#define WESTLAKE_INNER_PHY_BASE     0x00B00000
#define WESTLAKE_USB_BASE           0x00C00000
#define WESTLAKE_OUTER_PHY_BASE     0x00D00000
#define WESTLAKE_DEBUG_BASE         0x00E00000

/* CFG Registers */
#define WESTLAKE_INDIRECT_CMD_BUS   0x0030
#define WESTLAKE_INDIRECT_WDATA_BUS 0x0034
#define WESTLAKE_INDIRECT_RDATA_BUS 0x0038

#define WESTLAKE_EICR               0x0280
#define WESTLAKE_EICS               0x0284
#define WESTLAKE_EIMS               0x0288
#define WESTLAKE_EIMC               0x028C
#define WESTLAKE_EIAM               0x0290
#define WESTLAKE_EIAC               0x0294
#define WESTLAKE_EICR_EX(_i)        (0x0298 + ((_i) * 0x4))
#define WESTLAKE_EICS_EX(_i)        (0x02A0 + ((_i) * 0x4))
#define WESTLAKE_EIMS_EX(_i)        (0x02A8 + ((_i) * 0x4))
#define WESTLAKE_EIMC_EX(_i)        (0x02B0 + ((_i) * 0x4))
#define WESTLAKE_EIAM_EX(_i)        (0x02B8 + ((_i) * 0x4))
#define WESTLAKE_EITR(_i)           (0x02C0 + ((_i) * 0x4))
#define WESTLAKE_IVAR(_i)           (0x03C0 + ((_i) * 0x4))
#define WESTLAKE_IVAR_MISC          0x04C0
#define WESTLAKE_GPIE               0x04C4
#define WESTLAKE_TCPTIMER           0x04C8

#define WESTLAKE_FLUSH_REG          0x0004

/* GMAC Registers */
#define WESTLAKE_GMAC_CONFIG        0x0000

/* CRG Registers */
#define WESTLAKE_CFG_PTSW_MODE      (WESTLAKE_CRG_BASE + 0x0020)

/* ETH_PORT Registers */
#define WESTLAKE_MAC_WDATA          (WESTLAKE_ETH_PORT_BASE + 0x0018)
#define WESTLAKE_MAC_CMD            (WESTLAKE_ETH_PORT_BASE + 0x001C)
#define WESTLAKE_MAC_RDATA          (WESTLAKE_ETH_PORT_BASE + 0x0020)
#define WESTLAKE_CFG_TXBUF_RD_TH    (WESTLAKE_ETH_PORT_BASE + 0x0034)
#define WESTLAKE_CFG_AN_EDIT_EN     (WESTLAKE_ETH_PORT_BASE + 0x00C4)
#define WESTLAKE_SGM_PCS_CFG0       (WESTLAKE_ETH_PORT_BASE + 0x0400)
#define WESTLAKE_SGM_PCS_STAS0      (WESTLAKE_ETH_PORT_BASE + 0x0404)
#define WESTLAKE_SGM_PCS_CFG1       (WESTLAKE_ETH_PORT_BASE + 0x0408)
#define WESTLAKE_SDS_PMA_REF_CTRL   (WESTLAKE_ETH_PORT_BASE + 0x0494)
#define WESTLAKE_SDS_TXDEEMPH       (WESTLAKE_ETH_PORT_BASE + 0x04A4)
#define WESTLAKE_SDS_RX_EQ0         (WESTLAKE_ETH_PORT_BASE + 0x04A8)
#define WESTLAKE_SDS_RX_EQ1         (WESTLAKE_ETH_PORT_BASE + 0x04AC)
#define WESTLAKE_SDS_RX_EQ2         (WESTLAKE_ETH_PORT_BASE + 0x04B0)
#define WESTLAKE_GEPHY_LED_STATUS   (WESTLAKE_ETH_PORT_BASE + 0x0514)
#define WESTLAKE_CFG_SPEED          (WESTLAKE_ETH_PORT_BASE + 0x058C)
#define WESTLAKE_CFG_LINK_UP        (WESTLAKE_ETH_PORT_BASE + 0x0590)
#define WESTALEK_CFG_AN_EN          (WESTLAKE_ETH_PORT_BASE + 0x059C)
#define WESTLAKE_CFG_EXT_POLL       (WESTLAKE_ETH_PORT_BASE + 0x05A0)
#define WESTLAKE_CFG_INTF_CTRL      (WESTLAKE_ETH_PORT_BASE + 0x05A4)
#define WESTLAKE_CFG_LINK_MASK      (WESTLAKE_ETH_PORT_BASE + 0x05A8)




/* MACSEC Registers */
#define WESTLAKE_MACSEC_RX_SOP_CNT          (WESTLAKE_MACSEC_BASE + 0x0104)
#define WESTLAKE_MACSEC_ETH_LOOPBACK_CFG    (WESTLAKE_MACSEC_BASE + 0x0138)
#define WESTLAKE_MACSEC_RX_DEAL_INTERVAL    (WESTLAKE_MACSEC_BASE + 0x013C)

/* RX_CTL Registers */
#define WESTLAKE_FDIRCTRL           (WESTLAKE_RX_CTL_BASE + 0x0000)
#define WESTLAKE_FDIRHKEY           (WESTLAKE_RX_CTL_BASE + 0x0004)
#define WESTLAKE_FDIRDIP4M          (WESTLAKE_RX_CTL_BASE + 0x0008)
#define WESTLAKE_FDIRSIP4M          (WESTLAKE_RX_CTL_BASE + 0x000C)
#define WESTLAKE_FDIRTCPM           (WESTLAKE_RX_CTL_BASE + 0x0010)
#define WESTLAKE_FDIRUDPM           (WESTLAKE_RX_CTL_BASE + 0x0014)
#define WESTLAKE_FDIRIPV6M          (WESTLAKE_RX_CTL_BASE + 0x0018)
#define WESTLAKE_FDIRM              (WESTLAKE_RX_CTL_BASE + 0x001C)
#define WESTLAKE_FDIRFREE           (WESTLAKE_RX_CTL_BASE + 0x0020)
#define WESTLAKE_FDIRUSTAT          (WESTLAKE_RX_CTL_BASE + 0x0024)
#define WESTLAKE_FDIRFSTAT          (WESTLAKE_RX_CTL_BASE + 0x0028)
#define WESTLAKE_FDIRMATCH          (WESTLAKE_RX_CTL_BASE + 0x002C)
#define WESTLAKE_FDIRMISS           (WESTLAKE_RX_CTL_BASE + 0x0030)
#define WESTLAKE_FDISIPv6(_i)       (WESTLAKE_RX_CTL_BASE + 0x0034 + ((_i) * 0x4))
#define WESTLAKE_FDIRIPSA           (WESTLAKE_RX_CTL_BASE + 0x0040)
#define WESTLAKE_FDIRIPDA           (WESTLAKE_RX_CTL_BASE + 0x0044)
#define WESTLAKE_FDIRPORT           (WESTLAKE_RX_CTL_BASE + 0x0048)
#define WESTLAKE_FDIRVLAN           (WESTLAKE_RX_CTL_BASE + 0x004C)
#define WESTLAKE_FDIRHASH           (WESTLAKE_RX_CTL_BASE + 0x0050)
#define WESTLAKE_FDIRCMD            (WESTLAKE_RX_CTL_BASE + 0x0054)
#define WESTLAKE_FDIRIP6AT(_i)      (WESTLAKE_RX_CTL_BASE + 0x0058 + ((_i) * 0x4))
#define WESTLAKE_HLREG0             (WESTLAKE_RX_CTL_BASE + 0x0070)
#define WESTLAKE_FCTRL              (WESTLAKE_RX_CTL_BASE + 0x0074)
#define WESTLAKE_VLNCTRL            (WESTLAKE_RX_CTL_BASE + 0x0078)
#define WESTLAKE_MCSTCTRL           (WESTLAKE_RX_CTL_BASE + 0x007C)
#define WESTLAKE_RXCSUM             (WESTLAKE_RX_CTL_BASE + 0x0080)
#define WESTLAKE_MRQC               (WESTLAKE_RX_CTL_BASE + 0x0084)
#define WESTLAKE_SYNQF              (WESTLAKE_RX_CTL_BASE + 0x0088)
#define WESTLAKE_RSSRK(_i)          (WESTLAKE_RX_CTL_BASE + 0x0090 + ((_i) * 0x4))
#define WESTLAKE_ETQF(_i)           (WESTLAKE_RX_CTL_BASE + 0x00C0 + ((_i) * 0x4))
#define WESTLAKE_ETQS(_i)           (WESTLAKE_RX_CTL_BASE + 0x00E0 + ((_i) * 0x4))
#define WESTLAKE_LLITHRESH          (WESTLAKE_RX_CTL_BASE + 0x0100)
#define WESTLAKE_IMIRVP             (WESTLAKE_RX_CTL_BASE + 0x0104)
#define WESTLAKE_MAXFRS             (WESTLAKE_RX_CTL_BASE + 0x0108)
#define WESTLAKE_RXDCTL_VME         (WESTLAKE_RX_CTL_BASE + 0x010C)
#define WESTLAKE_MANC               (WESTLAKE_RX_CTL_BASE + 0x0110)
#define WESTLAKE_MFVAL              (WESTLAKE_RX_CTL_BASE + 0x0114)
#define WESTLAKE_MANC2H             (WESTLAKE_RX_CTL_BASE + 0x0118)
#define WESTLAKE_RXCTRL             (WESTLAKE_RX_CTL_BASE + 0x011C)
#define WESTLAKE_MTA(_i)            (WESTLAKE_RX_CTL_BASE + 0x0200 + ((_i) * 0x4))
#define WESTLAKE_VFTA(_i)           (WESTLAKE_RX_CTL_BASE + 0x0400 + ((_i) * 0x4))
#define WESTLAKE_RETA(_i)           (WESTLAKE_RX_CTL_BASE + 0x0600 + ((_i) * 0x4))
#define WESTLAKE_RAL(_i)            (WESTLAKE_RX_CTL_BASE + 0x0800 + ((_i) * 0x8))
#define WESTLAKE_RAH(_i)            (WESTLAKE_RX_CTL_BASE + 0x0804 + ((_i) * 0x8))
#define WESTLAKE_SAQF(_i)           (WESTLAKE_RX_CTL_BASE + 0x0C00 + ((_i) * 0x4))
#define WESTLAKE_DAQF(_i)           (WESTLAKE_RX_CTL_BASE + 0x0E00 + ((_i) * 0x4))
#define WESTLAKE_SDPQF(_i)          (WESTLAKE_RX_CTL_BASE + 0x1000 + ((_i) * 0x4))
#define WESTLAKE_FTQF(_i)           (WESTLAKE_RX_CTL_BASE + 0x1200 + ((_i) * 0x4))
#define WESTLAKE_L34T_IMIR(_i)      (WESTLAKE_RX_CTL_BASE + 0x1400 + ((_i) * 0x4))
#define WESTLAKE_MAVTV(_i)          (WESTLAKE_RX_CTL_BASE + 0x1600 + ((_i) * 0x4))
#define WESTLAKE_MFUTP(_i)          (WESTLAKE_RX_CTL_BASE + 0x1620 + ((_i) * 0x4))
#define WESTLAKE_METF(_i)           (WESTLAKE_RX_CTL_BASE + 0x1640 + ((_i) * 0x4))
#define WESTLAKE_MDEF(_i)           (WESTLAKE_RX_CTL_BASE + 0x1650 + ((_i) * 0x4))
#define WESTLAKE_MDEF_EXT(_i)       (WESTLAKE_RX_CTL_BASE + 0x1670 + ((_i) * 0x4))
#define WESTLAKE_RX_PBUF_STAT       (WESTLAKE_RX_CTL_BASE + 0x3024)


/* TX_CTL Registers */
#define WESTLAKE_DMATXCTL           (WESTLAKE_TX_CTL_BASE + 0x0000)

/* SMBUS Registers */

/* DMA_CTL Registers */
#define WESTLAKE_RDBAL(_i)          (WESTLAKE_DMA_CTL_BASE + 0x0120 + ((_i) * 0x40))
#define WESTLAKE_RDBAH(_i)          (WESTLAKE_DMA_CTL_BASE + 0x0124 + ((_i) * 0x40))
#define WESTLAKE_RDLEN(_i)          (WESTLAKE_DMA_CTL_BASE + 0x0128 + ((_i) * 0x40))
#define WESTLAKE_RDH(_i)            (WESTLAKE_DMA_CTL_BASE + 0x012C + ((_i) * 0x40))
#define WESTLAKE_RDT(_i)            (WESTLAKE_DMA_CTL_BASE + 0x0130 + ((_i) * 0x40))
#define WESTLAKE_RXDCTL(_i)         (WESTLAKE_DMA_CTL_BASE + 0x0134 + ((_i) * 0x40))
#define WESTLAKE_SRRCTL(_i)         (WESTLAKE_DMA_CTL_BASE + 0x0138 + ((_i) * 0x40))
#define WESTLAKE_TDBAL(_i)          (WESTLAKE_DMA_CTL_BASE + 0x013C + ((_i) * 0x40))
#define WESTLAKE_TDBAH(_i)          (WESTLAKE_DMA_CTL_BASE + 0x0140 + ((_i) * 0x40))
#define WESTLAKE_TDLEN(_i)          (WESTLAKE_DMA_CTL_BASE + 0x0144 + ((_i) * 0x40))
#define WESTLAKE_TDH(_i)            (WESTLAKE_DMA_CTL_BASE + 0x0148 + ((_i) * 0x40))
#define WESTLAKE_TDT(_i)            (WESTLAKE_DMA_CTL_BASE + 0x014C + ((_i) * 0x40))
#define WESTLAKE_TXDCTL(_i)         (WESTLAKE_DMA_CTL_BASE + 0x0150 + ((_i) * 0x40))

/* LED Registers */
#define WESTLAKE_LEDCTL             (WESTLAKE_LED_BASE + 0x0000)
#define WESTLAKE_LEDCTL_1           (WESTLAKE_LED_BASE + 0x0004)
#define WESTLAKE_LED_BLINK_TIMERL   (WESTLAKE_LED_BASE + 0x0018)
#define WESTLAKE_LED_BLINK_TIMERH   (WESTLAKE_LED_BASE + 0x001C)
#define WESTLAKE_LEDMODE            (WESTLAKE_LED_BASE + 0x0020)
#define WESTLAKE_LEDCMD             (WESTLAKE_CRG_BASE + 0x0040)

/* INNER_PHY/OUTER_PHY Registers */
#define WESTLAKE_MDIO_WDATA         0x0008
#define WESTLAKE_MDIO_CTL           0x000C
#define WESTLAKE_MDIO_EN            0x0010
#define WESTLAKE_MDIO_RDATA         0X0030

/* DEBUG Registers */
#define WESTLAKE_DEBUG_PHYSTATUS    (WESTLAKE_DEBUG_BASE + 0x001C)

#define IP_PROTOCOL_TCP 0x6
#define IP_PROTOCOL_UDP 0x11

#define XLNID_RXDCTL_VME_VME	0x40000000 /* VLAN mode enable */


#define XLNID_TXBUFFER_VALUE    0x3F

#define XLNID_UDP_HDR_LEN       8
#define XLNID_IPV6_HDR_LEN      40

#define XLNID_MAXFRS_SKYLAKE    0x3E000000
#define XLNID_MAXFRS_DEFAULT    0x3FFF0000

/* Vendor ID */
#define XLNID_VENDOR_ID_XEL			    0xffe1

/* Device IDs */
#define XLNID_DEV_ID_SKYLAKE            0xdd10
#define XLNID_DEV_ID_WESTLAKE           0xd200


/* INDIRECT_CMD_BUS */
#define XLNID_CMD_BUS_WEN               0x80000000
#define XLNID_CMD_EEPROM_DONE           0x20000000
#define XLNID_CMD_BUS_DONE              0x10000000
#define XLNID_CMD_BUS_BUSY              XLNID_CMD_BUS_DONE
#define XLNID_CMD_BUS_ADDR_MASK         0x01FFFFFF

#define XLNID_CMD_WAIT_MAX              10
#define XLNID_CMD_TRYS_MAX              100

#define XLNID_CAT(r,m) XLNID_##r##m

/* mac_cmd register */
#define XLNID_MAC_CMD_RD                0x00000000
#define XLNID_MAC_CMD_WR                0x80000000
#define XLNID_MAC_RW_ADDR_MASK          0x00003FFF

/* MAC Configuration Register */
#define XLNID_GMAC_CONFIG_RE            0x00000004

/* sgm_pcs_stas0 */
#define XLNID_SGM_PCS_LINK_UP           0x00000040

/* RX_PBUF_STAT Register */
#define XLNID_RXCTL_PBUF_EMPTY          0x00000010

/* cfg_ptsw_mode register */
#define XLNID_CRG_RX_PTSW_PORT          0x00000004
#define XLNID_CRG_RX_PTSW_SOFTWARE      0x00000002
#define XLNID_CRG_RX_PTSW_ENABLE        0x00000001
#define XLNID_CRG_RX_PTSW_SOFTEN        (XLNID_CRG_RX_PTSW_SOFTWARE | XLNID_CRG_RX_PTSW_ENABLE)
#define XLNID_CRG_TX_PTSW_ENABLE        0x00000008 


/* gephy_led_status register */
#define XLNID_PO_FIBER                  0x00000080
#define XLNID_PO_LINK                   0x00000008

/* NVM Registers */
#define XLNID_FLA		0x1001C
#define XLNID_EEMNGCTL	0x10110
#define XLNID_EEMNGDATA	0x10114
#define XLNID_FLMNGCTL	0x10118
#define XLNID_FLMNGDATA	0x1011C
#define XLNID_FLMNGCNT	0x10120
#define XLNID_FLOP	0x1013C
#define XLNID_GRC		0x10200
#define XLNID_SRAMREL		0x10210
#define XLNID_PHYDBG	0x10218

/* General Receive Control */
#define XLNID_GRC_MNG	0x00000001 /* Manageability Enable */
#define XLNID_GRC_APME	0x00000002 /* APM enabled in EEPROM */

#define XLNID_VPDDIAG0	0x10204
#define XLNID_VPDDIAG1	0x10208

/* I2CCTL Bit Masks */
#define XLNID_I2C_CLK_IN		0x00000001
#define XLNID_I2C_CLK_OUT		0x00000002
#define XLNID_I2C_DATA_IN		0x00000004
#define XLNID_I2C_DATA_OUT		0x00000008
#define XLNID_I2C_DATA_OE_N_EN		0
#define XLNID_I2C_BB_EN			0
#define XLNID_I2C_CLK_OE_N_EN		0
#define XLNID_I2C_CLOCK_STRETCHING_TIMEOUT	500

#define XLNID_I2C_THERMAL_SENSOR_ADDR	0xF8
#define XLNID_EMC_INTERNAL_DATA		0x00
#define XLNID_EMC_INTERNAL_THERM_LIMIT	0x20
#define XLNID_EMC_DIODE1_DATA		0x01
#define XLNID_EMC_DIODE1_THERM_LIMIT	0x19
#define XLNID_EMC_DIODE2_DATA		0x23
#define XLNID_EMC_DIODE2_THERM_LIMIT	0x1A

#define XLNID_MAX_SENSORS		3

struct xlnid_thermal_diode_data {
	u8 location;
	u8 temp;
	u8 caution_thresh;
	u8 max_op_thresh;
};

struct xlnid_thermal_sensor_data {
	struct xlnid_thermal_diode_data sensor[XLNID_MAX_SENSORS];
};

#define NVM_OROM_OFFSET		0x17
#define NVM_OROM_BLK_LOW	0x83
#define NVM_OROM_BLK_HI		0x84
#define NVM_OROM_PATCH_MASK	0xFF
#define NVM_OROM_SHIFT		8

#define NVM_VER_MASK		0x00FF /* version mask */
#define NVM_VER_SHIFT		8     /* version bit shift */
#define NVM_OEM_PROD_VER_PTR	0x1B  /* OEM Product version block pointer */
#define NVM_OEM_PROD_VER_CAP_OFF 0x1  /* OEM Product version format offset */
#define NVM_OEM_PROD_VER_OFF_L	0x2   /* OEM Product version offset low */
#define NVM_OEM_PROD_VER_OFF_H	0x3   /* OEM Product version offset high */
#define NVM_OEM_PROD_VER_CAP_MASK 0xF /* OEM Product version cap mask */
#define NVM_OEM_PROD_VER_MOD_LEN 0x3  /* OEM Product version module length */
#define NVM_ETK_OFF_LOW		0x2D  /* version low order word */
#define NVM_ETK_OFF_HI		0x2E  /* version high order word */
#define NVM_ETK_SHIFT		16    /* high version word shift */
#define NVM_VER_INVALID		0xFFFF
#define NVM_ETK_VALID		0x8000
#define NVM_INVALID_PTR		0xFFFF
#define NVM_VER_SIZE		32    /* version sting size */

struct xlnid_nvm_version {
	u32 etk_id;
	u8  nvm_major;
	u16 nvm_minor;
	u8  nvm_id;

	bool oem_valid;
	u8   oem_major;
	u8   oem_minor;
	u16  oem_release;

	bool or_valid;
	u8  or_major;
	u16 or_build;
	u8  or_patch;

};

/* EITR is only 12 bits, with the lower 3 always zero */
#define XLNID_MAX_INT_RATE	488281
#define XLNID_MIN_INT_RATE	956
#define XLNID_MAX_EITR		0x00000FF8
#define XLNID_MIN_EITR		8
#define XLNID_EITR_ITR_INT_MASK	0x00000FF8
#define XLNID_EITR_LLI_MOD	0x00008000
#define XLNID_EITR_CNT_WDIS	0x80000000
#define XLNID_EITRSEL		0x00894
#define XLNID_MSIXT		0x00000 /* MSI-X Table. 0x0000 - 0x01C */
#define XLNID_MSIXPBA		0x02000 /* MSI-X Pending bit array */
#define XLNID_PBACL(_i)	(((_i) == 0) ? (0x11068) : (0x110C0 + ((_i) * 4)))

/*
 * Split and Replication Receive Control Registers
 * 00-15 : 0x02100 + n*4
 * 16-64 : 0x01014 + n*0x40
 * 64-127: 0x0D014 + (n-64)*0x40
 */


/*
 * Rx DCA Control Register:
 * 00-15 : 0x02200 + n*4
 * 16-64 : 0x0100C + n*0x40
 * 64-127: 0x0D00C + (n-64)*0x40
 */
#define XLNID_DCA_RXCTRL(_i)	(((_i) <= 15) ? (0x02200 + ((_i) * 4)) : \
				 (((_i) < 64) ? (0x0100C + ((_i) * 0x40)) : \
				 (0x0D00C + (((_i) - 64) * 0x40))))
				 


/* 8 of these 0x03C00 - 0x03C1C */
#define XLNID_RXPBSIZE(_i)	(0x03C00 + ((_i) * 4))
#define XLNID_DROPEN		0x03D04
#define XLNID_RXPBSIZE_SHIFT	10
#define XLNID_RXPBSIZE_MASK	0x000FFC00


#define XLNID_RFCTL		0x05008
#define XLNID_DRECCCTL		0x02F08
#define XLNID_DRECCCTL_DISABLE	0
#define XLNID_DRECCCTL2		0x02F8C


#define XLNID_MPSAR_LO(_i)	(0x0A600 + ((_i) * 8))
#define XLNID_MPSAR_HI(_i)	(0x0A604 + ((_i) * 8))
/* Packet split receive type */
#define XLNID_PSRTYPE(_i)	(((_i) <= 15) ? (0x05480 + ((_i) * 4)) : \
				 (0x0EA00 + ((_i) * 4)))


/* 64 Mailboxes, 16 DW each */
#define XLNID_PFMBMEM(_i)	(0x13000 + (64 * (_i)))
#define XLNID_PFMBICR(_i)	(0x00710 + (4 * (_i))) /* 4 total */
#define XLNID_PFMBIMR(_i)	(0x00720 + (4 * (_i))) /* 4 total */
#define XLNID_VFRE(_i)		(0x051E0 + ((_i) * 4))
#define XLNID_VFTE(_i)		(0x08110 + ((_i) * 4))
#define XLNID_VMECM(_i)		(0x08790 + ((_i) * 4))
#define XLNID_QDE		0x2F04
#define XLNID_VMTXSW(_i)	(0x05180 + ((_i) * 4)) /* 2 total */
#define XLNID_VMOLR(_i)		(0x0F000 + ((_i) * 4)) /* 64 total */
#define XLNID_UTA(_i)		(0x0F400 + ((_i) * 4))
#define XLNID_MRCTL(_i)		(0x0F600 + ((_i) * 4))
#define XLNID_VMRVLAN(_i)	(0x0F610 + ((_i) * 4))
#define XLNID_VMRVM(_i)		(0x0F630 + ((_i) * 4))
#define XLNID_LVMMC_RX		0x2FA8
#define XLNID_LVMMC_TX		0x8108
#define XLNID_LMVM_RX		0x2FA4
#define XLNID_LMVM_TX		0x8124
#define XLNID_WQBR_RX(_i)	(0x2FB0 + ((_i) * 4)) /* 4 total */
#define XLNID_WQBR_TX(_i)	(0x8130 + ((_i) * 4)) /* 4 total */

#define XLNID_RXFECCERR0	0x051B8

#define XLNID_IMIR(_i)		(0x05A80 + ((_i) * 4))  /* 8 of these (0-7) */
#define XLNID_IMIREXT(_i)	(0x05AA0 + ((_i) * 4))  /* 8 of these (0-7) */
//#define XLNID_IMIRVP		0x05AC0

#define XLNID_VMD_CTL		0x0581C
//#define XLNID_RETA(_i)		(0x05C00 + ((_i) * 4))  /* 32 of these (0-31) */

#define XLNID_ERETA(_i)		(0x0EE80 + ((_i) * 4))  /* 96 of these (0-95) */
//#define XLNID_RSSRK(_i)		(0x05C80 + ((_i) * 4))  /* 10 of these (0-9) */


/* Registers for setting up RSS with SRIOV
 * _p - pool number (0..63)
 * _i - index (0..10 for PFVFRSSRK, 0..15 for PFVFRETA)
 */
#define XLNID_PFVFMRQC(_p)	(0x03400 + ((_p) * 4))
#define XLNID_PFVFRSSRK(_i, _p)	(0x018000 + ((_i) * 4) + ((_p) * 0x40))
#define XLNID_PFVFRETA(_i, _p)	(0x019000 + ((_i) * 4) + ((_p) * 0x40))




#define XLNID_DTXCTL		0x07E00


#define XLNID_PFVFSPOOF(_i)	(0x08200 + ((_i) * 4)) /* 8 of these 0 - 7 */
#define XLNID_DTXMXSZRQ		0x08100
#define XLNID_DTXTCPFLGL	0x04A88
#define XLNID_DTXTCPFLGH	0x04A8C
#define XLNID_LBDRPEN		0x0CA00
#define XLNID_TXPBTHRESH(_i)	(0x04950 + ((_i) * 4)) /* 8 of these 0 - 7 */

#define XLNID_DMATXCTL_TE	0x1 /* Transmit Enable */
#define XLNID_DMATXCTL_NS	0x2 /* No Snoop LSO hdr buffer */
#define XLNID_DMATXCTL_GDV	0x8 /* Global Double VLAN */
#define XLNID_DMATXCTL_MDP_EN	0x20 /* Bit 5 */
#define XLNID_DMATXCTL_MBINTEN	0x40 /* Bit 6 */
#define XLNID_DMATXCTL_VT_SHIFT	16  /* VLAN EtherType */

#define XLNID_PFDTXGSWC_VT_LBEN	0x1 /* Local L2 VT switch enable */

/* Wake up registers */
#define XLNID_WUC	0x05800
#define XLNID_WUFC	0x05808
#define XLNID_WUS	0x05810
#define XLNID_IPAV	0x05838
#define XLNID_IP4AT	0x05840 /* IPv4 table 0x5840-0x5858 */
#define XLNID_IP6AT	XLNID_RX_CTL_BASE + 0x05880 /* IPv6 table 0x5880-0x588F */

#define XLNID_WUPL	0x05900
#define XLNID_WUPM	0x05A00 /* wake up pkt memory 0x5A00-0x5A7C */
#define XLNID_PROXYS	0x05F60 /* Proxying Status Register */
#define XLNID_PROXYFC	0x05F64 /* Proxying Filter Control Register */
#define XLNID_VXLANCTRL	0x0000507C /* Rx filter VXLAN UDPPORT Register */

/* masks for accessing VXLAN and GENEVE UDP ports */
#define XLNID_VXLANCTRL_VXLAN_UDPPORT_MASK	0x0000ffff /* VXLAN port */
#define XLNID_VXLANCTRL_GENEVE_UDPPORT_MASK	0xffff0000 /* GENEVE port */
#define XLNID_VXLANCTRL_ALL_UDPPORT_MASK	0xffffffff /* GENEVE/VXLAN */
#define XLNID_VXLANCTRL_GENEVE_UDPPORT_SHIFT	16

/* Four Flexible Filters are supported */
#define XLNID_FLEXIBLE_FILTER_COUNT_MAX		4
/* Six Flexible Filters are supported */
#define XLNID_FLEXIBLE_FILTER_COUNT_MAX_6	6
/* Eight Flexible Filters are supported */
#define XLNID_FLEXIBLE_FILTER_COUNT_MAX_8	8
#define XLNID_EXT_FLEXIBLE_FILTER_COUNT_MAX	2


/* Each Flexible Filter is at most 128 (0x80) bytes in length */
#define XLNID_FLEXIBLE_FILTER_SIZE_MAX		128
#define XLNID_FHFT_LENGTH_OFFSET		0xFC  /* Length byte in FHFT */
#define XLNID_FHFT_LENGTH_MASK			0x0FF /* Length in lower byte */

/* Definitions for power management and wakeup registers */
/* Wake Up Control */
#define XLNID_WUC_PME_EN	0x00000002 /* PME Enable */
#define XLNID_WUC_PME_STATUS	0x00000004 /* PME Status */
#define XLNID_WUC_WKEN		0x00000010 /* Enable PE_WAKE_N pin assertion  */

/* Wake Up Filter Control */
#define XLNID_WUFC_LNKC	0x00000001 /* Link Status Change Wakeup Enable */
#define XLNID_WUFC_MAG	0x00000002 /* Magic Packet Wakeup Enable */
#define XLNID_WUFC_EX	0x00000004 /* Directed Exact Wakeup Enable */
#define XLNID_WUFC_MC	0x00000008 /* Directed Multicast Wakeup Enable */
#define XLNID_WUFC_BC	0x00000010 /* Broadcast Wakeup Enable */
#define XLNID_WUFC_ARP	0x00000020 /* ARP Request Packet Wakeup Enable */
#define XLNID_WUFC_IPV4	0x00000040 /* Directed IPv4 Packet Wakeup Enable */
#define XLNID_WUFC_IPV6	0x00000080 /* Directed IPv6 Packet Wakeup Enable */
#define XLNID_WUFC_MNG	0x00000100 /* Directed Mgmt Packet Wakeup Enable */

#define XLNID_WUFC_IGNORE_TCO	0x00008000 /* Ignore WakeOn TCO packets */
#define XLNID_WUFC_FLX0	0x00010000 /* Flexible Filter 0 Enable */
#define XLNID_WUFC_FLX1	0x00020000 /* Flexible Filter 1 Enable */
#define XLNID_WUFC_FLX2	0x00040000 /* Flexible Filter 2 Enable */
#define XLNID_WUFC_FLX3	0x00080000 /* Flexible Filter 3 Enable */
#define XLNID_WUFC_FLX4	0x00100000 /* Flexible Filter 4 Enable */
#define XLNID_WUFC_FLX5	0x00200000 /* Flexible Filter 5 Enable */
#define XLNID_WUFC_FLX_FILTERS		0x000F0000 /* Mask for 4 flex filters */
#define XLNID_WUFC_FLX_FILTERS_6	0x003F0000 /* Mask for 6 flex filters */
#define XLNID_WUFC_FLX_FILTERS_8	0x00FF0000 /* Mask for 8 flex filters */
#define XLNID_WUFC_FW_RST_WK	0x80000000 /* Ena wake on FW reset assertion */
/* Mask for Ext. flex filters */
#define XLNID_WUFC_EXT_FLX_FILTERS	0x00300000
#define XLNID_WUFC_ALL_FILTERS		0x000F00FF /* Mask all 4 flex filters */
#define XLNID_WUFC_ALL_FILTERS_6	0x003F00FF /* Mask all 6 flex filters */
#define XLNID_WUFC_ALL_FILTERS_8	0x00FF00FF /* Mask all 8 flex filters */
#define XLNID_WUFC_FLX_OFFSET	16 /* Offset to the Flexible Filters bits */

/* Wake Up Status */
#define XLNID_WUS_LNKC		XLNID_WUFC_LNKC
#define XLNID_WUS_MAG		XLNID_WUFC_MAG
#define XLNID_WUS_EX		XLNID_WUFC_EX
#define XLNID_WUS_MC		XLNID_WUFC_MC
#define XLNID_WUS_BC		XLNID_WUFC_BC
#define XLNID_WUS_ARP		XLNID_WUFC_ARP
#define XLNID_WUS_IPV4		XLNID_WUFC_IPV4
#define XLNID_WUS_IPV6		XLNID_WUFC_IPV6
#define XLNID_WUS_MNG		XLNID_WUFC_MNG
#define XLNID_WUS_FLX0		XLNID_WUFC_FLX0
#define XLNID_WUS_FLX1		XLNID_WUFC_FLX1
#define XLNID_WUS_FLX2		XLNID_WUFC_FLX2
#define XLNID_WUS_FLX3		XLNID_WUFC_FLX3
#define XLNID_WUS_FLX4		XLNID_WUFC_FLX4
#define XLNID_WUS_FLX5		XLNID_WUFC_FLX5
#define XLNID_WUS_FLX_FILTERS	XLNID_WUFC_FLX_FILTERS
#define XLNID_WUS_FW_RST_WK	XLNID_WUFC_FW_RST_WK
/* Proxy Status */
#define XLNID_PROXYS_EX		0x00000004 /* Exact packet received */
#define XLNID_PROXYS_ARP_DIR	0x00000020 /* ARP w/filter match received */
#define XLNID_PROXYS_NS		0x00000200 /* IPV6 NS received */
#define XLNID_PROXYS_NS_DIR	0x00000400 /* IPV6 NS w/DA match received */
#define XLNID_PROXYS_ARP	0x00000800 /* ARP request packet received */
#define XLNID_PROXYS_MLD	0x00001000 /* IPv6 MLD packet received */

/* Proxying Filter Control */
#define XLNID_PROXYFC_ENABLE	0x00000001 /* Port Proxying Enable */
#define XLNID_PROXYFC_EX	0x00000004 /* Directed Exact Proxy Enable */
#define XLNID_PROXYFC_ARP_DIR	0x00000020 /* Directed ARP Proxy Enable */
#define XLNID_PROXYFC_NS	0x00000200 /* IPv6 Neighbor Solicitation */
#define XLNID_PROXYFC_ARP	0x00000800 /* ARP Request Proxy Enable */
#define XLNID_PROXYFC_MLD	0x00000800 /* IPv6 MLD Proxy Enable */
#define XLNID_PROXYFC_NO_TCO	0x00008000 /* Ignore TCO packets */

#define XLNID_WUPL_LENGTH_MASK	0xFFFF

/* DCB registers */
#define XLNID_DCB_MAX_TRAFFIC_CLASS	8
#define XLNID_RMCS		0x03D00
#define XLNID_DPMCS		0x07F40
#define XLNID_PDPMCS		0x0CD00
#define XLNID_RUPPBMR		0x050A0
#define XLNID_RT2CR(_i)		(0x03C20 + ((_i) * 4)) /* 8 of these (0-7) */
#define XLNID_RT2SR(_i)		(0x03C40 + ((_i) * 4)) /* 8 of these (0-7) */
#define XLNID_TDTQ2TCCR(_i)	(0x0602C + ((_i) * 0x40)) /* 8 of these (0-7) */
#define XLNID_TDTQ2TCSR(_i)	(0x0622C + ((_i) * 0x40)) /* 8 of these (0-7) */
#define XLNID_TDPT2TCCR(_i)	(0x0CD20 + ((_i) * 4)) /* 8 of these (0-7) */
#define XLNID_TDPT2TCSR(_i)	(0x0CD40 + ((_i) * 4)) /* 8 of these (0-7) */

/* Power Management */
/* DMA Coalescing configuration */
struct xlnid_dmac_config {
	u16	watchdog_timer; /* usec units */
	bool	fcoe_en;
	u32	link_speed;
	u8	fcoe_tc;
	u8	num_tcs;
};

/*
 * DMA Coalescing threshold Rx PB TC[n] value in Kilobyte by link speed.
 * DMACRXT = 10Gbps = 10,000 bits / usec = 1250 bytes / usec 70 * 1250 ==
 * 87500 bytes [85KB]
 */
#define XLNID_DMACRXT_10G		0x55
#define XLNID_DMACRXT_1G		0x09
#define XLNID_DMACRXT_100M		0x01

/* DMA Coalescing registers */
#define XLNID_DMCMNGTH			0x15F20 /* Management Threshold */
#define XLNID_DMACR			0x02400 /* Control register */
#define XLNID_DMCTH(_i)			(0x03300 + ((_i) * 4)) /* 8 of these */
#define XLNID_DMCTLX			0x02404 /* Time to Lx request */
/* DMA Coalescing register fields */
#define XLNID_DMCMNGTH_DMCMNGTH_MASK	0x000FFFF0 /* Mng Threshold mask */
#define XLNID_DMCMNGTH_DMCMNGTH_SHIFT	4 /* Management Threshold shift */
#define XLNID_DMACR_DMACWT_MASK		0x0000FFFF /* Watchdog Timer mask */
#define XLNID_DMACR_HIGH_PRI_TC_MASK	0x00FF0000
#define XLNID_DMACR_HIGH_PRI_TC_SHIFT	16
#define XLNID_DMACR_EN_MNG_IND		0x10000000 /* Enable Mng Indications */
#define XLNID_DMACR_LX_COAL_IND		0x40000000 /* Lx Coalescing indicate */
#define XLNID_DMACR_DMAC_EN		0x80000000 /* DMA Coalescing Enable */
#define XLNID_DMCTH_DMACRXT_MASK	0x000001FF /* Receive Threshold mask */
#define XLNID_DMCTLX_TTLX_MASK		0x00000FFF /* Time to Lx request mask */

/* EEE registers */
#define XLNID_EEER			0x043A0 /* EEE register */
#define XLNID_EEE_STAT			0x04398 /* EEE Status */
#define XLNID_EEE_SU			0x04380 /* EEE Set up */
#define XLNID_EEE_SU_TEEE_DLY_SHIFT	26
#define XLNID_TLPIC			0x041F4 /* EEE Tx LPI count */
#define XLNID_RLPIC			0x041F8 /* EEE Rx LPI count */

/* EEE register fields */
#define XLNID_EEER_TX_LPI_EN		0x00010000 /* Enable EEE LPI TX path */
#define XLNID_EEER_RX_LPI_EN		0x00020000 /* Enable EEE LPI RX path */
#define XLNID_EEE_STAT_NEG		0x20000000 /* EEE support neg on link */
#define XLNID_EEE_RX_LPI_STATUS		0x40000000 /* RX Link in LPI status */
#define XLNID_EEE_TX_LPI_STATUS		0x80000000 /* TX Link in LPI status */

/* LinkSec (MacSec) Registers */
#define XLNID_LSECTXCAP		0x08A00
#define XLNID_LSECRXCAP		0x08F00
#define XLNID_LSECTXCTRL	0x08A04
#define XLNID_LSECTXSCL		0x08A08 /* SCI Low */
#define XLNID_LSECTXSCH		0x08A0C /* SCI High */
#define XLNID_LSECTXSA		0x08A10
#define XLNID_LSECTXPN0		0x08A14
#define XLNID_LSECTXPN1		0x08A18
#define XLNID_LSECTXKEY0(_n)	(0x08A1C + (4 * (_n))) /* 4 of these (0-3) */
#define XLNID_LSECTXKEY1(_n)	(0x08A2C + (4 * (_n))) /* 4 of these (0-3) */
#define XLNID_LSECRXCTRL	0x08F04
#define XLNID_LSECRXSCL		0x08F08
#define XLNID_LSECRXSCH		0x08F0C
#define XLNID_LSECRXSA(_i)	(0x08F10 + (4 * (_i))) /* 2 of these (0-1) */
#define XLNID_LSECRXPN(_i)	(0x08F18 + (4 * (_i))) /* 2 of these (0-1) */
#define XLNID_LSECRXKEY(_n, _m)	(0x08F20 + ((0x10 * (_n)) + (4 * (_m))))
#define XLNID_LSECTXUT		0x08A3C /* OutPktsUntagged */
#define XLNID_LSECTXPKTE	0x08A40 /* OutPktsEncrypted */
#define XLNID_LSECTXPKTP	0x08A44 /* OutPktsProtected */
#define XLNID_LSECTXOCTE	0x08A48 /* OutOctetsEncrypted */
#define XLNID_LSECTXOCTP	0x08A4C /* OutOctetsProtected */
#define XLNID_LSECRXUT		0x08F40 /* InPktsUntagged/InPktsNoTag */
#define XLNID_LSECRXOCTD	0x08F44 /* InOctetsDecrypted */
#define XLNID_LSECRXOCTV	0x08F48 /* InOctetsValidated */
#define XLNID_LSECRXBAD		0x08F4C /* InPktsBadTag */
#define XLNID_LSECRXNOSCI	0x08F50 /* InPktsNoSci */
#define XLNID_LSECRXUNSCI	0x08F54 /* InPktsUnknownSci */
#define XLNID_LSECRXUNCH	0x08F58 /* InPktsUnchecked */
#define XLNID_LSECRXDELAY	0x08F5C /* InPktsDelayed */
#define XLNID_LSECRXLATE	0x08F60 /* InPktsLate */
#define XLNID_LSECRXOK(_n)	(0x08F64 + (0x04 * (_n))) /* InPktsOk */
#define XLNID_LSECRXINV(_n)	(0x08F6C + (0x04 * (_n))) /* InPktsInvalid */
#define XLNID_LSECRXNV(_n)	(0x08F74 + (0x04 * (_n))) /* InPktsNotValid */
#define XLNID_LSECRXUNSA	0x08F7C /* InPktsUnusedSa */
#define XLNID_LSECRXNUSA	0x08F80 /* InPktsNotUsingSa */

/* LinkSec (MacSec) Bit Fields and Masks */
#define XLNID_LSECTXCAP_SUM_MASK	0x00FF0000
#define XLNID_LSECTXCAP_SUM_SHIFT	16
#define XLNID_LSECRXCAP_SUM_MASK	0x00FF0000
#define XLNID_LSECRXCAP_SUM_SHIFT	16

#define XLNID_LSECTXCTRL_EN_MASK	0x00000003
#define XLNID_LSECTXCTRL_DISABLE	0x0
#define XLNID_LSECTXCTRL_AUTH		0x1
#define XLNID_LSECTXCTRL_AUTH_ENCRYPT	0x2
#define XLNID_LSECTXCTRL_AISCI		0x00000020
#define XLNID_LSECTXCTRL_PNTHRSH_MASK	0xFFFFFF00
#define XLNID_LSECTXCTRL_RSV_MASK	0x000000D8

#define XLNID_LSECRXCTRL_EN_MASK	0x0000000C
#define XLNID_LSECRXCTRL_EN_SHIFT	2
#define XLNID_LSECRXCTRL_DISABLE	0x0
#define XLNID_LSECRXCTRL_CHECK		0x1
#define XLNID_LSECRXCTRL_STRICT		0x2
#define XLNID_LSECRXCTRL_DROP		0x3
#define XLNID_LSECRXCTRL_PLSH		0x00000040
#define XLNID_LSECRXCTRL_RP		0x00000080
#define XLNID_LSECRXCTRL_RSV_MASK	0xFFFFFF33

/* IpSec Registers */
#define XLNID_IPSTXIDX		0x08900
#define XLNID_IPSTXSALT		0x08904
#define XLNID_IPSTXKEY(_i)	(0x08908 + (4 * (_i))) /* 4 of these (0-3) */
#define XLNID_IPSRXIDX		0x08E00
#define XLNID_IPSRXIPADDR(_i)	(0x08E04 + (4 * (_i))) /* 4 of these (0-3) */
#define XLNID_IPSRXSPI		0x08E14
#define XLNID_IPSRXIPIDX	0x08E18
#define XLNID_IPSRXKEY(_i)	(0x08E1C + (4 * (_i))) /* 4 of these (0-3) */
#define XLNID_IPSRXSALT		0x08E2C
#define XLNID_IPSRXMOD		0x08E30

#define XLNID_SECTXCTRL_STORE_FORWARD_ENABLE	0x4

/* FCoE DMA Context Registers */
/* FCoE Direct DMA Context */
#define XLNID_FCDDC(_i, _j)	(0x20000 + ((_i) * 0x4) + ((_j) * 0x10))
#define XLNID_FCPTRL		0x02410 /* FC User Desc. PTR Low */
#define XLNID_FCPTRH		0x02414 /* FC USer Desc. PTR High */
#define XLNID_FCBUFF		0x02418 /* FC Buffer Control */
#define XLNID_FCDMARW		0x02420 /* FC Receive DMA RW */
#define XLNID_FCBUFF_VALID	(1 << 0)   /* DMA Context Valid */
#define XLNID_FCBUFF_BUFFSIZE	(3 << 3)   /* User Buffer Size */
#define XLNID_FCBUFF_WRCONTX	(1 << 7)   /* 0: Initiator, 1: Target */
#define XLNID_FCBUFF_BUFFCNT	0x0000ff00 /* Number of User Buffers */
#define XLNID_FCBUFF_OFFSET	0xffff0000 /* User Buffer Offset */
#define XLNID_FCBUFF_BUFFSIZE_SHIFT	3
#define XLNID_FCBUFF_BUFFCNT_SHIFT	8
#define XLNID_FCBUFF_OFFSET_SHIFT	16
#define XLNID_FCDMARW_WE		(1 << 14)   /* Write enable */
#define XLNID_FCDMARW_RE		(1 << 15)   /* Read enable */
#define XLNID_FCDMARW_FCOESEL		0x000001ff  /* FC X_ID: 11 bits */
#define XLNID_FCDMARW_LASTSIZE		0xffff0000  /* Last User Buffer Size */
#define XLNID_FCDMARW_LASTSIZE_SHIFT	16
/* FCoE SOF/EOF */
#define XLNID_TEOFF		0x04A94 /* Tx FC EOF */
#define XLNID_TSOFF		0x04A98 /* Tx FC SOF */
#define XLNID_REOFF		0x05158 /* Rx FC EOF */
#define XLNID_RSOFF		0x051F8 /* Rx FC SOF */
/* FCoE Filter Context Registers */
#define XLNID_FCD_ID		0x05114 /* FCoE D_ID */
#define XLNID_FCSMAC		0x0510C /* FCoE Source MAC */
#define XLNID_FCFLTRW_SMAC_HIGH_SHIFT	16
/* FCoE Direct Filter Context */
#define XLNID_FCDFC(_i, _j)	(0x28000 + ((_i) * 0x4) + ((_j) * 0x10))
#define XLNID_FCDFCD(_i)	(0x30000 + ((_i) * 0x4))
#define XLNID_FCFLT		0x05108 /* FC FLT Context */
#define XLNID_FCFLTRW		0x05110 /* FC Filter RW Control */
#define XLNID_FCPARAM		0x051d8 /* FC Offset Parameter */
#define XLNID_FCFLT_VALID	(1 << 0)   /* Filter Context Valid */
#define XLNID_FCFLT_FIRST	(1 << 1)   /* Filter First */
#define XLNID_FCFLT_SEQID	0x00ff0000 /* Sequence ID */
#define XLNID_FCFLT_SEQCNT	0xff000000 /* Sequence Count */
#define XLNID_FCFLTRW_RVALDT	(1 << 13)  /* Fast Re-Validation */
#define XLNID_FCFLTRW_WE	(1 << 14)  /* Write Enable */
#define XLNID_FCFLTRW_RE	(1 << 15)  /* Read Enable */
/* FCoE Receive Control */
#define XLNID_FCRXCTRL		0x05100 /* FC Receive Control */
#define XLNID_FCRXCTRL_FCOELLI	(1 << 0)   /* Low latency interrupt */
#define XLNID_FCRXCTRL_SAVBAD	(1 << 1)   /* Save Bad Frames */
#define XLNID_FCRXCTRL_FRSTRDH	(1 << 2)   /* EN 1st Read Header */
#define XLNID_FCRXCTRL_LASTSEQH	(1 << 3)   /* EN Last Header in Seq */
#define XLNID_FCRXCTRL_ALLH	(1 << 4)   /* EN All Headers */
#define XLNID_FCRXCTRL_FRSTSEQH	(1 << 5)   /* EN 1st Seq. Header */
#define XLNID_FCRXCTRL_ICRC	(1 << 6)   /* Ignore Bad FC CRC */
#define XLNID_FCRXCTRL_FCCRCBO	(1 << 7)   /* FC CRC Byte Ordering */
#define XLNID_FCRXCTRL_FCOEVER	0x00000f00 /* FCoE Version: 4 bits */
#define XLNID_FCRXCTRL_FCOEVER_SHIFT	8
/* FCoE Redirection */
#define XLNID_FCRECTL		0x0ED00 /* FC Redirection Control */
#define XLNID_FCRETA0		0x0ED10 /* FC Redirection Table 0 */
#define XLNID_FCRETA(_i)	(XLNID_FCRETA0 + ((_i) * 4)) /* FCoE Redir */
#define XLNID_FCRECTL_ENA	0x1 /* FCoE Redir Table Enable */
#define XLNID_FCRETASEL_ENA	0x2 /* FCoE FCRETASEL bit */
#define XLNID_FCRETA_SIZE	8 /* Max entries in FCRETA */
#define XLNID_FCRETA_ENTRY_MASK	0x0000007f /* 7 bits for the queue index */
/* Higher 7 bits for the queue index */
#define XLNID_FCRETA_ENTRY_HIGH_MASK	0x007F0000
#define XLNID_FCRETA_ENTRY_HIGH_SHIFT	16

#define XLNID_RQSMR(_i)	(0x02300 + ((_i) * 4))
#define XLNID_TQSMR(_i)	(((_i) <= 7) ? (0x07300 + ((_i) * 4)) : \
			 (0x08600 + ((_i) * 4)))
#define XLNID_TQSM(_i)	(0x08600 + ((_i) * 4))

#define XLNID_QPRC(_i)	(0x01030 + ((_i) * 0x40)) /* 16 of these */
#define XLNID_QPTC(_i)	(0x06030 + ((_i) * 0x40)) /* 16 of these */
#define XLNID_QBRC(_i)	(0x01034 + ((_i) * 0x40)) /* 16 of these */
#define XLNID_QBTC(_i)	(0x06034 + ((_i) * 0x40)) /* 16 of these */
#define XLNID_QBRC_L(_i)	(0x01034 + ((_i) * 0x40)) /* 16 of these */
#define XLNID_QBRC_H(_i)	(0x01038 + ((_i) * 0x40)) /* 16 of these */
#define XLNID_QPRDC(_i)		(0x01430 + ((_i) * 0x40)) /* 16 of these */
#define XLNID_QBTC_L(_i)	(0x08700 + ((_i) * 0x8)) /* 16 of these */
#define XLNID_QBTC_H(_i)	(0x08704 + ((_i) * 0x8)) /* 16 of these */
#define XLNID_FCCRC		0x05118 /* Num of Good Eth CRC w/ Bad FC CRC */
#define XLNID_FCOERPDC		0x0241C /* FCoE Rx Packets Dropped Count */
#define XLNID_FCLAST		0x02424 /* FCoE Last Error Count */
#define XLNID_FCOEPRC		0x02428 /* Number of FCoE Packets Received */
#define XLNID_FCOEDWRC		0x0242C /* Number of FCoE DWords Received */
#define XLNID_FCOEPTC		0x08784 /* Number of FCoE Packets Transmitted */
#define XLNID_FCOEDWTC		0x08788 /* Number of FCoE DWords Transmitted */
#define XLNID_FCCRC_CNT_MASK	0x0000FFFF /* CRC_CNT: bit 0 - 15 */
#define XLNID_FCLAST_CNT_MASK	0x0000FFFF /* Last_CNT: bit 0 - 15 */
#define XLNID_O2BGPTC		0x041C4
#define XLNID_O2BSPC		0x087B0
#define XLNID_B2OSPC		0x041C0
#define XLNID_B2OGPRC		0x02F90
#define XLNID_BUPRC		0x04180
#define XLNID_BMPRC		0x04184
#define XLNID_BBPRC		0x04188
#define XLNID_BUPTC		0x0418C
#define XLNID_BMPTC		0x04190
#define XLNID_BBPTC		0x04194
#define XLNID_BCRCERRS		0x04198
#define XLNID_BXONRXC		0x0419C
#define XLNID_BXOFFRXC		0x041E0
#define XLNID_BXONTXC		0x041E4
#define XLNID_BXOFFTXC		0x041E8


#define XLNID_LSWFW		0x15F14
#define XLNID_BMCIP(_i)		(0x05050 + ((_i) * 4)) /* 0x5050-0x505C */
#define XLNID_BMCIPVAL		0x05060
#define XLNID_BMCIP_IPADDR_TYPE	0x00000001
#define XLNID_BMCIP_IPADDR_VALID	0x00000002

/* Management Bit Fields and Masks */
#define XLNID_MANC_MPROXYE	0x40000000 /* Management Proxy Enable */
#define XLNID_MANC_RCV_TCO_EN	0x00020000 /* Rcv TCO packet enable */
#define XLNID_MANC_EN_BMC2OS	0x10000000 /* Ena BMC2OS and OS2BMC traffic */
#define XLNID_MANC_EN_BMC2OS_SHIFT	28

/* Firmware Semaphore Register */
#define XLNID_FWSM_MODE_MASK	0xE
#define XLNID_FWSM_TS_ENABLED	0x1
#define XLNID_FWSM_FW_MODE_PT	0x4
#define XLNID_FWSM_FW_NVM_RECOVERY_MODE (1 << 5)
#define XLNID_FWSM_EXT_ERR_IND_MASK 0x01F80000
#define XLNID_FWSM_FW_VAL_BIT	(1 << 15)

/* ARC Subsystem registers */
#define XLNID_HICR		0x15F00
#define XLNID_FWSTS		0x15F0C
#define XLNID_HSMC0R		0x15F04
#define XLNID_HSMC1R		0x15F08
#define XLNID_SWSR		0x15F10
#define XLNID_HFDR		0x15FE8
#define XLNID_FLEX_MNG		0x15800 /* 0x15800 - 0x15EFC */

#define XLNID_HICR_EN		0x01  /* Enable bit - RO */
/* Driver sets this bit when done to put command in RAM */
#define XLNID_HICR_C		0x02
#define XLNID_HICR_SV		0x04  /* Status Validity */
#define XLNID_HICR_FW_RESET_ENABLE	0x40
#define XLNID_HICR_FW_RESET	0x80

/* PCI-E registers */
#define XLNID_FACTPS		0x10150
#define XLNID_SWSM		0x10140
#define XLNID_FWSM		0x10148
#define XLNID_SWFW_SYNC		XLNID_GSSR
#define XLNID_GSSR		0x10160
#define XLNID_CIAA		0x11088
#define XLNID_CIAD		0x1108C

/* PCI Express Control */
#define XLNID_GCR_CMPL_TMOUT_MASK	0x0000F000
#define XLNID_GCR_CMPL_TMOUT_10ms	0x00001000
#define XLNID_GCR_CMPL_TMOUT_RESEND	0x00010000
#define XLNID_GCR_CAP_VER2		0x00040000

#define XLNID_GCR_EXT_MSIX_EN		0x80000000
#define XLNID_GCR_EXT_BUFFERS_CLEAR	0x40000000
#define XLNID_GCR_EXT_VT_MODE_16	0x00000001
#define XLNID_GCR_EXT_VT_MODE_32	0x00000002
#define XLNID_GCR_EXT_VT_MODE_64	0x00000003
#define XLNID_GCR_EXT_SRIOV		(XLNID_GCR_EXT_MSIX_EN | \
					 XLNID_GCR_EXT_VT_MODE_64)
#define XLNID_GCR_EXT_VT_MODE_MASK	0x00000003
/* Time Sync Registers */
#define XLNID_TSYNCRXCTL	0x05188 /* Rx Time Sync Control register - RW */
#define XLNID_TSYNCTXCTL	0x08C00 /* Tx Time Sync Control register - RW */
#define XLNID_RXSTMPL	0x051E8 /* Rx timestamp Low - RO */
#define XLNID_RXSTMPH	0x051A4 /* Rx timestamp High - RO */
#define XLNID_RXSATRL	0x051A0 /* Rx timestamp attribute low - RO */
#define XLNID_RXSATRH	0x051A8 /* Rx timestamp attribute high - RO */
#define XLNID_RXMTRL	0x05120 /* RX message type register low - RW */
#define XLNID_TXSTMPL	0x08C04 /* Tx timestamp value Low - RO */
#define XLNID_TXSTMPH	0x08C08 /* Tx timestamp value High - RO */
#define XLNID_SYSTIML	0x08C0C /* System time register Low - RO */
#define XLNID_SYSTIMH	0x08C10 /* System time register High - RO */
#define XLNID_SYSTIMR	0x08C58 /* System time register Residue - RO */
#define XLNID_TIMINCA	0x08C14 /* Increment attributes register - RW */
#define XLNID_TIMADJL	0x08C18 /* Time Adjustment Offset register Low - RW */
#define XLNID_TIMADJH	0x08C1C /* Time Adjustment Offset register High - RW */
#define XLNID_TSAUXC	0x08C20 /* TimeSync Auxiliary Control register - RW */
#define XLNID_TRGTTIML0	0x08C24 /* Target Time Register 0 Low - RW */
#define XLNID_TRGTTIMH0	0x08C28 /* Target Time Register 0 High - RW */
#define XLNID_TRGTTIML1	0x08C2C /* Target Time Register 1 Low - RW */
#define XLNID_TRGTTIMH1	0x08C30 /* Target Time Register 1 High - RW */
#define XLNID_CLKTIML	0x08C34 /* Clock Out Time Register Low - RW */
#define XLNID_CLKTIMH	0x08C38 /* Clock Out Time Register High - RW */
#define XLNID_FREQOUT0	0x08C34 /* Frequency Out 0 Control register - RW */
#define XLNID_FREQOUT1	0x08C38 /* Frequency Out 1 Control register - RW */
#define XLNID_AUXSTMPL0	0x08C3C /* Auxiliary Time Stamp 0 register Low - RO */
#define XLNID_AUXSTMPH0	0x08C40 /* Auxiliary Time Stamp 0 register High - RO */
#define XLNID_AUXSTMPL1	0x08C44 /* Auxiliary Time Stamp 1 register Low - RO */
#define XLNID_AUXSTMPH1	0x08C48 /* Auxiliary Time Stamp 1 register High - RO */
#define XLNID_TSIM	0x08C68 /* TimeSync Interrupt Mask Register - RW */
#define XLNID_TSICR	0x08C60 /* TimeSync Interrupt Cause Register - WO */
#define XLNID_TSSDP	0x0003C /* TimeSync SDP Configuration Register - RW */

/* Diagnostic Registers */
#define XLNID_RDSTATCTL		0x02C20
#define XLNID_RDSTAT(_i)	(0x02C00 + ((_i) * 4)) /* 0x02C00-0x02C1C */
#define XLNID_RDHMPN		0x02F08
#define XLNID_RIC_DW(_i)	(0x02F10 + ((_i) * 4))
#define XLNID_RDPROBE		0x02F20
#define XLNID_RDMAM		0x02F30
#define XLNID_RDMAD		0x02F34
#define XLNID_TDHMPN		0x07F08
#define XLNID_TDHMPN2		0x082FC
#define XLNID_TXDESCIC		0x082CC
#define XLNID_TIC_DW(_i)	(0x07F10 + ((_i) * 4))
#define XLNID_TIC_DW2(_i)	(0x082B0 + ((_i) * 4))
#define XLNID_TDPROBE		0x07F20
#define XLNID_TXBUFCTRL		0x0C600
#define XLNID_TXBUFDATA0	0x0C610
#define XLNID_TXBUFDATA1	0x0C614
#define XLNID_TXBUFDATA2	0x0C618
#define XLNID_TXBUFDATA3	0x0C61C
#define XLNID_RXBUFCTRL		0x03600
#define XLNID_RXBUFDATA0	0x03610
#define XLNID_RXBUFDATA1	0x03614
#define XLNID_RXBUFDATA2	0x03618
#define XLNID_RXBUFDATA3	0x0361C
#define XLNID_PCIE_DIAG(_i)	(0x11090 + ((_i) * 4)) /* 8 of these */
#define XLNID_RFVAL		0x050A4
#define XLNID_MDFTC1		0x042B8
#define XLNID_MDFTC2		0x042C0
#define XLNID_MDFTFIFO1		0x042C4
#define XLNID_MDFTFIFO2		0x042C8
#define XLNID_MDFTS		0x042CC
#define XLNID_RXDATAWRPTR(_i)	(0x03700 + ((_i) * 4)) /* 8 of these 3700-370C*/
#define XLNID_RXDESCWRPTR(_i)	(0x03710 + ((_i) * 4)) /* 8 of these 3710-371C*/
#define XLNID_RXDATARDPTR(_i)	(0x03720 + ((_i) * 4)) /* 8 of these 3720-372C*/
#define XLNID_RXDESCRDPTR(_i)	(0x03730 + ((_i) * 4)) /* 8 of these 3730-373C*/
#define XLNID_TXDATAWRPTR(_i)	(0x0C700 + ((_i) * 4)) /* 8 of these C700-C70C*/
#define XLNID_TXDESCWRPTR(_i)	(0x0C710 + ((_i) * 4)) /* 8 of these C710-C71C*/
#define XLNID_TXDATARDPTR(_i)	(0x0C720 + ((_i) * 4)) /* 8 of these C720-C72C*/
#define XLNID_TXDESCRDPTR(_i)	(0x0C730 + ((_i) * 4)) /* 8 of these C730-C73C*/
#define XLNID_PCIEECCCTL	0x1106C
#define XLNID_RXWRPTR(_i)	(0x03100 + ((_i) * 4)) /* 8 of these 3100-310C*/
#define XLNID_RXUSED(_i)	(0x03120 + ((_i) * 4)) /* 8 of these 3120-312C*/
#define XLNID_RXRDPTR(_i)	(0x03140 + ((_i) * 4)) /* 8 of these 3140-314C*/
#define XLNID_RXRDWRPTR(_i)	(0x03160 + ((_i) * 4)) /* 8 of these 3160-310C*/
#define XLNID_TXWRPTR(_i)	(0x0C100 + ((_i) * 4)) /* 8 of these C100-C10C*/
#define XLNID_TXUSED(_i)	(0x0C120 + ((_i) * 4)) /* 8 of these C120-C12C*/
#define XLNID_TXRDPTR(_i)	(0x0C140 + ((_i) * 4)) /* 8 of these C140-C14C*/
#define XLNID_TXRDWRPTR(_i)	(0x0C160 + ((_i) * 4)) /* 8 of these C160-C10C*/
#define XLNID_PCIEECCCTL0	0x11100
#define XLNID_PCIEECCCTL1	0x11104
#define XLNID_RXDBUECC		0x03F70
#define XLNID_TXDBUECC		0x0CF70
#define XLNID_RXDBUEST		0x03F74
#define XLNID_TXDBUEST		0x0CF74
#define XLNID_PBTXECC		0x0C300
#define XLNID_PBRXECC		0x03300
#define XLNID_GHECCR		0x110B0

/* MAC Registers */
#define XLNID_PCS1GCFIG		0x04200
#define XLNID_PCS1GLCTL		0x04208
#define XLNID_PCS1GLSTA		0x0420C
#define XLNID_PCS1GDBG0		0x04210
#define XLNID_PCS1GDBG1		0x04214
#define XLNID_PCS1GANA		0x04218
#define XLNID_PCS1GANLP		0x0421C
#define XLNID_PCS1GANNP		0x04220
#define XLNID_PCS1GANLPNP	0x04224

#define XLNID_PAP		0x04248
#define XLNID_MACA		0x0424C
#define XLNID_APAE		0x04250
#define XLNID_ARD		0x04254
#define XLNID_AIS		0x04258
#define XLNID_MSCA		0x0425C
#define XLNID_MSRWD		0x04260
#define XLNID_MLADD		0x04264
#define XLNID_MHADD		0x04268

#define XLNID_TREG		0x0426C
#define XLNID_PCSS1		0x04288
#define XLNID_PCSS2		0x0428C
#define XLNID_XPCSS		0x04290
#define XLNID_MFLCN		0x04294
#define XLNID_SERDESC		0x04298
#define XLNID_MAC_SGMII_BUSY	0x04298
#define XLNID_MACS		0x0429C
#define XLNID_AUTOC		0x042A0
#define XLNID_LINKS		0x042A4
#define XLNID_LINKS2		0x04324
#define XLNID_AUTOC2		0x042A8
#define XLNID_AUTOC3		0x042AC
#define XLNID_ANLP1		0x042B0
#define XLNID_ANLP2		0x042B4
#define XLNID_MACC		0x04330
#define XLNID_ATLASCTL		0x04800
#define XLNID_MMNGC		0x042D0
#define XLNID_ANLPNP1		0x042D4
#define XLNID_ANLPNP2		0x042D8
#define XLNID_KRPCSFC		0x042E0
#define XLNID_KRPCSS		0x042E4
#define XLNID_FECS1		0x042E8
#define XLNID_FECS2		0x042EC
#define XLNID_SMADARCTL		0x14F10
#define XLNID_MPVC		0x04318
#define XLNID_SGMIIC		0x04314

/* Statistics Registers */
#define XLNID_RXNFGPC		0x041B0
#define XLNID_RXNFGBCL		0x041B4
#define XLNID_RXNFGBCH		0x041B8
#define XLNID_RXDGPC		0x02F50
#define XLNID_RXDGBCL		0x02F54
#define XLNID_RXDGBCH		0x02F58
#define XLNID_RXDDGPC		0x02F5C
#define XLNID_RXDDGBCL		0x02F60
#define XLNID_RXDDGBCH		0x02F64
#define XLNID_RXLPBKGPC		0x02F68
#define XLNID_RXLPBKGBCL	0x02F6C
#define XLNID_RXLPBKGBCH	0x02F70
#define XLNID_RXDLPBKGPC	0x02F74
#define XLNID_RXDLPBKGBCL	0x02F78
#define XLNID_RXDLPBKGBCH	0x02F7C
#define XLNID_TXDGPC		0x087A0
#define XLNID_TXDGBCL		0x087A4
#define XLNID_TXDGBCH		0x087A8

#define XLNID_RXDSTATCTRL	0x02F40

/* Copper Pond 2 link timeout */
#define XLNID_VALIDATE_LINK_READY_TIMEOUT 50

/* Omer CORECTL */
#define XLNID_CORECTL			0x014F00
/* BARCTRL */
#define XLNID_BARCTRL			0x110F4
#define XLNID_BARCTRL_FLSIZE		0x0700
#define XLNID_BARCTRL_FLSIZE_SHIFT	8
#define XLNID_BARCTRL_CSRSIZE		0x2000

/* RSCCTL Bit Masks */
#define XLNID_RSCCTL_RSCEN	0x01
#define XLNID_RSCCTL_MAXDESC_1	0x00
#define XLNID_RSCCTL_MAXDESC_4	0x04
#define XLNID_RSCCTL_MAXDESC_8	0x08
#define XLNID_RSCCTL_MAXDESC_16	0x0C
#define XLNID_RSCCTL_TS_DIS	0x02

/* RSCDBU Bit Masks */
#define XLNID_RSCDBU_RSCSMALDIS_MASK	0x0000007F
#define XLNID_RSCDBU_RSCACKDIS		0x00000080

/* RDRXCTL Bit Masks */
#define XLNID_RDRXCTL_RDMTS_1_2		0x00000000 /* Rx Desc Min THLD Size */
#define XLNID_RDRXCTL_CRCSTRIP		0x00000002 /* CRC Strip */
#define XLNID_RDRXCTL_PSP		0x00000004 /* Pad Small Packet */
#define XLNID_RDRXCTL_MVMEN		0x00000020
#define XLNID_RDRXCTL_RSC_PUSH_DIS	0x00000020
#define XLNID_RDRXCTL_DMAIDONE		0x00000008 /* DMA init cycle done */
#define XLNID_RDRXCTL_RSC_PUSH		0x00000080
#define XLNID_RDRXCTL_AGGDIS		0x00010000 /* Aggregation disable */
#define XLNID_RDRXCTL_RSCFRSTSIZE	0x003E0000 /* RSC First packet size */
#define XLNID_RDRXCTL_RSCLLIDIS		0x00800000 /* Disable RSC compl on LLI*/
#define XLNID_RDRXCTL_RSCACKC		0x02000000 /* must set 1 when RSC ena */
#define XLNID_RDRXCTL_FCOE_WRFIX	0x04000000 /* must set 1 when RSC ena */
#define XLNID_RDRXCTL_MBINTEN		0x10000000
#define XLNID_RDRXCTL_MDP_EN		0x20000000

/* RQTC Bit Masks and Shifts */
#define XLNID_RQTC_SHIFT_TC(_i)	((_i) * 4)
#define XLNID_RQTC_TC0_MASK	(0x7 << 0)
#define XLNID_RQTC_TC1_MASK	(0x7 << 4)
#define XLNID_RQTC_TC2_MASK	(0x7 << 8)
#define XLNID_RQTC_TC3_MASK	(0x7 << 12)
#define XLNID_RQTC_TC4_MASK	(0x7 << 16)
#define XLNID_RQTC_TC5_MASK	(0x7 << 20)
#define XLNID_RQTC_TC6_MASK	(0x7 << 24)
#define XLNID_RQTC_TC7_MASK	(0x7 << 28)

/* PSRTYPE.RQPL Bit masks and shift */
#define XLNID_PSRTYPE_RQPL_MASK		0x7
#define XLNID_PSRTYPE_RQPL_SHIFT	29

/* CTRL Bit Masks */
#define XLNID_CTRL_GIO_DIS	0x00000004 /* Global IO Master Disable bit */
#define XLNID_CTRL_LNK_RST	0x00000008 /* Link Reset. Resets everything. */
#define XLNID_CTRL_RST		0x04000000 /* Reset (SW) */
#define XLNID_CTRL_RST_MASK	(XLNID_CTRL_LNK_RST | XLNID_CTRL_RST)

/* FACTPS */
#define XLNID_FACTPS_MNGCG	0x20000000 /* Manageblility Clock Gated */
#define XLNID_FACTPS_LFS	0x40000000 /* LAN Function Select */

/* MHADD Bit Masks */
#define XLNID_MHADD_MFS_MASK	0xFFFF0000
#define XLNID_MHADD_MFS_SHIFT	16

/* Extended Device Control */
#define XLNID_CTRL_EXT_PFRSTD	0x00004000 /* Physical Function Reset Done */
#define XLNID_CTRL_EXT_NS_DIS	0x00010000 /* No Snoop disable */
#define XLNID_CTRL_EXT_RO_DIS	0x00020000 /* Relaxed Ordering disable */
#define XLNID_CTRL_EXT_DRV_LOAD	0x10000000 /* Driver loaded bit for FW */

/* MSCA Bit Masks */
#define XLNID_MSCA_NP_ADDR_MASK		0x0000FFFF /* MDI Addr (new prot) */
#define XLNID_MSCA_NP_ADDR_SHIFT	0
#define XLNID_MSCA_DEV_TYPE_MASK	0x001F0000 /* Dev Type (new prot) */
#define XLNID_MSCA_DEV_TYPE_SHIFT	16 /* Register Address (old prot */
#define XLNID_MSCA_PHY_ADDR_MASK	0x03E00000 /* PHY Address mask */
#define XLNID_MSCA_PHY_ADDR_SHIFT	21 /* PHY Address shift*/
#define XLNID_MSCA_OP_CODE_MASK		0x0C000000 /* OP CODE mask */
#define XLNID_MSCA_OP_CODE_SHIFT	26 /* OP CODE shift */
#define XLNID_MSCA_ADDR_CYCLE		0x00000000 /* OP CODE 00 (addr cycle) */
#define XLNID_MSCA_WRITE		0x04000000 /* OP CODE 01 (wr) */
#define XLNID_MSCA_READ			0x0C000000 /* OP CODE 11 (rd) */
#define XLNID_MSCA_READ_AUTOINC		0x08000000 /* OP CODE 10 (rd auto inc)*/
#define XLNID_MSCA_ST_CODE_MASK		0x30000000 /* ST Code mask */
#define XLNID_MSCA_ST_CODE_SHIFT	28 /* ST Code shift */
#define XLNID_MSCA_NEW_PROTOCOL		0x00000000 /* ST CODE 00 (new prot) */
#define XLNID_MSCA_OLD_PROTOCOL		0x10000000 /* ST CODE 01 (old prot) */
#define XLNID_MSCA_MDI_COMMAND		0x40000000 /* Initiate MDI command */
#define XLNID_MSCA_MDI_IN_PROG_EN	0x80000000 /* MDI in progress ena */

/* MSRWD bit masks */
#define XLNID_MSRWD_WRITE_DATA_MASK	0x0000FFFF
#define XLNID_MSRWD_WRITE_DATA_SHIFT	0
#define XLNID_MSRWD_READ_DATA_MASK	0xFFFF0000
#define XLNID_MSRWD_READ_DATA_SHIFT	16

/* Atlas registers */
#define XLNID_ATLAS_PDN_LPBK		0x24
#define XLNID_ATLAS_PDN_10G		0xB
#define XLNID_ATLAS_PDN_1G		0xC
#define XLNID_ATLAS_PDN_AN		0xD

/* Atlas bit masks */
#define XLNID_ATLASCTL_WRITE_CMD	0x00010000
#define XLNID_ATLAS_PDN_TX_REG_EN	0x10
#define XLNID_ATLAS_PDN_TX_10G_QL_ALL	0xF0
#define XLNID_ATLAS_PDN_TX_1G_QL_ALL	0xF0
#define XLNID_ATLAS_PDN_TX_AN_QL_ALL	0xF0

/* Omer bit masks */
#define XLNID_CORECTL_WRITE_CMD		0x00010000

/* GEPHY */
#define WESTLAKE_GEPHY_ADDR         0x8
#define XLNID_MDIO_CL22_WRITE       0x00050000
#define XLNID_MDIO_CL22_READ        0x00060000
#define XLNID_MDIO_ADDR_MASK        0x1F

/* Device Type definitions for new protocol MDIO commands */
#define XLNID_MDIO_ZERO_DEV_TYPE		0x0
#define XLNID_MDIO_PMA_PMD_DEV_TYPE		0x1
#define XLNID_MDIO_PCS_DEV_TYPE			0x3
#define XLNID_MDIO_PHY_XS_DEV_TYPE		0x4
#define XLNID_MDIO_AUTO_NEG_DEV_TYPE		0x7
#define XLNID_MDIO_VENDOR_SPECIFIC_1_DEV_TYPE	0x1E   /* Device 30 */
#define XLNID_TWINAX_DEV			1

#define XLNID_MDIO_COMMAND_TIMEOUT	100 /* PHY Timeout for 1 GB mode */

#define XLNID_MDIO_VENDOR_SPECIFIC_1_CONTROL		0x0 /* VS1 Ctrl Reg */
#define XLNID_MDIO_VENDOR_SPECIFIC_1_STATUS		0x1 /* VS1 Status Reg */
#define XLNID_MDIO_VENDOR_SPECIFIC_1_LINK_STATUS	0x0008 /* 1 = Link Up */
#define XLNID_MDIO_VENDOR_SPECIFIC_1_SPEED_STATUS	0x0010 /* 0-10G, 1-1G */
#define XLNID_MDIO_VENDOR_SPECIFIC_1_10G_SPEED		0x0018
#define XLNID_MDIO_VENDOR_SPECIFIC_1_1G_SPEED		0x0010

#define XLNID_MDIO_AUTO_NEG_CONTROL	0x0 /* AUTO_NEG Control Reg */
#define XLNID_MDIO_AUTO_NEG_STATUS	0x1 /* AUTO_NEG Status Reg */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STAT	0xC800 /* AUTO_NEG Vendor Status Reg */
#define XLNID_MDIO_AUTO_NEG_VENDOR_TX_ALARM 0xCC00 /* AUTO_NEG Vendor TX Reg */
#define XLNID_MDIO_AUTO_NEG_VENDOR_TX_ALARM2 0xCC01 /* AUTO_NEG Vendor Tx Reg */
#define XLNID_MDIO_AUTO_NEG_VEN_LSC	0x1 /* AUTO_NEG Vendor Tx LSC */
#define XLNID_MDIO_AUTO_NEG_ADVT	0x10 /* AUTO_NEG Advt Reg */
#define XLNID_MDIO_AUTO_NEG_LP		0x13 /* AUTO_NEG LP Status Reg */
#define XLNID_MDIO_AUTO_NEG_EEE_ADVT	0x3C /* AUTO_NEG EEE Advt Reg */
#define XLNID_AUTO_NEG_10GBASE_EEE_ADVT	0x8  /* AUTO NEG EEE 10GBaseT Advt */
#define XLNID_AUTO_NEG_1000BASE_EEE_ADVT 0x4  /* AUTO NEG EEE 1000BaseT Advt */
#define XLNID_AUTO_NEG_100BASE_EEE_ADVT	0x2  /* AUTO NEG EEE 100BaseT Advt */
#define XLNID_MDIO_PHY_XS_CONTROL	0x0 /* PHY_XS Control Reg */
#define XLNID_MDIO_PHY_XS_RESET		0x8000 /* PHY_XS Reset */
#define XLNID_MDIO_PHY_ID_HIGH		0x2 /* PHY ID High Reg*/
#define XLNID_MDIO_PHY_ID_LOW		0x3 /* PHY ID Low Reg*/
#define XLNID_MDIO_PHY_SPEED_ABILITY	0x4 /* Speed Ability Reg */
#define XLNID_MDIO_PHY_SPEED_10G	0x0001 /* 10G capable */
#define XLNID_MDIO_PHY_SPEED_1G		0x0010 /* 1G capable */
#define XLNID_MDIO_PHY_SPEED_100M	0x0020 /* 100M capable */
#define XLNID_MDIO_PHY_EXT_ABILITY	0xB /* Ext Ability Reg */
#define XLNID_MDIO_PHY_10GBASET_ABILITY		0x0004 /* 10GBaseT capable */
#define XLNID_MDIO_PHY_1000BASET_ABILITY	0x0020 /* 1000BaseT capable */
#define XLNID_MDIO_PHY_100BASETX_ABILITY	0x0080 /* 100BaseTX capable */
#define XLNID_MDIO_PHY_SET_LOW_POWER_MODE	0x0800 /* Set low power mode */
#define XLNID_AUTO_NEG_LP_STATUS	0xE820 /* AUTO NEG Rx LP Status Reg */
#define XLNID_AUTO_NEG_LP_1000BASE_CAP	0x8000 /* AUTO NEG Rx LP 1000BaseT Cap */
#define XLNID_AUTO_NEG_LP_10GBASE_CAP	0x0800 /* AUTO NEG Rx LP 10GBaseT Cap */
#define XLNID_AUTO_NEG_10GBASET_STAT	0x0021 /* AUTO NEG 10G BaseT Stat */

#define XLNID_MDIO_TX_VENDOR_ALARMS_3		0xCC02 /* Vendor Alarms 3 Reg */
#define XLNID_MDIO_TX_VENDOR_ALARMS_3_RST_MASK	0x3 /* PHY Reset Complete Mask */
#define XLNID_MDIO_GLOBAL_RES_PR_10 0xC479 /* Global Resv Provisioning 10 Reg */
#define XLNID_MDIO_POWER_UP_STALL		0x8000 /* Power Up Stall */
#define XLNID_MDIO_GLOBAL_INT_CHIP_STD_MASK	0xFF00 /* int std mask */
#define XLNID_MDIO_GLOBAL_CHIP_STD_INT_FLAG	0xFC00 /* chip std int flag */
#define XLNID_MDIO_GLOBAL_INT_CHIP_VEN_MASK	0xFF01 /* int chip-wide mask */
#define XLNID_MDIO_GLOBAL_INT_CHIP_VEN_FLAG	0xFC01 /* int chip-wide mask */
#define XLNID_MDIO_GLOBAL_ALARM_1		0xCC00 /* Global alarm 1 */
#define XLNID_MDIO_GLOBAL_ALM_1_DEV_FAULT	0x0010 /* device fault */
#define XLNID_MDIO_GLOBAL_ALM_1_HI_TMP_FAIL	0x4000 /* high temp failure */
#define XLNID_MDIO_GLOBAL_FAULT_MSG	0xC850 /* Global Fault Message */
#define XLNID_MDIO_GLOBAL_FAULT_MSG_HI_TMP	0x8007 /* high temp failure */
#define XLNID_MDIO_GLOBAL_INT_MASK		0xD400 /* Global int mask */
#define XLNID_MDIO_GLOBAL_AN_VEN_ALM_INT_EN	0x1000 /* autoneg vendor alarm int enable */
#define XLNID_MDIO_GLOBAL_ALARM_1_INT		0x4 /* int in Global alarm 1 */
#define XLNID_MDIO_GLOBAL_VEN_ALM_INT_EN	0x1 /* vendor alarm int enable */
#define XLNID_MDIO_GLOBAL_STD_ALM2_INT		0x200 /* vendor alarm2 int mask */
#define XLNID_MDIO_GLOBAL_INT_HI_TEMP_EN	0x4000 /* int high temp enable */
#define XLNID_MDIO_GLOBAL_INT_DEV_FAULT_EN 0x0010 /* int dev fault enable */
#define XLNID_MDIO_PMA_PMD_CONTROL_ADDR	0x0000 /* PMA/PMD Control Reg */
#define XLNID_MDIO_PMA_PMD_SDA_SCL_ADDR	0xC30A /* PHY_XS SDA/SCL Addr Reg */
#define XLNID_MDIO_PMA_PMD_SDA_SCL_DATA	0xC30B /* PHY_XS SDA/SCL Data Reg */
#define XLNID_MDIO_PMA_PMD_SDA_SCL_STAT	0xC30C /* PHY_XS SDA/SCL Status Reg */
#define XLNID_MDIO_PMA_TX_VEN_LASI_INT_MASK 0xD401 /* PHY TX Vendor LASI */
#define XLNID_MDIO_PMA_TX_VEN_LASI_INT_EN   0x1 /* PHY TX Vendor LASI enable */
#define XLNID_MDIO_PMD_STD_TX_DISABLE_CNTR 0x9 /* Standard Transmit Dis Reg */
#define XLNID_MDIO_PMD_GLOBAL_TX_DISABLE 0x0001 /* PMD Global Transmit Dis */

#define XLNID_PCRC8ECL		0x0E810 /* PCR CRC-8 Error Count Lo */
#define XLNID_PCRC8ECH		0x0E811 /* PCR CRC-8 Error Count Hi */
#define XLNID_PCRC8ECH_MASK	0x1F
#define XLNID_LDPCECL		0x0E820 /* PCR Uncorrected Error Count Lo */
#define XLNID_LDPCECH		0x0E821 /* PCR Uncorrected Error Count Hi */

/* MII clause 22/28 definitions */
#define XLNID_MDIO_PHY_LOW_POWER_MODE	0x0800

#define XLNID_MDIO_XENPAK_LASI_STATUS		0x9005 /* XENPAK LASI Status register*/
#define XLNID_XENPAK_LASI_LINK_STATUS_ALARM	0x1 /* Link Status Alarm change */

#define XLNID_MDIO_AUTO_NEG_LINK_STATUS		0x4 /* Indicates if link is up */

#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_MASK	0x7 /* Speed/Duplex Mask */
#define XLNID_MDIO_AUTO_NEG_VEN_STAT_SPEED_MASK		0x6 /* Speed Mask */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_10M_HALF	0x0 /* 10Mb/s Half Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_10M_FULL	0x1 /* 10Mb/s Full Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_100M_HALF	0x2 /* 100Mb/s Half Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_100M_FULL	0x3 /* 100Mb/s Full Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_1GB_HALF	0x4 /* 1Gb/s Half Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_1GB_FULL	0x5 /* 1Gb/s Full Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_10GB_HALF	0x6 /* 10Gb/s Half Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_10GB_FULL	0x7 /* 10Gb/s Full Duplex */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_1GB		0x4 /* 1Gb/s */
#define XLNID_MDIO_AUTO_NEG_VENDOR_STATUS_10GB		0x6 /* 10Gb/s */

#define XLNID_MII_10GBASE_T_AUTONEG_CTRL_REG	0x20   /* 10G Control Reg */
#define XLNID_MII_AUTONEG_VENDOR_PROVISION_1_REG 0xC400 /* 1G Provisioning 1 */
#define XLNID_MII_AUTONEG_XNP_TX_REG		0x17   /* 1G XNP Transmit */
#define XLNID_MII_AUTONEG_ADVERTISE_REG		0x10   /* 100M Advertisement */
#define XLNID_MII_10GBASE_T_ADVERTISE		0x1000 /* full duplex, bit:12*/
#define XLNID_MII_1GBASE_T_ADVERTISE_XNP_TX	0x4000 /* full duplex, bit:14*/
#define XLNID_MII_1GBASE_T_ADVERTISE		0x8000 /* full duplex, bit:15*/
#define XLNID_MII_2_5GBASE_T_ADVERTISE		0x0400
#define XLNID_MII_5GBASE_T_ADVERTISE		0x0800
#define XLNID_MII_100BASE_T_ADVERTISE		0x0100 /* full duplex, bit:8 */
#define XLNID_MII_100BASE_T_ADVERTISE_HALF	0x0080 /* half duplex, bit:7 */
#define XLNID_MII_RESTART			0x200
#define XLNID_MII_AUTONEG_COMPLETE		0x20
#define XLNID_MII_AUTONEG_LINK_UP		0x04
#define XLNID_MII_AUTONEG_REG			0x0

#define XLNID_PHY_REVISION_MASK		0xFFFFFFF0
#define XLNID_MAX_PHY_ADDR		32

/* Special PHY Init Routine */
#define XLNID_PHY_INIT_OFFSET_NL	0x002B
#define XLNID_PHY_INIT_END_NL		0xFFFF
#define XLNID_CONTROL_MASK_NL		0xF000
#define XLNID_DATA_MASK_NL		0x0FFF
#define XLNID_CONTROL_SHIFT_NL		12
#define XLNID_DELAY_NL			0
#define XLNID_DATA_NL			1
#define XLNID_CONTROL_NL		0x000F
#define XLNID_CONTROL_EOL_NL		0x0FFF
#define XLNID_CONTROL_SOL_NL		0x0000

/* General purpose Interrupt Enable */
#define XLNID_GPIE_MSIX_MODE	0x00000010 /* MSI-X mode */
#define XLNID_GPIE_OCD		0x00000020 /* Other Clear Disable */
#define XLNID_GPIE_EIMEN	0x00000040 /* Immediate Interrupt Enable */
#define XLNID_GPIE_EIAME	0x40000000
#define XLNID_GPIE_PBA_SUPPORT	0x80000000
#define XLNID_GPIE_RSC_DELAY_SHIFT	11
#define XLNID_GPIE_VTMODE_MASK	0x0000C000 /* VT Mode Mask */
#define XLNID_GPIE_VTMODE_16	0x00004000 /* 16 VFs 8 queues per VF */
#define XLNID_GPIE_VTMODE_32	0x00008000 /* 32 VFs 4 queues per VF */
#define XLNID_GPIE_VTMODE_64	0x0000C000 /* 64 VFs 2 queues per VF */

/* Packet Buffer Initialization */
#define XLNID_MAX_PACKET_BUFFERS	8

#define XLNID_TXPBSIZE_20KB	0x00005000 /* 20KB Packet Buffer */
#define XLNID_TXPBSIZE_40KB	0x0000A000 /* 40KB Packet Buffer */
#define XLNID_RXPBSIZE_48KB	0x0000C000 /* 48KB Packet Buffer */
#define XLNID_RXPBSIZE_64KB	0x00010000 /* 64KB Packet Buffer */
#define XLNID_RXPBSIZE_80KB	0x00014000 /* 80KB Packet Buffer */
#define XLNID_RXPBSIZE_128KB	0x00020000 /* 128KB Packet Buffer */
#define XLNID_RXPBSIZE_MAX	0x00080000 /* 512KB Packet Buffer */
#define XLNID_TXPBSIZE_MAX	0x00028000 /* 160KB Packet Buffer */

#define XLNID_TXPKT_SIZE_MAX	0xA /* Max Tx Packet size */
#define XLNID_MAX_PB		8

/* Packet buffer allocation strategies */
enum {
	PBA_STRATEGY_EQUAL	= 0, /* Distribute PB space equally */
#define PBA_STRATEGY_EQUAL	PBA_STRATEGY_EQUAL
	PBA_STRATEGY_WEIGHTED	= 1, /* Weight front half of TCs */
#define PBA_STRATEGY_WEIGHTED	PBA_STRATEGY_WEIGHTED
};

/* Transmit Flow Control status */
#define XLNID_TFCS_TXOFF	0x00000001
#define XLNID_TFCS_TXOFF0	0x00000100
#define XLNID_TFCS_TXOFF1	0x00000200
#define XLNID_TFCS_TXOFF2	0x00000400
#define XLNID_TFCS_TXOFF3	0x00000800
#define XLNID_TFCS_TXOFF4	0x00001000
#define XLNID_TFCS_TXOFF5	0x00002000
#define XLNID_TFCS_TXOFF6	0x00004000
#define XLNID_TFCS_TXOFF7	0x00008000

/* TCP Timer */
#define XLNID_TCPTIMER_KS		0x00000100
#define XLNID_TCPTIMER_COUNT_ENABLE	0x00000200
#define XLNID_TCPTIMER_COUNT_FINISH	0x00000400
#define XLNID_TCPTIMER_LOOP		0x00000800
#define XLNID_TCPTIMER_DURATION_MASK	0x000000FF

/* HLREG0 Bit Masks */
#define XLNID_HLREG0_TXCRCEN		0x00000001 /* bit  0 */
#define XLNID_HLREG0_RXCRCSTRP		0x00000002 /* bit  1 */
#define XLNID_HLREG0_JUMBOEN		0x00000004 /* bit  2 */
#define XLNID_HLREG0_TXPADEN		0x00000400 /* bit 10 */
#define XLNID_HLREG0_TXPAUSEEN		0x00001000 /* bit 12 */
#define XLNID_HLREG0_RXPAUSEEN		0x00004000 /* bit 14 */
#define XLNID_HLREG0_LPBK		0x00008000 /* bit 15 */
#define XLNID_HLREG0_MDCSPD		0x00010000 /* bit 16 */
#define XLNID_HLREG0_CONTMDC		0x00020000 /* bit 17 */
#define XLNID_HLREG0_CTRLFLTR		0x00040000 /* bit 18 */
#define XLNID_HLREG0_PREPEND		0x00F00000 /* bits 20-23 */
#define XLNID_HLREG0_PRIPAUSEEN		0x01000000 /* bit 24 */
#define XLNID_HLREG0_RXPAUSERECDA	0x06000000 /* bits 25-26 */
#define XLNID_HLREG0_RXLNGTHERREN	0x08000000 /* bit 27 */
#define XLNID_HLREG0_RXPADSTRIPEN	0x10000000 /* bit 28 */

/* VMD_CTL bitmasks */
#define XLNID_VMD_CTL_VMDQ_EN		0x00000001
#define XLNID_VMD_CTL_VMDQ_FILTER	0x00000002

/* VT_CTL bitmasks */
#define XLNID_VT_CTL_DIS_DEFPL		0x20000000 /* disable default pool */
#define XLNID_VT_CTL_REPLEN		0x40000000 /* replication enabled */
#define XLNID_VT_CTL_VT_ENABLE		0x00000001  /* Enable VT Mode */
#define XLNID_VT_CTL_POOL_SHIFT		7
#define XLNID_VT_CTL_POOL_MASK		(0x3F << XLNID_VT_CTL_POOL_SHIFT)

/* VMOLR bitmasks */
#define XLNID_VMOLR_UPE		0x00400000 /* unicast promiscuous */
#define XLNID_VMOLR_VPE		0x00800000 /* VLAN promiscuous */
#define XLNID_VMOLR_AUPE	0x01000000 /* accept untagged packets */
#define XLNID_VMOLR_ROMPE	0x02000000 /* accept packets in MTA tbl */
#define XLNID_VMOLR_ROPE	0x04000000 /* accept packets in UC tbl */
#define XLNID_VMOLR_BAM		0x08000000 /* accept broadcast packets */
#define XLNID_VMOLR_MPE		0x10000000 /* multicast promiscuous */

/* VFRE bitmask */
#define XLNID_VFRE_ENABLE_ALL	0xFFFFFFFF

#define XLNID_VF_INIT_TIMEOUT	200 /* Number of retries to clear RSTI */

/* RDHMPN and TDHMPN bitmasks */
#define XLNID_RDHMPN_RDICADDR		0x007FF800
#define XLNID_RDHMPN_RDICRDREQ		0x00800000
#define XLNID_RDHMPN_RDICADDR_SHIFT	11
#define XLNID_TDHMPN_TDICADDR		0x003FF800
#define XLNID_TDHMPN_TDICRDREQ		0x00800000
#define XLNID_TDHMPN_TDICADDR_SHIFT	11

#define XLNID_RDMAM_MEM_SEL_SHIFT		13
#define XLNID_RDMAM_DWORD_SHIFT			9
#define XLNID_RDMAM_DESC_COMP_FIFO		1
#define XLNID_RDMAM_DFC_CMD_FIFO		2
#define XLNID_RDMAM_RSC_HEADER_ADDR		3
#define XLNID_RDMAM_TCN_STATUS_RAM		4
#define XLNID_RDMAM_WB_COLL_FIFO		5
#define XLNID_RDMAM_QSC_CNT_RAM			6
#define XLNID_RDMAM_QSC_FCOE_RAM		7
#define XLNID_RDMAM_QSC_QUEUE_CNT		8
#define XLNID_RDMAM_QSC_QUEUE_RAM		0xA
#define XLNID_RDMAM_QSC_RSC_RAM			0xB
#define XLNID_RDMAM_DESC_COM_FIFO_RANGE		135
#define XLNID_RDMAM_DESC_COM_FIFO_COUNT		4
#define XLNID_RDMAM_DFC_CMD_FIFO_RANGE		48
#define XLNID_RDMAM_DFC_CMD_FIFO_COUNT		7
#define XLNID_RDMAM_RSC_HEADER_ADDR_RANGE	32
#define XLNID_RDMAM_RSC_HEADER_ADDR_COUNT	4
#define XLNID_RDMAM_TCN_STATUS_RAM_RANGE	256
#define XLNID_RDMAM_TCN_STATUS_RAM_COUNT	9
#define XLNID_RDMAM_WB_COLL_FIFO_RANGE		8
#define XLNID_RDMAM_WB_COLL_FIFO_COUNT		4
#define XLNID_RDMAM_QSC_CNT_RAM_RANGE		64
#define XLNID_RDMAM_QSC_CNT_RAM_COUNT		4
#define XLNID_RDMAM_QSC_FCOE_RAM_RANGE		512
#define XLNID_RDMAM_QSC_FCOE_RAM_COUNT		5
#define XLNID_RDMAM_QSC_QUEUE_CNT_RANGE		32
#define XLNID_RDMAM_QSC_QUEUE_CNT_COUNT		4
#define XLNID_RDMAM_QSC_QUEUE_RAM_RANGE		128
#define XLNID_RDMAM_QSC_QUEUE_RAM_COUNT		8
#define XLNID_RDMAM_QSC_RSC_RAM_RANGE		32
#define XLNID_RDMAM_QSC_RSC_RAM_COUNT		8

#define XLNID_TXDESCIC_READY	0x80000000

/* Receive Checksum Control */
#define XLNID_RXCSUM_IPPCSE	0x00001000 /* IP payload checksum enable */
#define XLNID_RXCSUM_PCSD	0x00002000 /* packet checksum disabled */

/* FCRTL Bit Masks */
#define XLNID_FCRTL_XONE	0x80000000 /* XON enable */
#define XLNID_FCRTH_FCEN	0x80000000 /* Packet buffer fc enable */

/* PAP bit masks*/
#define XLNID_PAP_TXPAUSECNT_MASK	0x0000FFFF /* Pause counter mask */

/* RMCS Bit Masks */
#define XLNID_RMCS_RRM			0x00000002 /* Rx Recycle Mode enable */
/* Receive Arbitration Control: 0 Round Robin, 1 DFP */
#define XLNID_RMCS_RAC			0x00000004
/* Deficit Fixed Prio ena */
#define XLNID_RMCS_DFP			XLNID_RMCS_RAC
#define XLNID_RMCS_TFCE_802_3X		0x00000008 /* Tx Priority FC ena */
#define XLNID_RMCS_TFCE_PRIORITY	0x00000010 /* Tx Priority FC ena */
#define XLNID_RMCS_ARBDIS		0x00000040 /* Arbitration disable bit */

/* FCCFG Bit Masks */
#define XLNID_FCCFG_TFCE_802_3X		0x00000008 /* Tx link FC enable */
#define XLNID_FCCFG_TFCE_PRIORITY	0x00000010 /* Tx priority FC enable */

/* Interrupt register bitmasks */

/* Extended Interrupt Cause Read */
#define XLNID_EICR_RTX_QUEUE	0x0000FFFF /* RTx Queue Interrupt */
#define XLNID_EICR_FLOW_DIR	0x00010000 /* FDir Exception */
#define XLNID_EICR_RX_MISS	0x00020000 /* Packet Buffer Overrun */
#define XLNID_EICR_PCI		0x00040000 /* PCI Exception */
#define XLNID_EICR_MAILBOX	0x00080000 /* VF to PF Mailbox Interrupt */
#define XLNID_EICR_LSC		0x00100000 /* Link Status Change */
#define XLNID_EICR_LINKSEC	0x00200000 /* PN Threshold */
#define XLNID_EICR_MNG		0x00400000 /* Manageability Event Interrupt */
#define XLNID_EICR_FIFO_OVF	    0x00800000 /* FIFO Overflow Interrupt */
#define XLNID_EICR_FIFO_UNDF    0x01000000 /* FIFO Underflow Interrupt */
#define XLNID_EICR_GPI_SDP0	0x01000000 /* Gen Purpose Interrupt on SDP0 */
#define XLNID_EICR_GPI_SDP1	0x02000000 /* Gen Purpose Interrupt on SDP1 */
#define XLNID_EICR_GPI_SDP2	0x04000000 /* Gen Purpose Interrupt on SDP2 */
#define XLNID_EICR_ECC		0x10000000 /* ECC Error */
#define XLNID_EICR_PBUR		0x10000000 /* Packet Buffer Handler Error */
#define XLNID_EICR_DHER		0x20000000 /* Descriptor Handler Error */
#define XLNID_EICR_TCP_TIMER	0x40000000 /* TCP Timer */
#define XLNID_EICR_OTHER	0x80000000 /* Interrupt Cause Active */

/* Extended Interrupt Cause Set */
#define XLNID_EICS_RTX_QUEUE	XLNID_EICR_RTX_QUEUE /* RTx Queue Interrupt */
#define XLNID_EICS_FLOW_DIR	XLNID_EICR_FLOW_DIR  /* FDir Exception */
#define XLNID_EICS_RX_MISS	XLNID_EICR_RX_MISS   /* Pkt Buffer Overrun */
#define XLNID_EICS_PCI		XLNID_EICR_PCI /* PCI Exception */
#define XLNID_EICS_MAILBOX	XLNID_EICR_MAILBOX   /* VF to PF Mailbox Int */
#define XLNID_EICS_LSC		XLNID_EICR_LSC /* Link Status Change */
#define XLNID_EICS_MNG		XLNID_EICR_MNG /* MNG Event Interrupt */
#define XLNID_EICS_TIMESYNC	XLNID_EICR_TIMESYNC /* Timesync Event */
#define XLNID_EICS_GPI_SDP0	XLNID_EICR_GPI_SDP0 /* SDP0 Gen Purpose Int */
#define XLNID_EICS_GPI_SDP1	XLNID_EICR_GPI_SDP1 /* SDP1 Gen Purpose Int */
#define XLNID_EICS_GPI_SDP2	XLNID_EICR_GPI_SDP2 /* SDP2 Gen Purpose Int */
#define XLNID_EICS_ECC		XLNID_EICR_ECC /* ECC Error */
#define XLNID_EICS_PBUR		XLNID_EICR_PBUR /* Pkt Buf Handler Err */
#define XLNID_EICS_DHER		XLNID_EICR_DHER /* Desc Handler Error */
#define XLNID_EICS_TCP_TIMER	XLNID_EICR_TCP_TIMER /* TCP Timer */
#define XLNID_EICS_OTHER	XLNID_EICR_OTHER /* INT Cause Active */

/* Extended Interrupt Mask Set */
#define XLNID_EIMS_RTX_QUEUE	XLNID_EICR_RTX_QUEUE /* RTx Queue Interrupt */
#define XLNID_EIMS_FLOW_DIR	XLNID_EICR_FLOW_DIR /* FDir Exception */
#define XLNID_EIMS_RX_MISS	XLNID_EICR_RX_MISS /* Packet Buffer Overrun */
#define XLNID_EIMS_PCI		XLNID_EICR_PCI /* PCI Exception */
#define XLNID_EIMS_MAILBOX	XLNID_EICR_MAILBOX   /* VF to PF Mailbox Int */
#define XLNID_EIMS_LSC		XLNID_EICR_LSC /* Link Status Change */
#define XLNID_EIMS_MNG		XLNID_EICR_MNG /* MNG Event Interrupt */
#define XLNID_EIMS_GPI_SDP0	XLNID_EICR_GPI_SDP0 /* SDP0 Gen Purpose Int */
#define XLNID_EIMS_GPI_SDP1	XLNID_EICR_GPI_SDP1 /* SDP1 Gen Purpose Int */
#define XLNID_EIMS_GPI_SDP2	XLNID_EICR_GPI_SDP2 /* SDP2 Gen Purpose Int */
#define XLNID_EIMS_ECC		XLNID_EICR_ECC /* ECC Error */
#define XLNID_EIMS_PBUR		XLNID_EICR_PBUR /* Pkt Buf Handler Err */
#define XLNID_EIMS_DHER		XLNID_EICR_DHER /* Descr Handler Error */
#define XLNID_EIMS_TCP_TIMER	XLNID_EICR_TCP_TIMER /* TCP Timer */
#define XLNID_EIMS_OTHER	XLNID_EICR_OTHER /* INT Cause Active */

/* Extended Interrupt Mask Clear */
#define XLNID_EIMC_RTX_QUEUE	XLNID_EICR_RTX_QUEUE /* RTx Queue Interrupt */
#define XLNID_EIMC_FLOW_DIR	XLNID_EICR_FLOW_DIR /* FDir Exception */
#define XLNID_EIMC_RX_MISS	XLNID_EICR_RX_MISS /* Packet Buffer Overrun */
#define XLNID_EIMC_PCI		XLNID_EICR_PCI /* PCI Exception */
#define XLNID_EIMC_MAILBOX	XLNID_EICR_MAILBOX /* VF to PF Mailbox Int */
#define XLNID_EIMC_LSC		XLNID_EICR_LSC /* Link Status Change */
#define XLNID_EIMC_MNG		XLNID_EICR_MNG /* MNG Event Interrupt */
#define XLNID_EIMC_TIMESYNC	XLNID_EICR_TIMESYNC /* Timesync Event */
#define XLNID_EIMC_GPI_SDP0	XLNID_EICR_GPI_SDP0 /* SDP0 Gen Purpose Int */
#define XLNID_EIMC_GPI_SDP1	XLNID_EICR_GPI_SDP1 /* SDP1 Gen Purpose Int */
#define XLNID_EIMC_GPI_SDP2	XLNID_EICR_GPI_SDP2  /* SDP2 Gen Purpose Int */
#define XLNID_EIMC_ECC		XLNID_EICR_ECC /* ECC Error */
#define XLNID_EIMC_PBUR		XLNID_EICR_PBUR /* Pkt Buf Handler Err */
#define XLNID_EIMC_DHER		XLNID_EICR_DHER /* Desc Handler Err */
#define XLNID_EIMC_TCP_TIMER	XLNID_EICR_TCP_TIMER /* TCP Timer */
#define XLNID_EIMC_OTHER	XLNID_EICR_OTHER /* INT Cause Active */

#define XLNID_EIMS_ENABLE_MASK ( \
				XLNID_EIMS_RTX_QUEUE	| \
				XLNID_EIMS_LSC		| \
				XLNID_EIMS_TCP_TIMER	| \
				XLNID_EIMS_OTHER)

/* Immediate Interrupt Rx (A.K.A. Low Latency Interrupt) */
#define XLNID_IMIR_SIZE_BP	    0x00001000 /* Packet size bypass */
#define XLNID_IMIR_CTRL_URG	    0x00002000 /* Check URG bit in header */
#define XLNID_IMIR_CTRL_ACK	    0x00004000 /* Check ACK bit in header */
#define XLNID_IMIR_CTRL_PSH 	0x00008000 /* Check PSH bit in header */
#define XLNID_IMIR_CTRL_RST	    0x00010000 /* Check RST bit in header */
#define XLNID_IMIR_CTRL_SYN 	0x00020000 /* Check SYN bit in header */
#define XLNID_IMIR_CTRL_FIN 	0x00040000 /* Check FIN bit in header */
#define XLNID_IMIR_CTRL_BP	        0x00080000 /* Bypass chk of ctrl bits */
#define XLNID_IMIR_LLI_EN		    0x00100000 /* Enables low latency Int */
#define XLNID_IMIR_RX_QUEUE_MASK	0x0000007F /* Rx Queue Mask */
#define XLNID_IMIR_RX_QUEUE_SHIFT	21 /* Rx Queue Shift */
#define XLNID_IMIRVP_PRIORITY_MASK	0x00000007 /* VLAN priority mask */
#define XLNID_IMIRVP_PRIORITY_EN	0x00000008 /* VLAN priority enable */

#define XLNID_MAX_FTQF_FILTERS		128
#define XLNID_FTQF_PROTOCOL_MASK	0x00000003
#define XLNID_FTQF_PROTOCOL_TCP		0x00000000
#define XLNID_FTQF_PROTOCOL_UDP		0x00000001
#define XLNID_FTQF_PROTOCOL_SCTP	2
#define XLNID_FTQF_PRIORITY_MASK	0x00000007
#define XLNID_FTQF_PRIORITY_SHIFT	2
#define XLNID_FTQF_POOL_MASK		0x0000003F
#define XLNID_FTQF_POOL_SHIFT		8
#define XLNID_FTQF_5TUPLE_MASK_MASK	0x0000001F
#define XLNID_FTQF_5TUPLE_MASK_SHIFT	25
#define XLNID_FTQF_SOURCE_ADDR_MASK	0x1E
#define XLNID_FTQF_DEST_ADDR_MASK	0x1D
#define XLNID_FTQF_SOURCE_PORT_MASK	0x1B
#define XLNID_FTQF_DEST_PORT_MASK	0x17
#define XLNID_FTQF_PROTOCOL_COMP_MASK	0x0F
#define XLNID_FTQF_POOL_MASK_EN		0x40000000
#define XLNID_FTQF_QUEUE_ENABLE		0x80000000

/* Interrupt clear mask */
#define XLNID_IRQ_CLEAR_MASK	0xFFFFFFFF

/* Interrupt Vector Allocation Registers */
#define XLNID_IVAR_REG_NUM		64
#define XLNID_IVAR_TXRX_ENTRY		96
#define XLNID_IVAR_RX_ENTRY		64
#define XLNID_IVAR_RX_QUEUE(_i)		(0 + (_i))
#define XLNID_IVAR_TX_QUEUE(_i)		(64 + (_i))
#define XLNID_IVAR_TX_ENTRY		32

#define XLNID_IVAR_TCP_TIMER_INDEX	96 /* 0 based index */
#define XLNID_IVAR_OTHER_CAUSES_INDEX	97 /* 0 based index */

#define XLNID_MSIX_VECTOR(_i)		(0 + (_i))

#define XLNID_IVAR_ALLOC_VAL		0x80 /* Interrupt Allocation valid */

/* ETYPE Queue Filter/Select Bit Masks */
#define XLNID_MAX_ETQF_FILTERS		8
#define XLNID_ETQF_FCOE			0x08000000 /* bit 27 */
#define XLNID_ETQF_BCN			0x10000000 /* bit 28 */
#define XLNID_ETQF_TX_ANTISPOOF		0x20000000 /* bit 29 */
#define XLNID_ETQF_1588			0x40000000 /* bit 30 */
#define XLNID_ETQF_FILTER_EN		0x80000000 /* bit 31 */
#define XLNID_ETQF_POOL_ENABLE		(1 << 26) /* bit 26 */
#define XLNID_ETQF_POOL_SHIFT		20

#define XLNID_ETQS_RX_QUEUE		0x007F0000 /* bits 22:16 */
#define XLNID_ETQS_RX_QUEUE_SHIFT	16
#define XLNID_ETQS_LLI			0x20000000 /* bit 29 */
#define XLNID_ETQS_QUEUE_EN		0x80000000 /* bit 31 */

/*
 * ETQF filter list: one static filter per filter consumer. This is
 *		   to avoid filter collisions later. Add new filters
 *		   here!!
 *
 * Current filters:
 *	EAPOL 802.1x (0x888e): Filter 0
 *	FCoE (0x8906):	 Filter 2
 *	1588 (0x88f7):	 Filter 3
 *	FIP  (0x8914):	 Filter 4
 *	LLDP (0x88CC):	 Filter 5
 *	LACP (0x8809):	 Filter 6
 *	FC   (0x8808):	 Filter 7
 */
#define XLNID_ETQF_FILTER_EAPOL		0
#define XLNID_ETQF_FILTER_FCOE		2
#define XLNID_ETQF_FILTER_1588		3
#define XLNID_ETQF_FILTER_FIP		4
#define XLNID_ETQF_FILTER_LLDP		5
#define XLNID_ETQF_FILTER_LACP		6
#define XLNID_ETQF_FILTER_FC		7
/* VLAN Control Bit Masks */
#define XLNID_VLNCTRL_VET		0x0000FFFF  /* bits 0-15 */
#define XLNID_VLNCTRL_CFI		0x10000000  /* bit 28 */
#define XLNID_VLNCTRL_CFIEN		0x20000000  /* bit 29 */
#define XLNID_VLNCTRL_VFE		0x40000000  /* bit 30 */
#define XLNID_VLNCTRL_VME		0x80000000  /* bit 31 */

/* VLAN pool filtering masks */
#define XLNID_VLVF_VIEN			0x80000000  /* filter is valid */
#define XLNID_VLVF_ENTRIES		64
#define XLNID_VLVF_VLANID_MASK		0x00000FFF
/* Per VF Port VLAN insertion rules */
#define XLNID_VMVIR_VLANA_DEFAULT	0x40000000 /* Always use default VLAN */
#define XLNID_VMVIR_VLANA_NEVER		0x80000000 /* Never insert VLAN tag */

#define XLNID_ETHERNET_IEEE_VLAN_TYPE	0x8100  /* 802.1q protocol */

/* STATUS Bit Masks */
#define XLNID_STATUS_LAN_ID		0x0000000C /* LAN ID */
#define XLNID_STATUS_LAN_ID_SHIFT	2 /* LAN ID Shift*/
#define XLNID_STATUS_GIO		0x00080000 /* GIO Master Ena Status */

#define XLNID_STATUS_LAN_ID_0	0x00000000 /* LAN ID 0 */
#define XLNID_STATUS_LAN_ID_1	0x00000004 /* LAN ID 1 */

/* ESDP Bit Masks */
#define XLNID_ESDP_SDP0		0x00000001 /* SDP0 Data Value */
#define XLNID_ESDP_SDP1		0x00000002 /* SDP1 Data Value */
#define XLNID_ESDP_SDP2		0x00000004 /* SDP2 Data Value */
#define XLNID_ESDP_SDP3		0x00000008 /* SDP3 Data Value */
#define XLNID_ESDP_SDP4		0x00000010 /* SDP4 Data Value */
#define XLNID_ESDP_SDP5		0x00000020 /* SDP5 Data Value */
#define XLNID_ESDP_SDP6		0x00000040 /* SDP6 Data Value */
#define XLNID_ESDP_SDP7		0x00000080 /* SDP7 Data Value */
#define XLNID_ESDP_SDP0_DIR	0x00000100 /* SDP0 IO direction */
#define XLNID_ESDP_SDP1_DIR	0x00000200 /* SDP1 IO direction */
#define XLNID_ESDP_SDP2_DIR	0x00000400 /* SDP1 IO direction */
#define XLNID_ESDP_SDP3_DIR	0x00000800 /* SDP3 IO direction */
#define XLNID_ESDP_SDP4_DIR	0x00001000 /* SDP4 IO direction */
#define XLNID_ESDP_SDP5_DIR	0x00002000 /* SDP5 IO direction */
#define XLNID_ESDP_SDP6_DIR	0x00004000 /* SDP6 IO direction */
#define XLNID_ESDP_SDP7_DIR	0x00008000 /* SDP7 IO direction */
#define XLNID_ESDP_SDP0_NATIVE	0x00010000 /* SDP0 IO mode */
#define XLNID_ESDP_SDP1_NATIVE	0x00020000 /* SDP1 IO mode */


/* LEDCTL Bit Masks */
#define XLNID_LED_IVRT_BASE		0x00000040
#define XLNID_LED_BLINK_BASE		0x00000080
#define XLNID_LED_MODE_MASK_BASE	0x0000000F
#define XLNID_LED_OFFSET(_base, _i)	(_base << (8 * (_i)))
#define XLNID_LED_MODE_SHIFT(_i)	(8*(_i))
#define XLNID_LED_IVRT(_i)	XLNID_LED_OFFSET(XLNID_LED_IVRT_BASE, _i)
#define XLNID_LED_BLINK(_i)	XLNID_LED_OFFSET(XLNID_LED_BLINK_BASE, _i)
#define XLNID_LED_MODE_MASK(_i)	XLNID_LED_OFFSET(XLNID_LED_MODE_MASK_BASE, _i)

/* LED modes */
#define XLNID_LED_LINK	        0x0
#define XLNID_LED_ACTIVITY	    0x1
#define XLNID_LED_SPPED0	    0x2
#define XLNID_LED_SPEED1	    0x3
#define XLNID_LED_LINK_ACTIVE	0x4
#define XLNID_LED_ON		    0xE
#define XLNID_LED_OFF		    0xF

/* GMAC Bit Masks */
#define XLNID_GMAC_PS_MASK      0x00008000
#define XLNID_GMAC_FES_MASK     0x00004000
#define XLNID_GMAC_DM_MASK      0x00000800
#define XLNID_GMAC_LUD_MASK     0x00000100
#define XLNID_GMAC_LM_MASK      0x00001000

/* PHYSTATUS Bit Masks */
#define XLNID_PHYSTATUS_ADDR0   0x3E000000
#define XLNID_PHYSTATUS_LINK0   0x00040000
#define XLNID_PHYSTATUS_SPEED0  0x0000C000
#define XLNID_PHYSTATUS_DUPLEX0 0x00001000

#if 1
/* AUTOC Bit Masks */
#define XLNID_AUTOC_KX4_KX_SUPP_MASK 0xC0000000
#define XLNID_AUTOC_KX4_SUPP	0x80000000
#define XLNID_AUTOC_KX_SUPP	0x40000000
#define XLNID_AUTOC_PAUSE	0x30000000
#define XLNID_AUTOC_ASM_PAUSE	0x20000000
#define XLNID_AUTOC_SYM_PAUSE	0x10000000
#define XLNID_AUTOC_RF		0x08000000
#define XLNID_AUTOC_PD_TMR	0x06000000
#define XLNID_AUTOC_AN_RX_LOOSE	0x01000000
#define XLNID_AUTOC_AN_RX_DRIFT	0x00800000
#define XLNID_AUTOC_AN_RX_ALIGN	0x007C0000
#define XLNID_AUTOC_FECA	0x00040000
#define XLNID_AUTOC_FECR	0x00020000
#define XLNID_AUTOC_KR_SUPP	0x00010000
#define XLNID_AUTOC_AN_RESTART	0x00001000
#define XLNID_AUTOC_FLU		0x00000001
#define XLNID_AUTOC_LMS_SHIFT	13
#define XLNID_AUTOC_LMS_10G_SERIAL	(0x3 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_KX4_KX_KR	(0x4 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_SGMII_1G_100M	(0x5 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_KX4_KX_KR_1G_AN	(0x6 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_KX4_KX_KR_SGMII	(0x7 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_MASK		(0x7 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_1G_LINK_NO_AN	(0x0 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_10G_LINK_NO_AN	(0x1 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_1G_AN		(0x2 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_KX4_AN		(0x4 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_KX4_AN_1G_AN	(0x6 << XLNID_AUTOC_LMS_SHIFT)
#define XLNID_AUTOC_LMS_ATTACH_TYPE	(0x7 << XLNID_AUTOC_10G_PMA_PMD_SHIFT)

#define XLNID_AUTOC_1G_PMA_PMD_MASK	0x00000200
#define XLNID_AUTOC_1G_PMA_PMD_SHIFT	9
#define XLNID_AUTOC_10G_PMA_PMD_MASK	0x00000180
#define XLNID_AUTOC_10G_PMA_PMD_SHIFT	7
#define XLNID_AUTOC_10G_XAUI	(0x0 << XLNID_AUTOC_10G_PMA_PMD_SHIFT)
#define XLNID_AUTOC_10G_KX4	(0x1 << XLNID_AUTOC_10G_PMA_PMD_SHIFT)
#define XLNID_AUTOC_10G_CX4	(0x2 << XLNID_AUTOC_10G_PMA_PMD_SHIFT)
#define XLNID_AUTOC_1G_BX	(0x0 << XLNID_AUTOC_1G_PMA_PMD_SHIFT)
#define XLNID_AUTOC_1G_KX	(0x1 << XLNID_AUTOC_1G_PMA_PMD_SHIFT)
#define XLNID_AUTOC_1G_SFI	(0x0 << XLNID_AUTOC_1G_PMA_PMD_SHIFT)
#define XLNID_AUTOC_1G_KX_BX	(0x1 << XLNID_AUTOC_1G_PMA_PMD_SHIFT)

#define XLNID_AUTOC2_UPPER_MASK	0xFFFF0000
#define XLNID_AUTOC2_10G_SERIAL_PMA_PMD_MASK	0x00030000
#define XLNID_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT	16
#define XLNID_AUTOC2_10G_KR	(0x0 << XLNID_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT)
#define XLNID_AUTOC2_10G_XFI	(0x1 << XLNID_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT)
#define XLNID_AUTOC2_10G_SFI	(0x2 << XLNID_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT)
#define XLNID_AUTOC2_LINK_DISABLE_ON_D3_MASK	0x50000000
#define XLNID_AUTOC2_LINK_DISABLE_MASK		0x70000000

#define XLNID_MACC_FLU		0x00000001
#define XLNID_MACC_FSV_10G	0x00030000
#define XLNID_MACC_FS		0x00040000
#define XLNID_MAC_RX2TX_LPBK	0x00000002
#endif

/* Veto Bit definiton */
#define XLNID_MMNGC_MNG_VETO	0x00000001

/* LINKS Bit Masks */
#define XLNID_XPCS_LINK_UP      0x00001000
#define XLNID_LINKS_KX_AN_COMP	0x80000000
#define XLNID_LINKS_UP		0x40000000
#define XLNID_LINKS_SPEED	0x20000000
#define XLNID_LINKS_MODE	0x18000000
#define XLNID_LINKS_RX_MODE	0x06000000
#define XLNID_LINKS_TX_MODE	0x01800000
#define XLNID_LINKS_XGXS_EN	0x00400000
#define XLNID_LINKS_SGMII_EN	0x02000000
#define XLNID_LINKS_PCS_1G_EN	0x00200000
#define XLNID_LINKS_1G_AN_EN	0x00100000
#define XLNID_LINKS_KX_AN_IDLE	0x00080000
#define XLNID_LINKS_1G_SYNC	0x00040000
#define XLNID_LINKS_10G_ALIGN	0x00020000
#define XLNID_LINKS_10G_LANE_SYNC	0x00017000
#define XLNID_LINKS_TL_FAULT		0x00001000
#define XLNID_LINKS_SIGNAL		0x00000F00

#define XLNID_LINK_UP_TIME		90 /* 9.0 Seconds */
#define XLNID_AUTO_NEG_TIME		45 /* 4.5 Seconds */

#define XLNID_LINKS2_AN_SUPPORTED	0x00000040

/* PCS1GLSTA Bit Masks */
#define XLNID_PCS1GLSTA_LINK_OK		1
#define XLNID_PCS1GLSTA_SYNK_OK		0x10
#define XLNID_PCS1GLSTA_AN_COMPLETE	0x10000
#define XLNID_PCS1GLSTA_AN_PAGE_RX	0x20000
#define XLNID_PCS1GLSTA_AN_TIMED_OUT	0x40000
#define XLNID_PCS1GLSTA_AN_REMOTE_FAULT	0x80000
#define XLNID_PCS1GLSTA_AN_ERROR_RWS	0x100000

#define XLNID_PCS1GANA_SYM_PAUSE	0x80
#define XLNID_PCS1GANA_ASM_PAUSE	0x100

/* PCS1GLCTL Bit Masks */
#define XLNID_PCS1GLCTL_AN_1G_TIMEOUT_EN 0x00040000 /* PCS 1G autoneg to en */
#define XLNID_PCS1GLCTL_FLV_LINK_UP	1
#define XLNID_PCS1GLCTL_FORCE_LINK	0x20
#define XLNID_PCS1GLCTL_LOW_LINK_LATCH	0x40
#define XLNID_PCS1GLCTL_AN_ENABLE	0x10000
#define XLNID_PCS1GLCTL_AN_RESTART	0x20000

/* ANLP1 Bit Masks */
#define XLNID_ANLP1_PAUSE		0x0C00
#define XLNID_ANLP1_SYM_PAUSE		0x0400
#define XLNID_ANLP1_ASM_PAUSE		0x0800
#define XLNID_ANLP1_AN_STATE_MASK	0x000f0000

/* SW Semaphore Register bitmasks */
#define XLNID_SWSM_SMBI		0x00000001 /* Driver Semaphore bit */
#define XLNID_SWSM_SWESMBI	0x00000002 /* FW Semaphore bit */
#define XLNID_SWSM_WMNG		0x00000004 /* Wake MNG Clock */
#define XLNID_SWFW_REGSMP	0x80000000 /* Register Semaphore bit 31 */

/* SW_FW_SYNC/GSSR definitions */
#define XLNID_GSSR_EEP_SM		0x0001
#define XLNID_GSSR_PHY0_SM		0x0002
#define XLNID_GSSR_PHY1_SM		0x0004
#define XLNID_GSSR_MAC_CSR_SM		0x0008
#define XLNID_GSSR_FLASH_SM		0x0010
#define XLNID_GSSR_NVM_UPDATE_SM	0x0200
#define XLNID_GSSR_SW_MNG_SM		0x0400
#define XLNID_GSSR_TOKEN_SM	0x40000000 /* SW bit for shared access */
#define XLNID_GSSR_SHARED_I2C_SM 0x1806 /* Wait for both phys and both I2Cs */
#define XLNID_GSSR_I2C_MASK	0x1800
#define XLNID_GSSR_NVM_PHY_MASK	0xF

/* FW Status register bitmask */
#define XLNID_FWSTS_FWRI	0x00000200 /* Firmware Reset Indication */

/* EEC Register */
#define XLNID_EEC_SK		0x00000001 /* EEPROM Clock */
#define XLNID_EEC_CS		0x00000002 /* EEPROM Chip Select */
#define XLNID_EEC_DI		0x00000004 /* EEPROM Data In */
#define XLNID_EEC_DO		0x00000008 /* EEPROM Data Out */
#define XLNID_EEC_FWE_MASK	0x00000030 /* FLASH Write Enable */
#define XLNID_EEC_FWE_DIS	0x00000010 /* Disable FLASH writes */
#define XLNID_EEC_FWE_EN	0x00000020 /* Enable FLASH writes */
#define XLNID_EEC_FWE_SHIFT	4
#define XLNID_EEC_REQ		0x00000040 /* EEPROM Access Request */
#define XLNID_EEC_GNT		0x00000080 /* EEPROM Access Grant */
#define XLNID_EEC_PRES		0x00000100 /* EEPROM Present */
#define XLNID_EEC_ARD		0x00000200 /* EEPROM Auto Read Done */
#define XLNID_EEC_FLUP		0x00800000 /* Flash update command */
#define XLNID_EEC_SEC1VAL	0x02000000 /* Sector 1 Valid */
#define XLNID_EEC_FLUDONE	0x04000000 /* Flash update done */
/* EEPROM Addressing bits based on type (0-small, 1-large) */
#define XLNID_EEC_ADDR_SIZE	0x00000400
#define XLNID_EEC_SIZE		0x00007800 /* EEPROM Size */
#define XLNID_EERD_MAX_ADDR	0x00003FFF /* EERD alows 14 bits for addr. */

#define XLNID_EEC_SIZE_SHIFT		11
#define XLNID_EEPROM_WORD_SIZE_SHIFT	6
#define XLNID_EEPROM_OPCODE_BITS	8

/* FLA Register */
#define XLNID_FLA_LOCKED	0x00000040

/* Part Number String Length */
#define XLNID_PBANUM_LENGTH	11

/* Checksum and EEPROM pointers */
#define XLNID_PBANUM_PTR_GUARD		0xFAFA
#define XLNID_EEPROM_CHECKSUM		0x3F
#define XLNID_EEPROM_SUM		0xBABA
#define XLNID_EEPROM_CTRL_4		0x45
#define XLNID_EE_CTRL_4_INST_ID		0x10
#define XLNID_EE_CTRL_4_INST_ID_SHIFT	4
#define XLNID_PCIE_ANALOG_PTR		0x03
#define XLNID_ATLAS0_CONFIG_PTR		0x04
#define XLNID_PHY_PTR			0x04
#define XLNID_ATLAS1_CONFIG_PTR		0x05
#define XLNID_OPTION_ROM_PTR		0x05
#define XLNID_PCIE_GENERAL_PTR		0x06
#define XLNID_PCIE_CONFIG0_PTR		0x07
#define XLNID_PCIE_CONFIG1_PTR		0x08
#define XLNID_CORE0_PTR			0x09
#define XLNID_CORE1_PTR			0x0A
#define XLNID_MAC0_PTR			0x0B
#define XLNID_MAC1_PTR			0x0C
#define XLNID_CSR0_CONFIG_PTR		0x0D
#define XLNID_CSR1_CONFIG_PTR		0x0E
#define XLNID_XLNID_PCIE_GENERAL_SIZE	0x24
#define XLNID_PCIE_CONFIG_SIZE		0x08
#define XLNID_EEPROM_LAST_WORD		0x41
#define XLNID_FW_PTR			0x0F
#define XLNID_PBANUM0_PTR		0x15
#define XLNID_PBANUM1_PTR		0x16
#define XLNID_ALT_MAC_ADDR_PTR		0x37
#define XLNID_FREE_SPACE_PTR		0X3E

/* External Thermal Sensor Config */
#define XLNID_ETS_CFG			0x26
#define XLNID_ETS_LTHRES_DELTA_MASK	0x07C0
#define XLNID_ETS_LTHRES_DELTA_SHIFT	6
#define XLNID_ETS_TYPE_MASK		0x0038
#define XLNID_ETS_TYPE_SHIFT		3
#define XLNID_ETS_TYPE_EMC		0x000
#define XLNID_ETS_NUM_SENSORS_MASK	0x0007
#define XLNID_ETS_DATA_LOC_MASK		0x3C00
#define XLNID_ETS_DATA_LOC_SHIFT	10
#define XLNID_ETS_DATA_INDEX_MASK	0x0300
#define XLNID_ETS_DATA_INDEX_SHIFT	8
#define XLNID_ETS_DATA_HTHRESH_MASK	0x00FF

#define XLNID_SAN_MAC_ADDR_PTR		0x28
#define XLNID_DEVICE_CAPS		0x2C

#define XLNID_MAX_MSIX_VECTORS	    0x40
#define XLNID_PCIE_MSIX_CAPS        0xB2

/* MSI-X capability fields masks */
#define XLNID_PCIE_MSIX_TBL_SZ_MASK	0x7FF

/* Legacy EEPROM word offsets */
#define XLNID_ISCSI_BOOT_CAPS		0x0033
#define XLNID_ISCSI_SETUP_PORT_0	0x0030
#define XLNID_ISCSI_SETUP_PORT_1	0x0034

/* EEPROM Commands - SPI */
#define XLNID_EEPROM_MAX_RETRY_SPI	5000 /* Max wait 5ms for RDY signal */
#define XLNID_EEPROM_STATUS_RDY_SPI	0x01
#define XLNID_EEPROM_READ_OPCODE_SPI	0x03  /* EEPROM read opcode */
#define XLNID_EEPROM_WRITE_OPCODE_SPI	0x02  /* EEPROM write opcode */
#define XLNID_EEPROM_A8_OPCODE_SPI	0x08  /* opcode bit-3 = addr bit-8 */
#define XLNID_EEPROM_WREN_OPCODE_SPI	0x06  /* EEPROM set Write Ena latch */
/* EEPROM reset Write Enable latch */
#define XLNID_EEPROM_WRDI_OPCODE_SPI	0x04
#define XLNID_EEPROM_RDSR_OPCODE_SPI	0x05  /* EEPROM read Status reg */
#define XLNID_EEPROM_WRSR_OPCODE_SPI	0x01  /* EEPROM write Status reg */
#define XLNID_EEPROM_ERASE4K_OPCODE_SPI	0x20  /* EEPROM ERASE 4KB */
#define XLNID_EEPROM_ERASE64K_OPCODE_SPI	0xD8  /* EEPROM ERASE 64KB */
#define XLNID_EEPROM_ERASE256_OPCODE_SPI	0xDB  /* EEPROM ERASE 256B */

/* EEPROM Read Register */
#define XLNID_EEPROM_RW_REG_DATA	16 /* data offset in EEPROM read reg */
#define XLNID_EEPROM_RW_REG_DONE	2 /* Offset to READ done bit */
#define XLNID_EEPROM_RW_REG_START	1 /* First bit to start operation */
#define XLNID_EEPROM_RW_ADDR_SHIFT	2 /* Shift to the address bits */
#define XLNID_NVM_POLL_WRITE		1 /* Flag for polling for wr complete */
#define XLNID_NVM_POLL_READ		0 /* Flag for polling for rd complete */

#define NVM_INIT_CTRL_3		0x38
#define NVM_INIT_CTRL_3_LPLU	0x8
#define NVM_INIT_CTRL_3_D10GMP_PORT0 0x40
#define NVM_INIT_CTRL_3_D10GMP_PORT1 0x100

#define XLNID_ETH_LENGTH_OF_ADDRESS	6

#define XLNID_EEPROM_PAGE_SIZE_MAX	128
#define XLNID_EEPROM_RD_BUFFER_MAX_COUNT	256 /* words rd in burst */
#define XLNID_EEPROM_WR_BUFFER_MAX_COUNT	256 /* words wr in burst */
#define XLNID_EEPROM_CTRL_2		1 /* EEPROM CTRL word 2 */
#define XLNID_EEPROM_CCD_BIT		2

#ifndef XLNID_EEPROM_GRANT_ATTEMPTS
#define XLNID_EEPROM_GRANT_ATTEMPTS	1000 /* EEPROM attempts to gain grant */
#endif

#ifndef XLNID_EERD_EEWR_ATTEMPTS
/* Number of 5 microseconds we wait for EERD read and
 * EERW write to complete */
#define XLNID_EERD_EEWR_ATTEMPTS	100000
#endif

#ifndef XLNID_FLUDONE_ATTEMPTS
/* # attempts we wait for flush update to complete */
#define XLNID_FLUDONE_ATTEMPTS		20000
#endif

#define XLNID_PCIE_CTRL2		0x5   /* PCIe Control 2 Offset */
#define XLNID_PCIE_CTRL2_DUMMY_ENABLE	0x8   /* Dummy Function Enable */
#define XLNID_PCIE_CTRL2_LAN_DISABLE	0x2   /* LAN PCI Disable */
#define XLNID_PCIE_CTRL2_DISABLE_SELECT	0x1   /* LAN Disable Select */

#define XLNID_SAN_MAC_ADDR_PORT0_OFFSET		0x0
#define XLNID_SAN_MAC_ADDR_PORT1_OFFSET		0x3
#define XLNID_DEVICE_CAPS_ALLOW_ANY_SFP		0x1
#define XLNID_DEVICE_CAPS_FCOE_OFFLOADS		0x2
#define XLNID_DEVICE_CAPS_NO_CROSSTALK_WR	(1 << 7)
#define XLNID_FW_LESM_PARAMETERS_PTR		0x2
#define XLNID_FW_LESM_STATE_1			0x1
#define XLNID_FW_LESM_STATE_ENABLED		0x8000 /* LESM Enable bit */
#define XLNID_FW_LESM_2_STATES_ENABLED_MASK	0x1F
#define XLNID_FW_LESM_2_STATES_ENABLED		0x12
#define XLNID_FW_LESM_STATE0_10G_ENABLED	0x6FFF
#define XLNID_FW_LESM_STATE1_10G_ENABLED	0x4FFF
#define XLNID_FW_LESM_STATE0_10G_DISABLED	0x0FFF
#define XLNID_FW_LESM_STATE1_10G_DISABLED	0x2FFF
#define XLNID_FW_LESM_PORT0_STATE0_OFFSET	0x2
#define XLNID_FW_LESM_PORT0_STATE1_OFFSET	0x3
#define XLNID_FW_LESM_PORT1_STATE0_OFFSET	0x6
#define XLNID_FW_LESM_PORT1_STATE1_OFFSET	0x7
#define XLNID_FW_PASSTHROUGH_PATCH_CONFIG_PTR	0x4
#define XLNID_FW_PATCH_VERSION_4		0x7
#define XLNID_FCOE_IBA_CAPS_BLK_PTR		0x33 /* iSCSI/FCOE block */
#define XLNID_FCOE_IBA_CAPS_FCOE		0x20 /* FCOE flags */
#define XLNID_ISCSI_FCOE_BLK_PTR		0x17 /* iSCSI/FCOE block */
#define XLNID_ISCSI_FCOE_FLAGS_OFFSET		0x0 /* FCOE flags */
#define XLNID_ISCSI_FCOE_FLAGS_ENABLE		0x1 /* FCOE flags enable bit */
#define XLNID_ALT_SAN_MAC_ADDR_BLK_PTR		0x27 /* Alt. SAN MAC block */
#define XLNID_ALT_SAN_MAC_ADDR_CAPS_OFFSET	0x0 /* Alt SAN MAC capability */
#define XLNID_ALT_SAN_MAC_ADDR_PORT0_OFFSET	0x1 /* Alt SAN MAC 0 offset */
#define XLNID_ALT_SAN_MAC_ADDR_PORT1_OFFSET	0x4 /* Alt SAN MAC 1 offset */
#define XLNID_ALT_SAN_MAC_ADDR_WWNN_OFFSET	0x7 /* Alt WWNN prefix offset */
#define XLNID_ALT_SAN_MAC_ADDR_WWPN_OFFSET	0x8 /* Alt WWPN prefix offset */
#define XLNID_ALT_SAN_MAC_ADDR_CAPS_SANMAC	0x0 /* Alt SAN MAC exists */
#define XLNID_ALT_SAN_MAC_ADDR_CAPS_ALTWWN	0x1 /* Alt WWN base exists */

#define XLNID_DEVICE_CAPS_WOL_PORT0_1	0x4 /* WoL supported on ports 0 & 1 */
#define XLNID_DEVICE_CAPS_WOL_PORT0	0x8 /* WoL supported on port 0 */
#define XLNID_DEVICE_CAPS_WOL_MASK	0xC /* Mask for WoL capabilities */

/* PCI Bus Info */
#define XLNID_PCI_DEVICE_STATUS		0x7A
#define XLNID_PCI_DEVICE_STATUS_TRANSACTION_PENDING	0x0020
#define XLNID_PCI_LINK_STATUS		0x82
#define XLNID_PCI_DEVICE_CONTROL2	0x98
#define XLNID_PCI_LINK_WIDTH		0x3F0
#define XLNID_PCI_LINK_WIDTH_1		0x10
#define XLNID_PCI_LINK_WIDTH_2		0x20
#define XLNID_PCI_LINK_WIDTH_4		0x40
#define XLNID_PCI_LINK_WIDTH_8		0x80
#define XLNID_PCI_LINK_SPEED		0xF
#define XLNID_PCI_LINK_SPEED_2500	0x1
#define XLNID_PCI_LINK_SPEED_5000	0x2
#define XLNID_PCI_LINK_SPEED_8000	0x3
#define XLNID_PCI_HEADER_TYPE_REGISTER	0x0E
#define XLNID_PCI_HEADER_TYPE_MULTIFUNC	0x80
#define XLNID_PCI_DEVICE_CONTROL2_16ms	0x0005

#define XLNID_PCIDEVCTRL2_TIMEO_MASK	0xf
#define XLNID_PCIDEVCTRL2_16_32ms_def	0x0
#define XLNID_PCIDEVCTRL2_50_100us	0x1
#define XLNID_PCIDEVCTRL2_1_2ms		0x2
#define XLNID_PCIDEVCTRL2_16_32ms	0x5
#define XLNID_PCIDEVCTRL2_65_130ms	0x6
#define XLNID_PCIDEVCTRL2_260_520ms	0x9
#define XLNID_PCIDEVCTRL2_1_2s		0xa
#define XLNID_PCIDEVCTRL2_4_8s		0xd
#define XLNID_PCIDEVCTRL2_17_34s	0xe

/* Number of 100 microseconds we wait for PCI Express master disable */
#define XLNID_PCI_MASTER_DISABLE_TIMEOUT	800

/* Check whether address is multicast. This is little-endian specific check.*/
#define XLNID_IS_MULTICAST(Address) \
		(bool)(((u8 *)(Address))[0] & ((u8)0x01))

/* Check whether an address is broadcast. */
#define XLNID_IS_BROADCAST(Address) \
		((((u8 *)(Address))[0] == ((u8)0xff)) && \
		(((u8 *)(Address))[1] == ((u8)0xff)))

/* RAH */
#define XLNID_RAH_VIND_MASK	0x003C0000
#define XLNID_RAH_VIND_SHIFT	18
#define XLNID_RAH_AV		0x80000000
#define XLNID_CLEAR_VMDQ_ALL	0xFFFFFFFF

/* Header split receive */
#define XLNID_RFCTL_ISCSI_DIS		0x00000001
#define XLNID_RFCTL_ISCSI_DWC_MASK	0x0000003E
#define XLNID_RFCTL_ISCSI_DWC_SHIFT	1
#define XLNID_RFCTL_RSC_DIS		0x00000020
#define XLNID_RFCTL_NFSW_DIS		0x00000040
#define XLNID_RFCTL_NFSR_DIS		0x00000080
#define XLNID_RFCTL_NFS_VER_MASK	0x00000300
#define XLNID_RFCTL_NFS_VER_SHIFT	8
#define XLNID_RFCTL_NFS_VER_2		0
#define XLNID_RFCTL_NFS_VER_3		1
#define XLNID_RFCTL_NFS_VER_4		2
#define XLNID_RFCTL_IPV6_DIS		0x00000400
#define XLNID_RFCTL_IPV6_XSUM_DIS	0x00000800
#define XLNID_RFCTL_IPFRSP_DIS		0x00004000
#define XLNID_RFCTL_IPV6_EX_DIS		0x00010000
#define XLNID_RFCTL_NEW_IPV6_EXT_DIS	0x00020000

/* Transmit Config masks */
#define XLNID_TXDCTL_ENABLE		0x02000000 /* Ena specific Tx Queue */
#define XLNID_TXDCTL_SWFLSH		0x04000000 /* Tx Desc. wr-bk flushing */
#define XLNID_TXDCTL_WTHRESH_SHIFT	16 /* shift to WTHRESH bits */
/* Enable short packet padding to 64 bytes */
#define XLNID_TX_PAD_ENABLE		0x00000400
#define XLNID_JUMBO_FRAME_ENABLE	0x00000004  /* Allow jumbo frames */
/* This allows for 16K packets + 4k for vlan */
#define XLNID_MAX_FRAME_SZ		0x40040000

#define XLNID_TDWBAL_HEAD_WB_ENABLE	0x1 /* Tx head write-back enable */
#define XLNID_TDWBAL_SEQNUM_WB_ENABLE	0x2 /* Tx seq# write-back enable */

/* Receive Config masks */
#define XLNID_RXCTRL_RXEN		0x00000001 /* Enable Receiver */
#define XLNID_RXCTRL_DMBYPS		0x00000002 /* Desc Monitor Bypass */
#define XLNID_RXDCTL_ENABLE		0x02000000 /* Ena specific Rx Queue */
#define XLNID_RXDCTL_SWFLSH		0x04000000 /* Rx Desc wr-bk flushing */
#define XLNID_RXDCTL_RLPML_EN		0x00008000
#define XLNID_RXDCTL_PTHRESH    0x10

#define XLNID_TSAUXC_EN_CLK		0x00000004
#define XLNID_TSAUXC_SYNCLK		0x00000008
#define XLNID_TSAUXC_SDP0_INT		0x00000040
#define XLNID_TSAUXC_EN_TT0		0x00000001
#define XLNID_TSAUXC_EN_TT1		0x00000002
#define XLNID_TSAUXC_ST0		0x00000010
#define XLNID_TSAUXC_DISABLE_SYSTIME	0x80000000

#define XLNID_TSSDP_TS_SDP0_SEL_MASK	0x000000C0
#define XLNID_TSSDP_TS_SDP0_CLK0	0x00000080
#define XLNID_TSSDP_TS_SDP0_EN		0x00000100

#define XLNID_TSYNCTXCTL_VALID		0x00000001 /* Tx timestamp valid */
#define XLNID_TSYNCTXCTL_ENABLED	0x00000010 /* Tx timestamping enabled */

#define XLNID_TSYNCRXCTL_VALID		0x00000001 /* Rx timestamp valid */
#define XLNID_TSYNCRXCTL_TYPE_MASK	0x0000000E /* Rx type mask */
#define XLNID_TSYNCRXCTL_TYPE_L2_V2	0x00
#define XLNID_TSYNCRXCTL_TYPE_L4_V1	0x02
#define XLNID_TSYNCRXCTL_TYPE_L2_L4_V2	0x04
#define XLNID_TSYNCRXCTL_TYPE_ALL	0x08
#define XLNID_TSYNCRXCTL_TYPE_EVENT_V2	0x0A
#define XLNID_TSYNCRXCTL_ENABLED	0x00000010 /* Rx Timestamping enabled */
#define XLNID_TSYNCRXCTL_TSIP_UT_EN	0x00800000 /* Rx Timestamp in Packet */
#define XLNID_TSYNCRXCTL_TSIP_UP_MASK	0xFF000000 /* Rx Timestamp UP Mask */

#define XLNID_TSIM_SYS_WRAP		0x00000001
#define XLNID_TSIM_TXTS			0x00000002
#define XLNID_TSIM_TADJ			0x00000080

#define XLNID_TSICR_SYS_WRAP		XLNID_TSIM_SYS_WRAP
#define XLNID_TSICR_TXTS		XLNID_TSIM_TXTS
#define XLNID_TSICR_TADJ		XLNID_TSIM_TADJ

#define XLNID_RXMTRL_V1_CTRLT_MASK	0x000000FF
#define XLNID_RXMTRL_V1_SYNC_MSG	0x00
#define XLNID_RXMTRL_V1_DELAY_REQ_MSG	0x01
#define XLNID_RXMTRL_V1_FOLLOWUP_MSG	0x02
#define XLNID_RXMTRL_V1_DELAY_RESP_MSG	0x03
#define XLNID_RXMTRL_V1_MGMT_MSG	0x04

#define XLNID_RXMTRL_V2_MSGID_MASK	0x0000FF00
#define XLNID_RXMTRL_V2_SYNC_MSG	0x0000
#define XLNID_RXMTRL_V2_DELAY_REQ_MSG	0x0100
#define XLNID_RXMTRL_V2_PDELAY_REQ_MSG	0x0200
#define XLNID_RXMTRL_V2_PDELAY_RESP_MSG	0x0300
#define XLNID_RXMTRL_V2_FOLLOWUP_MSG	0x0800
#define XLNID_RXMTRL_V2_DELAY_RESP_MSG	0x0900
#define XLNID_RXMTRL_V2_PDELAY_FOLLOWUP_MSG 0x0A00
#define XLNID_RXMTRL_V2_ANNOUNCE_MSG	0x0B00
#define XLNID_RXMTRL_V2_SIGNALLING_MSG	0x0C00
#define XLNID_RXMTRL_V2_MGMT_MSG	0x0D00

#define XLNID_FCTRL_SBP		0x00000002 /* Store Bad Packet */
#define XLNID_FCTRL_MPE		0x00000100 /* Multicast Promiscuous Ena*/
#define XLNID_FCTRL_UPE		0x00000200 /* Unicast Promiscuous Ena */
#define XLNID_FCTRL_BAM		0x00000400 /* Broadcast Accept Mode */
#define XLNID_FCTRL_PMCF	0x00001000 /* Pass MAC Control Frames */
#define XLNID_FCTRL_DPF		0x00002000 /* Discard Pause Frame */
/* Receive Priority Flow Control Enable */
#define XLNID_FCTRL_RPFCE	0x00004000
#define XLNID_FCTRL_RFCE	0x00008000 /* Receive Flow Control Ena */
#define XLNID_MFLCN_PMCF	0x00000001 /* Pass MAC Control Frames */
#define XLNID_MFLCN_DPF		0x00000002 /* Discard Pause Frame */
#define XLNID_MFLCN_RPFCE	0x00000004 /* Receive Priority FC Enable */
#define XLNID_MFLCN_RFCE	0x00000008 /* Receive FC Enable */
#define XLNID_MFLCN_RPFCE_MASK	0x00000FF4 /* Rx Priority FC bitmap mask */
#define XLNID_MFLCN_RPFCE_SHIFT	4 /* Rx Priority FC bitmap shift */

/* Multiple Receive Queue Control */
#define XLNID_MRQC_RSSEN	0x00000001  /* RSS Enable */
#define XLNID_MRQC_MRQE_MASK	0xF /* Bits 3:0 */
#define XLNID_MRQC_RT8TCEN	0x00000002 /* 8 TC no RSS */
#define XLNID_MRQC_RT4TCEN	0x00000003 /* 4 TC no RSS */
#define XLNID_MRQC_RTRSS8TCEN	0x00000004 /* 8 TC w/ RSS */
#define XLNID_MRQC_RTRSS4TCEN	0x00000005 /* 4 TC w/ RSS */
#define XLNID_MRQC_VMDQEN	0x00000008 /* VMDq2 64 pools no RSS */
#define XLNID_MRQC_VMDQRSS32EN	0x0000000A /* VMDq2 32 pools w/ RSS */
#define XLNID_MRQC_VMDQRSS64EN	0x0000000B /* VMDq2 64 pools w/ RSS */
#define XLNID_MRQC_VMDQRT8TCEN	0x0000000C /* VMDq2/RT 16 pool 8 TC */
#define XLNID_MRQC_VMDQRT4TCEN	0x0000000D /* VMDq2/RT 32 pool 4 TC */
#define XLNID_MRQC_L3L4TXSWEN	0x00008000 /* Enable L3/L4 Tx switch */
#define XLNID_MRQC_RSS_FIELD_MASK	0xFFFF0000
#define XLNID_MRQC_RSS_FIELD_IPV4_TCP	0x00010000
#define XLNID_MRQC_RSS_FIELD_IPV4	0x00020000
#define XLNID_MRQC_RSS_FIELD_IPV6_EX_TCP 0x00040000
#define XLNID_MRQC_RSS_FIELD_IPV6_EX	0x00080000
#define XLNID_MRQC_RSS_FIELD_IPV6	0x00100000
#define XLNID_MRQC_RSS_FIELD_IPV6_TCP	0x00200000
#define XLNID_MRQC_RSS_FIELD_IPV4_UDP	0x00400000
#define XLNID_MRQC_RSS_FIELD_IPV6_UDP	0x00800000
#define XLNID_MRQC_RSS_FIELD_IPV6_EX_UDP 0x01000000
#define XLNID_MRQC_MULTIPLE_RSS		0x00002000

/* Queue Drop Enable */
#define XLNID_QDE_ENABLE	0x00000001
#define XLNID_QDE_HIDE_VLAN	0x00000002
#define XLNID_QDE_IDX_MASK	0x00007F00
#define XLNID_QDE_IDX_SHIFT	8
#define XLNID_QDE_WRITE		0x00010000
#define XLNID_QDE_READ		0x00020000

#define XLNID_TXD_POPTS_IXSM	0x01 /* Insert IP checksum */
#define XLNID_TXD_POPTS_TXSM	0x02 /* Insert TCP/UDP checksum */
#define XLNID_TXD_CMD_EOP	0x01000000 /* End of Packet */
#define XLNID_TXD_CMD_IFCS	0x02000000 /* Insert FCS (Ethernet CRC) */
#define XLNID_TXD_CMD_IC	0x04000000 /* Insert Checksum */
#define XLNID_TXD_CMD_RS	0x08000000 /* Report Status */
#define XLNID_TXD_CMD_DEXT	0x20000000 /* Desc extension (0 = legacy) */
#define XLNID_TXD_CMD_VLE	0x40000000 /* Add VLAN tag */
#define XLNID_TXD_STAT_DD	0x80000000 /* Descriptor Done */

#define XLNID_RXDADV_IPSEC_STATUS_SECP		0x00020000
#define XLNID_RXDADV_IPSEC_ERROR_INVALID_PROTOCOL 0x08000000
#define XLNID_RXDADV_IPSEC_ERROR_INVALID_LENGTH	0x10000000
#define XLNID_RXDADV_IPSEC_ERROR_AUTH_FAILED	0x18000000
#define XLNID_RXDADV_IPSEC_ERROR_BIT_MASK	0x18000000
/* Multiple Transmit Queue Command Register */
#define XLNID_MTQC_RT_ENA	0x1 /* DCB Enable */
#define XLNID_MTQC_VT_ENA	0x2 /* VMDQ2 Enable */
#define XLNID_MTQC_64Q_1PB	0x0 /* 64 queues 1 pack buffer */
#define XLNID_MTQC_32VF		0x8 /* 4 TX Queues per pool w/32VF's */
#define XLNID_MTQC_64VF		0x4 /* 2 TX Queues per pool w/64VF's */
#define XLNID_MTQC_4TC_4TQ	0x8 /* 4 TC if RT_ENA and VT_ENA */
#define XLNID_MTQC_8TC_8TQ	0xC /* 8 TC if RT_ENA or 8 TQ if VT_ENA */

/* Receive Descriptor bit definitions */
#define XLNID_RXD_STAT_DD	0x01 /* Descriptor Done */
#define XLNID_RXD_STAT_EOP	0x02 /* End of Packet */
#define XLNID_RXD_STAT_FLM	0x04 /* FDir Match */
#define XLNID_RXD_STAT_VP	0x08 /* IEEE VLAN Packet */
#define XLNID_RXDADV_NEXTP_MASK	0x000FFFF0 /* Next Descriptor Index */
#define XLNID_RXDADV_NEXTP_SHIFT	0x00000004
#define XLNID_RXD_STAT_UDPCS	0x10 /* UDP xsum calculated */
#define XLNID_RXD_STAT_L4CS	0x20 /* L4 xsum calculated */
#define XLNID_RXD_STAT_IPCS	0x40 /* IP xsum calculated */
#define XLNID_RXD_STAT_PIF	0x80 /* passed in-exact filter */
#define XLNID_RXD_STAT_CRCV	0x100 /* Speculative CRC Valid */
#define XLNID_RXD_STAT_OUTERIPCS	0x100 /* Cloud IP xsum calculated */
#define XLNID_RXD_STAT_VEXT	0x200 /* 1st VLAN found */
#define XLNID_RXD_STAT_UDPV	0x400 /* Valid UDP checksum */
#define XLNID_RXD_STAT_DYNINT	0x800 /* Pkt caused INT via DYNINT */
#define XLNID_RXD_STAT_LLINT	0x800 /* Pkt caused Low Latency Interrupt */
#define XLNID_RXD_STAT_TSIP	0x08000 /* Time Stamp in packet buffer */
#define XLNID_RXD_STAT_TS	0x10000 /* Time Stamp */
#define XLNID_RXD_STAT_SECP	0x20000 /* Security Processing */
#define XLNID_RXD_STAT_LB	0x40000 /* Loopback Status */
#define XLNID_RXD_STAT_ACK	0x8000 /* ACK Packet indication */
#define XLNID_RXD_ERR_CE	0x01 /* CRC Error */
#define XLNID_RXD_ERR_LE	0x02 /* Length Error */
#define XLNID_RXD_ERR_PE	0x08 /* Packet Error */
#define XLNID_RXD_ERR_OSE	0x10 /* Oversize Error */
#define XLNID_RXD_ERR_USE	0x20 /* Undersize Error */
#define XLNID_RXD_ERR_TCPE	0x40 /* TCP/UDP Checksum Error */
#define XLNID_RXD_ERR_IPE	0x80 /* IP Checksum Error */
#define XLNID_RXDADV_ERR_MASK		0xfff00000 /* RDESC.ERRORS mask */
#define XLNID_RXDADV_ERR_SHIFT		20 /* RDESC.ERRORS shift */
#define XLNID_RXDADV_ERR_OUTERIPER	0x04000000 /* CRC IP Header error */
#define XLNID_RXDADV_ERR_RXE		0x20000000 /* Any MAC Error */
#define XLNID_RXDADV_ERR_FCEOFE		0x80000000 /* FCEOFe/IPE */
#define XLNID_RXDADV_ERR_FCERR		0x00700000 /* FCERR/FDIRERR */
#define XLNID_RXDADV_ERR_FDIR_LEN	0x00100000 /* FDIR Length error */
#define XLNID_RXDADV_ERR_FDIR_DROP	0x00200000 /* FDIR Drop error */
#define XLNID_RXDADV_ERR_FDIR_COLL	0x00400000 /* FDIR Collision error */
#define XLNID_RXDADV_ERR_HBO	0x00800000 /*Header Buffer Overflow */
#define XLNID_RXDADV_ERR_CE	0x01000000 /* CRC Error */
#define XLNID_RXDADV_ERR_LE	0x02000000 /* Length Error */
#define XLNID_RXDADV_ERR_PE	0x08000000 /* Packet Error */
#define XLNID_RXDADV_ERR_OSE	0x10000000 /* Oversize Error */
#define XLNID_RXDADV_ERR_USE	0x20000000 /* Undersize Error */
#define XLNID_RXDADV_ERR_TCPE	0x40000000 /* TCP/UDP Checksum Error */
#define XLNID_RXDADV_ERR_IPE	0x80000000 /* IP Checksum Error */
#define XLNID_RXD_VLAN_ID_MASK	0x0FFF  /* VLAN ID is in lower 12 bits */
#define XLNID_RXD_PRI_MASK	0xE000  /* Priority is in upper 3 bits */
#define XLNID_RXD_PRI_SHIFT	13
#define XLNID_RXD_CFI_MASK	0x1000  /* CFI is bit 12 */
#define XLNID_RXD_CFI_SHIFT	12

#define XLNID_RXDADV_STAT_DD		XLNID_RXD_STAT_DD  /* Done */
#define XLNID_RXDADV_STAT_EOP		XLNID_RXD_STAT_EOP /* End of Packet */
#define XLNID_RXDADV_STAT_FLM		XLNID_RXD_STAT_FLM /* FDir Match */
#define XLNID_RXDADV_STAT_VP		XLNID_RXD_STAT_VP  /* IEEE VLAN Pkt */
#define XLNID_RXDADV_STAT_MASK		0x000fffff /* Stat/NEXTP: bit 0-19 */
#define XLNID_RXDADV_STAT_FCEOFS	0x00000040 /* FCoE EOF/SOF Stat */
#define XLNID_RXDADV_STAT_FCSTAT	0x00000030 /* FCoE Pkt Stat */
#define XLNID_RXDADV_STAT_FCSTAT_NOMTCH	0x00000000 /* 00: No Ctxt Match */
#define XLNID_RXDADV_STAT_FCSTAT_NODDP	0x00000010 /* 01: Ctxt w/o DDP */
#define XLNID_RXDADV_STAT_FCSTAT_FCPRSP	0x00000020 /* 10: Recv. FCP_RSP */
#define XLNID_RXDADV_STAT_FCSTAT_DDP	0x00000030 /* 11: Ctxt w/ DDP */
#define XLNID_RXDADV_STAT_TS		0x00040000 /* IEEE1588 Time Stamp */
#define XLNID_RXDADV_STAT_TSIP		0x00008000 /* Time Stamp in packet buffer */

/* PSRTYPE bit definitions */
#define XLNID_PSRTYPE_TCPHDR	0x00000010
#define XLNID_PSRTYPE_UDPHDR	0x00000020
#define XLNID_PSRTYPE_IPV4HDR	0x00000100
#define XLNID_PSRTYPE_IPV6HDR	0x00000200
#define XLNID_PSRTYPE_L2HDR	0x00001000

/* SRRCTL bit definitions */
#define XLNID_SRRCTL_BSIZEPKT_SHIFT	10 /* so many KBs */
#define XLNID_SRRCTL_BSIZEHDRSIZE_SHIFT	2 /* 64byte resolution (>> 6)
					   * + at bit 8 offset (<< 8)
					   *  = (<< 2)
					   */
#define XLNID_SRRCTL_RDMTS_SHIFT	22
#define XLNID_SRRCTL_RDMTS_MASK		0x01C00000
#define XLNID_SRRCTL_DROP_EN		0x10000000
#define XLNID_SRRCTL_BSIZEPKT_MASK	0x0000007F
#define XLNID_SRRCTL_BSIZEHDR_MASK	0x00003F00
#define XLNID_SRRCTL_DESCTYPE_LEGACY	0x00000000
#define XLNID_SRRCTL_DESCTYPE_ADV_ONEBUF 0x02000000
#define XLNID_SRRCTL_DESCTYPE_HDR_SPLIT	0x04000000
#define XLNID_SRRCTL_DESCTYPE_HDR_REPLICATION_LARGE_PKT 0x08000000
#define XLNID_SRRCTL_DESCTYPE_HDR_SPLIT_ALWAYS 0x0A000000
#define XLNID_SRRCTL_DESCTYPE_MASK	0x0E000000

#define XLNID_RXDPS_HDRSTAT_HDRSP	0x00008000
#define XLNID_RXDPS_HDRSTAT_HDRLEN_MASK	0x000003FF

#define XLNID_RXDADV_RSSTYPE_MASK	0x0000000F
#define XLNID_RXDADV_PKTTYPE_MASK	0x0000FFF0
#define XLNID_RXDADV_PKTTYPE_MASK_EX	0x0001FFF0
#define XLNID_RXDADV_HDRBUFLEN_MASK	0x00007FE0
#define XLNID_RXDADV_RSCCNT_MASK	0x001E0000
#define XLNID_RXDADV_RSCCNT_SHIFT	17
#define XLNID_RXDADV_HDRBUFLEN_SHIFT	5
#define XLNID_RXDADV_SPLITHEADER_EN	0x00001000
#define XLNID_RXDADV_SPH		0x8000

/* RSS Hash results */
#define XLNID_RXDADV_RSSTYPE_NONE	0x00000000
#define XLNID_RXDADV_RSSTYPE_IPV4_TCP	0x00000001
#define XLNID_RXDADV_RSSTYPE_IPV4	0x00000002
#define XLNID_RXDADV_RSSTYPE_IPV6_TCP	0x00000003
#define XLNID_RXDADV_RSSTYPE_IPV6_EX	0x00000004
#define XLNID_RXDADV_RSSTYPE_IPV6	0x00000005
#define XLNID_RXDADV_RSSTYPE_IPV6_TCP_EX 0x00000006
#define XLNID_RXDADV_RSSTYPE_IPV4_UDP	0x00000007
#define XLNID_RXDADV_RSSTYPE_IPV6_UDP	0x00000008
#define XLNID_RXDADV_RSSTYPE_IPV6_UDP_EX 0x00000009

/* RSS Packet Types as indicated in the receive descriptor. */
#define XLNID_RXDADV_PKTTYPE_NONE	0x00000000
#define XLNID_RXDADV_PKTTYPE_IPV4	0x00000010 /* IPv4 hdr present */
#define XLNID_RXDADV_PKTTYPE_IPV4_EX	0x00000020 /* IPv4 hdr + extensions */
#define XLNID_RXDADV_PKTTYPE_IPV6	0x00000040 /* IPv6 hdr present */
#define XLNID_RXDADV_PKTTYPE_IPV6_EX	0x00000080 /* IPv6 hdr + extensions */
#define XLNID_RXDADV_PKTTYPE_TCP	0x00000100 /* TCP hdr present */
#define XLNID_RXDADV_PKTTYPE_UDP	0x00000200 /* UDP hdr present */
#define XLNID_RXDADV_PKTTYPE_SCTP	0x00000400 /* SCTP hdr present */
#define XLNID_RXDADV_PKTTYPE_NFS	0x00000800 /* NFS hdr present */
#define XLNID_RXDADV_PKTTYPE_GENEVE	0x00000800 /* GENEVE hdr present */
#define XLNID_RXDADV_PKTTYPE_VXLAN	0x00000800 /* VXLAN hdr present */
#define XLNID_RXDADV_PKTTYPE_TUNNEL	0x00010000 /* Tunnel type */
#define XLNID_RXDADV_PKTTYPE_IPSEC_ESP	0x00001000 /* IPSec ESP */
#define XLNID_RXDADV_PKTTYPE_IPSEC_AH	0x00002000 /* IPSec AH */
#define XLNID_RXDADV_PKTTYPE_LINKSEC	0x00004000 /* LinkSec Encap */
#define XLNID_RXDADV_PKTTYPE_ETQF	0x00008000 /* PKTTYPE is ETQF index */
#define XLNID_RXDADV_PKTTYPE_ETQF_MASK	0x00000070 /* ETQF has 8 indices */
#define XLNID_RXDADV_PKTTYPE_ETQF_SHIFT	4 /* Right-shift 4 bits */

/* Security Processing bit Indication */
#define XLNID_RXDADV_LNKSEC_STATUS_SECP		0x00020000
#define XLNID_RXDADV_LNKSEC_ERROR_NO_SA_MATCH	0x08000000
#define XLNID_RXDADV_LNKSEC_ERROR_REPLAY_ERROR	0x10000000
#define XLNID_RXDADV_LNKSEC_ERROR_BIT_MASK	0x18000000
#define XLNID_RXDADV_LNKSEC_ERROR_BAD_SIG	0x18000000

/* Masks to determine if packets should be dropped due to frame errors */
#define XLNID_RXD_ERR_FRAME_ERR_MASK ( \
				XLNID_RXD_ERR_CE | \
				XLNID_RXD_ERR_LE | \
				XLNID_RXD_ERR_PE | \
				XLNID_RXD_ERR_OSE | \
				XLNID_RXD_ERR_USE)

#define XLNID_RXDADV_ERR_FRAME_ERR_MASK ( \
				XLNID_RXDADV_ERR_CE | \
				XLNID_RXDADV_ERR_LE | \
				XLNID_RXDADV_ERR_PE | \
				XLNID_RXDADV_ERR_OSE | \
				XLNID_RXDADV_ERR_USE)

/* Multicast bit mask */
#define XLNID_MCSTCTRL_MFE	0x4

/* Number of Transmit and Receive Descriptors must be a multiple of 8 */
#define XLNID_REQ_TX_DESCRIPTOR_MULTIPLE	8
#define XLNID_REQ_RX_DESCRIPTOR_MULTIPLE	8
#define XLNID_REQ_TX_BUFFER_GRANULARITY		1024

/* Vlan-specific macros */
#define XLNID_RX_DESC_SPECIAL_VLAN_MASK	0x0FFF /* VLAN ID in lower 12 bits */
#define XLNID_RX_DESC_SPECIAL_PRI_MASK	0xE000 /* Priority in upper 3 bits */
#define XLNID_RX_DESC_SPECIAL_PRI_SHIFT	0x000D /* Priority in upper 3 of 16 */
#define XLNID_TX_DESC_SPECIAL_PRI_SHIFT	XLNID_RX_DESC_SPECIAL_PRI_SHIFT

/* SR-IOV specific macros */
#define XLNID_MBVFICR_INDEX(vf_number)	(vf_number >> 4)
#define XLNID_MBVFICR(_i)		(0x00710 + ((_i) * 4))
#define XLNID_VFLRE(_i)			(((_i & 1) ? 0x001C0 : 0x00600))
#define XLNID_VFLREC(_i)		 (0x00700 + ((_i) * 4))
/* Translated register #defines */
#define XLNID_PVFCTRL(P)	(0x00300 + (4 * (P)))
#define XLNID_PVFSTATUS(P)	(0x00008 + (0 * (P)))
#define XLNID_PVFLINKS(P)	(0x042A4 + (0 * (P)))
#define XLNID_PVFRTIMER(P)	(0x00048 + (0 * (P)))
#define XLNID_PVFMAILBOX(P)	(0x04C00 + (4 * (P)))
#define XLNID_PVFRXMEMWRAP(P)	(0x03190 + (0 * (P)))
#define XLNID_PVTEICR(P)	(0x00B00 + (4 * (P)))
#define XLNID_PVTEICS(P)	(0x00C00 + (4 * (P)))
#define XLNID_PVTEIMS(P)	(0x00D00 + (4 * (P)))
#define XLNID_PVTEIMC(P)	(0x00E00 + (4 * (P)))
#define XLNID_PVTEIAC(P)	(0x00F00 + (4 * (P)))
#define XLNID_PVTEIAM(P)	(0x04D00 + (4 * (P)))
#define XLNID_PVTEITR(P)	(((P) < 24) ? (0x00820 + ((P) * 4)) : \
				 (0x012300 + (((P) - 24) * 4)))
#define XLNID_PVTIVAR(P)	(0x12500 + (4 * (P)))
#define XLNID_PVTIVAR_MISC(P)	(0x04E00 + (4 * (P)))
#define XLNID_PVTRSCINT(P)	(0x12000 + (4 * (P)))
#define XLNID_VFPBACL(P)	(0x110C8 + (4 * (P)))
#define XLNID_PVFRDBAL(P)	((P < 64) ? (0x01000 + (0x40 * (P))) \
				 : (0x0D000 + (0x40 * ((P) - 64))))
#define XLNID_PVFRDBAH(P)	((P < 64) ? (0x01004 + (0x40 * (P))) \
				 : (0x0D004 + (0x40 * ((P) - 64))))
#define XLNID_PVFRDLEN(P)	((P < 64) ? (0x01008 + (0x40 * (P))) \
				 : (0x0D008 + (0x40 * ((P) - 64))))
#define XLNID_PVFRDH(P)		((P < 64) ? (0x01010 + (0x40 * (P))) \
				 : (0x0D010 + (0x40 * ((P) - 64))))
#define XLNID_PVFRDT(P)		((P < 64) ? (0x01018 + (0x40 * (P))) \
				 : (0x0D018 + (0x40 * ((P) - 64))))
#define XLNID_PVFRXDCTL(P)	((P < 64) ? (0x01028 + (0x40 * (P))) \
				 : (0x0D028 + (0x40 * ((P) - 64))))
#define XLNID_PVFSRRCTL(P)	((P < 64) ? (0x01014 + (0x40 * (P))) \
				 : (0x0D014 + (0x40 * ((P) - 64))))
#define XLNID_PVFPSRTYPE(P)	(0x0EA00 + (4 * (P)))
#define XLNID_PVFTDBAL(P)	(0x06000 + (0x40 * (P)))
#define XLNID_PVFTDBAH(P)	(0x06004 + (0x40 * (P)))
#define XLNID_PVFTDLEN(P)	(0x06008 + (0x40 * (P)))
#define XLNID_PVFTDH(P)		(0x06010 + (0x40 * (P)))
#define XLNID_PVFTDT(P)		(0x06018 + (0x40 * (P)))
#define XLNID_PVFTXDCTL(P)	(0x06028 + (0x40 * (P)))
#define XLNID_PVFTDWBAL(P)	(0x06038 + (0x40 * (P)))
#define XLNID_PVFTDWBAH(P)	(0x0603C + (0x40 * (P)))
#define XLNID_PVFDCA_RXCTRL(P)	(((P) < 64) ? (0x0100C + (0x40 * (P))) \
				 : (0x0D00C + (0x40 * ((P) - 64))))
#define XLNID_PVFDCA_TXCTRL(P)	(0x0600C + (0x40 * (P)))
#define XLNID_PVFGPRC(x)	(0x0101C + (0x40 * (x)))
#define XLNID_PVFGPTC(x)	(0x08300 + (0x04 * (x)))
#define XLNID_PVFGORC_LSB(x)	(0x01020 + (0x40 * (x)))
#define XLNID_PVFGORC_MSB(x)	(0x0D020 + (0x40 * (x)))
#define XLNID_PVFGOTC_LSB(x)	(0x08400 + (0x08 * (x)))
#define XLNID_PVFGOTC_MSB(x)	(0x08404 + (0x08 * (x)))
#define XLNID_PVFMPRC(x)	(0x0D01C + (0x40 * (x)))

#define XLNID_PVFTDWBALn(q_per_pool, vf_number, vf_q_index) \
		(XLNID_PVFTDWBAL((q_per_pool)*(vf_number) + (vf_q_index)))
#define XLNID_PVFTDWBAHn(q_per_pool, vf_number, vf_q_index) \
		(XLNID_PVFTDWBAH((q_per_pool)*(vf_number) + (vf_q_index)))

#define XLNID_PVFTDHn(q_per_pool, vf_number, vf_q_index) \
		(XLNID_PVFTDH((q_per_pool)*(vf_number) + (vf_q_index)))
#define XLNID_PVFTDTn(q_per_pool, vf_number, vf_q_index) \
		(XLNID_PVFTDT((q_per_pool)*(vf_number) + (vf_q_index)))

/* Little Endian defines */
#ifndef __le16
#define __le16  u16
#endif
#ifndef __le32
#define __le32  u32
#endif
#ifndef __le64
#define __le64  u64

#endif
#ifndef __be16
/* Big Endian defines */
#define __be16  u16
#define __be32  u32
#define __be64  u64

#endif
enum xlnid_fdir_pballoc_type {
	XLNID_FDIR_PBALLOC_NONE = 0,
	XLNID_FDIR_PBALLOC_64K  = 1,
	XLNID_FDIR_PBALLOC_128K = 2,
	XLNID_FDIR_PBALLOC_256K = 3,
};

/* Flow Director register values */
#define XLNID_FDIRCTRL_PBALLOC_64K		0x00000001
#define XLNID_FDIRCTRL_PBALLOC_128K		0x00000002
#define XLNID_FDIRCTRL_PBALLOC_256K		0x00000003
#define XLNID_FDIRCTRL_INIT_DONE		0x00000008
#define XLNID_FDIRCTRL_PERFECT_MATCH		0x00000010
#define XLNID_FDIRCTRL_REPORT_STATUS		0x00000020
#define XLNID_FDIRCTRL_REPORT_STATUS_ALWAYS	0x00000080
#define XLNID_FDIRCTRL_DROP_Q_SHIFT		8
#define XLNID_FDIRCTRL_DROP_Q_MASK		0x00007F00
#define XLNID_FDIRCTRL_FLEX_SHIFT		16
#define XLNID_FDIRCTRL_DROP_NO_MATCH		0x00008000
#define XLNID_FDIRCTRL_FILTERMODE_SHIFT		21
#define XLNID_FDIRCTRL_FILTERMODE_MACVLAN	0x0001 /* bit 23:21, 001b */
#define XLNID_FDIRCTRL_FILTERMODE_CLOUD		0x0002 /* bit 23:21, 010b */
#define XLNID_FDIRCTRL_SEARCHLIM		0x00800000
#define XLNID_FDIRCTRL_FILTERMODE_MASK		0x00E00000
#define XLNID_FDIRCTRL_MAX_LENGTH_SHIFT		24
#define XLNID_FDIRCTRL_FULL_THRESH_MASK		0xF0000000
#define XLNID_FDIRCTRL_FULL_THRESH_SHIFT	28

#define XLNID_FDIRTCPM_DPORTM_SHIFT		16
#define XLNID_FDIRUDPM_DPORTM_SHIFT		16
#define XLNID_FDIRIP6M_DIPM_SHIFT		16
#define XLNID_FDIRM_VLANID			0x00000001
#define XLNID_FDIRM_VLANP			0x00000002
#define XLNID_FDIRM_POOL			0x00000004
#define XLNID_FDIRM_L4P				0x00000008
#define XLNID_FDIRM_FLEX			0x00000010
#define XLNID_FDIRM_DIPv6			0x00000020
#define XLNID_FDIRM_L3P				0x00000040

#define XLNID_FDIRIP6M_INNER_MAC	0x03F0 /* bit 9:4 */
#define XLNID_FDIRIP6M_TUNNEL_TYPE	0x0800 /* bit 11 */
#define XLNID_FDIRIP6M_TNI_VNI		0xF000 /* bit 15:12 */
#define XLNID_FDIRIP6M_TNI_VNI_24	0x1000 /* bit 12 */
#define XLNID_FDIRIP6M_ALWAYS_MASK	0x040F /* bit 10, 3:0 */

#define XLNID_FDIRFREE_FREE_MASK		0xFFFF
#define XLNID_FDIRFREE_FREE_SHIFT		0
#define XLNID_FDIRFREE_COLL_MASK		0x7FFF0000
#define XLNID_FDIRFREE_COLL_SHIFT		16
#define XLNID_FDIRLEN_MAXLEN_MASK		0x3F
#define XLNID_FDIRLEN_MAXLEN_SHIFT		0
#define XLNID_FDIRLEN_MAXHASH_MASK		0x7FFF0000
#define XLNID_FDIRLEN_MAXHASH_SHIFT		16
#define XLNID_FDIRUSTAT_ADD_MASK		0xFFFF
#define XLNID_FDIRUSTAT_ADD_SHIFT		0
#define XLNID_FDIRUSTAT_REMOVE_MASK		0xFFFF0000
#define XLNID_FDIRUSTAT_REMOVE_SHIFT		16
#define XLNID_FDIRFSTAT_FADD_MASK		0x00FF
#define XLNID_FDIRFSTAT_FADD_SHIFT		0
#define XLNID_FDIRFSTAT_FREMOVE_MASK		0xFF00
#define XLNID_FDIRFSTAT_FREMOVE_SHIFT		8
#define XLNID_FDIRPORT_DESTINATION_SHIFT	16
#define XLNID_FDIRVLAN_FLEX_SHIFT		16
#define XLNID_FDIRHASH_BUCKET_VALID_SHIFT	15
#define XLNID_FDIRHASH_SIG_SW_INDEX_SHIFT	16

#define XLNID_FDIRCMD_CMD_MASK			0x00000003
#define XLNID_FDIRCMD_CMD_ADD_FLOW		0x00000001
#define XLNID_FDIRCMD_CMD_REMOVE_FLOW		0x00000002
#define XLNID_FDIRCMD_CMD_QUERY_REM_FILT	0x00000003
#define XLNID_FDIRCMD_FILTER_VALID		0x00000004
#define XLNID_FDIRCMD_FILTER_UPDATE		0x00000008
#define XLNID_FDIRCMD_IPv6DMATCH		0x00000010
#define XLNID_FDIRCMD_L4TYPE_UDP		0x00000020
#define XLNID_FDIRCMD_L4TYPE_TCP		0x00000040
#define XLNID_FDIRCMD_L4TYPE_SCTP		0x00000060
#define XLNID_FDIRCMD_IPV6			0x00000080
#define XLNID_FDIRCMD_CLEARHT			0x00000100
#define XLNID_FDIRCMD_DROP			0x00000200
#define XLNID_FDIRCMD_INT			0x00000400
#define XLNID_FDIRCMD_LAST			0x00000800
#define XLNID_FDIRCMD_COLLISION			0x00001000
#define XLNID_FDIRCMD_QUEUE_EN			0x00008000
#define XLNID_FDIRCMD_FLOW_TYPE_SHIFT		5
#define XLNID_FDIRCMD_RX_QUEUE_SHIFT		16
#define XLNID_FDIRCMD_TUNNEL_FILTER_SHIFT	23
#define XLNID_FDIRCMD_VT_POOL_SHIFT		24
#define XLNID_FDIR_INIT_DONE_POLL		10
#define XLNID_FDIRCMD_CMD_POLL			10
#define XLNID_FDIRCMD_TUNNEL_FILTER		0x00800000
#define XLNID_FDIR_DROP_QUEUE			127

/* Manageablility Host Interface defines */
#define XLNID_HI_MAX_BLOCK_BYTE_LENGTH	1792 /* Num of bytes in range */
#define XLNID_HI_MAX_BLOCK_DWORD_LENGTH	448 /* Num of dwords in range */
#define XLNID_HI_COMMAND_TIMEOUT	500 /* Process HI command limit */
#define XLNID_HI_FLASH_ERASE_TIMEOUT	1000 /* Process Erase command limit */
#define XLNID_HI_FLASH_UPDATE_TIMEOUT	5000 /* Process Update command limit */
#define XLNID_HI_FLASH_APPLY_TIMEOUT	0 /* Process Apply command limit */
#define XLNID_HI_PHY_MGMT_REQ_TIMEOUT	2000 /* Wait up to 2 seconds */

/* CEM Support */
#define FW_CEM_HDR_LEN			0x4
#define FW_CEM_CMD_DRIVER_INFO		0xDD
#define FW_CEM_CMD_DRIVER_INFO_LEN	0x5
#define FW_CEM_CMD_RESERVED		0X0
#define FW_CEM_UNUSED_VER		0x0
#define FW_CEM_MAX_RETRIES		3
#define FW_CEM_RESP_STATUS_SUCCESS	0x1
#define FW_CEM_DRIVER_VERSION_SIZE	39 /* +9 would send 48 bytes to fw */
#define FW_READ_SHADOW_RAM_CMD		0x31
#define FW_READ_SHADOW_RAM_LEN		0x6
#define FW_WRITE_SHADOW_RAM_CMD		0x33
#define FW_WRITE_SHADOW_RAM_LEN		0xA /* 8 plus 1 WORD to write */
#define FW_SHADOW_RAM_DUMP_CMD		0x36
#define FW_SHADOW_RAM_DUMP_LEN		0
#define FW_DEFAULT_CHECKSUM		0xFF /* checksum always 0xFF */
#define FW_NVM_DATA_OFFSET		3
#define FW_MAX_READ_BUFFER_SIZE		1024
#define FW_DISABLE_RXEN_CMD		0xDE
#define FW_DISABLE_RXEN_LEN		0x1
#define FW_PHY_MGMT_REQ_CMD		0x20
#define FW_PHY_TOKEN_REQ_CMD		0xA
#define FW_PHY_TOKEN_REQ_LEN		2
#define FW_PHY_TOKEN_REQ		0
#define FW_PHY_TOKEN_REL		1
#define FW_PHY_TOKEN_OK			1
#define FW_PHY_TOKEN_RETRY		0x80
#define FW_PHY_TOKEN_DELAY		5	/* milliseconds */
#define FW_PHY_TOKEN_WAIT		5	/* seconds */
#define FW_PHY_TOKEN_RETRIES ((FW_PHY_TOKEN_WAIT * 1000) / FW_PHY_TOKEN_DELAY)
#define FW_INT_PHY_REQ_CMD		0xB
#define FW_INT_PHY_REQ_LEN		10
#define FW_INT_PHY_REQ_READ		0
#define FW_INT_PHY_REQ_WRITE		1
#define FW_PHY_ACT_REQ_CMD		5
#define FW_PHY_ACT_DATA_COUNT		4
#define FW_PHY_ACT_REQ_LEN		(4 + 4 * FW_PHY_ACT_DATA_COUNT)
#define FW_PHY_ACT_INIT_PHY		1
#define FW_PHY_ACT_SETUP_LINK		2
#define FW_PHY_ACT_LINK_SPEED_10	(1u << 0)
#define FW_PHY_ACT_LINK_SPEED_100	(1u << 1)
#define FW_PHY_ACT_LINK_SPEED_1G	(1u << 2)
#define FW_PHY_ACT_LINK_SPEED_2_5G	(1u << 3)
#define FW_PHY_ACT_LINK_SPEED_5G	(1u << 4)
#define FW_PHY_ACT_LINK_SPEED_10G	(1u << 5)
#define FW_PHY_ACT_LINK_SPEED_20G	(1u << 6)
#define FW_PHY_ACT_LINK_SPEED_25G	(1u << 7)
#define FW_PHY_ACT_LINK_SPEED_40G	(1u << 8)
#define FW_PHY_ACT_LINK_SPEED_50G	(1u << 9)
#define FW_PHY_ACT_LINK_SPEED_100G	(1u << 10)
#define FW_PHY_ACT_SETUP_LINK_PAUSE_SHIFT 16
#define FW_PHY_ACT_SETUP_LINK_PAUSE_MASK (3u << \
					  FW_PHY_ACT_SETUP_LINK_PAUSE_SHIFT)
#define FW_PHY_ACT_SETUP_LINK_PAUSE_NONE 0u
#define FW_PHY_ACT_SETUP_LINK_PAUSE_TX	1u
#define FW_PHY_ACT_SETUP_LINK_PAUSE_RX	2u
#define FW_PHY_ACT_SETUP_LINK_PAUSE_RXTX 3u
#define FW_PHY_ACT_SETUP_LINK_LP	(1u << 18)
#define FW_PHY_ACT_SETUP_LINK_HP	(1u << 19)
#define FW_PHY_ACT_SETUP_LINK_EEE	(1u << 20)
#define FW_PHY_ACT_SETUP_LINK_AN	(1u << 22)
#define FW_PHY_ACT_SETUP_LINK_RSP_DOWN	(1u << 0)
#define FW_PHY_ACT_GET_LINK_INFO	3
#define FW_PHY_ACT_GET_LINK_INFO_EEE	(1u << 19)
#define FW_PHY_ACT_GET_LINK_INFO_FC_TX	(1u << 20)
#define FW_PHY_ACT_GET_LINK_INFO_FC_RX	(1u << 21)
#define FW_PHY_ACT_GET_LINK_INFO_POWER	(1u << 22)
#define FW_PHY_ACT_GET_LINK_INFO_AN_COMPLETE	(1u << 24)
#define FW_PHY_ACT_GET_LINK_INFO_TEMP	(1u << 25)
#define FW_PHY_ACT_GET_LINK_INFO_LP_FC_TX	(1u << 28)
#define FW_PHY_ACT_GET_LINK_INFO_LP_FC_RX	(1u << 29)
#define FW_PHY_ACT_FORCE_LINK_DOWN	4
#define FW_PHY_ACT_FORCE_LINK_DOWN_OFF	(1u << 0)
#define FW_PHY_ACT_PHY_SW_RESET		5
#define FW_PHY_ACT_PHY_HW_RESET		6
#define FW_PHY_ACT_GET_PHY_INFO		7
#define FW_PHY_ACT_UD_2			0x1002
#define FW_PHY_ACT_UD_2_10G_KR_EEE	(1u << 6)
#define FW_PHY_ACT_UD_2_10G_KX4_EEE	(1u << 5)
#define FW_PHY_ACT_UD_2_1G_KX_EEE	(1u << 4)
#define FW_PHY_ACT_UD_2_10G_T_EEE	(1u << 3)
#define FW_PHY_ACT_UD_2_1G_T_EEE	(1u << 2)
#define FW_PHY_ACT_UD_2_100M_TX_EEE	(1u << 1)
#define FW_PHY_ACT_RETRIES		50
#define FW_PHY_INFO_SPEED_MASK		0xFFFu
#define FW_PHY_INFO_ID_HI_MASK		0xFFFF0000u
#define FW_PHY_INFO_ID_LO_MASK		0x0000FFFFu

/* Host Interface Command Structures */

#pragma pack(push, 1)

struct xlnid_hic_hdr {
	u8 cmd;
	u8 buf_len;
	union {
		u8 cmd_resv;
		u8 ret_status;
	} cmd_or_resp;
	u8 checksum;
};

struct xlnid_hic_hdr2_req {
	u8 cmd;
	u8 buf_lenh;
	u8 buf_lenl;
	u8 checksum;
};

struct xlnid_hic_hdr2_rsp {
	u8 cmd;
	u8 buf_lenl;
	u8 buf_lenh_status;	/* 7-5: high bits of buf_len, 4-0: status */
	u8 checksum;
};

union xlnid_hic_hdr2 {
	struct xlnid_hic_hdr2_req req;
	struct xlnid_hic_hdr2_rsp rsp;
};

struct xlnid_hic_drv_info {
	struct xlnid_hic_hdr hdr;
	u8 port_num;
	u8 ver_sub;
	u8 ver_build;
	u8 ver_min;
	u8 ver_maj;
	u8 pad; /* end spacing to ensure length is mult. of dword */
	u16 pad2; /* end spacing to ensure length is mult. of dword2 */
};

struct xlnid_hic_drv_info2 {
	struct xlnid_hic_hdr hdr;
	u8 port_num;
	u8 ver_sub;
	u8 ver_build;
	u8 ver_min;
	u8 ver_maj;
	char driver_string[FW_CEM_DRIVER_VERSION_SIZE];
};

/* These need to be dword aligned */
struct xlnid_hic_read_shadow_ram {
	union xlnid_hic_hdr2 hdr;
	u32 address;
	u16 length;
	u16 pad2;
	u16 data;
	u16 pad3;
};

struct xlnid_hic_write_shadow_ram {
	union xlnid_hic_hdr2 hdr;
	u32 address;
	u16 length;
	u16 pad2;
	u16 data;
	u16 pad3;
};

struct xlnid_hic_disable_rxen {
	struct xlnid_hic_hdr hdr;
	u8  port_number;
	u8  pad2;
	u16 pad3;
};

struct xlnid_hic_phy_token_req {
	struct xlnid_hic_hdr hdr;
	u8 port_number;
	u8 command_type;
	u16 pad;
};

struct xlnid_hic_internal_phy_req {
	struct xlnid_hic_hdr hdr;
	u8 port_number;
	u8 command_type;
	__be16 address;
	u16 rsv1;
	__be32 write_data;
	u16 pad;
};

struct xlnid_hic_internal_phy_resp {
	struct xlnid_hic_hdr hdr;
	__be32 read_data;
};

struct xlnid_hic_phy_activity_req {
	struct xlnid_hic_hdr hdr;
	u8 port_number;
	u8 pad;
	__le16 activity_id;
	__be32 data[FW_PHY_ACT_DATA_COUNT];
};

struct xlnid_hic_phy_activity_resp {
	struct xlnid_hic_hdr hdr;
	__be32 data[FW_PHY_ACT_DATA_COUNT];
};

#pragma pack(pop)

/* Transmit Descriptor - Legacy */
struct xlnid_legacy_tx_desc {
	u64 buffer_addr; /* Address of the descriptor's data buffer */
	union {
		__le32 data;
		struct {
			__le16 length; /* Data buffer length */
			u8 cso; /* Checksum offset */
			u8 cmd; /* Descriptor control */
		} flags;
	} lower;
	union {
		__le32 data;
		struct {
			u8 status; /* Descriptor status */
			u8 css; /* Checksum start */
			__le16 vlan;
		} fields;
	} upper;
};

/* Transmit Descriptor - Advanced */
union xlnid_adv_tx_desc {
	struct {
		__le64 buffer_addr; /* Address of descriptor's data buf */
		__le32 cmd_type_len;
		__le32 olinfo_status;
	} read;
	struct {
		__le64 rsvd; /* Reserved */
		__le32 nxtseq_seed;
		__le32 status;
	} wb;
};

/* Receive Descriptor - Legacy */
struct xlnid_legacy_rx_desc {
	__le64 buffer_addr; /* Address of the descriptor's data buffer */
	__le16 length; /* Length of data DMAed into data buffer */
	__le16 csum; /* Packet checksum */
	u8 status;   /* Descriptor status */
	u8 errors;   /* Descriptor Errors */
	__le16 vlan;
};

/* Receive Descriptor - Advanced */
union xlnid_adv_rx_desc {
	struct {
		__le64 pkt_addr; /* Packet buffer address */
		__le64 hdr_addr; /* Header buffer address */
	} read;
	struct {
		struct {
			union {
				__le32 data;
				struct {
					__le16 pkt_info; /* RSS, Pkt type */
					__le16 hdr_info; /* Splithdr, hdrlen */
				} hs_rss;
			} lo_dword;
			union {
				__le32 rss; /* RSS Hash */
				struct {
					__le16 ip_id; /* IP id */
					__le16 csum; /* Packet Checksum */
				} csum_ip;
			} hi_dword;
		} lower;
		struct {
			__le32 status_error; /* ext status/error */
			__le16 length; /* Packet length */
			__le16 vlan; /* VLAN tag */
		} upper;
	} wb;  /* writeback */
};

/* Context descriptors */
struct xlnid_adv_tx_context_desc {
	__le32 vlan_macip_lens;
	__le32 seqnum_seed;
	__le32 type_tucmd_mlhl;
	__le32 mss_l4len_idx;
};

/* Adv Transmit Descriptor Config Masks */
#define XLNID_ADVTXD_DTALEN_MASK	0x0000FFFF /* Data buf length(bytes) */
#define XLNID_ADVTXD_MAC_LINKSEC	0x00040000 /* Insert LinkSec */
#define XLNID_ADVTXD_MAC_TSTAMP		0x00080000 /* IEEE1588 time stamp */
#define XLNID_ADVTXD_IPSEC_SA_INDEX_MASK 0x000003FF /* IPSec SA index */
#define XLNID_ADVTXD_IPSEC_ESP_LEN_MASK	0x000001FF /* IPSec ESP length */
#define XLNID_ADVTXD_DTYP_MASK		0x00F00000 /* DTYP mask */
#define XLNID_ADVTXD_DTYP_CTXT		0x00200000 /* Adv Context Desc */
#define XLNID_ADVTXD_DTYP_DATA		0x00300000 /* Adv Data Descriptor */
#define XLNID_ADVTXD_DCMD_EOP		XLNID_TXD_CMD_EOP  /* End of Packet */
#define XLNID_ADVTXD_DCMD_IFCS		XLNID_TXD_CMD_IFCS /* Insert FCS */
#define XLNID_ADVTXD_DCMD_RS		XLNID_TXD_CMD_RS /* Report Status */
#define XLNID_ADVTXD_DCMD_DDTYP_ISCSI	0x10000000 /* DDP hdr type or iSCSI */
#define XLNID_ADVTXD_DCMD_DEXT		XLNID_TXD_CMD_DEXT /* Desc ext 1=Adv */
#define XLNID_ADVTXD_DCMD_VLE		XLNID_TXD_CMD_VLE  /* VLAN pkt enable */
#define XLNID_ADVTXD_DCMD_TSE		0x80000000 /* TCP Seg enable */
#define XLNID_ADVTXD_STAT_DD		XLNID_TXD_STAT_DD  /* Descriptor Done */
#define XLNID_ADVTXD_STAT_SN_CRC	0x00000002 /* NXTSEQ/SEED pres in WB */
#define XLNID_ADVTXD_STAT_RSV		0x0000000C /* STA Reserved */
#define XLNID_ADVTXD_IDX_SHIFT		4 /* Adv desc Index shift */
#define XLNID_ADVTXD_CC			0x00000080 /* Check Context */
#define XLNID_ADVTXD_POPTS_SHIFT	8  /* Adv desc POPTS shift */
#define XLNID_ADVTXD_POPTS_IXSM		(XLNID_TXD_POPTS_IXSM << \
					 XLNID_ADVTXD_POPTS_SHIFT)
#define XLNID_ADVTXD_POPTS_TXSM		(XLNID_TXD_POPTS_TXSM << \
					 XLNID_ADVTXD_POPTS_SHIFT)		 
#define XLNID_ADVTXD_POPTS_ISCO_1ST	0x00000000 /* 1st TSO of iSCSI PDU */
#define XLNID_ADVTXD_POPTS_ISCO_MDL	0x00000800 /* Middle TSO of iSCSI PDU */
#define XLNID_ADVTXD_POPTS_ISCO_LAST	0x00001000 /* Last TSO of iSCSI PDU */
/* 1st&Last TSO-full iSCSI PDU */
#define XLNID_ADVTXD_POPTS_ISCO_FULL	0x00001800
#define XLNID_ADVTXD_POPTS_RSV		0x00002000 /* POPTS Reserved */
#define XLNID_ADVTXD_PAYLEN_SHIFT	14 /* Adv desc PAYLEN shift */
#define XLNID_ADVTXD_MACLEN_SHIFT	9  /* Adv ctxt desc mac len shift */
#define XLNID_ADVTXD_VLAN_SHIFT		16  /* Adv ctxt vlan tag shift */
#define XLNID_ADVTXD_TUCMD_IPV4		0x00000400 /* IP Packet Type: 1=IPv4 */
#define XLNID_ADVTXD_TUCMD_IPV6		0x00000000 /* IP Packet Type: 0=IPv6 */
#define XLNID_ADVTXD_TUCMD_L4T_UDP	0x00000000 /* L4 Packet TYPE of UDP */
#define XLNID_ADVTXD_TUCMD_L4T_TCP	0x00000800 /* L4 Packet TYPE of TCP */
#define XLNID_ADVTXD_TUCMD_L4T_SCTP	0x00001000 /* L4 Packet TYPE of SCTP */
#define XLNID_ADVTXD_TUCMD_L4T_RSV	0x00001800 /* RSV L4 Packet TYPE */
#define XLNID_ADVTXD_TUCMD_MKRREQ	0x00002000 /* req Markers and CRC */
//#define XLNID_ADVTXD_POPTS_IPSEC	0x00000400 /* IPSec offload request */
#define XLNID_ADVTXD_POPTS_MACSEC   0x00000400 /* Insert MACSec */
#define XLNID_ADVTXD_TUCMD_IPSEC_TYPE_ESP 0x00002000 /* IPSec Type ESP */
#define XLNID_ADVTXD_TUCMD_IPSEC_ENCRYPT_EN 0x00004000/* ESP Encrypt Enable */
#define XLNID_ADVTXT_TUCMD_FCOE		0x00008000 /* FCoE Frame Type */
#define XLNID_ADVTXD_FCOEF_EOF_MASK	(0x3 << 10) /* FC EOF index */
#define XLNID_ADVTXD_FCOEF_SOF		((1 << 2) << 10) /* FC SOF index */
#define XLNID_ADVTXD_FCOEF_PARINC	((1 << 3) << 10) /* Rel_Off in F_CTL */
#define XLNID_ADVTXD_FCOEF_ORIE		((1 << 4) << 10) /* Orientation End */
#define XLNID_ADVTXD_FCOEF_ORIS		((1 << 5) << 10) /* Orientation Start */
#define XLNID_ADVTXD_FCOEF_EOF_N	(0x0 << 10) /* 00: EOFn */
#define XLNID_ADVTXD_FCOEF_EOF_T	(0x1 << 10) /* 01: EOFt */
#define XLNID_ADVTXD_FCOEF_EOF_NI	(0x2 << 10) /* 10: EOFni */
#define XLNID_ADVTXD_FCOEF_EOF_A	(0x3 << 10) /* 11: EOFa */
#define XLNID_ADVTXD_L4LEN_SHIFT	8  /* Adv ctxt L4LEN shift */
#define XLNID_ADVTXD_MSS_SHIFT		16  /* Adv ctxt MSS shift */

#define XLNID_ADVTXD_OUTER_IPLEN	16 /* Adv ctxt OUTERIPLEN shift */
#define XLNID_ADVTXD_TUNNEL_LEN 	24 /* Adv ctxt TUNNELLEN shift */
#define XLNID_ADVTXD_TUNNEL_TYPE_SHIFT	16 /* Adv Tx Desc Tunnel Type shift */
#define XLNID_ADVTXD_OUTERIPCS_SHIFT	17 /* Adv Tx Desc OUTERIPCS Shift */
#define XLNID_ADVTXD_TUNNEL_TYPE_NVGRE	1  /* Adv Tx Desc Tunnel Type NVGRE */

/* Autonegotiation advertised speeds */
typedef u32 xlnid_autoneg_advertised;
/* Link speed */
typedef u32 xlnid_link_speed;
#define XLNID_LINK_SPEED_UNKNOWN	0
#define XLNID_LINK_SPEED_10_HALF    0x0001
#define XLNID_LINK_SPEED_10_FULL	0x0002
#define XLNID_LINK_SPEED_100_HALF   0x0004
#define XLNID_LINK_SPEED_100_FULL	0x0008
#define XLNID_LINK_SPEED_1GB_HALF   0x0010
#define XLNID_LINK_SPEED_1GB_FULL	0x0020
#define XLNID_LINK_SPEED_2_5GB_FULL	0x0400
#define XLNID_LINK_SPEED_5GB_FULL	0x0800
#define XLNID_LINK_SPEED_10GB_FULL	0x0080
#define WESTLAKE_LINK_SPEED_AUTONEG	(XLNID_LINK_SPEED_10_HALF | \
                     XLNID_LINK_SPEED_10_FULL | \
                     XLNID_LINK_SPEED_100_HALF | \
                     XLNID_LINK_SPEED_100_FULL | \
					 XLNID_LINK_SPEED_1GB_FULL)
#define WESTLAKE_LINK_SPEED_RMII    (XLNID_LINK_SPEED_10_HALF | \
                     XLNID_LINK_SPEED_10_FULL | \
                     XLNID_LINK_SPEED_100_HALF | \
                     XLNID_LINK_SPEED_100_FULL)		

/* Link mode */
typedef u32 xlnid_link_mode;
#define XLNID_LINK_MODE_GEPHY	     0
#define XLNID_LINK_MODE_1000BASEX	 0x1



/* Physical layer type */
typedef u64 xlnid_physical_layer;
#define XLNID_PHYSICAL_LAYER_UNKNOWN		0
#define XLNID_PHYSICAL_LAYER_10GBASE_T		0x00001
#define XLNID_PHYSICAL_LAYER_1000BASE_T		0x00002
#define XLNID_PHYSICAL_LAYER_100BASE_TX		0x00004
#define XLNID_PHYSICAL_LAYER_SFP_PLUS_CU	0x00008
#define XLNID_PHYSICAL_LAYER_10GBASE_LR		0x00010
#define XLNID_PHYSICAL_LAYER_10GBASE_LRM	0x00020
#define XLNID_PHYSICAL_LAYER_10GBASE_SR		0x00040
#define XLNID_PHYSICAL_LAYER_10GBASE_KX4	0x00080
#define XLNID_PHYSICAL_LAYER_10GBASE_CX4	0x00100
#define XLNID_PHYSICAL_LAYER_1000BASE_KX	0x00200
#define XLNID_PHYSICAL_LAYER_1000BASE_BX	0x00400
#define XLNID_PHYSICAL_LAYER_10GBASE_KR		0x00800
#define XLNID_PHYSICAL_LAYER_10GBASE_XAUI	0x01000
#define XLNID_PHYSICAL_LAYER_SFP_ACTIVE_DA	0x02000
#define XLNID_PHYSICAL_LAYER_1000BASE_SX	0x04000
#define XLNID_PHYSICAL_LAYER_10BASE_T		0x08000
#define XLNID_PHYSICAL_LAYER_2500BASE_KX	0x10000

/* Flow Control Data Sheet defined values
 * Calculation and defines taken from 802.1bb Annex O
 */

/* BitTimes (BT) conversion */
#define XLNID_BT2KB(BT)		((BT + (8 * 1024 - 1)) / (8 * 1024))
#define XLNID_B2BT(BT)		(BT * 8)

/* Calculate Delay to respond to PFC */
#define XLNID_PFC_D	672

/* Calculate Cable Delay */
#define XLNID_CABLE_DC	5556 /* Delay Copper */
#define XLNID_CABLE_DO	5000 /* Delay Optical */

/* Calculate Delay incurred from higher layer */
#define XLNID_HD	6144

/* Calculate PCI Bus delay for low thresholds */
#define XLNID_PCI_DELAY	10000

/* Calculate delay value in bit times */
#define XLNID_DV(_max_frame_link, _max_frame_tc) \
			((36 * \
			  (XLNID_B2BT(_max_frame_link) + \
			   XLNID_PFC_D + \
			   (2 * XLNID_CABLE_DC) + \
			   (2 * XLNID_ID) + \
			   XLNID_HD) / 25 + 1) + \
			 2 * XLNID_B2BT(_max_frame_tc))

/* Software ATR hash keys */
#define XLNID_ATR_BUCKET_HASH_KEY	0x3DAD14E2
#define XLNID_ATR_SIGNATURE_HASH_KEY	0x174D3614

/* Software ATR input stream values and masks */
#define XLNID_ATR_HASH_MASK		0x7fff
#define XLNID_ATR_L4TYPE_MASK		0x3
#define XLNID_ATR_L4TYPE_UDP		0x1
#define XLNID_ATR_L4TYPE_TCP		0x2
#define XLNID_ATR_L4TYPE_SCTP		0x3
#define XLNID_ATR_L4TYPE_IPV6_MASK	0x4
#define XLNID_ATR_L4TYPE_TUNNEL_MASK	0x10
enum xlnid_atr_flow_type {
	XLNID_ATR_FLOW_TYPE_IPV4	= 0x0,
	XLNID_ATR_FLOW_TYPE_UDPV4	= 0x1,
	XLNID_ATR_FLOW_TYPE_TCPV4	= 0x2,
	XLNID_ATR_FLOW_TYPE_SCTPV4	= 0x3,
	XLNID_ATR_FLOW_TYPE_IPV6	= 0x4,
	XLNID_ATR_FLOW_TYPE_UDPV6	= 0x5,
	XLNID_ATR_FLOW_TYPE_TCPV6	= 0x6,
	XLNID_ATR_FLOW_TYPE_SCTPV6	= 0x7,
	XLNID_ATR_FLOW_TYPE_TUNNELED_IPV4	= 0x10,
	XLNID_ATR_FLOW_TYPE_TUNNELED_UDPV4	= 0x11,
	XLNID_ATR_FLOW_TYPE_TUNNELED_TCPV4	= 0x12,
	XLNID_ATR_FLOW_TYPE_TUNNELED_SCTPV4	= 0x13,
	XLNID_ATR_FLOW_TYPE_TUNNELED_IPV6	= 0x14,
	XLNID_ATR_FLOW_TYPE_TUNNELED_UDPV6	= 0x15,
	XLNID_ATR_FLOW_TYPE_TUNNELED_TCPV6	= 0x16,
	XLNID_ATR_FLOW_TYPE_TUNNELED_SCTPV6	= 0x17,
};

/* Flow Director ATR input struct. */
union xlnid_atr_input {
	/*
	 * Byte layout in order, all values with MSB first:
	 *
	 * vm_pool	- 1 byte
	 * flow_type	- 1 byte
	 * vlan_id	- 2 bytes
	 * src_ip	- 16 bytes
	 * inner_mac	- 6 bytes
	 * cloud_mode	- 2 bytes
	 * tni_vni	- 4 bytes
	 * dst_ip	- 16 bytes
	 * src_port	- 2 bytes
	 * dst_port	- 2 bytes
	 * flex_bytes	- 2 bytes
	 * bkt_hash	- 2 bytes
	 */
	struct {
		u8 vm_pool;
		u8 flow_type;
		__be16 vlan_id;
		__be32 dst_ip[4];
		__be32 src_ip[4];
		u8 inner_mac[6];
		__be16 tunnel_type;
		__be32 tni_vni;
		__be16 src_port;
		__be16 dst_port;
		__be16 flex_bytes;
		__be16 bkt_hash;
	} formatted;
	__be32 dword_stream[14];
};

/* Flow Director compressed ATR hash input struct */
union xlnid_atr_hash_dword {
	struct {
		u8 vm_pool;
		u8 flow_type;
		__be16 vlan_id;
	} formatted;
	__be32 ip;
	struct {
		__be16 src;
		__be16 dst;
	} port;
	__be16 flex_bytes;
	__be32 dword;
};

#define XLNID_MVALS_INIT(m)	\
	XLNID_CAT(FLA, m),		\
	XLNID_CAT(GRC, m),		\
	XLNID_CAT(SRAMREL, m),		\
	XLNID_CAT(FACTPS, m),		\
	XLNID_CAT(SWSM, m),		\
	XLNID_CAT(SWFW_SYNC, m),	\
	XLNID_CAT(FWSM, m),		\
	XLNID_CAT(CIAA, m),		\
	XLNID_CAT(CIAD, m)

enum xlnid_mvals {
	XLNID_MVALS_INIT(_IDX),
	XLNID_MVALS_IDX_LIMIT
};

/*
 * Unavailable: The FCoE Boot Option ROM is not present in the flash.
 * Disabled: Present; boot order is not set for any targets on the port.
 * Enabled: Present; boot order is set for at least one target on the port.
 */
enum xlnid_fcoe_boot_status {
	xlnid_fcoe_bootstatus_disabled = 0,
	xlnid_fcoe_bootstatus_enabled = 1,
	xlnid_fcoe_bootstatus_unavailable = 0xFFFF
};

enum xlnid_eeprom_type {
	xlnid_eeprom_uninitialized = 0,
	xlnid_eeprom_spi,
	xlnid_flash,
	xlnid_eeprom_none /* No NVM support */
};

enum xlnid_mac_type {
    xlnid_mac_unknown = 0,
	xlnid_mac_SKYLAKE,
	xlnid_mac_WESTLAKE,
	xlnid_num_macs
};

enum xlnid_phy_type {
	xlnid_phy_unknown = 0,
	xlnid_phy_none,
	xlnid_phy_tn,
	xlnid_phy_aq,
	xlnid_phy_ext_1g_t,
	xlnid_phy_cu_unknown,
	xlnid_phy_qt,
	xlnid_phy_xaui,
	xlnid_phy_nl,
	xlnid_phy_sfp_passive_tyco,
	xlnid_phy_sfp_passive_unknown,
	xlnid_phy_sfp_active_unknown,
	xlnid_phy_sfp_avago,
	xlnid_phy_sfp_ftl,
	xlnid_phy_sfp_ftl_active,
	xlnid_phy_sfp_unknown,
	xlnid_phy_qsfp_passive_unknown,
	xlnid_phy_qsfp_active_unknown,
	xlnid_phy_qsfp_unknown,
	xlnid_phy_sfp_unsupported, /*Enforce bit set with unsupported module*/
	xlnid_phy_sgmii,
	xlnid_phy_fw,
	xlnid_phy_generic
};

/*
 * SFP+ module type IDs:
 *
 * ID	Module Type
 * =============
 * 0	SFP_DA_CU
 * 1	SFP_SR
 * 2	SFP_LR
 * 3	SFP_DA_CU_CORE0
 * 4	SFP_DA_CU_CORE1
 * 5	SFP_SR/LR_CORE0
 * 6	SFP_SR/LR_CORE1
 */
enum xlnid_sfp_type {
	xlnid_sfp_type_da_cu = 0,
	xlnid_sfp_type_sr = 1,
	xlnid_sfp_type_lr = 2,
	xlnid_sfp_type_da_cu_core0 = 3,
	xlnid_sfp_type_da_cu_core1 = 4,
	xlnid_sfp_type_srlr_core0 = 5,
	xlnid_sfp_type_srlr_core1 = 6,
	xlnid_sfp_type_da_act_lmt_core0 = 7,
	xlnid_sfp_type_da_act_lmt_core1 = 8,
	xlnid_sfp_type_1g_cu_core0 = 9,
	xlnid_sfp_type_1g_cu_core1 = 10,
	xlnid_sfp_type_1g_sx_core0 = 11,
	xlnid_sfp_type_1g_sx_core1 = 12,
	xlnid_sfp_type_1g_lx_core0 = 13,
	xlnid_sfp_type_1g_lx_core1 = 14,
	xlnid_sfp_type_not_present = 0xFFFE,
	xlnid_sfp_type_unknown = 0xFFFF
};

enum xlnid_media_type {
	xlnid_media_type_unknown = 0,
	xlnid_media_type_fiber,
	xlnid_media_type_fiber_qsfp,
	xlnid_media_type_copper,
	xlnid_media_type_backplane,
	xlnid_media_type_cx4,
	xlnid_media_type_virtual
};

/* Flow Control Settings */
enum xlnid_fc_mode {
	xlnid_fc_none = 0,
	xlnid_fc_rx_pause,
	xlnid_fc_tx_pause,
	xlnid_fc_full,
	xlnid_fc_default
};

/* Smart Speed Settings */
#define XLNID_SMARTSPEED_MAX_RETRIES	3
enum xlnid_smart_speed {
	xlnid_smart_speed_auto = 0,
	xlnid_smart_speed_on,
	xlnid_smart_speed_off
};

/* PCI bus types */
enum xlnid_bus_type {
	xlnid_bus_type_unknown = 0,
	xlnid_bus_type_pci,
	xlnid_bus_type_pcix,
	xlnid_bus_type_pci_express,
	xlnid_bus_type_internal,
	xlnid_bus_type_reserved
};

/* PCI bus speeds */
enum xlnid_bus_speed {
	xlnid_bus_speed_unknown	= 0,
	xlnid_bus_speed_33	= 33,
	xlnid_bus_speed_66	= 66,
	xlnid_bus_speed_100	= 100,
	xlnid_bus_speed_120	= 120,
	xlnid_bus_speed_133	= 133,
	xlnid_bus_speed_2500	= 2500,
	xlnid_bus_speed_5000	= 5000,
	xlnid_bus_speed_8000	= 8000,
	xlnid_bus_speed_reserved
};

/* PCI bus widths */
enum xlnid_bus_width {
	xlnid_bus_width_unknown	= 0,
	xlnid_bus_width_pcie_x1	= 1,
	xlnid_bus_width_pcie_x2	= 2,
	xlnid_bus_width_pcie_x4	= 4,
	xlnid_bus_width_pcie_x8	= 8,
	xlnid_bus_width_32	= 32,
	xlnid_bus_width_64	= 64,
	xlnid_bus_width_reserved
};

struct xlnid_addr_filter_info {
	u32 num_mc_addrs;
	u32 rar_used_count;
	u32 mta_in_use;
	u32 overflow_promisc;
	bool user_set_promisc;
};

/* Bus parameters */
struct xlnid_bus_info {
	enum xlnid_bus_speed speed;
	enum xlnid_bus_width width;
	enum xlnid_bus_type type;

	u16 func;
	u8 lan_id;
	u16 instance_id;
};

/* Flow control parameters */
struct xlnid_fc_info {
	u32 high_water[XLNID_DCB_MAX_TRAFFIC_CLASS]; /* Flow Ctrl High-water */
	u32 low_water[XLNID_DCB_MAX_TRAFFIC_CLASS]; /* Flow Ctrl Low-water */
	u16 pause_time; /* Flow Control Pause timer */
	bool send_xon; /* Flow control send XON */
	bool strict_ieee; /* Strict IEEE mode */
	bool disable_fc_autoneg; /* Do not autonegotiate FC */
	bool fc_was_autonegged; /* Is current_mode the result of autonegging? */
	enum xlnid_fc_mode current_mode; /* FC mode in effect */
	enum xlnid_fc_mode requested_mode; /* FC mode requested by caller */
};

/* Statistics counters collected by the MAC */
struct xlnid_hw_stats {
	u64 crcerrs;
	u64 illerrc;
	u64 errbc;
	u64 mspdc;
	u64 mpctotal;
	u64 mpc[8];
	u64 mlfc;
	u64 mrfc;
	u64 rlec;
	u64 lxontxc;
	u64 lxonrxc;
	u64 lxofftxc;
	u64 lxoffrxc;
	u64 pxontxc[8];
	u64 pxonrxc[8];
	u64 pxofftxc[8];
	u64 pxoffrxc[8];
	u64 prc64;
	u64 prc127;
	u64 prc255;
	u64 prc511;
	u64 prc1023;
	u64 prc1522;
	u64 gprc;
	u64 bprc;
	u64 mprc;
	u64 gptc;
	u64 gorc;
	u64 gotc;
	u64 rnbc[8];
	u64 ruc;
	u64 rfc;
	u64 roc;
	u64 rjc;
	u64 mngprc;
	u64 mngpdc;
	u64 mngptc;
	u64 tor;
	u64 tpr;
	u64 tpt;
	u64 ptc64;
	u64 ptc127;
	u64 ptc255;
	u64 ptc511;
	u64 ptc1023;
	u64 ptc1522;
	u64 mptc;
	u64 bptc;
	u64 xec;
	u64 qprc[16];
	u64 qptc[16];
	u64 qbrc[16];
	u64 qbtc[16];
	u64 qprdc[16];
	u64 pxon2offc[8];
	u64 fdirustat_add;
	u64 fdirustat_remove;
	u64 fdirfstat_fadd;
	u64 fdirfstat_fremove;
	u64 fdirmatch;
	u64 fdirmiss;
	u64 fccrc;
	u64 fclast;
	u64 fcoerpdc;
	u64 fcoeprc;
	u64 fcoeptc;
	u64 fcoedwrc;
	u64 fcoedwtc;
	u64 fcoe_noddp;
	u64 fcoe_noddp_ext_buff;
	u64 ldpcec;
	u64 pcrc8ec;
	u64 b2ospc;
	u64 b2ogprc;
	u64 o2bgptc;
	u64 o2bspc;
};

/* forward declaration */
struct xlnid_hw;

/* iterator type for walking multicast address lists */
typedef u8* (*xlnid_mc_addr_itr) (struct xlnid_hw *hw, u8 **mc_addr_ptr,
				  u32 *vmdq);

/* Function pointer table */
struct xlnid_eeprom_operations {
	s32 (*init_params)(struct xlnid_hw *);
	s32 (*read)(struct xlnid_hw *, u16, u16 *);
	s32 (*read_buffer)(struct xlnid_hw *, u16, u16, u16 *);
	s32 (*write)(struct xlnid_hw *, u16, u16);
	s32 (*write_buffer)(struct xlnid_hw *, u16, u16, u16 *);
	s32 (*validate_checksum)(struct xlnid_hw *, u16 *);
	s32 (*update_checksum)(struct xlnid_hw *);
	s32 (*calc_checksum)(struct xlnid_hw *);
};

struct xlnid_mac_operations {
	s32 (*init_hw)(struct xlnid_hw *);
	s32 (*reset_hw)(struct xlnid_hw *);
	s32 (*start_hw)(struct xlnid_hw *);
	s32 (*clear_hw_cntrs)(struct xlnid_hw *);
	enum xlnid_media_type (*get_media_type)(struct xlnid_hw *);
	u64 (*get_supported_physical_layer)(struct xlnid_hw *);
	s32 (*get_mac_addr)(struct xlnid_hw *, u8 *);
	s32 (*get_san_mac_addr)(struct xlnid_hw *, u8 *);
	s32 (*set_san_mac_addr)(struct xlnid_hw *, u8 *);
	s32 (*get_device_caps)(struct xlnid_hw *, u16 *);
	s32 (*get_wwn_prefix)(struct xlnid_hw *, u16 *, u16 *);
	s32 (*get_fcoe_boot_status)(struct xlnid_hw *, u16 *);
	s32 (*stop_adapter)(struct xlnid_hw *);
	s32 (*get_bus_info)(struct xlnid_hw *);
	void (*set_lan_id)(struct xlnid_hw *);
	s32 (*read_analog_reg8)(struct xlnid_hw*, u32, u8*);
	s32 (*write_analog_reg8)(struct xlnid_hw*, u32, u8);
	s32 (*setup_sfp)(struct xlnid_hw *);
	s32 (*enable_rx_dma)(struct xlnid_hw *, u32);
	s32 (*disable_sec_rx_path)(struct xlnid_hw *);
	s32 (*enable_sec_rx_path)(struct xlnid_hw *);
	s32 (*acquire_swfw_sync)(struct xlnid_hw *, u32);
	void (*release_swfw_sync)(struct xlnid_hw *, u32);
	void (*init_swfw_sync)(struct xlnid_hw *);
	s32 (*prot_autoc_read)(struct xlnid_hw *, bool *, u32 *);
	s32 (*prot_autoc_write)(struct xlnid_hw *, u32, bool);

	/* Link */
	void (*disable_tx_laser)(struct xlnid_hw *);
	void (*enable_tx_laser)(struct xlnid_hw *);
	void (*flap_tx_laser)(struct xlnid_hw *);
	s32 (*setup_link)(struct xlnid_hw *, xlnid_link_speed, bool);
	s32 (*setup_mac_link)(struct xlnid_hw *, xlnid_link_speed, bool);
	s32 (*check_link)(struct xlnid_hw *, xlnid_link_speed *, bool *, bool);
	s32 (*get_link_capabilities)(struct xlnid_hw *, xlnid_link_speed *,
				     bool *);
	void (*set_rate_select_speed)(struct xlnid_hw *, xlnid_link_speed);

	/* Packet Buffer manipulation */
	void (*setup_rxpba)(struct xlnid_hw *, int, u32, int);

	/* LED */
	s32 (*led_on)(struct xlnid_hw *, u32);
	s32 (*led_off)(struct xlnid_hw *, u32);
	s32 (*blink_led_start)(struct xlnid_hw *, u32);
	s32 (*blink_led_stop)(struct xlnid_hw *, u32);
	s32 (*init_led_link_act)(struct xlnid_hw *);

	/* RAR, Multicast, VLAN */
	s32 (*set_rar)(struct xlnid_hw *, u32, u8 *, u32, u32);
	s32 (*set_uc_addr)(struct xlnid_hw *, u32, u8 *);
	s32 (*clear_rar)(struct xlnid_hw *, u32);
	s32 (*insert_mac_addr)(struct xlnid_hw *, u8 *, u32);
	s32 (*set_vmdq)(struct xlnid_hw *, u32, u32);
	s32 (*set_vmdq_san_mac)(struct xlnid_hw *, u32);
	s32 (*clear_vmdq)(struct xlnid_hw *, u32, u32);
	s32 (*init_rx_addrs)(struct xlnid_hw *);
	s32 (*update_uc_addr_list)(struct xlnid_hw *, u8 *, u32,
				   xlnid_mc_addr_itr);
	s32 (*update_mc_addr_list)(struct xlnid_hw *, u8 *, u32,
				   xlnid_mc_addr_itr, bool clear);
	s32 (*enable_mc)(struct xlnid_hw *);
	s32 (*disable_mc)(struct xlnid_hw *);
	s32 (*clear_vfta)(struct xlnid_hw *);
	s32 (*set_vfta)(struct xlnid_hw *, u32, u32, bool, bool);
	s32 (*set_vlvf)(struct xlnid_hw *, u32, u32, bool, u32 *, u32,
			bool);
	s32 (*init_uta_tables)(struct xlnid_hw *);
	void (*set_mac_anti_spoofing)(struct xlnid_hw *, bool, int);
	void (*set_vlan_anti_spoofing)(struct xlnid_hw *, bool, int);

	/* Flow Control */
	s32 (*fc_enable)(struct xlnid_hw *);
	s32 (*setup_fc)(struct xlnid_hw *);
	void (*fc_autoneg)(struct xlnid_hw *);

	/* Manageability interface */
	s32 (*set_fw_drv_ver)(struct xlnid_hw *, u8, u8, u8, u8, u16,
			      const char *);
	s32 (*get_thermal_sensor_data)(struct xlnid_hw *);
	s32 (*init_thermal_sensor_thresh)(struct xlnid_hw *hw);
	void (*get_rtrup2tc)(struct xlnid_hw *hw, u8 *map);
	void (*disable_rx)(struct xlnid_hw *hw);
	void (*enable_rx)(struct xlnid_hw *hw);
	void (*set_source_address_pruning)(struct xlnid_hw *, bool,
					   unsigned int);
	void (*set_ethertype_anti_spoofing)(struct xlnid_hw *, bool, int);
	s32 (*dmac_update_tcs)(struct xlnid_hw *hw);
	s32 (*dmac_config_tcs)(struct xlnid_hw *hw);
	s32 (*dmac_config)(struct xlnid_hw *hw);
	s32 (*setup_eee)(struct xlnid_hw *hw, bool enable_eee);
	s32 (*read_iosf_sb_reg)(struct xlnid_hw *, u32, u32, u32 *);
	s32 (*write_iosf_sb_reg)(struct xlnid_hw *, u32, u32, u32);
	void (*disable_mdd)(struct xlnid_hw *hw);
	void (*enable_mdd)(struct xlnid_hw *hw);
	void (*mdd_event)(struct xlnid_hw *hw, u32 *vf_bitmap);
	void (*restore_mdd_vf)(struct xlnid_hw *hw, u32 vf);
	bool (*fw_recovery_mode)(struct xlnid_hw *hw);
};

struct xlnid_phy_operations {
	s32 (*identify)(struct xlnid_hw *);
	s32 (*identify_sfp)(struct xlnid_hw *);
	s32 (*init)(struct xlnid_hw *);
	s32 (*reset)(struct xlnid_hw *);
	s32 (*read_reg)(struct xlnid_hw *, u32, u32, u16 *);
	s32 (*write_reg)(struct xlnid_hw *, u32, u32, u16);
	s32 (*read_reg_mdi)(struct xlnid_hw *, u32, u32, u16 *);
	s32 (*write_reg_mdi)(struct xlnid_hw *, u32, u32, u16);
	s32 (*setup_link)(struct xlnid_hw *);
	s32 (*setup_internal_link)(struct xlnid_hw *);
	s32 (*setup_link_speed)(struct xlnid_hw *, xlnid_link_speed, bool);
	s32 (*check_link)(struct xlnid_hw *, xlnid_link_speed *, bool *);
	s32 (*get_firmware_version)(struct xlnid_hw *, u16 *);
	s32 (*read_i2c_byte)(struct xlnid_hw *, u8, u8, u8 *);
	s32 (*write_i2c_byte)(struct xlnid_hw *, u8, u8, u8);
	s32 (*read_i2c_sff8472)(struct xlnid_hw *, u8 , u8 *);
	s32 (*read_i2c_eeprom)(struct xlnid_hw *, u8 , u8 *);
	s32 (*write_i2c_eeprom)(struct xlnid_hw *, u8, u8);
	void (*i2c_bus_clear)(struct xlnid_hw *);
	s32 (*check_overtemp)(struct xlnid_hw *);
	s32 (*set_phy_power)(struct xlnid_hw *, bool on);
	s32 (*enter_lplu)(struct xlnid_hw *);
	s32 (*handle_lasi)(struct xlnid_hw *hw);
	s32 (*read_i2c_byte_unlocked)(struct xlnid_hw *, u8 offset, u8 addr,
				      u8 *value);
	s32 (*write_i2c_byte_unlocked)(struct xlnid_hw *, u8 offset, u8 addr,
				       u8 value);
};

struct xlnid_link_operations {
	s32 (*read_link)(struct xlnid_hw *, u8 addr, u16 reg, u16 *val);
	s32 (*read_link_unlocked)(struct xlnid_hw *, u8 addr, u16 reg,
				  u16 *val);
	s32 (*write_link)(struct xlnid_hw *, u8 addr, u16 reg, u16 val);
	s32 (*write_link_unlocked)(struct xlnid_hw *, u8 addr, u16 reg,
				   u16 val);
};

struct xlnid_link_info {
	struct xlnid_link_operations ops;
	u8 addr;
};

struct xlnid_eeprom_info {
	struct xlnid_eeprom_operations ops;
	enum xlnid_eeprom_type type;
	u32 semaphore_delay;
	u16 word_size;
	u16 address_bits;
	u16 word_page_size;
	u16 ctrl_word_3;
};

#define XLNID_FLAGS_DOUBLE_RESET_REQUIRED	0x01
struct xlnid_mac_info {
	struct xlnid_mac_operations ops;
	enum xlnid_mac_type type;
	u8 addr[XLNID_ETH_LENGTH_OF_ADDRESS];
	u8 perm_addr[XLNID_ETH_LENGTH_OF_ADDRESS];
	u8 san_addr[XLNID_ETH_LENGTH_OF_ADDRESS];
	/* prefix for World Wide Node Name (WWNN) */
	u16 wwnn_prefix;
	/* prefix for World Wide Port Name (WWPN) */
	u16 wwpn_prefix;
#define XLNID_MAX_MTA			128
	u32 mta_shadow[XLNID_MAX_MTA];
	s32 mc_filter_type;
	u32 mcft_size;
	u32 vft_size;
	u32 num_rar_entries;
	u32 rar_highwater;
	u32 rx_pb_size;
	u32 max_tx_queues;
	u32 max_rx_queues;
	u32 orig_autoc;
	u8  san_mac_rar_index;
	bool get_link_status;
	u32 orig_autoc2;
	u16 max_msix_vectors;
	bool arc_subsystem_valid;
	bool orig_link_settings_stored;
	bool autotry_restart;
	u8 flags;
	struct xlnid_thermal_sensor_data  thermal_sensor_data;
	bool thermal_sensor_enabled;
	struct xlnid_dmac_config dmac_config;
	bool set_lben;
	u32  max_link_up_time;
	u8   led_link_act;
};

struct xlnid_phy_info {
	struct xlnid_phy_operations ops;
	enum xlnid_phy_type type;
	u32 addr;
	u32 id;
	enum xlnid_sfp_type sfp_type;
	bool sfp_setup_needed;
	u32 revision;
	enum xlnid_media_type media_type;
	u32 phy_semaphore_mask;
	bool reset_disable;
	xlnid_autoneg_advertised autoneg_advertised;
	xlnid_link_speed speeds_supported;
	xlnid_link_speed eee_speeds_supported;
	xlnid_link_speed eee_speeds_advertised;
	enum xlnid_smart_speed smart_speed;
	bool smart_speed_active;
	bool multispeed_fiber;
	bool reset_if_overtemp;
	bool qsfp_shared_i2c_bus;
	u32 nw_mng_if_sel;
};

//#include "xlnid_mbx.h"

struct xlnid_mbx_operations {
	void (*init_params)(struct xlnid_hw *hw);
	s32  (*read)(struct xlnid_hw *, u32 *, u16,  u16);
	s32  (*write)(struct xlnid_hw *, u32 *, u16, u16);
	s32  (*read_posted)(struct xlnid_hw *, u32 *, u16,  u16);
	s32  (*write_posted)(struct xlnid_hw *, u32 *, u16, u16);
	s32  (*check_for_msg)(struct xlnid_hw *, u16);
	s32  (*check_for_ack)(struct xlnid_hw *, u16);
	s32  (*check_for_rst)(struct xlnid_hw *, u16);
};

struct xlnid_mbx_stats {
	u32 msgs_tx;
	u32 msgs_rx;

	u32 acks;
	u32 reqs;
	u32 rsts;
};

struct xlnid_mbx_info {
	struct xlnid_mbx_operations ops;
	struct xlnid_mbx_stats stats;
	u32 timeout;
	u32 usec_delay;
	u32 v2p_mailbox;
	u16 size;
};

struct xlnid_hw {
	u8 IOMEM *hw_addr;
	void *back;
	struct xlnid_mac_info mac;
	struct xlnid_addr_filter_info addr_ctrl;
	struct xlnid_fc_info fc;
	struct xlnid_phy_info phy;
	struct xlnid_link_info link;
	struct xlnid_eeprom_info eeprom;
	struct xlnid_bus_info bus;
	struct xlnid_mbx_info mbx;
	const u32 *mvals;
	u16 device_id;
	u16 vendor_id;
	u16 subsystem_device_id;
	u16 subsystem_vendor_id;
	u8 revision_id;
	bool adapter_stopped;
	int api_version;
	bool force_full_reset;
	bool allow_unsupported_sfp;
	bool wol_enabled;
	bool need_crosstalk_fix;
    bool ptsw_enable;
    spinlock_t  indirect_lock;
};

#define xlnid_call_func(hw, func, params, error) \
		(func != NULL) ? func params : error

/* Error Codes */
#define XLNID_SUCCESS				0
#define XLNID_ERR_EEPROM			-1
#define XLNID_ERR_EEPROM_CHECKSUM		-2
#define XLNID_ERR_PHY				-3
#define XLNID_ERR_CONFIG			-4
#define XLNID_ERR_PARAM				-5
#define XLNID_ERR_MAC_TYPE			-6
#define XLNID_ERR_UNKNOWN_PHY			-7
#define XLNID_ERR_LINK_SETUP			-8
#define XLNID_ERR_ADAPTER_STOPPED		-9
#define XLNID_ERR_INVALID_MAC_ADDR		-10
#define XLNID_ERR_DEVICE_NOT_SUPPORTED		-11
#define XLNID_ERR_MASTER_REQUESTS_PENDING	-12
#define XLNID_ERR_INVALID_LINK_SETTINGS		-13
#define XLNID_ERR_AUTONEG_NOT_COMPLETE		-14
#define XLNID_ERR_RESET_FAILED			-15
#define XLNID_ERR_SWFW_SYNC			-16
#define XLNID_ERR_PHY_ADDR_INVALID		-17
#define XLNID_ERR_I2C				-18
#define XLNID_ERR_SFP_NOT_SUPPORTED		-19
#define XLNID_ERR_SFP_NOT_PRESENT		-20
#define XLNID_ERR_SFP_NO_INIT_SEQ_PRESENT	-21
#define XLNID_ERR_NO_SAN_ADDR_PTR		-22
#define XLNID_ERR_FDIR_REINIT_FAILED		-23
#define XLNID_ERR_EEPROM_VERSION		-24
#define XLNID_ERR_NO_SPACE			-25
#define XLNID_ERR_OVERTEMP			-26
#define XLNID_ERR_FC_NOT_NEGOTIATED		-27
#define XLNID_ERR_FC_NOT_SUPPORTED		-28
#define XLNID_ERR_SFP_SETUP_NOT_COMPLETE	-30
#define XLNID_ERR_PBA_SECTION			-31
#define XLNID_ERR_INVALID_ARGUMENT		-32
#define XLNID_ERR_HOST_INTERFACE_COMMAND	-33
#define XLNID_ERR_OUT_OF_MEM			-34
#define XLNID_ERR_FEATURE_NOT_SUPPORTED		-36
#define XLNID_ERR_EEPROM_PROTECTED_REGION	-37
#define XLNID_ERR_FDIR_CMD_INCOMPLETE		-38
#define XLNID_ERR_FW_RESP_INVALID		-39
#define XLNID_ERR_TOKEN_RETRY			-40

#define XLNID_NOT_IMPLEMENTED			0x7FFFFFFF

#define XLNID_FUSES0_GROUP(_i)		(0x11158 + ((_i) * 4))
#define XLNID_FUSES0_300MHZ		(1 << 5)
#define XLNID_FUSES0_REV_MASK		(3 << 6)

#define XLNID_KRM_PORT_CAR_GEN_CTRL(P)	((P) ? 0x8010 : 0x4010)
#define XLNID_KRM_LINK_S1(P)		((P) ? 0x8200 : 0x4200)
#define XLNID_KRM_LINK_CTRL_1(P)	((P) ? 0x820C : 0x420C)
#define XLNID_KRM_AN_CNTL_1(P)		((P) ? 0x822C : 0x422C)
#define XLNID_KRM_AN_CNTL_4(P)		((P) ? 0x8238 : 0x4238)
#define XLNID_KRM_AN_CNTL_8(P)		((P) ? 0x8248 : 0x4248)
#define XLNID_KRM_PCS_KX_AN(P)		((P) ? 0x9918 : 0x5918)
#define XLNID_KRM_PCS_KX_AN_LP(P)	((P) ? 0x991C : 0x591C)
#define XLNID_KRM_SGMII_CTRL(P)		((P) ? 0x82A0 : 0x42A0)
#define XLNID_KRM_LP_BASE_PAGE_HIGH(P)	((P) ? 0x836C : 0x436C)
#define XLNID_KRM_DSP_TXFFE_STATE_4(P)	((P) ? 0x8634 : 0x4634)
#define XLNID_KRM_DSP_TXFFE_STATE_5(P)	((P) ? 0x8638 : 0x4638)
#define XLNID_KRM_RX_TRN_LINKUP_CTRL(P)	((P) ? 0x8B00 : 0x4B00)
#define XLNID_KRM_PMD_DFX_BURNIN(P)	((P) ? 0x8E00 : 0x4E00)
#define XLNID_KRM_PMD_FLX_MASK_ST20(P)	((P) ? 0x9054 : 0x5054)
#define XLNID_KRM_TX_COEFF_CTRL_1(P)	((P) ? 0x9520 : 0x5520)
#define XLNID_KRM_RX_ANA_CTL(P)		((P) ? 0x9A00 : 0x5A00)

#define XLNID_KRM_PMD_FLX_MASK_ST20_SFI_10G_DA		~(0x3 << 20)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SFI_10G_SR		(1u << 20)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SFI_10G_LR		(0x2 << 20)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SGMII_EN		(1u << 25)
#define XLNID_KRM_PMD_FLX_MASK_ST20_AN37_EN		(1u << 26)
#define XLNID_KRM_PMD_FLX_MASK_ST20_AN_EN		(1u << 27)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SPEED_10M		~(0x7 << 28)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SPEED_100M		(1u << 28)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SPEED_1G		(0x2 << 28)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SPEED_10G		(0x3 << 28)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SPEED_AN		(0x4 << 28)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SPEED_2_5G		(0x7 << 28)
#define XLNID_KRM_PMD_FLX_MASK_ST20_SPEED_MASK		(0x7 << 28)
#define XLNID_KRM_PMD_FLX_MASK_ST20_FW_AN_RESTART	(1u << 31)

#define XLNID_KRM_PORT_CAR_GEN_CTRL_NELB_32B		(1 << 9)
#define XLNID_KRM_PORT_CAR_GEN_CTRL_NELB_KRPCS		(1 << 11)

#define XLNID_KRM_LINK_CTRL_1_TETH_FORCE_SPEED_MASK	(0x7 << 8)
#define XLNID_KRM_LINK_CTRL_1_TETH_FORCE_SPEED_1G	(2 << 8)
#define XLNID_KRM_LINK_CTRL_1_TETH_FORCE_SPEED_10G	(4 << 8)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_SGMII_EN		(1 << 12)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_CLAUSE_37_EN	(1 << 13)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_FEC_REQ		(1 << 14)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_CAP_FEC		(1 << 15)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_CAP_KX		(1 << 16)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_CAP_KR		(1 << 18)
#define XLNID_KRM_LINK_CTRL_1_TETH_EEE_CAP_KX		(1 << 24)
#define XLNID_KRM_LINK_CTRL_1_TETH_EEE_CAP_KR		(1 << 26)
#define XLNID_KRM_LINK_S1_MAC_AN_COMPLETE		(1 << 28)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_ENABLE		(1 << 29)
#define XLNID_KRM_LINK_CTRL_1_TETH_AN_RESTART		(1 << 31)

#define XLNID_KRM_AN_CNTL_1_SYM_PAUSE			(1 << 28)
#define XLNID_KRM_AN_CNTL_1_ASM_PAUSE			(1 << 29)
#define XLNID_KRM_PCS_KX_AN_SYM_PAUSE			(1 << 1)
#define XLNID_KRM_PCS_KX_AN_ASM_PAUSE			(1 << 2)
#define XLNID_KRM_PCS_KX_AN_LP_SYM_PAUSE		(1 << 2)
#define XLNID_KRM_PCS_KX_AN_LP_ASM_PAUSE		(1 << 3)
#define XLNID_KRM_AN_CNTL_4_ECSR_AN37_OVER_73		(1 << 29)
#define XLNID_KRM_AN_CNTL_8_LINEAR			(1 << 0)
#define XLNID_KRM_AN_CNTL_8_LIMITING			(1 << 1)

#define XLNID_KRM_LP_BASE_PAGE_HIGH_SYM_PAUSE		(1 << 10)
#define XLNID_KRM_LP_BASE_PAGE_HIGH_ASM_PAUSE		(1 << 11)

#define XLNID_KRM_SGMII_CTRL_MAC_TAR_FORCE_100_D	(1 << 12)
#define XLNID_KRM_SGMII_CTRL_MAC_TAR_FORCE_10_D		(1 << 19)

#define XLNID_KRM_DSP_TXFFE_STATE_C0_EN			(1 << 6)
#define XLNID_KRM_DSP_TXFFE_STATE_CP1_CN1_EN		(1 << 15)
#define XLNID_KRM_DSP_TXFFE_STATE_CO_ADAPT_EN		(1 << 16)

#define XLNID_KRM_RX_TRN_LINKUP_CTRL_CONV_WO_PROTOCOL	(1 << 4)
#define XLNID_KRM_RX_TRN_LINKUP_CTRL_PROTOCOL_BYPASS	(1 << 2)

#define XLNID_KRM_PMD_DFX_BURNIN_TX_RX_KR_LB_MASK	(0x3 << 16)

#define XLNID_KRM_TX_COEFF_CTRL_1_CMINUS1_OVRRD_EN	(1 << 1)
#define XLNID_KRM_TX_COEFF_CTRL_1_CPLUS1_OVRRD_EN	(1 << 2)
#define XLNID_KRM_TX_COEFF_CTRL_1_CZERO_EN		(1 << 3)
#define XLNID_KRM_TX_COEFF_CTRL_1_OVRRD_EN		(1 << 31)

#define XLNID_SB_IOSF_INDIRECT_CTRL	0x00011144
#define XLNID_SB_IOSF_INDIRECT_DATA	0x00011148

#define XLNID_SB_IOSF_CTRL_ADDR_SHIFT		0
#define XLNID_SB_IOSF_CTRL_ADDR_MASK		0xFF
#define XLNID_SB_IOSF_CTRL_RESP_STAT_SHIFT	18
#define XLNID_SB_IOSF_CTRL_RESP_STAT_MASK	\
				(0x3 << XLNID_SB_IOSF_CTRL_RESP_STAT_SHIFT)
#define XLNID_SB_IOSF_CTRL_CMPL_ERR_SHIFT	20
#define XLNID_SB_IOSF_CTRL_CMPL_ERR_MASK	\
				(0xFF << XLNID_SB_IOSF_CTRL_CMPL_ERR_SHIFT)
#define XLNID_SB_IOSF_CTRL_TARGET_SELECT_SHIFT	28
#define XLNID_SB_IOSF_CTRL_TARGET_SELECT_MASK	0x7
#define XLNID_SB_IOSF_CTRL_BUSY_SHIFT		31
#define XLNID_SB_IOSF_CTRL_BUSY		(1 << XLNID_SB_IOSF_CTRL_BUSY_SHIFT)
#define XLNID_SB_IOSF_TARGET_KR_PHY	0

#define XLNID_NW_MNG_IF_SEL		0x00011178
#define XLNID_NW_MNG_IF_SEL_MDIO_ACT	(1u << 1)
#define XLNID_NW_MNG_IF_SEL_MDIO_IF_MODE	(1u << 2)
#define XLNID_NW_MNG_IF_SEL_EN_SHARED_MDIO	(1u << 13)
#define XLNID_NW_MNG_IF_SEL_PHY_SPEED_10M	(1u << 17)
#define XLNID_NW_MNG_IF_SEL_PHY_SPEED_100M	(1u << 18)
#define XLNID_NW_MNG_IF_SEL_PHY_SPEED_1G	(1u << 19)
#define XLNID_NW_MNG_IF_SEL_PHY_SPEED_2_5G	(1u << 20)
#define XLNID_NW_MNG_IF_SEL_PHY_SPEED_10G	(1u << 21)
#define XLNID_NW_MNG_IF_SEL_SGMII_ENABLE	(1u << 25)
#define XLNID_NW_MNG_IF_SEL_INT_PHY_MODE (1 << 24) /* X552 reg field only */
#define XLNID_NW_MNG_IF_SEL_MDIO_PHY_ADD_SHIFT 3
#define XLNID_NW_MNG_IF_SEL_MDIO_PHY_ADD	\
				(0x1F << XLNID_NW_MNG_IF_SEL_MDIO_PHY_ADD_SHIFT)

#include "xlnid_osdep2.h"

#endif /* _XLNID_TYPE_H_ */
