#include <platform.h>
#include <global.h>
#include <genlib.h>
#include <test_item.h>
#include "dependency.h"
#include "mem-ctrl.h"
#include "memctrl-regs.h"
#include "mem-menu.h"


#define SCU_AHB_MODE (0x1<<6)
#define printcount  0xfff

enum bus_mode_select {
		normal = 0,
		fpga_axi,
		fpga_ahb,
	} bus_mode;

static u32 mem_pattern[]=
{
    MEM_PATTERN0,
    MEM_PATTERN1,
    MEM_PATTERN2,
    MEM_PATTERN3,
    MEM_PATTERN4,
    MEM_PATTERN5,
    MEM_PATTERN6,
    MEM_PATTERN7,
};

static u32  addr_start[SQ_MEMORY_BANKS];
static u32  bank_size[SQ_MEMORY_BANKS];
static u32  mem_start = 0;
static u32  mem_end = 0;
static u32  mem_size = 0;
static u32  test_start_addr = 0;
static u32  test_size = 0;
static u32  Module = 0;
static u32  Sram_Retention_Test = 0;
static int  BankNumber = 0;
static u8   initial_mDDR = 0;

static u32 itcm_mem_start;
static u32 itcm_mem_size;
static u32 dtcm_mem_start;
static u32 dtcm_mem_size;


static int mem_rw_test (u32 start, u32 bank_size, u32 pattern, u8 type);
static int mem_bank_pattern_test(int burn);
static int MemoryAddrSetting(void);
static void socle_mDDR_reg_wr(u32 val, u32 reg);
static u32 socle_mDDR_reg_rd(u32 reg);
static void socle_mDDR_mem_wr(u32 val, u32 reg);
static u32 socle_mDDR_mem_rd(u32 reg);

static int ConfigMDDRModule(void);
static int CheckAutoModeStatus(u8 SpecialMode);
static int power_wake_up(void);
static int mddr_compare_data(u32 start, u32 bank_size);
static void mddr_write_data(u32 start, u32 bank_size, u8 clear_flag);

static void fpga_ddr_initial(void);


static void
enable_I_TCM0()
{
	int a;
	int b;
#ifdef CONFIG_MASTER
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c1,1\n"		
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x20000000\n"
		"mcr p15,0,r0,c9,c1,1\n"
		:[output] "=r" (a)
		:[input] "r" (b));
#else
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c1,1\n"
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x20000000\n"
		"mcr p15,0,r0,c9,c1,1\n"
		:[output] "=r" (a)
		:[input] "r" (b));

#endif
}

static void
enable_I_TCM1()
{
	int a;
	int b;
#ifdef CONFIG_MASTER
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c2,0\n"		
		"orr r0, r0, #0x00000001\n"		
		"mcr p15,0,r0,c9,c2,0\n"		
		"mrc p15,0,r0,c9,c1,1\n"		
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x22000000\n"
		"mcr p15,0,r0,c9,c1,1\n"
		:[output] "=r" (a)
		:[input] "r" (b));
#else
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c2,0\n"		
		"orr r0, r0, #0x00000001\n"		
		"mcr p15,0,r0,c9,c2,0\n"
		"mrc p15,0,r0,c9,c1,1\n"
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x22000000\n"
		"mcr p15,0,r0,c9,c1,1\n"
		:[output] "=r" (a)
		:[input] "r" (b));

#endif
}


static u32
enable_D_TCM0()
{
	int a;
	int b;
#ifdef CONFIG_MASTER
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c1,0\n"
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x00040000\n"
		"orr r0, r0, #0x20000000\n"
		"mcr p15,0,r0,c9,c1,0\n"
		"mrc p15,0,r1,c9,c1,0\n"
		:[output] "=r" (a)
		:[input] "r" (b));
#else
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c1,0\n"
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x00040000\n"
		"orr r0, r0, #0x20000000\n"
		"mcr p15,0,r0,c9,c1,0\n"
		"mrc p15,0,r1,c9,c1,0\n"
		:[output] "=r" (a)
		:[input] "r" (b));

#endif
	return 0;
}

static u32
enable_D_TCM1()
{
	int a;
	int b;
#ifdef CONFIG_MASTER
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c2,0\n"		
		"orr r0, r0, #0x00000001\n"		
		"mcr p15,0,r0,c9,c2,0\n"
		"mrc p15,0,r0,c9,c1,0\n"
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x00040000\n"
		"orr r0, r0, #0x22000000\n"
		"mcr p15,0,r0,c9,c1,0\n"
		"mrc p15,0,r1,c9,c1,0\n"
		:[output] "=r" (a)
		:[input] "r" (b));
#else
	__asm__ __volatile__ (
		"mrc p15,0,r0,c9,c2,0\n"		
		"orr r0, r0, #0x00000001\n"		
		"mcr p15,0,r0,c9,c2,0\n"
		"mrc p15,0,r0,c9,c1,0\n"
		"orr r0, r0, #0x00000001\n"
		"orr r0, r0, #0x00040000\n"
		"orr r0, r0, #0x22000000\n"
		"mcr p15,0,r0,c9,c1,0\n"
		"mrc p15,0,r1,c9,c1,0\n"
		:[output] "=r" (a)
		:[input] "r" (b));

#endif
	return 0;
}

int memory_test(int autotest)
{
    int ret = 0;

    ret = test_item_ctrl(&mem_device_select_container, autotest);
    return ret;
}

extern struct test_item_container mem_fpga_container;
int mdk3d_fpga_test(int autotest)
{
	int ret = 0;

    ret = test_item_ctrl(&mem_fpga_container, autotest);
    return ret;
}

extern struct test_item_container mem_fpga_mode_container;
int mem_fpga_axi_test(int autotest)
{
	int ret = 0;

	bus_mode = fpga_axi;
   	ret = test_item_ctrl(&mem_fpga_mode_container, autotest);
	bus_mode = normal;	
    	return ret;

}

int mem_fpga_ahb_test(int autotest)
{
	int ret = 0;

	bus_mode = fpga_ahb;
    	ret = test_item_ctrl(&mem_fpga_mode_container, autotest);
	bus_mode = normal;	
    	return ret;
}

int mem_fpga_ddr2_test(int autotest)
{
	int ret = 0;

	fpga_ddr_initial();

	ret = sdram_test(autotest);

	return ret;

}
int mem_fpga_sram_test(int autotest)
{
	int ret = 0;

	if (bus_mode == fpga_axi)
		iowrite32(0x00000011, 0x30028020);
	else if (bus_mode == fpga_ahb)
		iowrite32(0x00000011, 0x20028020);
		
	ret = sram_test(autotest);

	return ret;
}

int sdram_test(int autotest)
{
    int ret = 0;

    Module = SDRAM_Module;
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int itcm_test0(int autotest)
{
    int ret = 0;

    enable_I_TCM0();
    Module = ITCM_Module;

#ifdef CONFIG_ITCM
	itcm_mem_start = SQ_ITCM_ADDR_START_0;
	itcm_mem_size = SQ_ITCM_ADDR_SIZE_ARM9;
#endif

    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int itcm_test1(int autotest)
{
    int ret = 0;

    enable_I_TCM1();
    Module = ITCM_Module;

#ifdef CONFIG_ITCM
	itcm_mem_start = SQ_ITCM_ADDR_START_1;
	itcm_mem_size = SQ_ITCM_ADDR_SIZE_ARM9;
#endif

    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int dtcm_test0(int autotest)
{
    int ret = 0;

    enable_D_TCM0();
    Module = DTCM_Module;

#ifdef CONFIG_DTCM
	dtcm_mem_start = SQ_DTCM_ADDR_START_0;
	dtcm_mem_size = SQ_DTCM_ADDR_SIZE_ARM9;
#endif

    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int dtcm_test1(int autotest)
{
    int ret = 0;

    enable_D_TCM1();
    Module = DTCM_Module;

#ifdef CONFIG_DTCM
	dtcm_mem_start = SQ_DTCM_ADDR_START_1;
	dtcm_mem_size = SQ_DTCM_ADDR_SIZE_ARM9;
#endif
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int another_dtcm_test(int autotest)
{
    int ret = 0;

#ifdef CONFIG_DTCM	
	enable_D_TCM0();
#endif

    Module = ANOTHER_DTCM_Module;
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int fpga_sram_test(int autotest)
{
    int ret = 0;

    Module = FPGA_SRAM_Module;
    //read scu to get amba mode
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    if (!(ioread32(SQ_SCU0 + 0x28) & SCU_AHB_MODE))
    {
        printf("\n Error!! Don't support AMBA mode \n");
        ret = -1;
        return ret;
    }

    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int mddr_test(int autotest)
{
    int ret = 0;

    Module = MDDR_Module;
    printf("\n------>ConfigMDDRModule \n");
    ret = ConfigMDDRModule();
    if (ret != 0)
    {
        printf("\nmDDR Configure Fail!!!!\n");
        return ret;	
    }
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);
    return ret;
}

int mddr_power_mode_test(int autotest)
{
    int ret = 0;

    Module = MDDR_Module;
    ret = ConfigMDDRModule();
    if (ret != 0)
    {
        printf("\nmDDR Configure Fail!!!!\n");
        return ret;	
    }
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    ret = test_item_ctrl(&mddr_power_mode_container, autotest);    
    return ret;
}

int sram_test(int autotest)
{
    int ret = 0;

    Module = SRAM_Module;
    Sram_Retention_Test = 0;
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);

    #if (Retention_Enable_Disable == 1)
        if (ret != 0)
        {
            return ret;
        }
        Sram_Retention_Test = 1;
        ret = test_item_ctrl(&mem_main_container, autotest);
    #endif
    return ret;
}

int dp_sram_test(int autotest)
{
    int ret = 0;

    Module = DP_SRAM_Module;
    Sram_Retention_Test = 0;
    ret = MemoryAddrSetting();
    if (ret != 0)
    {
        return ret;	
    }
    
    ret = test_item_ctrl(&mem_main_container, autotest);    
    return ret;
}

static int MemoryAddrSetting(void)
{
    int ret = 0;
    u32 size = 0;

    switch (Module)
    {
        case SDRAM_Module:
        {
		if ((bus_mode == fpga_axi) || (bus_mode == fpga_ahb))
			size = 0x8000000;
		else
			size = SQ_MEMORY_ADDR_SIZE;

		if (bus_mode == fpga_axi) {
			mem_start = 0x37000000;
			mem_end = 0x37000000 + size;
		}else if (bus_mode == fpga_ahb) {
			mem_start = 0x27000000;
			mem_end = 0x27000000 + size;
		}else {
			mem_start = (u32)SQ_MEMORY_ADDR_START | (u32)_end;
			mem_end = (u32)SQ_MEMORY_ADDR_START + size;
		}
            mem_size = (u32)mem_end - (u32)mem_start;
            break;
        }
        case ITCM_Module:
        {
            mem_start = itcm_mem_start;
            mem_size = itcm_mem_size;
            mem_end = mem_start + mem_size;
            break;
        }
        case DTCM_Module:
        {
            mem_start = dtcm_mem_start;
            mem_size = dtcm_mem_size;
            mem_end = mem_start + mem_size;
            break;
        }
        case ANOTHER_DTCM_Module:
        {
            mem_start = SQ_ANOTHER_DTCM_ADDR;
            mem_size = SQ_ANOTHER_DTCM_SIZE;
            mem_end = mem_start + mem_size;
            break;
        }
        case FPGA_SRAM_Module:
        {
            mem_start = SQ_FPGA_SRAM_STAR;
            mem_size = SQ_FPGA_SRAM_SIZE;
            mem_end = mem_start + mem_size;
            break;
        }
        case MDDR_Module:
        {
            
            mem_start = (u32)SQ_MM_DDR_SDR_ADDR_START;
            mem_size =(u32) SQ_MM_DDR_SDR_ADDR_SIZE;

            mem_end = mem_start + mem_size;
            return ret;
            break;
        }
        case SRAM_Module:
        {
		if (bus_mode == fpga_axi) {
			mem_start = 0x31000000;
			mem_size = 0x80000;
		}else if (bus_mode == fpga_ahb) {
			mem_start = 0x21000000;
			mem_size = 0x80000;
		}else {
			mem_start = SRAM_BANK0_START;
			mem_size = SRAM_BANK0_SIZE;
		}
            mem_end = mem_start + mem_size;
            break;
        }
        case DP_SRAM_Module:
        {
            mem_start = SQ_INT_DP_SRAM_ADDR;
            mem_size = SQ_INT_DP_SRAM_SIZE;
            mem_end = mem_start + mem_size;
            break;
        }
        default:
            break;
    }	
    return ret;
}

//------------------Start of Range Test ----------------------------------------------------------------------
int mem_range_test(int autotest)
{
    int ret = 0,tmp = 0, burn=0 ,cnt = 1;

    printf("test all range from 0x%08x to 0x%08x\n", mem_start, (mem_end-1));
    test_start_addr = mem_start;
    test_size = mem_size;

    if(!autotest)
    {
        printf("test range : 1.all range 2.set range : ");
        scanf("%d\n", &tmp);
        if(tmp == 2)
        {
            printf("mem_start : ");
            scanf("%x", &test_start_addr);
            printf("mem_size : ");
            scanf("%x", &test_size);
        }
        printf("all pattern burn in(y/n)");
        scanf("%c\n", &burn);
        if(burn == 'y' || burn == 'Y')
        {
            printf("loop : ");
            scanf("%d\n", &burn);
            printf("Start test from 0x%08x to 0x%08x\n", test_start_addr, (test_start_addr+test_size-1));
            while(burn--)
            {
                printf("loop : %d\n", cnt);
                ret = test_item_ctrl(&mem_range_test_container, 1);
                cnt ++;
                if(ret == -1)
                {
                    return ret;
                }
            }
        }
    }

	printf("Start test from 0x%08x to 0x%08x\n", test_start_addr, (test_start_addr+test_size-1));
	ret = test_item_ctrl(&mem_range_test_container, autotest);
    return ret;
}


int mem_pattern0_test(int autotest)
{
    int ret = 0;

    printf("PATTERN0 (0x%08x) test", MEM_PATTERN0);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[0], 0);
    return ret;
}

int mem_pattern1_test(int autotest)
{
    int ret = 0;

    printf("PATTERN1 (0x%08x) test", MEM_PATTERN1);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[1], 0);
    return ret;
}

int mem_pattern2_test(int autotest)
{
    int ret = 0;

    printf("PATTERN2 (0x%08x) test", MEM_PATTERN2);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[2], 0);
    return ret;
}

int mem_pattern3_test(int autotest)
{
    int ret = 0;

    printf("PATTERN3 (0x%08x) test", MEM_PATTERN3);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[3], 0);
    return ret;
}

int mem_pattern4_test(int autotest)
{
    int ret = 0;

    printf("PATTERN4 (0x%08x) test", MEM_PATTERN4);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[4], 0);
    return ret;
}

int mem_pattern5_test(int autotest)
{
    int ret = 0;

    printf("PATTERN5 (0x%08x) test", MEM_PATTERN5);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[5], 0);
    return ret;
}

int mem_pattern6_test(int autotest)
{
    int ret = 0;

    printf("PATTERN6 (0x%08x) test", MEM_PATTERN6);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[6], 0);
    return ret;
}

int mem_pattern7_test(int autotest)
{
    int ret = 0;

    printf("PATTERN7 (0x%08x) test", MEM_PATTERN7);
    ret = mem_rw_test (test_start_addr, test_size, mem_pattern[7], 0);
    return ret;
}

int mem_pattern8_test(int autotest)
{
    int ret = 0;

    printf("PATTERN8 (addr) test");
    ret = mem_rw_test (test_start_addr, test_size, 0,1);
    return ret;
}

static int mem_rw_test (u32 start, u32 bank_size, u32 pattern, u8 type)
{
    register u32  end = start + bank_size;
    register u32  *p;
    u32 count = 0, tmp = 0;
    int ret = 0;


    printf("\n start address=  %x  \n",start);
    printf("\n end address=  %x  \n",end);

    p = (u32 *)start;
    if ( bank_size <= 0 )
    {
        printf("error bank size\n");
        return -1;
    }
    if(type==0)
    {
        while ((u32)p < end)
        {
            *p = pattern;
            if((count % printcount) == 0)
            {
                printf(".");
            }
            p ++;
            count ++;
        }

        if(Sram_Retention_Test)
        {
            tmp = ioread32(0x1d1a0030);
            //sram_retent control enable
            tmp |= 0x80;
            iowrite32(tmp, 0x1d1a0030);
            //sram_power_off
            tmp &= ~0x2;
            iowrite32(tmp, 0x1d1a0030);
            MSDELAY(0x10);
            //sram_power_on
            tmp |= 0x2;
            iowrite32(tmp, 0x1d1a0030);
            //sram_retent control disable
            tmp &= ~0x80;
            iowrite32(tmp, 0x1d1a0030);
        }
        printf("\n Check Result\n");  
        p = (u32 *)start;
        while ((u32)p < end)
        {
            if (pattern != *((volatile u32 *)p))
            {
                printf("\nFail! Error at address(%08X) (%08x) Get(%08x)\n", (u32)p, pattern, *p);
                ret = -1;
            }
            if((count & printcount) == 0)
            {
                printf(".");
            }
            p ++;
            count ++;
        }
    }
    else // if(type==0)
    {
        while ((u32)p < end)
        {
            *p = (u32)p;
            if((count & printcount) == 0)
            {
                printf(".");
            }
            p ++;
            count ++;
        }

        if(Sram_Retention_Test)
        {
            tmp = ioread32(0x1d1a0030);
            //sram_retent control enable
            tmp |= 0x80;
            iowrite32(tmp, 0x1d1a0030);
            //sram_power_off
            tmp &= ~0x2;
            iowrite32(tmp, 0x1d1a0030);
            MSDELAY(0x10);
            //sram_power_on
            tmp |= 0x2;
            iowrite32(tmp, 0x1d1a0030);
            //sram_retent control disable
            tmp &= ~0x80;
            iowrite32(tmp, 0x1d1a0030);
        }

        p = (u32 *)start;
        while ((u32)p < end)
        {
            if ((u32)p != *((volatile u32 *)p))
            {
                printf("\nFail at address(%08x)Patten(%08x) Get(%08x)\n", (u32)p,(u32)p, *p);
                return -1;
            }
            if((count & printcount) == 0)
            {
                printf(".");
            }
            p ++;
            count ++;
        }
    }
    return ret;
}
//------------------End of Range Test ----------------------------------------------------------------------

//------------------Start of Bank Test ----------------------------------------------------------------------
int mem_bank_test(int autotest)
{
    int count = 0, ret = 0;

    switch (Module)
    {
        case SDRAM_Module:
        {
            BankNumber = (int) SQ_MEMORY_BANKS;
            for(count=0;count<BankNumber;count++)
            {
                addr_start[count] = BANK_ADDRESS[count] + 0x200000;
                bank_size[count] = 0x10000;
            }
            break;
        }
        case ITCM_Module:
        {
            printf("\n----[No Bank Test]\n");
            return ret;
            break;
        }
        case DTCM_Module:
        {
            printf("\n----[No Bank Test]\n");
            return ret;
            break;
        }
        case ANOTHER_DTCM_Module:
        {
            printf("\n----[No Bank Test]\n");
            return ret;
            break;
        }
        case FPGA_SRAM_Module:
        {
            printf("\n----[No Bank Test]\n");
            return ret;
            break;
        }
        case MDDR_Module:
        {
            printf("\n----[No Bank Test]\n");
            return ret;
            break;
        }
        case SRAM_Module:
        {
            BankNumber = 2;

            addr_start[0] = SRAM_BANK0_START;
            bank_size[0] = SRAM_BANK0_SIZE;
            addr_start[1] = SRAM_BANK1_START;
            bank_size[1] = SRAM_BANK1_SIZE;
            break;
        }
        case DP_SRAM_Module:
        {
            printf("\n----[No Bank Test]\n");
            return ret;
            break;
        }
        default:
            break;
    }


    ret = test_item_ctrl(&mem_bank_test_container, autotest);
    return ret;
}

int mem_bank_normal_test(int autotest)
{
    return  mem_bank_pattern_test(0);
}

int mem_bank_burn_in_test(int autotest)
{
    return  mem_bank_pattern_test(1);
}
static int mem_bank_pattern_test(int burn)
{
    int ret = 0, pat = 0, bank = 0;

    do
    {
        for (bank = 0; bank < BankNumber; bank++)
        {
            printf("Start Test Special Memory Address (%08x), Size(%08x)\n", addr_start[bank], bank_size[bank]);

            //for(pat=0;pat<(sizeof(mem_pattern)/sizeof(u32));pat++)
            for(pat=0;pat<(sizeof(mem_pattern)>>2);pat++)
            {
                if (burn == 0)
                {
                    printf("---- Test Pattern( %d ): %08x\n",pat ,mem_pattern[pat]);
                }
                else
                {
                    printf(".");
                }
            }
            ret = mem_rw_test(addr_start[bank], bank_size[bank], mem_pattern[pat], 0);
            if (ret != 0)
            {
                printf("\n !!!!!Error happen!!!!\n");
                return ret;
            }
        }
    }while(burn);
    return ret;
}
//------------------End of Bank Test ----------------------------------------------------------------------

//------------------Start of Align Test ----------------------------------------------------------------------
int mem_align_test(int autotest)
{
    int ret = 0;
    u32 *addr;
    u32 align_Gap = 0;
    u32 align_Mask = 0;

    switch (Module)
    {
        case SDRAM_Module:
        {
            align_Mask = 0xfffe0000;
            align_Gap = 0x8000;
            break;
        }
        case ITCM_Module:
        {
            align_Mask = 0xfffe0000;
		align_Gap = 0x1000;
   //         align_Gap = 0x8000;
 //           ret = -1;
   //         printf("\n----[Not ready yet]\n");
   //         return ret;
            break;
        }
        case DTCM_Module:
        {
            align_Mask = 0xfffe0000;
		align_Gap = 0x1000;	
    //        align_Gap = 0x8000;
  //          ret = -1;
  //          printf("\n----[Not ready yet]\n");
  //          return ret;
            break;
        }
        case ANOTHER_DTCM_Module:
        {
            align_Mask = 0xfffe0000;
            align_Gap = 0x8000;
            ret = -1;
            printf("\n----[Not ready yet]\n");
            return ret;
            break;
        } 
        case FPGA_SRAM_Module:
        {
            align_Mask = 0xfffe0000;
            align_Gap = 0x8000;
            ret = -1;
            printf("\n----[Not ready yet]\n");
            return ret;
            break;
        }
        case MDDR_Module:
        {
            align_Mask = 0xfffe0000;
            align_Gap = 0x8000;
            ret = -1;
            printf("\n----[Not ready yet]\n");
            return ret;
            break;
        }
        case SRAM_Module:
        {
            align_Mask = 0xfffe0000;
            align_Gap = 0x1000;
//		align_Gap = 0x8000;
//            ret = -1;
 //           printf("\n----[Not ready yet]\n");
 //           return ret;
            break;
        }
        case DP_SRAM_Module:
        {
            ret = -1;
            printf("\n----[Not ready yet]\n");
            return ret;
            break;
        }
        default:
            break;
    }

    addr = (u32 *)(mem_start & align_Mask) + align_Gap;

    while ((u32)addr < mem_end)
    {
        *addr = (u32) addr;
        addr += align_Gap;
    }

    addr = (u32 *)(mem_start & align_Mask) + align_Gap;
    while((u32)addr < mem_end)
    {
        if(*addr != (u32)addr)
        {
            printf("error occur : addr : 0x%08x (0x%08x), value : 0x%08x\n", addr, addr, *addr);
            return -1;
        }
        addr += align_Gap;
    }
    return ret;
}

//------------------End of Align Test ----------------------------------------------------------------------

//------------------Start of Repeat Write Test ----------------------------------------------------------------------
int repeat_write_test(int autotest)
{
    int ret = 0;
    u32 *addr,addr_value;
    u32 data;
    int loop, tmp;

    switch (Module)
    {
        case SDRAM_Module:
        {
            break;
        }
        case ITCM_Module:
        {
              break;
        }
        case DTCM_Module:
        {
              break;
        }
        case ANOTHER_DTCM_Module:
        {
              break;
        }
        case FPGA_SRAM_Module:
        {
            break;
        }
        case MDDR_Module:
        {
              break;
        }
        case SRAM_Module:
        {
             break;
        }
        case DP_SRAM_Module:
        {
             break;
        }
        default:
            break;
    }

    printf("repeat write someone address\n");
    printf("address : ");
    scanf("%x\n", &addr_value);
    addr = (u32 *)addr_value;

    printf("\ndata : ");
    scanf("%x\n", &data);

    printf("\nloop : ");
    scanf("%x\n", &loop);

    for(tmp=0;tmp<loop;tmp++)
    {
        *addr = data;
        printf("\n Test!!! addr (%x)= data (%x)\n",addr,data);
        if(*addr != data)
        {
            printf("\n---------->error happen");
            return -1;
        }
    }
    return ret;
}
//------------------End of Repeat Write Test ----------------------------------------------------------------------



static void socle_mDDR_reg_wr(u32 val, u32 reg)
{
#ifdef CONFIG_SLAVE

#else
    iowrite32(val, reg + SQ_MM_DDR_SDR_CONFIG_ADDR);
#endif
}
static u32 socle_mDDR_reg_rd(u32 reg)
{
#ifdef CONFIG_SLAVE
    return 0;     
#else
    return ioread32(reg + SQ_MM_DDR_SDR_CONFIG_ADDR);
#endif
}

static void socle_mDDR_mem_wr(u32 val, u32 reg)
{
    iowrite32(val, reg + SQ_MM_DDR_SDR_ADDR_START);
}

static u32 socle_mDDR_mem_rd(u32 reg)
{
    return ioread32(reg + SQ_MM_DDR_SDR_ADDR_START);
}


static int ConfigMDDRModule(void)
{
    int ret = 0;
    
    mem_start = (u32)SQ_MM_DDR_SDR_ADDR_START;
    mem_size =(u32) SQ_MM_DDR_SDR_ADDR_SIZE;
    printf("\naamem_start = 0x%8x,  mem_size =0x%8x\n",mem_start,mem_size); 
    mem_end = mem_start + mem_size;

#ifdef CONFIG_SLAVE
    return ret;
#endif

    if (initial_mDDR == 1)
    {
        return ret;
    }
    else
    {
        initial_mDDR = 1;
    }

    u32 tempvalue = 0;
    u8 mDDRcounter = 0;
    
    socle_mDDR_reg_wr(0x01010101, DENALI_CTL_00); socle_mDDR_reg_wr(0x00010101, DENALI_CTL_01); socle_mDDR_reg_wr(0x00010000 , DENALI_CTL_02); socle_mDDR_reg_wr(0x00000101 , DENALI_CTL_03); 
    socle_mDDR_reg_wr(0x01000001, DENALI_CTL_04); socle_mDDR_reg_wr(0x01000101, DENALI_CTL_05); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_06); socle_mDDR_reg_wr(0x01000100 , DENALI_CTL_07);  
    socle_mDDR_reg_wr(0x00000100, DENALI_CTL_08); socle_mDDR_reg_wr(0x01010000, DENALI_CTL_09); socle_mDDR_reg_wr(0x01010000 , DENALI_CTL_10); socle_mDDR_reg_wr(0x00000101 , DENALI_CTL_11);  
    socle_mDDR_reg_wr(0x03000100, DENALI_CTL_12); socle_mDDR_reg_wr(0x01000000, DENALI_CTL_13); socle_mDDR_reg_wr(0x00020102 , DENALI_CTL_14); socle_mDDR_reg_wr(0x01020000 , DENALI_CTL_15);  
    socle_mDDR_reg_wr(0x01020200, DENALI_CTL_16); socle_mDDR_reg_wr(0x00010001, DENALI_CTL_17); socle_mDDR_reg_wr(0x00010000 , DENALI_CTL_18); socle_mDDR_reg_wr(0x0f010000 , DENALI_CTL_19);  
    socle_mDDR_reg_wr(0x0106060a, DENALI_CTL_20); socle_mDDR_reg_wr(0x02010f01, DENALI_CTL_21); socle_mDDR_reg_wr(0x00030000 , DENALI_CTL_22); socle_mDDR_reg_wr(0x00060102 , DENALI_CTL_23);  
    socle_mDDR_reg_wr(0x02020002, DENALI_CTL_24); socle_mDDR_reg_wr(0x01010302, DENALI_CTL_25); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_26); socle_mDDR_reg_wr(0x04010100 , DENALI_CTL_27);  
    socle_mDDR_reg_wr(0x06000202, DENALI_CTL_28); socle_mDDR_reg_wr(0x64006406, DENALI_CTL_29); socle_mDDR_reg_wr(0x02000000 , DENALI_CTL_30); socle_mDDR_reg_wr(0x00080204 , DENALI_CTL_31);  
    socle_mDDR_reg_wr(0x00000000, DENALI_CTL_32); socle_mDDR_reg_wr(0x00000000, DENALI_CTL_33); socle_mDDR_reg_wr(0x00200020 , DENALI_CTL_34); socle_mDDR_reg_wr(0x00200020 , DENALI_CTL_35);  
    socle_mDDR_reg_wr(0x00320032, DENALI_CTL_36); socle_mDDR_reg_wr(0x00000000, DENALI_CTL_37); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_38); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_39);  
    socle_mDDR_reg_wr(0x06120612, DENALI_CTL_40); socle_mDDR_reg_wr(0x06120612, DENALI_CTL_41); socle_mDDR_reg_wr(0x06120612 , DENALI_CTL_42); socle_mDDR_reg_wr(0x00000612 , DENALI_CTL_43);  
    socle_mDDR_reg_wr(0xffffffff, DENALI_CTL_44); socle_mDDR_reg_wr(0xffffffff, DENALI_CTL_45); socle_mDDR_reg_wr(0x0000ffff , DENALI_CTL_46); socle_mDDR_reg_wr(0x00010000 , DENALI_CTL_47);  
    socle_mDDR_reg_wr(0x00081b58, DENALI_CTL_48); socle_mDDR_reg_wr(0x0000000a, DENALI_CTL_49); socle_mDDR_reg_wr(0x00000014 , DENALI_CTL_50); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_51);  
    socle_mDDR_reg_wr(0x00000000, DENALI_CTL_52); socle_mDDR_reg_wr(0x00000000, DENALI_CTL_53); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_54); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_55);  
    socle_mDDR_reg_wr(0x10149f7b, DENALI_CTL_56); socle_mDDR_reg_wr(0x10149f7b, DENALI_CTL_57); socle_mDDR_reg_wr(0x10149f7b , DENALI_CTL_58); socle_mDDR_reg_wr(0x10149f7b , DENALI_CTL_59);  
    socle_mDDR_reg_wr(0x00141f21, DENALI_CTL_60); socle_mDDR_reg_wr(0x00141f21, DENALI_CTL_61); socle_mDDR_reg_wr(0x00141f21 , DENALI_CTL_62); socle_mDDR_reg_wr(0x00141f21 , DENALI_CTL_63);  
    socle_mDDR_reg_wr(0x00000000, DENALI_CTL_64); socle_mDDR_reg_wr(0x00000000, DENALI_CTL_65); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_66); socle_mDDR_reg_wr(0x00000000 , DENALI_CTL_67);  
    socle_mDDR_reg_wr(0x000f1100, DENALI_CTL_68); socle_mDDR_reg_wr(0xf3003926, DENALI_CTL_69); socle_mDDR_reg_wr(0xf3003926 , DENALI_CTL_70); socle_mDDR_reg_wr(0xf3003926 , DENALI_CTL_71); 
    socle_mDDR_reg_wr(0xf3003926, DENALI_CTL_72); socle_mDDR_reg_wr(0x07400300, DENALI_CTL_73); socle_mDDR_reg_wr(0x07400300 , DENALI_CTL_74); socle_mDDR_reg_wr(0x07400300 , DENALI_CTL_75); 
    socle_mDDR_reg_wr(0x07400300, DENALI_CTL_76); socle_mDDR_reg_wr(0x00800004, DENALI_CTL_77);
    
    socle_mDDR_reg_wr(0x01010100, DENALI_CTL_10);
   
    while ((tempvalue != 0x204) && (tempvalue != 0x284))
    { 
        tempvalue = socle_mDDR_reg_rd(DENALI_CTL_33);
        mDDRcounter++;
        printf("\nMDDR Check--->0x00000084 = %x\n",tempvalue );
        if (mDDRcounter > 10)
        {
            ret = -1;
            break;
        }
    }
    return ret;	
}

int mddr_power_mode_1(int autotest)
{
    int ret = 0;
    u32 tmp= 0;

    mddr_write_data(mem_start, mem_size, 1);
    mddr_write_data(mem_start, mem_size, 0);

    socle_mDDR_reg_wr(0x00100000, DENALI_CTL_26);
    if ((0x00100000 !=  socle_mDDR_reg_rd(DENALI_CTL_26))||(0x01000101 !=  socle_mDDR_reg_rd(DENALI_CTL_03)))
    {
        printf("\n --Fail Power mode 1");
        ret = -1;
    }
    MSDELAY(1000); 
    printf("\n---Please push enter key for continuous\n");
    scanf("%d\n", &tmp);
    ret = power_wake_up();
    if (ret != 0)
        return ret; 
    ret = mddr_compare_data(mem_start,mem_size );

    return ret;
}

int mddr_power_mode_2(int autotest)
{
    int ret = 0;
  
    mddr_write_data(mem_start, mem_size, 1);
    mddr_write_data(mem_start, mem_size, 0);
  
    socle_mDDR_reg_wr(0x00080000, DENALI_CTL_26);   
    if ((0x00080000 !=  socle_mDDR_reg_rd(DENALI_CTL_26))||(0x01000101 !=  socle_mDDR_reg_rd(DENALI_CTL_03)))
    {
        printf("\n --Fail Power mode 2");
        ret = -1;
    }
    MSDELAY(1000); 
    ret = power_wake_up();
    if (ret != 0)
        return ret; 
    ret = mddr_compare_data(mem_start,mem_size );
    return ret;
}

int mddr_power_mode_3(int autotest)
{
    int ret = 0;
    u32 tmp= 0;

    mddr_write_data(mem_start, mem_size, 1);
    mddr_write_data(mem_start, mem_size, 0);

    socle_mDDR_reg_wr(0x00040000, DENALI_CTL_26);   
    if ((0x00040000 !=  socle_mDDR_reg_rd(DENALI_CTL_26))||(0x01000101 !=  socle_mDDR_reg_rd(DENALI_CTL_03)))
    {
        printf("\n --Fail Power mode 3");
        ret = -1;
    }
    MSDELAY(1000);
    printf("\n---Please push enter key for continuous\n");
    scanf("%d\n", &tmp);
    ret = power_wake_up();
    if (ret != 0)
        return ret; 
    ret = mddr_compare_data(mem_start,mem_size );
    return ret;
}

int mddr_power_mode_4(int autotest)
{
    int ret = 0;

    mddr_write_data(mem_start, mem_size, 1);
    mddr_write_data(mem_start, mem_size, 0);
    
    socle_mDDR_reg_wr(0x00020000, DENALI_CTL_26);   
    if ((0x00020000 !=  socle_mDDR_reg_rd(DENALI_CTL_26))||(0x01000101 !=  socle_mDDR_reg_rd(DENALI_CTL_03)))
    {
        printf("\n --Fail Power mode 4");
        ret = -1;
    }
    MSDELAY(1000); 
    ret = power_wake_up();
    if (ret != 0)
        return ret; 
    ret = mddr_compare_data(mem_start,mem_size );
    return ret;
}

int mddr_power_auto_mode(int autotest)
{
    int ret = 0;
    
    
    //entry auto power mode 
    socle_mDDR_reg_wr(0x00001f00, DENALI_CTL_26);   
    
    //set mode 1
    printf("\n Power Auto Mode 1\n");
    socle_mDDR_reg_wr(0x00101f00, DENALI_CTL_26);
    ret = CheckAutoModeStatus(0);
    if (ret != 0)
    {
        goto End;
    }

    //set mode 2
    printf("\n Power Auto Mode 2\n");
    socle_mDDR_reg_wr(0x00081f00, DENALI_CTL_26);
    ret = CheckAutoModeStatus(0);
    if (ret != 0)
    {
        goto End;
    }

    //set mode 3
    printf("\n Power Auto Mode 3\n");
    socle_mDDR_reg_wr(0x00041f00, DENALI_CTL_26);
    ret = CheckAutoModeStatus(0);
    if (ret != 0)
    {
        goto End;
    }
    //set mode 4
    printf("\n Power Auto Mode 4\n");
    socle_mDDR_reg_wr(0x00021f00 , DENALI_CTL_26);
    ret = CheckAutoModeStatus(0);
    if (ret != 0)
    {
        goto End;
    }

    //set mode 5
    printf("\n Power Auto Mode 5\n");
    socle_mDDR_reg_wr(0x00011f00 , DENALI_CTL_26);
    ret = CheckAutoModeStatus(1);
    if (ret != 0)
    {
        goto End;
    }

End:
    socle_mDDR_reg_wr(0x00000000, DENALI_CTL_26);
    return ret;
}

static int CheckAutoModeStatus(u8 SpecialMode)
{
    int ret = 0;
    u32 tempval = 0x1cc00000;
    //u32 tmp;
    
    // wait for enty save mode then check status 
    //printf("\n---Please push enter key for continuous\n");
    //scanf("%d\n", &tmp);
    MSDELAY(3000);// at least 3 sec 
    
    if ((0x01000101 !=  socle_mDDR_reg_rd(DENALI_CTL_03)) && (SpecialMode == 0)) 
    {
        printf("\n --Fail entry Power auto mode 0x%8x\n",socle_mDDR_reg_rd(DENALI_CTL_03));
        ret = -1;
        return ret;
    }
    mddr_write_data(mem_start, mem_size, 0);
    ret = mddr_compare_data(mem_start,mem_size );
    if (ret != 0)
        return ret;
    // Read/Wrtie
    socle_mDDR_mem_wr(tempval,0x00000000);    
    if (tempval != socle_mDDR_mem_rd(0x00000000))
    {
    	printf("\n --Read/Write Fail 0x%8x\n",socle_mDDR_mem_rd(0x00000000));
    	ret = -1;
        return ret;
    }    
    if (0x00000101 !=  socle_mDDR_reg_rd(DENALI_CTL_03))
    {
        printf("\n --Read/Write Condition fail 0x%8x\n",socle_mDDR_reg_rd(DENALI_CTL_03));
        ret = -1;
        return ret;
    }
    return ret;	
}


static void mddr_write_data(u32 start, u32 bank_size, u8 clear_flag)
{
    register u32  end = start + bank_size;
    register u32  *p;
    u32 count = 0; 

    p = (u32 *)start;
    while ((u32)p < end)
    {
        if (clear_flag)
            *p = 0;
        else    
            *p = (u32)p;

        if((count & printcount) == 0)
        {
            //printf(".");
        }
        #if 0
        if ( count < 10 )
        {
         printf("\n Addr=0x%8x =0x%8x", p, *((volatile u32 *)p) );
           //  printf("\n.");
        }  
        #endif             
        
        p ++;
        count ++;
    }
}

static int mddr_compare_data(u32 start, u32 bank_size)
{
    register u32  end = start + bank_size;
    register u32  *p;
    u32 count = 0;
    int ret = 0;

    p = (u32 *)start;
    while ((u32)p < end)
    {
        #if 0
        //if ( ( count % 1 ) == 0)
        {
         printf("\n Addr=0x%8x =0x%8x", p, (u32)p );
        }  
        #endif             
        if ((u32)p != *((volatile u32 *)p))
        {
            printf("\nFail at address(%08x)Patten(%08x) Get(%08x)\n", (u32)p,(u32)p, *p);
            return -1;
        }
        if((count & printcount) == 0)
        {
            //printf(".");
        }
        p ++;
        count ++;
    }
    return ret; 
}

static int power_wake_up(void)
{
    int ret = 0;
    
    socle_mDDR_reg_wr(0x00000000, DENALI_CTL_26);   
    if ((0x00000000 !=  socle_mDDR_reg_rd(DENALI_CTL_26))||(0x00000101 !=  socle_mDDR_reg_rd(DENALI_CTL_03)))
    {
        printf("\n --Fail Power wake up");
        ret = -1;
    }    
    return ret;
}

static void
fpga_ddr_initial(void)
{
	if (bus_mode == fpga_axi) {
		iowrite32(0x00000101, 0x30060000);
		iowrite32(0x00000100, 0x30060004);
		iowrite32(0x00010100, 0x30060008);
		iowrite32(0x01000100, 0x3006000c);
		iowrite32(0x00000101, 0x30060010);
		iowrite32(0x00010100, 0x30060018);
		iowrite32(0x00000001, 0x3006001c);
		iowrite32(0x01010101, 0x30060024);
		iowrite32(0x02020000, 0x30060028);
		iowrite32(0x03010200, 0x30060034);
		iowrite32(0x00030005, 0x30060038);
		iowrite32(0x03000300, 0x3006003c);
		iowrite32(0x0a0f0002, 0x30060040);
		iowrite32(0x040f0a0a, 0x30060044);
		iowrite32(0x00000002, 0x30060048);
		iowrite32(0x01020004, 0x3006004c);
		iowrite32(0x00010007, 0x30060050);
		iowrite32(0x04020402, 0x30060054);
		iowrite32(0x00000004, 0x30060058);
		iowrite32(0x0e080000, 0x3006005c);
		iowrite32(0x00040f02, 0x30060060);
		iowrite32(0x00320000, 0x30060064);
		iowrite32(0x040b0400, 0x30060070);
		iowrite32(0x00000035, 0x30060074);
		iowrite32(0x081b0000, 0x30060080);
		iowrite32(0x00000040, 0x30060084);
		iowrite32(0x08520000, 0x30060088);
		iowrite32(0x0000ffff, 0x3006008c);
		iowrite32(0x00c8006b, 0x300600a0);
		iowrite32(0x48e10002, 0x300600a4);
		iowrite32(0x00c80038, 0x300600a8);
		iowrite32(0x00000036, 0x300600b0);
		iowrite32(0x01010001, 0x30060020);
	}
	else if (bus_mode == fpga_ahb) {
		iowrite32(0x00000101, 0x20060000);
		iowrite32(0x00000100, 0x20060004);
		iowrite32(0x00010100, 0x20060008);
		iowrite32(0x01000100, 0x2006000c);
		iowrite32(0x00000101, 0x20060010);
		iowrite32(0x00010100, 0x20060018);
		iowrite32(0x00000001, 0x2006001c);
		iowrite32(0x01010101, 0x20060024);
		iowrite32(0x02020000, 0x20060028);
		iowrite32(0x03010200, 0x20060034);
		iowrite32(0x00030005, 0x20060038);
		iowrite32(0x03000300, 0x2006003c);
		iowrite32(0x0a0f0002, 0x20060040);
		iowrite32(0x040f0a0a, 0x20060044);
		iowrite32(0x00000002, 0x20060048);
		iowrite32(0x01020004, 0x2006004c);
		iowrite32(0x00010007, 0x20060050);
		iowrite32(0x04020402, 0x20060054);
		iowrite32(0x00000004, 0x20060058);
		iowrite32(0x0e080000, 0x2006005c);
		iowrite32(0x00040f02, 0x20060060);
		iowrite32(0x00320000, 0x20060064);
		iowrite32(0x040b0400, 0x20060070);
		iowrite32(0x00000035, 0x20060074);
		iowrite32(0x081b0000, 0x20060080);
		iowrite32(0x00000040, 0x20060084);
		iowrite32(0x08520000, 0x20060088);
		iowrite32(0x0000ffff, 0x2006008c);
		iowrite32(0x00c8006b, 0x200600a0);
		iowrite32(0x48e10002, 0x200600a4);
		iowrite32(0x00c80038, 0x200600a8);
		iowrite32(0x00000036, 0x200600b0);
		iowrite32(0x01010001, 0x20060020);
	}
	
}


#if 0
#include "gzip_pattern.h"

int gunzip(void *dst, int dstlen, unsigned char *src, unsigned long *lenp);

extern char *trace_point = 0x43000100;
extern int *function_point = 0x43000000;

extern int
gunzip_test(int autotest)
{
	unsigned int dstlen = 0x400000;
	unsigned long lenp;
	unsigned char *src;
	void *dst;	

	src=__3_jpg_gz;
	lenp=__3_jpg_gz_len;

	
	//printf("--------------------\n");
	//printf("dst : 0x%08x\n", dst);
	//printf("dstlen : 0x%08x\n", dstlen);
	//printf("src : 0x%08x\n", src);
	//printf("lenp : 0x%08x\n", lenp);
	
	
	//trace_point = 0x43000100;
	//function_point = 0x43000000;


	gunzip (dst, dstlen, src, &lenp);

	//iowrite32(trace_point, 0x43000000);
	return 0;
}
#endif
