#define FB_SYNC_HOR_HIGH_ACT    1       /* horizontal sync high active  */
#define FB_SYNC_VERT_HIGH_ACT   2       /* vertical sync high active    */

struct screen_info {
	unsigned int xres;                  /* visible resolution */
	unsigned int yres;
	unsigned int pixclock;              /* pixel clock in ps (pico seconds) */
	unsigned int left_margin;           /* time from sync to picture */
	unsigned int right_margin;          /* time from picture to sync */
	unsigned int upper_margin;          /* time from sync to picture */
	unsigned int lower_margin;
	unsigned int hsync_len;             /* length of horizontal sync */
	unsigned int vsync_len;             /* length of vertical sync */
	unsigned int sync;        
};

struct mem_info {
		unsigned int page0_addr;
		unsigned int page1_addr;
		unsigned int lut_addr;
};

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

enum {
	PANEL35=0,
	PANEL48,
	PANEL70,
	VGA480,
	SVGA600,
	XVGA768,
	HD720,
	HD1080
};

int fb_init(int mode);
int fb_set_screen_info(struct screen_info *screen);
int fb_get_screen_info(struct screen_info *screen);
struct screen_info* fb_get_platform_screen(void);
struct screen_info* fb_get_screen_by_type(int type);

int fb_get_mem_info(struct mem_info *mem);
int fb_enable(void);
int fb_disable(void);
