#ifndef _regs_MEMCTRL_H_
#define _regs_MEMCTRL_H_

static UINT BANK_ADDRESS[SOCLE_MEMORY_BANKS] = 
{ 
    SOCLE_MM_DDR_SDR_BANK0,
    #ifdef  SOCLE_MM_DDR_SDR_BANK1
        SOCLE_MM_DDR_SDR_BANK1,
    #endif
    #ifdef  SOCLE_MM_DDR_SDR_BANK2
        SOCLE_MM_DDR_SDR_BANK2,
    #endif
    #ifdef  SOCLE_MM_DDR_SDR_BANK3
        SOCLE_MM_DDR_SDR_BANK3,
    #endif
    #ifdef  SOCLE_MM_DDR_SDR_BANK4
        SOCLE_MM_DDR_SDR_BANK4,
    #endif
    #ifdef  SOCLE_MM_DDR_SDR_BANK5
        SOCLE_MM_DDR_SDR_BANK5,
    #endif
    #ifdef  SOCLE_MM_DDR_SDR_BANK6
        SOCLE_MM_DDR_SDR_BANK6,
    #endif
    #ifdef  SOCLE_MM_DDR_SDR_BANK7
        SOCLE_MM_DDR_SDR_BANK7,
    #endif            
};
                    
#define SOCLE_DDRMC0_SD_BASIC   (addr_t) (SOCLE_DDRMC0 + 0x000C)
#define SOCLE_DDRMC0_AHBS1CR    (addr_t) (SOCLE_DDRMC0 + 0x0050)
#define SOCLE_DDRMC0_AHBS2CR    (addr_t) (SOCLE_DDRMC0 + 0x0054)
#define SOCLE_DDRMC0_AHBS3CR    (addr_t) (SOCLE_DDRMC0 + 0x0058)
#define SOCLE_DDRMC0_AHBS4CR    (addr_t) (SOCLE_DDRMC0 + 0x005C)

#define AHBSnCR_PRIORITY_S  0
#define AHBSnCR_RESPONSE_S  3

#define AHBSnCR_PRIORITY_M  (7<<AHBSnCR_PRIORITY_S)
#define AHBSnCR_RESPONSE_M  (1<<AHBSnCR_RESPONSE_S)

#define AHBS1CR_SET_PRIORITY(val) writew(((readw(SOCLE_DDRMC0_AHBS1CR) & (~AHBSnCR_PRIORITY_M)) | (val << AHBSnCR_PRIORITY_S)),SOCLE_DDRMC0_AHBS1CR)
#define AHBS2CR_SET_PRIORITY(val) writew(((readw(SOCLE_DDRMC0_AHBS2CR) & (~AHBSnCR_PRIORITY_M)) | (val << AHBSnCR_PRIORITY_S)),SOCLE_DDRMC0_AHBS2CR)
#define AHBS3CR_SET_PRIORITY(val) writew(((readw(SOCLE_DDRMC0_AHBS3CR) & (~AHBSnCR_PRIORITY_M)) | (val << AHBSnCR_PRIORITY_S)),SOCLE_DDRMC0_AHBS3CR)
#define AHBS4CR_SET_PRIORITY(val) writew(((readw(SOCLE_DDRMC0_AHBS4CR) & (~AHBSnCR_PRIORITY_M)) | (val << AHBSnCR_PRIORITY_S)),SOCLE_DDRMC0_AHBS4CR)
#define AHBS1CR_SET_RESPONSE(val) writew(((readw(SOCLE_DDRMC0_AHBS1CR) & (~AHBSnCR_RESPONSE_M)) | (val << AHBSnCR_RESPONSE_S)),SOCLE_DDRMC0_AHBS1CR)
#define AHBS2CR_SET_RESPONSE(val) writew(((readw(SOCLE_DDRMC0_AHBS2CR) & (~AHBSnCR_RESPONSE_M)) | (val << AHBSnCR_RESPONSE_S)),SOCLE_DDRMC0_AHBS2CR)
#define AHBS3CR_SET_RESPONSE(val) writew(((readw(SOCLE_DDRMC0_AHBS3CR) & (~AHBSnCR_RESPONSE_M)) | (val << AHBSnCR_RESPONSE_S)),SOCLE_DDRMC0_AHBS3CR)
#define AHBS4CR_SET_RESPONSE(val) writew(((readw(SOCLE_DDRMC0_AHBS4CR) & (~AHBSnCR_RESPONSE_M)) | (val << AHBSnCR_RESPONSE_S)),SOCLE_DDRMC0_AHBS4CR)

#define AHBS1CR_clear   writew(0,SOCLE_DDRMC0_AHBS1CR)
#define AHBS2CR_clear   writew(0,SOCLE_DDRMC0_AHBS2CR)
#define AHBS3CR_clear   writew(0,SOCLE_DDRMC0_AHBS3CR)
#define AHBS4CR_clear   writew(0,SOCLE_DDRMC0_AHBS4CR)
      
#define SD_BASIC_ARBITER_S  4
#define SD_BASIC_ARBITER_M  (1<<SD_BASIC_ARBITER_S)
#define FIX   1
#define RR    0

#define SD_BASIC_ARBITER(val) writew(((readw(SOCLE_DDRMC0_SD_BASIC) & (~SD_BASIC_ARBITER_M)) | \
                              (val << SD_BASIC_ARBITER_S)),SOCLE_DDRMC0_SD_BASIC)

#define DENALI_CTL_00     0x00 
#define DENALI_CTL_01     0x04 
#define DENALI_CTL_02     0x08 
#define DENALI_CTL_03     0x0c 
#define DENALI_CTL_04     0x10 
#define DENALI_CTL_05     0x14 
#define DENALI_CTL_06     0x18 
#define DENALI_CTL_07     0x1c 
#define DENALI_CTL_08     0x20 
#define DENALI_CTL_09     0x24 
#define DENALI_CTL_10     0x28 
#define DENALI_CTL_11     0x2c 
#define DENALI_CTL_12     0x30 
#define DENALI_CTL_13     0x34 
#define DENALI_CTL_14     0x38 
#define DENALI_CTL_15     0x3c 
#define DENALI_CTL_16     0x40 
#define DENALI_CTL_17     0x44 
#define DENALI_CTL_18     0x48 
#define DENALI_CTL_19     0x4c 
#define DENALI_CTL_20     0x50 
#define DENALI_CTL_21     0x54 
#define DENALI_CTL_22     0x58 
#define DENALI_CTL_23     0x5c 
#define DENALI_CTL_24     0x60 
#define DENALI_CTL_25     0x64 
#define DENALI_CTL_26     0x68 
#define DENALI_CTL_27     0x6c 
#define DENALI_CTL_28     0x70 
#define DENALI_CTL_29     0x74 
#define DENALI_CTL_30     0x78 
#define DENALI_CTL_31     0x7c 
#define DENALI_CTL_32     0x80 
#define DENALI_CTL_33     0x84 
#define DENALI_CTL_34     0x88 
#define DENALI_CTL_35     0x8c 
#define DENALI_CTL_36     0x90 
#define DENALI_CTL_37     0x94 
#define DENALI_CTL_38     0x98 
#define DENALI_CTL_39     0x9c 
#define DENALI_CTL_40     0xa0 
#define DENALI_CTL_41     0xa4 
#define DENALI_CTL_42     0xa8 
#define DENALI_CTL_43     0xac 
#define DENALI_CTL_44     0xb0 
#define DENALI_CTL_45     0xb4 
#define DENALI_CTL_46     0xb8 
#define DENALI_CTL_47     0xbc 
#define DENALI_CTL_48     0xc0 
#define DENALI_CTL_49     0xc4 
#define DENALI_CTL_50     0xc8 
#define DENALI_CTL_51     0xcc 
#define DENALI_CTL_52     0xd0 
#define DENALI_CTL_53     0xd4 
#define DENALI_CTL_54     0xd8 
#define DENALI_CTL_55     0xdc 
#define DENALI_CTL_56     0xe0 
#define DENALI_CTL_57     0xe4 
#define DENALI_CTL_58     0xe8 
#define DENALI_CTL_59     0xec 
#define DENALI_CTL_60     0xf0 
#define DENALI_CTL_61     0xf4 
#define DENALI_CTL_62     0xf8 
#define DENALI_CTL_63     0xfc 
#define DENALI_CTL_64     0x100 
#define DENALI_CTL_65     0x104 
#define DENALI_CTL_66     0x108 
#define DENALI_CTL_67     0x10c 
#define DENALI_CTL_68     0x110 
#define DENALI_CTL_69     0x114 
#define DENALI_CTL_70     0x118 
#define DENALI_CTL_71     0x11c 
#define DENALI_CTL_72     0x120 
#define DENALI_CTL_73     0x124 
#define DENALI_CTL_74     0x128 
#define DENALI_CTL_75     0x12c 
#define DENALI_CTL_76     0x130 
#define DENALI_CTL_77     0x134 
#define DENALI_CTL_78     0x138 
#define DENALI_CTL_79     0x13c 
#define DENALI_CTL_80     0x140 
#define DENALI_CTL_81     0x144 
#define DENALI_CTL_82     0x148 
#define DENALI_CTL_83     0x14c 
#define DENALI_CTL_84     0x150 
#define DENALI_CTL_85     0x154 
#define DENALI_CTL_86     0x158 
#define DENALI_CTL_87     0x15c 
#define DENALI_CTL_88     0x160 
#define DENALI_CTL_89     0x164 
#define DENALI_CTL_90     0x168 
#define DENALI_CTL_91     0x16c 
#define DENALI_CTL_92     0x170 
#define DENALI_CTL_93     0x174 
#define DENALI_CTL_94     0x178 
#define DENALI_CTL_95     0x17c 
#define DENALI_CTL_96     0x180 
#define DENALI_CTL_97     0x184 
#define DENALI_CTL_98     0x188 
#define DENALI_CTL_99     0x18c 
#define DENALI_CTL_100    0x190 
#define DENALI_CTL_101    0x194 
#define DENALI_CTL_102    0x198 
#define DENALI_CTL_103    0x19c 
#define DENALI_CTL_104    0x1a0 
#define DENALI_CTL_105    0x1a4 
#define DENALI_CTL_106    0x1a8 
#define DENALI_CTL_107    0x1ac 
#define DENALI_CTL_108    0x1b0 
#define DENALI_CTL_109    0x1b4 
#define DENALI_CTL_110    0x1b8 
#define DENALI_CTL_111    0x1bc 
#define DENALI_CTL_112    0x1c0 
#define DENALI_CTL_113    0x1c4 
#define DENALI_CTL_114    0x1c8 
#define DENALI_CTL_115    0x1cc 
#define DENALI_CTL_116    0x1d0 
#define DENALI_CTL_117    0x1d4 
#define DENALI_CTL_118    0x1d8 
#define DENALI_CTL_119    0x1dc 
#define DENALI_CTL_120    0x1e0 
#define DENALI_CTL_121    0x1e4 
#define DENALI_CTL_122    0x1e8 
#define DENALI_CTL_123    0x1ec 
#define DENALI_CTL_124    0x1f0 
#define DENALI_CTL_125    0x1f4 
#define DENALI_CTL_126    0x1f8 
#define DENALI_CTL_127    0x1fc 
#define DENALI_CTL_128    0x200 
#define DENALI_CTL_129    0x204 
#define DENALI_CTL_130    0x208 
#define DENALI_CTL_131    0x20c 
#define DENALI_CTL_132    0x210 
#define DENALI_CTL_133    0x214 
#define DENALI_CTL_134    0x218 
#define DENALI_CTL_135    0x21c 
#define DENALI_CTL_136    0x220 
#define DENALI_CTL_137    0x224 
#define DENALI_CTL_138    0x228 
#define DENALI_CTL_139    0x22c 
#define DENALI_CTL_140    0x230 
#define DENALI_CTL_141    0x234 
#define DENALI_CTL_142    0x238 
#define DENALI_CTL_143    0x23c 
#define DENALI_CTL_144    0x240 
#define DENALI_CTL_145    0x244 
#define DENALI_CTL_146    0x248 
#define DENALI_CTL_147    0x24c 
#define DENALI_CTL_148    0x250 
#define DENALI_CTL_149    0x254 
#define DENALI_CTL_150    0x258 
#define DENALI_CTL_151    0x25c 
#define DENALI_CTL_152    0x260 
#define DENALI_CTL_153    0x264 
#define DENALI_CTL_154    0x268 
#define DENALI_CTL_155    0x26c 
#define DENALI_CTL_156    0x270 
#define DENALI_CTL_157    0x274 
#endif //_regs_MEMCTRL_H_
