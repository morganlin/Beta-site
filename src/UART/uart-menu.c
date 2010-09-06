#include <test_item.h>
#include "dependency.h"

extern int sq_uart_0_test(int autotest);
extern int sq_uart_1_test(int autotest);
extern int sq_uart_2_test(int autotest);
extern int sq_uart_3_test(int autotest);

struct test_item sq_uart_ip_test_items[] = {
	{"UART 0 Test",
	 sq_uart_0_test,
	 1,
	 SQ_UART_IP_0_TEST},
	{"UART 1 Test",
	 sq_uart_1_test,
	 1,
	 SQ_UART_IP_1_TEST},
	{"UART 2 Test",
	 sq_uart_2_test,
	 1,
	 SQ_UART_IP_2_TEST},
	{"UART 3 Test",
	 sq_uart_3_test,
	 1,
	 SQ_UART_IP_3_TEST}
};

struct test_item_container sq_uart_ip_test_container = {
	.menu_name = "UART IP Test ",
	.shell_name = "uart",
	.items = sq_uart_ip_test_items,
	.test_item_size = sizeof(sq_uart_ip_test_items)
};

extern int sq_uart_normal_test(int autotest);
extern int sq_uart_hwdma_panther7_hdma_test(int autotest);

struct test_item sq_uart_type_test_items[] = {
	{"Normal Test",
	 sq_uart_normal_test,
	 1,
	 1},
	
	{"Hardware Dma with Panther7 HDMA Test",
	 sq_uart_hwdma_panther7_hdma_test,
	 1,
	 SQ_UART_HWDMA_PANTHER7_HDMA_TEST
        },
};

struct test_item_container sq_uart_type_test_container = {
	.menu_name = "UART Type Test",
	.shell_name = "uart",
	.items = sq_uart_type_test_items,
	.test_item_size = sizeof(sq_uart_type_test_items)
};

extern int sq_uart_inter_loopback_test(int autotest);
extern int sq_uart_ext_loopback_test(int autotest);
extern int sq_uart_module_test(int autotest);

struct test_item sq_uart_transfer_test_items[] = {
	{"Internal Loopback Test",
	 sq_uart_inter_loopback_test,
	 1,
	 1},
	{"Externel Loopback Test",
	 sq_uart_ext_loopback_test,
	 1,
	 1},
/* added by morganlin */	 
#ifdef GDR_UART	 
        {"Module Test",
         sq_uart_module_test,
         0,
         0
        }
#else
	{"Module Test",
	 sq_uart_module_test,
	 1,
	 1
        }
#endif	 
};

struct test_item_container sq_uart_transfer_test_container = {
	.menu_name = "UART Transfer Test",
	.shell_name = "uart",
	.items = sq_uart_transfer_test_items,
	.test_item_size = sizeof(sq_uart_transfer_test_items)
};

extern int sq_uart_gps_test(int autotest);

struct test_item sq_uart_module_test_items[] = {
	{"GPS Test",
	 sq_uart_gps_test,
	 1,
	 1},
};

struct test_item_container sq_uart_module_test_container = {
	.menu_name = "UART Module Test",
	.shell_name = "uart",
	.items = sq_uart_module_test_items,
	.test_item_size = sizeof(sq_uart_module_test_items)
};

extern int sq_uart_baudrate_75(int autotest);
extern int sq_uart_baudrate_300(int autotest);
extern int sq_uart_baudrate_1200(int autotest);
extern int sq_uart_baudrate_2400(int autotest);
extern int sq_uart_baudrate_9600(int autotest);
extern int sq_uart_baudrate_19200(int autotest);
extern int sq_uart_baudrate_38400(int autotest);
extern int sq_uart_baudrate_57600(int autotest);
extern int sq_uart_baudrate_115200(int autotest);

struct test_item sq_uart_baudrate_test_items[] = {
	{"75",
	 sq_uart_baudrate_75,
	 1,
	 0},
	{"300",
	 sq_uart_baudrate_300,
	 1,
	 0},
	{"1200",
	 sq_uart_baudrate_1200,
	 1,
	 0},
	{"2400",
	 sq_uart_baudrate_2400,
	 1,
	 0},
	{"9600",
	 sq_uart_baudrate_9600,
	 1,
	 1},
	{"19200",
	 sq_uart_baudrate_19200,
	 1,
	 1},
	{"38400",
	 sq_uart_baudrate_38400,
	 1,
	 1},
	{"57600",
	 sq_uart_baudrate_57600,
	 1,
	 1},
	{"115200",
	 sq_uart_baudrate_115200,
	 1,
	 1},
};

struct test_item_container sq_uart_baudrate_test_container = {
	.menu_name = "UART Baudrate Test",
	.shell_name = "uart",
	.items = sq_uart_baudrate_test_items,
	.test_item_size = sizeof(sq_uart_baudrate_test_items)
};

extern int sq_uart_panther7_hdma_bursttype_single(int autotest);
extern int sq_uart_panther7_hdma_bursttype_incr4(int autotest);
extern int sq_uart_panther7_hdma_bursttype_incr8(int autotest);

struct test_item sq_uart_panther7_hdma_bursttype_test_items[] = {
	{"Single",
	 sq_uart_panther7_hdma_bursttype_single,
	 1,
	 1},
	{"INCR4",
	 sq_uart_panther7_hdma_bursttype_incr4,
	 1,
	 1},
	{"INCR8",
	 sq_uart_panther7_hdma_bursttype_incr8,
	 1,
	 1}
};

struct test_item_container sq_uart_panther7_hdma_bursttype_test_container = {
	.menu_name = "UART Panther7 HDMA Burst Type Test",
	.shell_name = "uart",
	.items = sq_uart_panther7_hdma_bursttype_test_items,
	.test_item_size = sizeof(sq_uart_panther7_hdma_bursttype_test_items)
};
