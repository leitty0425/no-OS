#include <stdio.h>
#include <sleep.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <xil_cache.h>
#include <xparameters.h>
#include "xil_printf.h"
#include "spi_engine.h"
#include "ad469x.h"
#include "error.h"
#include "delay.h"
#include "clk_axi_clkgen.h"
#include "gpio.h"
#include "gpio_extra.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD469x_EVB_SAMPLE_NO			10000
#define AD469x_DMA_BASEADDR             XPAR_AXI_AD4696_DMA_BASEADDR
#define AD469x_SPI_ENGINE_BASEADDR      XPAR_SPI_AD4696_AXI_REGMAP_BASEADDR
#define AD469x_SPI_CS                   0
#define AD469x_SPI_ENG_REF_CLK_FREQ_HZ	XPAR_PS7_SPI_0_SPI_CLK_FREQ_HZ
#define RX_CLKGEN_BASEADDR				XPAR_SPI_CLKGEN_BASEADDR
#define GPIO_OFFSET			54
#define GPIO_RESETN_1			GPIO_OFFSET + 32
#define GPIO_DEVICE_ID			XPAR_PS7_GPIO_0_DEVICE_ID
#define SPI_TRIGGER_BASEADDR	XPAR_SPI_AD4696_TRIGGER_GEN_BASEADDR

#define SPI_ENGINE_OFFLOAD_EXAMPLE	1

int main()
{
	struct ad469x_dev *dev;
	uint32_t *offload_data;
	uint32_t adc_data;
	struct spi_engine_offload_message msg;
	uint8_t commands_data[2] = {0xFF, 0xFF};
	int32_t ret, data;
	uint32_t i;
//	main_sergiu();

	struct spi_engine_offload_init_param spi_engine_offload_init_param = {
		.offload_config = OFFLOAD_RX_EN,
		.rx_dma_baseaddr = AD469x_DMA_BASEADDR,
	};

	struct spi_engine_init_param spi_eng_init_param  = {
		.ref_clk_hz = AD469x_SPI_ENG_REF_CLK_FREQ_HZ,
		.type = SPI_ENGINE,
		.spi_engine_baseaddr = AD469x_SPI_ENGINE_BASEADDR,
		.cs_delay = 2,
		.data_width = 32,
	};

	struct axi_clkgen_init clkgen_init = {
		.name = "rx_clkgen",
		.base = RX_CLKGEN_BASEADDR,
		.parent_rate = 100000000,
	};
	struct xil_gpio_init_param gpio_extra_param;
	struct gpio_init_param ad469x_resetn = {
		.number = GPIO_RESETN_1,
		.extra = &gpio_extra_param
	};

	struct ad469x_init_param ad469x_init_param = {
		.spi_init = {
			.chip_select = AD469x_SPI_CS,
			.max_speed_hz = 20000000,
			.mode = SPI_MODE_0,
			.platform_ops = &spi_eng_platform_ops,
			.extra = (void*)&spi_eng_init_param,
		},
		.clkgen_init = clkgen_init,
		.reg_access_speed = 20000000,
		.dev_id = ID_AD4003, /* dev_id */
		.gpio_resetn = &ad469x_resetn,
	};

	print("Test\n\r");

	gpio_extra_param.device_id = GPIO_DEVICE_ID;
	gpio_extra_param.type = GPIO_PS;

	uint32_t spi_eng_msg_cmds[3] = {
		CS_LOW,
		READ(2),
		CS_HIGH
	};

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	ret = ad469x_init(&dev, &ad469x_init_param);
	if (ret < 0)
		return ret;

	if (SPI_ENGINE_OFFLOAD_EXAMPLE == 0) {
		while(1) {
			ad469x_spi_single_conversion(dev, &adc_data);
			xil_printf("ADC: %d\n\r", adc_data);
		}
	}
	/* Offload example */
	else {
		//axi_write(0x44a70010, BIT(1));
		axi_io_write(SPI_TRIGGER_BASEADDR, 0x10, BIT(1));

		ret = spi_engine_offload_init(dev->spi_desc, &spi_engine_offload_init_param);
		if (ret != SUCCESS)
			return FAILURE;

		msg.commands = spi_eng_msg_cmds;
		msg.no_commands = ARRAY_SIZE(spi_eng_msg_cmds);
		msg.rx_addr = 0x800000;
		msg.tx_addr = 0xA000000;
		msg.commands_data = commands_data;

		ret = spi_engine_offload_transfer(dev->spi_desc, msg, AD469x_EVB_SAMPLE_NO);
		if (ret != SUCCESS)
			return ret;

		mdelay(2000);
		Xil_DCacheInvalidateRange(0x800000, AD469x_EVB_SAMPLE_NO * 4);
		offload_data = (uint32_t *)msg.rx_addr;

		for(i = 0; i < AD469x_EVB_SAMPLE_NO / 2; i++) {
			data = *offload_data & 0xFFFFF;
			if (data > 524287)
				data = data - 1048576;
			printf("ADC%d: %d \n", i, data);
			offload_data += 1;
		}
	}

	print("Success\n\r");

	Xil_DCacheDisable();
	Xil_ICacheDisable();
}
