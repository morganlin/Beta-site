#include <lib/io.h>

#include "zlib.h"




int  gunzip (void *, int, unsigned char *, unsigned long *);

static void *zalloc(void *, unsigned, unsigned);
static void zfree(void *, void *, unsigned);

extern char *trace_point;
extern int *function_point;


int zalloct_count=0;

#define	ZALLOC_ALIGNMENT	16

static void *zalloc(void *x, unsigned items, unsigned size)
{
	void *p;

        *(trace_point++)= DEF_zalloct;
        *(function_point+DEF_zalloct)=++zalloct_count;
	
	//printf("x=0x%x, x=0x%x, items=0x%x, size=0x%x\n", (char) *x, (char)x, items, size);
	ARM11BUG("items=0x%x, size=0x%x\n",  items, size);

	//ARM11BUG("p=0x%08x, *p=0x%08x\n", p, *p);

	size *= items;
	ARM11BUG("size1 = 0x%x\n", size);
	size = (size + ZALLOC_ALIGNMENT - 1) & ~(ZALLOC_ALIGNMENT - 1);

	ARM11BUG("size2 = 0x%x\n", size);
	p = malloc (size);

	ARM11BUG("p=0x%x\n", p);

	return (p);
}


int zfree_count=0;

static void zfree(void *x, void *addr, unsigned nb)
{

        *(trace_point++)= DEF_zfree;
        *(function_point+DEF_zfree)=++zfree_count;

	ARM11BUG("addr = 0x%x\n", addr);

	free (addr);
}



#define HEAD_CRC	2
#define EXTRA_FIELD	4
#define ORIG_NAME	8
#define COMMENT		0x10
#define RESERVED	0xe0

#define DEFLATED	8


int gunzip_count=0;

int gunzip(void *dst, int dstlen, unsigned char *src, unsigned long *lenp)
{
	z_stream s;
	int r, i, flags;

        *(trace_point++)= DEF_gunzip;
        *(function_point+DEF_gunzip)=++gunzip_count;

	//ARM11BUG("*dst=0x%x, dstlen=0x%x, *src=0x%x, *lenp=0x%x\n", *dst, dstlen, *src, *lenp);
		

	/* skip header */
	i = 10;
	flags = src[3];
	ARM11BUG("flags=0x%x, src[2]=\n", flags, src[2]);
	if (src[2] != DEFLATED || (flags & RESERVED) != 0) {
		iowrite32(0x1, 0x1fc001d0);
		//printf("Error: Bad gzipped data\n");
		return (-1);
	}
	if ((flags & EXTRA_FIELD) != 0){
		ARM11BUG("i=0x%x, src[10]=0x%x, src[11]=0x%x\n", i, src[10], src[11]);
		i = 12 + src[10] + (src[11] << 8);
	}
	if ((flags & ORIG_NAME) != 0){
		ARM11BUG("(flags & ORIG_NAME) != 0\n");
		while (src[i++] != 0)
			;
	}
	if ((flags & COMMENT) != 0){
		ARM11BUG("(flags & COMMENT) != 0\n");
		while (src[i++] != 0)
			;
	}
	if ((flags & HEAD_CRC) != 0){
		ARM11BUG("i=0x%x\n", i);
		i += 2;
	}
	if (i >= *lenp) {
		ARM11BUG("i=0x%x, *lenp=0x%x\n", i, *lenp);
		iowrite32(0x1, 0x1fc001d0);
		//printf ("Error: gunzip out of data in header\n");
		return (-1);
	}

	s.zalloc = zalloc;
	s.zfree = zfree;
	s.outcb = Z_NULL;
	ARM11BUG("s.zalloc=0x%x, s.zfree=0x%x, s.outcb=0x%x\n", s.zalloc, s.zfree, s.outcb);
	ARM11BUG("&s=0x%x, -MAX_WBITS=0x%x\n", &s, -MAX_WBITS);

	r = inflateInit2(&s, -MAX_WBITS);
	ARM11BUG("r=0x%x\n", r);	

	if (r != Z_OK) {
		iowrite32(0x1, 0x1fc001d0);
		//printf ("Error: inflateInit2() returned %d\n", r);
		return (-1);
	}
	s.next_in = src + i;
	s.avail_in = *lenp - i;
	s.next_out = dst;
	s.avail_out = dstlen;
	ARM11BUG("s.next_in=0x%x, s.avail_in=0x%x, s.next_out=0x%x, s.avail_out=0x%x\n", s.next_in, s.avail_in, s.next_out, s.avail_out);
	ARM11BUG("&s=0x%x, Z_FINISH=0x%x\n", &s, Z_FINISH);
	r = inflate(&s, Z_FINISH);
	if (r != Z_OK && r != Z_STREAM_END) {
		iowrite32(0x1, 0x1fc001d0);
		//printf ("Error: inflate() returned %d\n", r);
		return (-1);
	}
	*lenp = s.next_out - (unsigned char *) dst;
	ARM11BUG("*lenp=0x%x\n", *lenp);
	ARM11BUG("&s=0x%x\n", &s);
	inflateEnd(&s);

	return (0);
}
