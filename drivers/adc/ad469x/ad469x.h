/***************************************************************************//**
 *   @file   ad469x.h
 *   @brief  Header file for ad469x Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef SRC_AD400X_H_
#define SRC_AD400X_H_

#include "spi_engine.h"
#include "clk_axi_clkgen.h"
#include "gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD469x_REG_READ(x)		( (1 << 7) | (x & 0xFF) )		// Read from register x
#define AD469x_REG_WRITE(x)		( (~(1 << 7)) & (x & 0xFF) )  	// Write to register x

#define AD469x_REG_IF_CONFIG_A		0x000
#define AD469x_REG_IF_CONFIG_B		0x001
#define AD469x_REG_DEVICE_TYPE		0x003
#define AD469x_REG_DEVICE_ID_L		0x004
#define AD469x_REG_DEVICE_ID_H		0x005
#define AD469x_REG_SCRATCH_PAD		0x00A
#define AD469x_REG_VENDOR_L			0x00C
#define AD469x_REG_VENDOR_H			0x00D
#define AD469x_REG_LOOP_MODE		0x00E
#define AD469x_REG_IF_CONFIG_C		0x010
#define AD469x_REG_IF_STATUS		0x011
#define AD469x_REG_STATUS			0x014
#define AD469x_REG_ALERT_STATUS1	0x015
#define AD469x_REG_ALERT_STATUS2	0x016
#define AD469x_REG_ALERT_STATUS3	0x017
#define AD469x_REG_ALERT_STATUS4	0x018
#define AD469x_REG_CLAMP_STATUS1	0x01A
#define AD469x_REG_CLAMP_STATUS2	0x01B
#define AD469x_REG_SETUP			0x020
#define AD469x_REG_REF_CTRL			0x021
#define AD469x_REG_SEQ_CTRL			0x022
#define AD469x_REG_AC_CTRL			0x023
#define AD469x_REG_STD_SEQ_CONFIG	0x024
#define AD469x_REG_GPIO_CTRL		0x026
#define AD469x_REG_GP_MODE			0x027
#define AD469x_REG_GPIO_STATE		0x028
#define AD469x_REG_TEMP_CTRL		0x029

/* 5-bit SDI Conversion Mode Commands */
#define AD469x_CMD_REG_CONFIG_MODE		(0x0A << 3)
#define AD469x_CMD_SEL_TEMP_SNSOR_CH	(0x0F << 3)
#define AD469x_CMD_CONFIG_CH_SEL 		(0x10 << 3)

/* AD469x_REG_SETUP */
#define AD469x_REG_SETUP_IF_MODE_MASK		(0x01 << 2)
#define AD469x_REG_SETUP_IF_MODE(x)			((x & 0x01) << 2)

/* AD469x_REG_GP_MODE */
#define AD469x_REG_GP_MODE_BUSY_GP_EN_MASK		(0x01 << 1)
#define AD469x_REG_GP_MODE_BUSY_GP_EN(x)		((x & 0x01) << 1)
#define AD469x_REG_GP_MODE_BUSY_GP_SEL_MASK		(0x01 << 4)
#define AD469x_REG_GP_MODE_BUSY_GP_SEL(x)		((x & 0x01) << 4)

/*****************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum ad469x_interface_mode {
	AD469x_IF_REGISTER_MODE = 0,
	AD469x_IF_CONVERSION_MODE = 1,
};

enum ad469x_busy_gp_sel {
	AD469x_busy_gp0 = 0,
	AD469x_busy_gp3 = 1,
};

enum ad400x_supported_dev_ids {
	ID_AD4003,
	ID_AD4007,
	ID_AD4011,
	ID_AD4020,
};

struct ad469x_dev {
	/* SPI */
	spi_desc *spi_desc;
	/* Clock gen for hdl design structure */
	struct axi_clkgen *clkgen;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum ad400x_supported_dev_ids dev_id;
	/** RESET GPIO handler. */
	struct gpio_desc		*gpio_resetn;
};

struct ad469x_init_param {
	/* SPI */
	spi_init_param spi_init;
	/* Clock gen for hdl design init structure */
	struct axi_clkgen_init clkgen_init;
	/* Register access speed */
	uint32_t reg_access_speed;
	/* Device Settings */
	enum ad400x_supported_dev_ids dev_id;
	/** RESET GPIO initialization structure. */
	struct gpio_init_param *gpio_resetn;
};

int32_t ad469x_spi_reg_read(struct ad469x_dev *dev,
			     uint16_t reg_addr,
			     uint8_t *reg_data);
int32_t ad469x_spi_reg_write(struct ad469x_dev *dev,
			      uint16_t reg_addr,
			      uint8_t reg_data);
int32_t ad469x_spi_read_mask(struct ad469x_dev *dev,
			      uint16_t reg_addr,
			      uint8_t mask,
			      uint8_t *data);
int32_t ad469x_spi_write_mask(struct ad469x_dev *dev,
			       uint16_t reg_addr,
			       uint8_t mask,
			       uint8_t data);
int32_t ad469x_init(struct ad469x_dev **device,
		    struct ad469x_init_param *init_param);
int32_t ad469x_remove(struct ad469x_dev *dev);
int32_t ad469x_spi_single_conversion(struct ad469x_dev *dev,
				     uint32_t *adc_data);

#endif /* SRC_AD400X_H_ */
