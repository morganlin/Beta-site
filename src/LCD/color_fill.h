
#define  COLOR_BALCK 			0X00000000
#define  COLOR_RED  				0X00FF0000
#define  COLOR_GREEN  			0X0000FF00
#define  COLOR_BLUE  			0X000000FF
#define  COLOR_CYAN  			0X0000FFFF
#define  COLOR_MAGENTA  		0X00FF00FF
#define  COLOR_YELLOW  			0X00FFFF00
#define  COLOR_WHITE  			0X00FFFFFF

#define  COLOR_BALCK_16BPP_R		0X0000
#define  COLOR_RED_16BPP_R 			0XFC00
#define  COLOR_GREEN_16BPP_R  		0X03E0
#define  COLOR_BLUE_16BPP_R		0X001F
#define  COLOR_CYAN_16BPP_R		0X03FF
#define  COLOR_MAGENTA_16BPP_R 	0XFC1F
#define  COLOR_YELLOW_16BPP_R 		0XFFE0
#define  COLOR_WHITE_16BPP_R 		0XFFFF

#define  COLOR_BALCK_16BPP_G		0X0000
#define  COLOR_RED_16BPP_G 			0XF800
#define  COLOR_GREEN_16BPP_G  		0X07E0
#define  COLOR_BLUE_16BPP_G		0X001F
#define  COLOR_CYAN_16BPP_G		0X07FF
#define  COLOR_MAGENTA_16BPP_G 	0XF81F
#define  COLOR_YELLOW_16BPP_G 		0xFFE0
#define  COLOR_WHITE_16BPP_G 		0XFFFF

#define  COLOR_BALCK_16BPP_B		0X0000
#define  COLOR_RED_16BPP_B 			0XF800
#define  COLOR_GREEN_16BPP_B  		0X07C0
#define  COLOR_BLUE_16BPP_B		0X003F
#define  COLOR_CYAN_16BPP_B		0X07FF
#define  COLOR_MAGENTA_16BPP_B 	0XF83F
#define  COLOR_YELLOW_16BPP_B 		0XFFC0
#define  COLOR_WHITE_16BPP_B 		0XFFFF

//Lum 000
#define  COLOR_BALCK_LUM_0			0X0000
#define  COLOR_RED_LUM_0 			0x7C00
#define  COLOR_GREEN_LUM_0  		0x03E0
#define  COLOR_BLUE_LUM_0			0x001F
#define  COLOR_CYAN_LUM_0			0x03FF
#define  COLOR_MAGENTA_LUM_0 		0x7C1F
#define  COLOR_YELLOW_LUM_0 		0x7FE0
#define  COLOR_WHITE_LUM_0 		0x7FFF


//Lum - R
#define  COLOR_BALCK_LUM_R			0X0000
#define  COLOR_RED_LUM_R 			0xFC00
#define  COLOR_GREEN_LUM_R  		0x03E0
#define  COLOR_BLUE_LUM_R			0x001F
#define  COLOR_CYAN_LUM_R			0x03FF
#define  COLOR_MAGENTA_LUM_R 		0xFC1F
#define  COLOR_YELLOW_LUM_R 		0xFFE0
#define  COLOR_WHITE_LUM_R 		0xFFFF

//Lum - G
#define  COLOR_BALCK_LUM_G			0X0000
#define  COLOR_RED_LUM_G 			0x7C00
#define  COLOR_GREEN_LUM_G  		0x83E0
#define  COLOR_BLUE_LUM_G			0x001F
#define  COLOR_CYAN_LUM_G			0x83FF
#define  COLOR_MAGENTA_LUM_G 		0x7C1F
#define  COLOR_YELLOW_LUM_G 		0xFFE0
#define  COLOR_WHITE_LUM_G 		0xFFFF

//Lum - B
#define  COLOR_BALCK_LUM_B			0X0000
#define  COLOR_RED_LUM_B 			0x7C00
#define  COLOR_GREEN_LUM_B  		0x03E0
#define  COLOR_BLUE_LUM_B			0x801F
#define  COLOR_CYAN_LUM_B			0x83FF
#define  COLOR_MAGENTA_LUM_B 		0xFC1F
#define  COLOR_YELLOW_LUM_B 		0x7FE0
#define  COLOR_WHITE_LUM_B 		0xFFFF

//Lum RGB
#define  COLOR_BALCK_LUM_RGB		0X0000
#define  COLOR_RED_LUM_RGB			0xFC00
#define  COLOR_GREEN_LUM_RGB  		0x83E0
#define  COLOR_BLUE_LUM_RGB		0x801F
#define  COLOR_CYAN_LUM_RGB		0x83FF
#define  COLOR_MAGENTA_LUM_RGB	0xFC1F
#define  COLOR_YELLOW_LUM_RGB 		0xFFE0
#define  COLOR_WHITE_LUM_RGB 		0xFFFF




//...............................
#define WHITE_Y		235
#define WHITE_CB	128
#define	WHITE_CR	128
#define YELLOW_Y	162
#define YELLOW_CB	44
#define	YELLOW_CR	142
#define CYAN_Y		131
#define CYAN_CB		156
#define	CYAN_CR		44
#define GREEN_Y		112
#define GREEN_CB	72
#define	GREEN_CR	58
#define MAGENTA_Y	84
#define MAGENTA_CB	184
#define	MAGENTA_CR	198
#define RED_Y		65
#define RED_CB		100
#define	RED_CR		212
#define BLUE_Y		35
#define BLUE_CB		212
#define	BLUE_CR		114
#define	BLACK_Y		16
#define BLACK_CB	128
#define	BLACK_CR	128

enum {
	BLACK=0,
	RED,
	GREEN,
	BLUE,
	CYAN,
	MAGENTA,
	YELLOW,
	WHITE,
	COLORBAR,
};


struct color_info {
	int xres;
	int yres;
	int color;
	int mode;  
	unsigned int addr;
};

int socle_color_fill(struct color_info *cinfo);
void socle_yuv2rgb(unsigned char y,unsigned char u, unsigned char v, unsigned int *rgb);


