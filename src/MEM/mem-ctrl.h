#ifndef _MEMCTRL_H_
#define _MEMCTRL_H_

enum
{
    SDRAM_Module = 0,
    ITCM_Module,
    DTCM_Module,
    ANOTHER_DTCM_Module,
    FPGA_SRAM_Module,
    MDDR_Module,
    SRAM_Module,
    DP_SRAM_Module,
};

#define MEM_PATTERN0	0x55aa55aa
#define MEM_PATTERN1 	0xaa55aa55
#define MEM_PATTERN2 	0x55555555
#define MEM_PATTERN3 	0xaaaaaaaa
#define MEM_PATTERN4 	0xffffffff
#define MEM_PATTERN5 	0x00000000
#define MEM_PATTERN6 	0xffff0000
#define MEM_PATTERN7 	0x0000ffff



extern int memory_test(int autotest);

extern int mdk3d_fpga_test(int autotest);
extern int mem_fpga_axi_test(int autotest);
extern int mem_fpga_ahb_test(int autotest);
extern int mem_fpga_ddr2_test(int autotest);
extern int mem_fpga_sram_test(int autotest);

extern int sdram_test(int autotest);
extern int itcm_test0(int autotest);
extern int itcm_test1(int autotest);
extern int dtcm_test0(int autotest);
extern int dtcm_test1(int autotest);
extern int another_dtcm_test(int autotest);
extern int fpga_sram_test(int autotest);
extern int mddr_test(int autotest);
extern int mddr_power_mode_test(int autotest);
extern int sram_test(int autotest);
extern int dp_sram_test(int autotest);
extern int mem_range_test(int autotest);

extern int mem_pattern0_test(int autotest);
extern int mem_pattern1_test(int autotest);
extern int mem_pattern2_test(int autotest);
extern int mem_pattern3_test(int autotest);
extern int mem_pattern4_test(int autotest);
extern int mem_pattern5_test(int autotest);
extern int mem_pattern6_test(int autotest);
extern int mem_pattern7_test(int autotest);
extern int mem_pattern8_test(int autotest);

extern int mem_bank_test(int autotest);
extern int mem_bank_normal_test(int autotest);
extern int mem_bank_burn_in_test(int autotest);

extern int mem_align_test(int autotest);
extern int repeat_write_test(int autotest);

extern int mddr_power_mode_1(int autotest);
extern int mddr_power_mode_2(int autotest);
extern int mddr_power_mode_3(int autotest);
extern int mddr_power_mode_4(int autotest);

extern int mddr_power_auto_mode(int autotest);
#endif // _MEMCTRL_H_
