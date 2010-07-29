#include "color_fill.h"

enum {
	NONE=0,
	RGB888,
	RGB655,
	RGB565,
	RGB556,
	RGB555,
	RGB555_R,
	RGB555_G,
	RGB555_B,
	YUV422,
	YUV420
};

unsigned int BPP24_RGB[8]=
{
		COLOR_BALCK,
		COLOR_RED, 
		COLOR_GREEN,
		COLOR_BLUE, 
		COLOR_CYAN,
		COLOR_MAGENTA, 
		COLOR_YELLOW,  
		COLOR_WHITE,  
};

unsigned short BPP16_RGB_R[8] =
{
		COLOR_BALCK_16BPP_R,
		COLOR_RED_16BPP_R,
		COLOR_GREEN_16BPP_R,
		COLOR_BLUE_16BPP_R,
		COLOR_CYAN_16BPP_R,
		COLOR_MAGENTA_16BPP_R,
		COLOR_YELLOW_16BPP_R,
		COLOR_WHITE_16BPP_R
};

unsigned short BPP16_RGB_G[8] =
{
		COLOR_BALCK_16BPP_G,
		COLOR_RED_16BPP_G,
		COLOR_GREEN_16BPP_G,
		COLOR_BLUE_16BPP_G,
		COLOR_CYAN_16BPP_G,
		COLOR_MAGENTA_16BPP_G,
		COLOR_YELLOW_16BPP_G,
		COLOR_WHITE_16BPP_G
};

unsigned short BPP16_RGB_B[8] =
{
		COLOR_BALCK_16BPP_B,
		COLOR_RED_16BPP_B,
		COLOR_GREEN_16BPP_B,
		COLOR_BLUE_16BPP_B,
		COLOR_CYAN_16BPP_B,
		COLOR_MAGENTA_16BPP_B,
		COLOR_YELLOW_16BPP_B,
		COLOR_WHITE_16BPP_B
};


unsigned short BPP16_LUM_0[8] =
{
		COLOR_BALCK_LUM_0,
		COLOR_RED_LUM_0,
		COLOR_GREEN_LUM_0,
		COLOR_BLUE_LUM_0,
		COLOR_CYAN_LUM_0,
		COLOR_MAGENTA_LUM_0,
		COLOR_YELLOW_LUM_0,
		COLOR_WHITE_LUM_0
};

unsigned short BPP16_LUM_R[8] =
{
		COLOR_BALCK_LUM_R,
		COLOR_RED_LUM_R,
		COLOR_GREEN_LUM_R,
		COLOR_BLUE_LUM_R,
		COLOR_CYAN_LUM_R,
		COLOR_MAGENTA_LUM_R,
		COLOR_YELLOW_LUM_R,
		COLOR_WHITE_LUM_R
};

unsigned short BPP16_LUM_G[8] =
{
		COLOR_BALCK_LUM_G,
		COLOR_RED_LUM_G,
		COLOR_GREEN_LUM_G,
		COLOR_BLUE_LUM_G,
		COLOR_CYAN_LUM_G,
		COLOR_MAGENTA_LUM_G,
		COLOR_YELLOW_LUM_G,
		COLOR_WHITE_LUM_G
};

unsigned short BPP16_LUM_B[8] =
{
		COLOR_BALCK_LUM_B,
		COLOR_RED_LUM_B,
		COLOR_GREEN_LUM_B,
		COLOR_BLUE_LUM_B,
		COLOR_CYAN_LUM_B,
		COLOR_MAGENTA_LUM_B,
		COLOR_YELLOW_LUM_B,
		COLOR_WHITE_LUM_B
};

unsigned short BPP16_LUM_RGB[8] =
{
		COLOR_BALCK_LUM_RGB,
		COLOR_RED_LUM_RGB,
		COLOR_GREEN_LUM_RGB,
		COLOR_BLUE_LUM_RGB,
		COLOR_CYAN_LUM_RGB,
		COLOR_MAGENTA_LUM_RGB,
		COLOR_YELLOW_LUM_RGB,
		COLOR_WHITE_LUM_RGB
};

unsigned char colorpatternY[] = {
  	BLACK_Y, RED_Y, GREEN_Y, BLUE_Y, CYAN_Y, MAGENTA_Y, YELLOW_Y, WHITE_Y
};
unsigned char colorpatternCb[] = {
  	BLACK_CB, RED_CB, GREEN_CB, BLUE_CB, CYAN_CB, MAGENTA_CB, YELLOW_CB, WHITE_CB
};
unsigned char colorpatternCr[] = {
  	BLACK_CR, RED_CR, GREEN_CR, BLUE_CR, CYAN_CR, MAGENTA_CR, YELLOW_CR, WHITE_CR
};

static int rgb24_fill(struct color_info *cinfo)
{
	int i,j,size;
	unsigned int color;
	unsigned int *p = (unsigned int *)cinfo->addr;
	
	//color bar
	if(cinfo->color==COLORBAR) {
		size=cinfo->xres*cinfo->yres/8;
		for(j=0;j<8;j++) {
			color=BPP24_RGB[j];
			for (i = 0; i < size; i++)
				*p++ = color;
		}		
		return 0;	
	}
	
	//single color
	size=cinfo->xres*cinfo->yres;
	color=BPP24_RGB[cinfo->color];
  for (i = 0; i < size; i++)
		*p++ = color;

  return 0;
}

static int rgb16_fill(struct color_info *cinfo)
{
	int i,j,size;
	unsigned short color;
	unsigned short *p = (unsigned short *)cinfo->addr;
	unsigned short *color_format;
	
	switch(cinfo->mode) {
		case RGB655:
			color_format=BPP16_RGB_R;
			break;
		case RGB565:
			color_format=BPP16_RGB_G;
			break;
		case RGB556:
			color_format=BPP16_RGB_B;
			break;
		case RGB555:
			color_format=BPP16_LUM_0;
			break;
		case RGB555_R:
			color_format=BPP16_LUM_R;
			break;
		case RGB555_G:
			color_format=BPP16_LUM_G;
			break;
		case RGB555_B:
			color_format=BPP16_LUM_B;
			break;
		default:
			color_format=BPP16_RGB_B;
			break;
	}
	
	//color bar
	if(cinfo->color==COLORBAR) {
		size=cinfo->xres*cinfo->yres/8;
		for(j=0;j<8;j++) {
			color=color_format[j];
			for (i = 0; i < size; i++)
				*p++ = color;
		}		
		return 0;	
	}
	
	//single color
	size=cinfo->xres*cinfo->yres;
	color=color_format[cinfo->color];
  for (i = 0; i < size; i++)
		*p++ = color;

  return 0;
}

static int yuv_fill(struct color_info *cinfo)
{
	int i,j,size;
	unsigned char color;
	unsigned char *p = (unsigned char *)cinfo->addr;

	//color bar
	if(cinfo->color==COLORBAR) {
		//fill y
		size=cinfo->xres*cinfo->yres/8;
		for(j=0;j<8;j++) {
			color=colorpatternY[j];
			for(i=0;i<size;i++)
				*p++ = color;
		}
		//fill u v
		if(cinfo->mode==YUV422)
			size/=2;
		else
			size/=4;
		for(j=0;j<8;j++) {
			color=colorpatternCb[j];
			for(i=0;i<size;i++)
				*p++ = color;
		}
		for(j=0;j<8;j++) {
			color=colorpatternCr[j];
			for(i=0;i<size;i++)
				*p++ = color;
		}
		return 0;
	}
	
	//single color
	//fill y
	size=cinfo->xres*cinfo->yres;
	color=colorpatternY[cinfo->color];
	for(i=0;i<size;i++)
		*p++ = color;
	
	//fill u v
	if(cinfo->mode==YUV422)
		size=cinfo->xres*cinfo->yres/2;
	else
		size=cinfo->xres*cinfo->yres/4;
	
	color=colorpatternCb[cinfo->color];
	for(i=0;i<size;i++)
		*p++ = color;	
		
	color=colorpatternCr[cinfo->color];
	for(i=0;i<size;i++)
		*p++ = color;
	
	return 0;
}

int socle_color_fill(struct color_info *cinfo)
{
	switch(cinfo->mode) {
		case RGB888:
			rgb24_fill(cinfo);
			break;
		case RGB655:
		case RGB565:
		case RGB556:
		case RGB555:
		case RGB555_R:
		case RGB555_G:
		case RGB555_B:
			rgb16_fill(cinfo);
			break;
		case YUV422:
		case YUV420:
			yuv_fill(cinfo);
			break;
	}
	return 0;
}
//fix point yuv2rgb
void socle_yuv2rgb(unsigned char y,unsigned char u, unsigned char v, unsigned int *rgb)
{
		int tmp_r,tmp_g,tmp_b;
	
		
		
		tmp_r= (298*(y-16) + 409*(v-128) + 128 )>>8;
		tmp_g= (298*(y-16) - 208*(v-128) - 100*(u-128) + 128)>>8;
		tmp_b= (298*(y-16) + 516*(u-128) + 128)>>8;
		
		if (tmp_r > 255) tmp_r = 255;
    if (tmp_g > 255) tmp_g = 255;
    if (tmp_b > 255) tmp_b = 255;

    if (tmp_r < 0) tmp_r = 0;
    if (tmp_g < 0) tmp_g = 0;
    if (tmp_b < 0) tmp_b = 0;
		
	
		*rgb=(tmp_r<<16) | (tmp_g<<8) | tmp_b;
}
