// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2022 - 2023 Mucse Corporation. */

#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/mdio.h>

#include "rnp_common.h"
#include "rnp_phy.h"

#define RNP_PHY_REVISION_MASK 0xFFFFFFF0
#define RNP_MAX_PHY_ADDR 32

static void rnp_i2c_start(struct rnp_hw *hw);
static void rnp_i2c_stop(struct rnp_hw *hw);
static s32 rnp_clock_in_i2c_byte(struct rnp_hw *hw, u8 *data);
static s32 rnp_clock_out_i2c_byte(struct rnp_hw *hw, u8 data);
static s32 rnp_get_i2c_ack(struct rnp_hw *hw);
static s32 rnp_clock_in_i2c_bit(struct rnp_hw *hw, bool *data);
static s32 rnp_clock_out_i2c_bit(struct rnp_hw *hw, bool data);
static void rnp_raise_i2c_clk(struct rnp_hw *hw, u32 *i2cctl);
static void rnp_lower_i2c_clk(struct rnp_hw *hw, u32 *i2cctl);
static s32 rnp_set_i2c_data(struct rnp_hw *hw, u32 *i2cctl, bool data);
static bool rnp_get_i2c_data(u32 *i2cctl);
static void rnp_i2c_bus_clear(struct rnp_hw *hw);
static enum rnp_phy_type rnp_get_phy_type_from_id(u32 phy_id);
static s32 rnp_get_phy_id(struct rnp_hw *hw);

/**
 *  rnp_identify_phy_generic - Get physical layer module
 *  @hw: pointer to hardware structure
 *
 *  Determines the physical layer module found on the current adapter.
 **/
s32 rnp_identify_phy_generic(struct rnp_hw *hw)
{
	s32 status = RNP_ERR_PHY_ADDR_INVALID;

	return status;
}

/**
 *  rnp_get_phy_id - Get the phy type
 *  @hw: pointer to hardware structure
 *
 **/
__maybe_unused static s32 rnp_get_phy_id(struct rnp_hw *hw)
{
	u32 status;
	u16 phy_id_high = 0;
	u16 phy_id_low = 0;

	status = hw->phy.ops.read_reg(hw, MDIO_DEVID1, MDIO_MMD_PMAPMD,
				      &phy_id_high);

	if (status == 0) {
		hw->phy.id = (u32)(phy_id_high << 16);
		status = hw->phy.ops.read_reg(
			hw, MDIO_DEVID2, MDIO_MMD_PMAPMD, &phy_id_low);
		hw->phy.id |= (u32)(phy_id_low & RNP_PHY_REVISION_MASK);
		hw->phy.revision =
			(u32)(phy_id_low & ~RNP_PHY_REVISION_MASK);
	}
	return status;
}

/**
 *  rnp_get_phy_type_from_id - Get the phy type
 *  @hw: pointer to hardware structure
 *
 **/
__maybe_unused static enum rnp_phy_type
rnp_get_phy_type_from_id(u32 phy_id)
{
	enum rnp_phy_type phy_type = rnp_phy_unknown;

	return phy_type;
}

/**
 *  rnp_reset_phy_generic - Performs a PHY reset
 *  @hw: pointer to hardware structure
 **/
s32 rnp_reset_phy_generic(struct rnp_hw *hw)
{
	s32 status = 0;

	return status;
}

/**
 *  rnp_read_phy_reg_generic - Reads a value from a specified PHY register
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit address of PHY register to read
 *  @phy_data: Pointer to read data from PHY register
 **/
s32 rnp_read_phy_reg_generic(struct rnp_hw *hw, u32 reg_addr,
			     u32 device_type, u16 *phy_data)
{
	s32 status = 0;

	return status;
}

/**
 *  rnp_write_phy_reg_generic - Writes a value to specified PHY register
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit PHY register to write
 *  @device_type: 5 bit device type
 *  @phy_data: Data to write to the PHY register
 **/
s32 rnp_write_phy_reg_generic(struct rnp_hw *hw, u32 reg_addr,
			      u32 device_type, u16 phy_data)
{
	s32 status = 0;

	return status;
}

/**
 *  rnp_setup_phy_link_generic - Set and restart autoneg
 *  @hw: pointer to hardware structure
 *
 *  Restart autonegotiation and PHY and waits for completion.
 **/
s32 rnp_setup_phy_link_generic(struct rnp_hw *hw)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_setup_phy_link_speed_generic - Sets the auto advertised 
 * 		capabilities
 * 
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 **/
s32 rnp_setup_phy_link_speed_generic(struct rnp_hw *hw,
				     rnp_link_speed speed,
				     bool autoneg_wait_to_complete)
{
	/*
	 * Clear autoneg_advertised and set new values based on input link
	 * speed.
	 */
	hw->phy.autoneg_advertised = 0;

	if (speed & RNP_LINK_SPEED_10GB_FULL)
		hw->phy.autoneg_advertised |= RNP_LINK_SPEED_10GB_FULL;

	if (speed & RNP_LINK_SPEED_1GB_FULL)
		hw->phy.autoneg_advertised |= RNP_LINK_SPEED_1GB_FULL;

	if (speed & RNP_LINK_SPEED_100_FULL)
		hw->phy.autoneg_advertised |= RNP_LINK_SPEED_100_FULL;

	/* Setup link based on the new speed settings */
	hw->phy.ops.setup_link(hw);

	return 0;
}

/**
 * rnp_get_copper_link_capabilities_generic - Determines link capabilities
 * @hw: pointer to hardware structure
 * @speed: pointer to link speed
 * @autoneg: boolean auto-negotiation value
 *
 * Determines the link capabilities by reading the AUTOC register.
 */
s32 rnp_get_copper_link_capabilities_generic(struct rnp_hw *hw,
					     rnp_link_speed *speed,
					     bool *autoneg)
{
	s32 status = RNP_ERR_LINK_SETUP;
	u16 speed_ability;

	*speed = 0;
	*autoneg = true;

	status = hw->phy.ops.read_reg(hw, MDIO_SPEED, MDIO_MMD_PMAPMD,
				      &speed_ability);

	if (status == 0) {
		if (speed_ability & MDIO_SPEED_10G)
			*speed |= RNP_LINK_SPEED_10GB_FULL;
		if (speed_ability & MDIO_PMA_SPEED_1000)
			*speed |= RNP_LINK_SPEED_1GB_FULL;
		if (speed_ability & MDIO_PMA_SPEED_100)
			*speed |= RNP_LINK_SPEED_100_FULL;
	}

	return status;
}

/**
 *  rnp_check_phy_link_tnx - Determine link and speed status
 *  @hw: pointer to hardware structure
 *
 *  Reads the VS1 register to determine if link is up and the current 
 *  speed for the PHY.
 **/
s32 rnp_check_phy_link_tnx(struct rnp_hw *hw, rnp_link_speed *speed,
			   bool *link_up)
{
	s32 status = 0;
	return status;
}

/**
 *	rnp_setup_phy_link_tnx - Set and restart autoneg
 *	@hw: pointer to hardware structure
 *
 *	Restart autonegotiation and PHY and waits for completion.
 **/
s32 rnp_setup_phy_link_tnx(struct rnp_hw *hw)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_get_phy_firmware_version_tnx - Gets the PHY Firmware Version
 *  @hw: pointer to hardware structure
 *  @firmware_version: pointer to the PHY Firmware Version
 **/
s32 rnp_get_phy_firmware_version_tnx(struct rnp_hw *hw,
				     u16 *firmware_version)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_get_phy_firmware_version_generic - Gets the PHY Firmware Version
 *  @hw: pointer to hardware structure
 *  @firmware_version: pointer to the PHY Firmware Version
 **/
s32 rnp_get_phy_firmware_version_generic(struct rnp_hw *hw,
					 u16 *firmware_version)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_reset_phy_nl - Performs a PHY reset
 *  @hw: pointer to hardware structure
 **/
s32 rnp_reset_phy_nl(struct rnp_hw *hw)
{
	s32 ret_val = 0;
	return ret_val;
}

/**
 *  rnp_identify_sfp_module_generic - Identifies SFP modules
 *  @hw: pointer to hardware structure
 *
 *  Searches for and identifies the SFP module and assigns appropriate PHY type.
 **/
s32 rnp_identify_sfp_module_generic(struct rnp_hw *hw)
{
	return RNP_ERR_SFP_NOT_PRESENT;
}

/**
 *  rnp_get_sfp_init_sequence_offsets - Provides offset of PHY init sequence
 *  @hw: pointer to hardware structure
 *  @list_offset: offset to the SFP ID list
 *  @data_offset: offset to the SFP data block
 *
 *  Checks the MAC's EEPROM to see if it supports a given SFP+ module type, if
 *  so it returns the offsets to the phy init sequence block.
 **/
s32 rnp_get_sfp_init_sequence_offsets(struct rnp_hw *hw, u16 *list_offset,
				      u16 *data_offset)
{
	return 0;
}

/**
 *  rnp_read_i2c_eeprom_generic - Reads 8 bit EEPROM word over I2C interface
 *  @hw: pointer to hardware structure
 *  @byte_offset: EEPROM byte offset to read
 *  @eeprom_data: value read
 *
 *  Performs byte read operation to SFP module's EEPROM over I2C interface.
 **/
s32 rnp_read_i2c_eeprom_generic(struct rnp_hw *hw, u8 byte_offset,
				u8 *eeprom_data)
{
	return -EIO;
}

/**
 *  rnp_read_i2c_sff8472_generic - Reads 8 bit word over I2C interface
 *  @hw: pointer to hardware structure
 *  @byte_offset: byte offset at address 0xA2
 *  @eeprom_data: value read
 *
 *  Performs byte read operation to SFP module's SFF-8472 data over I2C
 **/
s32 rnp_read_i2c_sff8472_generic(struct rnp_hw *hw, u8 byte_offset,
				 u8 *sff8472_data)
{
	//return hw->phy.ops.read_i2c_byte(hw, byte_offset, RNP_I2C_EEPROM_DEV_ADDR2, sff8472_data);
	return -EIO;
}

/**
 *  rnp_write_i2c_eeprom_generic - Writes 8 bit EEPROM word over I2C interface
 *  @hw: pointer to hardware structure
 *  @byte_offset: EEPROM byte offset to write
 *  @eeprom_data: value to write
 *
 *  Performs byte write operation to SFP module's EEPROM over I2C interface.
 **/
s32 rnp_write_i2c_eeprom_generic(struct rnp_hw *hw, u8 byte_offset,
				 u8 eeprom_data)
{
	//return hw->phy.ops.write_i2c_byte(hw, byte_offset, RNP_I2C_EEPROM_DEV_ADDR, eeprom_data);
	return -EIO;
}

/**
 *  rnp_read_i2c_byte_generic - Reads 8 bit word over I2C
 *  @hw: pointer to hardware structure
 *  @byte_offset: byte offset to read
 *  @data: value read
 *
 *  Performs byte read operation to SFP module's EEPROM over I2C interface at
 *  a specified device address.
 **/
s32 rnp_read_i2c_byte_generic(struct rnp_hw *hw, u8 byte_offset,
			      u8 dev_addr, u8 *data)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_write_i2c_byte_generic - Writes 8 bit word over I2C
 *  @hw: pointer to hardware structure
 *  @byte_offset: byte offset to write
 *  @data: value to write
 *
 *  Performs byte write operation to SFP module's EEPROM over I2C interface at
 *  a specified device address.
 **/
s32 rnp_write_i2c_byte_generic(struct rnp_hw *hw, u8 byte_offset,
			       u8 dev_addr, u8 data)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_i2c_start - Sets I2C start condition
 *  @hw: pointer to hardware structure
 *
 *  Sets I2C start condition (High -> Low on SDA while SCL is High)
 **/
__maybe_unused static void rnp_i2c_start(struct rnp_hw *hw)
{
}

/**
 *  rnp_i2c_stop - Sets I2C stop condition
 *  @hw: pointer to hardware structure
 *
 *  Sets I2C stop condition (Low -> High on SDA while SCL is High)
 **/
__maybe_unused static void rnp_i2c_stop(struct rnp_hw *hw)
{
}

/**
 *  rnp_clock_in_i2c_byte - Clocks in one byte via I2C
 *  @hw: pointer to hardware structure
 *  @data: data byte to clock in
 *
 *  Clocks in one byte data via I2C data/clock
 **/
__maybe_unused static s32 rnp_clock_in_i2c_byte(struct rnp_hw *hw,
						u8 *data)
{
	s32 i;
	bool bit = false;

	for (i = 7; i >= 0; i--) {
		rnp_clock_in_i2c_bit(hw, &bit);
		*data |= bit << i;
	}

	return 0;
}

/**
 *  rnp_clock_out_i2c_byte - Clocks out one byte via I2C
 *  @hw: pointer to hardware structure
 *  @data: data byte clocked out
 *
 *  Clocks out one byte data via I2C data/clock
 **/
__maybe_unused static s32 rnp_clock_out_i2c_byte(struct rnp_hw *hw,
						 u8 data)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_get_i2c_ack - Polls for I2C ACK
 *  @hw: pointer to hardware structure
 *
 *  Clocks in/out one bit via I2C data/clock
 **/
__maybe_unused static s32 rnp_get_i2c_ack(struct rnp_hw *hw)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_clock_in_i2c_bit - Clocks in one bit via I2C data/clock
 *  @hw: pointer to hardware structure
 *  @data: read data value
 *
 *  Clocks in one bit via I2C data/clock
 **/
static s32 rnp_clock_in_i2c_bit(struct rnp_hw *hw, bool *data)
{
	return 0;
}

/**
 *  rnp_clock_out_i2c_bit - Clocks in/out one bit via I2C data/clock
 *  @hw: pointer to hardware structure
 *  @data: data value to write
 *
 *  Clocks out one bit via I2C data/clock
 **/
__maybe_unused static s32 rnp_clock_out_i2c_bit(struct rnp_hw *hw,
						bool data)
{
	s32 status = 0;
	return status;
}
/**
 *  rnp_raise_i2c_clk - Raises the I2C SCL clock
 *  @hw: pointer to hardware structure
 *  @i2cctl: Current value of I2CCTL register
 *
 *  Raises the I2C clock line '0'->'1'
 **/
__maybe_unused static void rnp_raise_i2c_clk(struct rnp_hw *hw,
					     u32 *i2cctl)
{
}

/**
 *  rnp_lower_i2c_clk - Lowers the I2C SCL clock
 *  @hw: pointer to hardware structure
 *  @i2cctl: Current value of I2CCTL register
 *
 *  Lowers the I2C clock line '1'->'0'
 **/
__maybe_unused static void rnp_lower_i2c_clk(struct rnp_hw *hw,
					     u32 *i2cctl)
{
}

/**
 *  rnp_set_i2c_data - Sets the I2C data bit
 *  @hw: pointer to hardware structure
 *  @i2cctl: Current value of I2CCTL register
 *  @data: I2C data value (0 or 1) to set
 *
 *  Sets the I2C data bit
 **/
__maybe_unused static s32 rnp_set_i2c_data(struct rnp_hw *hw, u32 *i2cctl,
					   bool data)
{
	s32 status = 0;
	return status;
}

/**
 *  rnp_get_i2c_data - Reads the I2C SDA data bit
 *  @hw: pointer to hardware structure
 *  @i2cctl: Current value of I2CCTL register
 *
 *  Returns the I2C data bit value
 **/
__maybe_unused static bool rnp_get_i2c_data(u32 *i2cctl)
{
	bool data = false;
	return data;
}

/**
 *  rnp_i2c_bus_clear - Clears the I2C bus
 *  @hw: pointer to hardware structure
 *
 *  Clears the I2C bus by sending nine clock pulses.
 *  Used when data line is stuck low.
 **/
__maybe_unused static void rnp_i2c_bus_clear(struct rnp_hw *hw)
{
}

/**
 *  rnp_tn_check_overtemp - Checks if an overtemp occurred.
 *  @hw: pointer to hardware structure
 *
 *  Checks if the LASI temp alarm status was triggered due to overtemp
 **/
s32 rnp_tn_check_overtemp(struct rnp_hw *hw)
{
	s32 status = 0;
	return status;
}
