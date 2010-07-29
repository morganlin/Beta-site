#include <global.h>
#include <devapi.h>


_usb_device_handle   dev_handle;
//extern int in_init;

//extern u32 iso_length=0, iso_max_packet_length=0, iso_out_mult=0, iso_in_mult=0;
u32 iso_length=0, iso_max_packet_length=0, iso_out_mult=0, iso_in_mult=0; //jerryh modify 2010/03/01

uint_8 _usb_device_init (uint_8 devnum,_usb_device_handle _PTR_  handle, uint_8 endpoints);

unsigned long initialize_device (void);
	
extern int
otg_device_start(int autotest)
{
        int ret = 0, iso_test;
#if 0
	pintf("iso test (y/n) : ");
	iso_test = getchar();
	if((iso_test == 'y') || (iso_test == 'Y')){
		printf("iso_length : ");
		scanf("%d\n", &iso_length);
		printf("iso_max_packet_length : ");
		scanf("%d\n", &iso_max_packet_length);
		printf("iso_out_mult : ");
		scanf("%d\n", &iso_out_mult);
		printf("iso_in_mult : ");
		scanf("%d\n", &iso_in_mult);
		printf("iso length = %d\n", iso_length);
		printf("iso_max_packet_length = %d\n", iso_max_packet_length);
		printf("iso_out_mult = %d\n", iso_out_mult);
		printf("iso_in_mult = %d\n", iso_in_mult);
	}
#endif	
	ret = initialize_device();

        //printf("otg_device_start\n");
        ret = _usb_device_init(0, &dev_handle, 4);

	printf("wait for transfer\n");

	while(1);

        return ret;
}


