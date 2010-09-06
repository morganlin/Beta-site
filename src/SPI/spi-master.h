#include <global.h>
#include "spi-regs.h"
#include "dependency.h"

#define MODE_CHAR_LEN_8	0x0
#define MODE_CHAR_LEN_16 0x1
#define SET_TX_RX_LEN(tx, rx)	(((tx) << 16) | (rx))
#define GET_TX_LEN(len)		((len) >> 16)
#define GET_RX_LEN(len)		((len) & 0xffff)

extern void sq_spi_write(u32 val, u32 reg);
extern u32 sq_spi_read(u32 reg);
extern void sq_spi_master_init(u32 spi_base, u32 irq_num);
extern void sq_spi_master_free();
extern void sq_spi_set_current_mode(int mode);
extern int sq_spi_get_current_mode(void);
extern int sq_spi_transfer(void *tx_buf, void *rx_buf, u32 len);
extern u32 sq_spi_calculate_divisor(u32 clk);

//for test use
#define PATTERN_BUF_ADDR 0x00a00000
#define PATTERN_BUF_SIZE 2048
#define SLAVE_NORMAL 0x0
#define SLAVE_HDMA 0x01
#define SLAVE_RESET 0x02
#define SLAVE_PURE 0x03
#define SLAVE_DIV 0x3f
#define LOOKBACK_DIV 0x3f
extern void sq_spi_make_test_pattern(u8 *buf, u32 size);
extern int sq_spi_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size, int skip_cmpr_result);

//
