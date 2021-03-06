//MAX1110 Test Function 
#include "spi-master.h"
#include "regs-max1110.h"
#include "../PWM/pwmt-regs.h"
#include <test_item.h>

static int sq_spi_max1110_convert(void);
static void sq_max1110_input_pwm_init(void);
static void sq_max1110_input_ref_init(void);
static int sq_spi_max1110_unipolar_calculate(int);
static int sq_spi_max1110_bipolar_calculate(int);

static u8 max1110_cmd;
int unipo_mode;


#define SQ_MAX1110_REFIN             2048    //2.048
#define SQ_MAX1110_COM		0
#define SQ_MAX1110_PWM_INPUT         (1650 * 10 / 131)    //1.563v   (5.1k + 1k*8 = 13.1k)	//JUMP 23
#define SQ_MAX1110_REF_INPUT         (3300 * 10 / 131)    //3.248v   (5.1k + 1k*8 = 13.1k)	//JUMP 12

#define SQ_MAX1110_ERR_ALLOW		(SQ_MAX1110_REFIN / 256 / 2)		//+- 0.5 LSB	LSB = Vrefin/256
#define SQ_MAX1110_ERR_ALLOW_BEST	(SQ_MAX1110_REFIN / 256 / 10)	//+- 0.1 LSB	LSB = Vrefin/256
#define SQ_MAX1110_ERR_ALLOW_WIDEST   50        				//0.03V

#define SQ_MAX1110_ERR	SQ_MAX1110_ERR_ALLOW_WIDEST		//choose error allowable range


int sq_min(int a, int b)
{
        if(a<b)
                return a;
        else
                return b;

}

int comp_value[8][2];
int comp_value_x;

static void
sq_max1110_input_pwm_init()
{
        int i;
	int min_ref = SQ_MAX1110_REFIN;

        comp_value[0][0] = sq_min((SQ_MAX1110_PWM_INPUT * 8), min_ref);
        comp_value[1][0] = sq_min((SQ_MAX1110_PWM_INPUT * 7), min_ref);
        comp_value[2][0] = sq_min((SQ_MAX1110_PWM_INPUT * 6), min_ref);
        comp_value[3][0] = sq_min((SQ_MAX1110_PWM_INPUT * 5), min_ref);
        comp_value[4][0] = sq_min((SQ_MAX1110_PWM_INPUT * 4), min_ref);
        comp_value[5][0] = sq_min((SQ_MAX1110_PWM_INPUT * 3), min_ref);
        comp_value[6][0] = sq_min((SQ_MAX1110_PWM_INPUT * 2), min_ref);
        comp_value[7][0] = sq_min((SQ_MAX1110_PWM_INPUT * 1), min_ref);

        comp_value[0][1] = SQ_MAX1110_PWM_INPUT;
        comp_value[1][1] = 0 - comp_value[0][1];
        comp_value[2][1] = comp_value[4][1] = comp_value[6][1] = comp_value[0][1];
        comp_value[3][1] = comp_value[5][1] = comp_value[7][1] = comp_value[1][1];

	printf("===== Single-ended mode ch value =====\n");
        for(i=0;i<8;i++)
                printf("ch%d = %d.%03d\n", i, comp_value[i][0]/1000, comp_value[i][0]%1000);
	printf("===== Differential mode ch value =====\n");
        for(i=0;i<8;i++){
		if(comp_value[i][1] > 0)
	                printf("ch%d = %d.%03d\n", i, comp_value[i][1]/1000, comp_value[i][1]%1000);
		else
			printf("ch%d = -%d.%03d\n", i, (-comp_value[i][1])/1000, (-comp_value[i][1])%1000);
	}
}

static void
sq_max1110_input_ref_init()
{
        int i;
	int min_ref = SQ_MAX1110_REFIN;
	
        comp_value[0][0] = sq_min((SQ_MAX1110_REF_INPUT * 8), min_ref);
        comp_value[1][0] = sq_min((SQ_MAX1110_REF_INPUT * 7), min_ref);
        comp_value[2][0] = sq_min((SQ_MAX1110_REF_INPUT * 6), min_ref);
        comp_value[3][0] = sq_min((SQ_MAX1110_REF_INPUT * 5), min_ref);
        comp_value[4][0] = sq_min((SQ_MAX1110_REF_INPUT * 4), min_ref);
        comp_value[5][0] = sq_min((SQ_MAX1110_REF_INPUT * 3), min_ref);
        comp_value[6][0] = sq_min((SQ_MAX1110_REF_INPUT * 2), min_ref);
        comp_value[7][0] = sq_min((SQ_MAX1110_REF_INPUT * 1), min_ref);

        comp_value[0][1] = SQ_MAX1110_REF_INPUT;
        comp_value[1][1] = 0 - comp_value[0][1];
        comp_value[2][1] = comp_value[4][1] = comp_value[6][1] = comp_value[0][1];
        comp_value[3][1] = comp_value[5][1] = comp_value[7][1] = comp_value[1][1];

	printf("===== Single-ended mode ch value =====\n");
        for(i=0;i<8;i++)
                printf("ch%d = %d.%03d\n", i, comp_value[i][0]/1000, comp_value[i][0]%1000);
	printf("===== Differential mode ch value =====\n");
        for(i=0;i<8;i++){
		if(comp_value[i][1] > 0)
	                printf("ch%d = %d.%03d\n", i, comp_value[i][1]/1000, comp_value[i][1]%1000);
		else
			printf("ch%d = -%d.%03d\n", i, (-comp_value[i][1])/1000, (-comp_value[i][1])%1000);
	}
}

static int
sq_spi_max1110_read (u16 addr, u16 *buf, u32 len)
{
	u8 tx_buf[1] = {0};

	tx_buf[0] = addr;
	
	if (sq_spi_transfer(tx_buf, buf, SET_TX_RX_LEN(1, len)))
		return -1;
	return 0;
}

static int
sq_max1110_init_pwmt(int hrc, int lrc)
{

#ifdef CONFIG_PWM
	//PWM enable
	static struct sq_pwmt *p_pwmt;
	struct sq_pwmt_driver *pwmt_drv;

	p_pwmt = get_sq_pwmt_structure(MAX1110_USE_PWM_NUM);
	if (NULL == p_pwmt) {
		printf("MAX1110 can't get PWMT structure!!\n");
		return -1;
	}
	
	pwmt_drv = p_pwmt->drv;
	
	pwmt_drv->claim_pwm_lock();

	pwmt_drv->reset(p_pwmt);
	pwmt_drv->write_prescale_factor(p_pwmt, MAX1110_DEFAULT_PRE_SCL);
	pwmt_drv->write_hrc(p_pwmt, hrc);
	pwmt_drv->write_lrc(p_pwmt, lrc);
	pwmt_drv->output_enable(p_pwmt, 1);
	pwmt_drv->enable(p_pwmt, 1);

	pwmt_drv->release_pwm_lock();

	return 0;
#else
	printf("can not use PWMT\n");	

	return -1;
#endif
}

extern struct test_item_container sq_spi_max1110_test_container;

extern int
sq_spi_max1110_adc(int autotest)
{
	u8 divisor;
	int ret;
	
	max1110_cmd = 0;
	
	//PWM enable
	ret = sq_max1110_init_pwmt(MAX1110_DEFAULT_HRC, MAX1110_DEFAULT_LRC);
	if(ret == -1)
		return ret;

	/* Reset SPI controller */
	sq_spi_write(
			sq_spi_read(SQ_SPI_FWCR) | 
			SQ_SPI_SOFT_RST,
			SQ_SPI_FWCR);

	/* Configure SPI controller */
	sq_spi_write(
#if defined (CONFIG_PC9223)
			SQ_SPI_MASTER_SIGNAL_CTL_HW |
			SQ_SPI_MASTER_SIGNAL_ACT_NO |		
			SQ_SPI_MODE_MASTER |
#endif			
			SQ_SPI_SOFT_N_RST |
			SQ_SPI_TXRX_N_RUN |
			SQ_SPI_CLK_IDLE_AST |
			SQ_SPI_TXRX_SIMULT_DIS |
			SQ_SPI_CPOL_0 |
			SQ_SPI_CPHA_0 |
			SQ_SPI_TX_MSB_FIRST |
			SQ_SPI_OP_NORMAL,
			SQ_SPI_FWCR);


	/* Enable SPI interrupt */
	sq_spi_write(
			SQ_SPI_IER_RXFIFO_INT_EN |
			SQ_SPI_IER_RXFIFO_OVR_INT_EN |
			SQ_SPI_IER_RX_COMPLETE_INT_EN,
			SQ_SPI_IER);

	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
			SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_RXFIFO_CLR |
			SQ_SPI_TXFIFO_CLR,
			SQ_SPI_FCR);
     
	/* Set the SPI slaves select and characteristic control register */
	divisor = sq_spi_calculate_divisor(100000); /* 500 KHz clock rate */

	sq_spi_write(
			//SQ_SPI_CHAR_LEN_16 |
			SQ_SPI_CHAR_LEN_8 |
			SQ_SPI_SLAVE_SEL_0 |
			SQ_SPI_CLK_DIV(divisor),
			SQ_SPI_SSCR);

	/* Config SPI clock delay */
	sq_spi_write(
			SQ_SPI_PBTXRX_DELAY_NONE |
			SQ_SPI_PBCT_DELAY_NONE |
			SQ_SPI_PBCA_DELAY_1_2,
			SQ_SPI_DLYCR);

	/* Set per char length */
	//sq_spi_set_current_mode(MODE_CHAR_LEN_16);
	sq_spi_set_current_mode(MODE_CHAR_LEN_8);

	ret = test_item_ctrl(&sq_spi_max1110_test_container, autotest);

	return ret;	

}

extern struct test_item_container sq_spi_max1110_input_container;

extern int 
sq_spi_max1110_input_pwm_test(int autotest)
{
	int ret;

	sq_max1110_input_pwm_init();


	ret = test_item_ctrl(&sq_spi_max1110_input_container, autotest);
	
	return ret;	
}
extern int 
sq_spi_max1110_input_ref_test(int autotest)
{
	int ret;

	sq_max1110_input_ref_init();

	ret = test_item_ctrl(&sq_spi_max1110_input_container, autotest);
	
	return ret;	
}


extern struct test_item_container sq_spi_max1110_sgl_container;
extern struct test_item_container sq_spi_max1110_dif_container;

extern int 
sq_spi_max1110_sgl_test(int autotest)
{
	int ret;

	unipo_mode=1;

	max1110_cmd = (max1110_cmd & ~MAX1110_UNI_BIP) | MAX1110_UNI_BIP_UNIPOLAR;	//set Unipolar mode
	max1110_cmd = (max1110_cmd & ~MAX1110_SGL_DIF) | MAX1110_SGL_DIF_SGL;		//set Single ended mode

	ret = test_item_ctrl(&sq_spi_max1110_sgl_container, autotest);
	
	return ret;	
}

extern int 
sq_spi_max1110_dif_test(int autotest)
{
	int ret;

	unipo_mode=0;

	max1110_cmd = (max1110_cmd & ~MAX1110_UNI_BIP) | MAX1110_UNI_BIP_BIPOLAR;	//set Bipolar mode
	max1110_cmd = (max1110_cmd & ~MAX1110_SGL_DIF) | MAX1110_SGL_DIF_DIF;		//set Differential mode

	ret = test_item_ctrl(&sq_spi_max1110_dif_container, autotest);
	
	return ret;	
}

extern int
sq_spi_max1110_chx_test()
{
        int ret;
        int diff;

        ret = sq_spi_max1110_convert();

        if(ret < comp_value_x){
                diff = comp_value_x - ret;
        }else{
                diff = ret - comp_value_x;
        }

        if(diff < SQ_MAX1110_ERR){
                printf("convert success!!\n");
                ret = 0;
        }else{
                printf("convert fail!!\n");
                printf("estimate value : %d.%03d\n", comp_value_x/1000, comp_value_x%1000);
                ret = 1;
        }

        return ret;
}


extern int 
sq_spi_max1110_ch0_test(int autotest)
{
	int ret;

	max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH0;
	comp_value_x = comp_value[0][0];

	ret = sq_spi_max1110_chx_test();

	return ret;	
}

extern int 
sq_spi_max1110_ch1_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH1;
        comp_value_x = comp_value[1][0];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch2_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH2;
        comp_value_x = comp_value[2][0];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch3_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH3;
        comp_value_x = comp_value[3][0];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch4_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH4;
        comp_value_x = comp_value[4][0];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch5_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH5;
        comp_value_x = comp_value[5][0];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch6_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH6;
        comp_value_x = comp_value[6][0];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch7_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_SINGLE_END_CH7;
        comp_value_x = comp_value[7][0];

        ret = sq_spi_max1110_chx_test();

        return ret;
}


extern int 
sq_spi_max1110_ch0_ch1_test(int autotest)
{
	int ret;

	max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH0_CH1;
        comp_value_x = comp_value[0][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch1_ch0_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH1_CH0;
        comp_value_x = comp_value[1][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch2_ch3_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH2_CH3;
        comp_value_x = comp_value[2][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch3_ch2_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH3_CH2;
        comp_value_x = comp_value[3][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch4_ch5_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH4_CH5;
        comp_value_x = comp_value[4][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch5_ch4_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH5_CH4;
        comp_value_x = comp_value[5][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch6_ch7_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH6_CH7;
        comp_value_x = comp_value[6][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}

extern int 
sq_spi_max1110_ch7_ch6_test(int autotest)
{
        int ret;

        max1110_cmd = (max1110_cmd & ~MAX1110_CHx) | MAX1110_DIFFERENTIAL_CH7_CH6;
        comp_value_x = comp_value[7][1];

        ret = sq_spi_max1110_chx_test();

        return ret;
}



static int 
sq_spi_max1110_convert()
{
	u8 rx_buf[2] = {0, 0};
	u8 reg_value;
	int adc_value;

	//max1110_cmd |= MAX1110_START;
	max1110_cmd = max1110_cmd | MAX1110_START | MAX1110_PD1_FULL_OP | MAX1110_PD0_EXT_CLK;

	//printf("max1110_cmd = 0x%x\n", max1110_cmd);

	sq_spi_max1110_read(max1110_cmd, (void *)rx_buf, 2);

	//printf("get digital value : %x\n", *(u16 *)rx_buf);

	reg_value = ((rx_buf[0]&0x3f)<<2) | ((rx_buf[1]&0xc0)>>6);

	printf("reg value = %4d\n", reg_value);

	if(unipo_mode)
		adc_value = sq_spi_max1110_unipolar_calculate(reg_value);
	else
		adc_value = sq_spi_max1110_bipolar_calculate(reg_value);	

	if(adc_value > 0)
		printf("adc_value = %d.%03d\n", adc_value/1000, adc_value%1000);
	else
		printf("adc_value = -%d.%03d\n", (-adc_value)/1000, (-adc_value)%1000);

	return adc_value;
}


static int
sq_spi_max1110_unipolar_calculate(int adc)
{
	int value;

	value = (SQ_MAX1110_REFIN * adc / 256) +  SQ_MAX1110_COM;

	return value;

}

static int
sq_spi_max1110_bipolar_calculate(int adc)
{
	int value;
	int n_flag=0;

	if(adc>=128){		//- val
		n_flag=1;
		adc = 256 - adc;
	}

	value = SQ_MAX1110_REFIN * adc / 256;

	if(n_flag)
		value = SQ_MAX1110_COM - value;
	else
		value = value - SQ_MAX1110_COM;

	return value;

}

