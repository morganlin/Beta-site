

#ifndef PSP_CACHE_LINE_SIZE
#define PSP_CACHE_LINE_SIZE    (32)
#endif
/**************************************************************************
Include the USB stack header files.
**************************************************************************/

#include "devapi.h"

/**************************************************************************
Local header files for this application
**************************************************************************/

#include "demo.h"
#include "demo_d.h"


#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <type.h>		//leonid


#define USB_uint_16_low(x)                   ((x) & 0xFF)
#define USB_uint_16_high(x)                  (((x) >> 8) & 0xFF)

extern int in_init;
int in_flag;
int out_flag;

//leonid+ for iso test
extern u32 iso_max_packet_length;
extern u32 iso_out_mult, iso_in_mult;

/**************************************************************************
global variables and some defines for device.
**************************************************************************/

#define  APP_CONTROL_MAX_PKT_SIZE         (64)
#define  DEV_DESC_MAX_PACKET_SIZE         (7)
#define  EP1_FS_MAX_PACKET_SIZE           (64)
#define  EP1_HS_MAX_PACKET_SIZE           (512)
#define  CFG_DESC_EP1TX_MAX_PACKET_SIZE   (22)
#define  CFG_DESC_EP1RX_MAX_PACKET_SIZE   (29)
#undef   TOTAL_LOGICAL_ADDRESS_BLOCKS
#define  TOTAL_LOGICAL_ADDRESS_BLOCKS     (720)
#define  LENGTH_OF_EACH_LAB               (512)


/**********************************
Buffers for use by this application
***********************************/
uchar_ptr Send_Buffer_aligned;

#define BUF_CNT 128
#define BUF_SIZE 512
//leonid+ for iso
#define ISO_BUF_CNT 128
#define ISO_MAX_BUF_SIZE 1024*3

extern u32 iso_length;

//uchar mass_ep1_buf;
char mass_ep1_buf[BUF_CNT][BUF_SIZE];
char mass_ep3_buf[ISO_BUF_CNT][ISO_MAX_BUF_SIZE];		//leonid+ for iso

/* A temproray buffer for control enpoint to send data */
uchar_ptr epTemp_buf;

volatile boolean OKAY_TO_DO_HNP = FALSE;
volatile uchar    device_speed = 0;

/*The following global variables are used for USB electrical testing 
in which device should switch to test mode under a control command*/

volatile boolean  ENTER_TEST_MODE = FALSE; 
volatile uint_16  test_mode_index = 0;


/**************************************************************************
DESCRIPTORS DESCRIPTORS DESCRIPTORS DESCRIPTORS DESCRIPTORS DESCRIPTORS
**************************************************************************/

#define DEVICE_DESCRIPTOR_SIZE 18
uchar_ptr DevDesc;
uint_8  DevDescData[DEVICE_DESCRIPTOR_SIZE] =
{
   /* Length of DevDesc */
   DEVICE_DESCRIPTOR_SIZE,
   /* "Device" Type of descriptor */
   1,
   /* BCD USB version */
   0, 2,
   /* Device Class is indicated in the interface descriptors */
   0x00,
   /* Device Subclass is indicated in the interface descriptors */
   0x00,
   /* Mass storage devices do not use class-specific protocols */
   0x00,
   /* Max packet size */
   APP_CONTROL_MAX_PKT_SIZE,
   /* Vendor ID */
   //USB_uint_16_low(0x0B20), USB_uint_16_high(0x0B20),
   0xcb, 0x05,											//leonid+ for socle descriptor
   /* Product ID */
   //USB_uint_16_low(0x4), USB_uint_16_high(0x4),
   0x83, 0x14,											//leonid+ for socle descriptor
   /* BCD Device version */
   //USB_uint_16_low(0x0002), USB_uint_16_high(0x0002),
   0x00, 0x00,											//leonid+ for socle descriptor
   /* Manufacturer string index */
   0x1,
   /* Product string index */
   0x2,
   /* Serial number string index */
   //0x6,
   0x0,												//leonid+ for socle descriptor
   /* Number of configurations available */
   0x1
};

#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE 10
uchar_ptr DevQualifierDesc;
/* USB 2.0 specific descriptor */
uint_8  DevQualifierDescData[DEVICE_QUALIFIER_DESCRIPTOR_SIZE] =
{
   DEVICE_QUALIFIER_DESCRIPTOR_SIZE,  /* bLength Length of this descriptor */
   6,                         /* bDescType This is a DEVICE Qualifier descr */
   0,2,                       /* bcdUSB USB revision 2.0 */
   0,                         /* bDeviceClass */
   0,                         /* bDeviceSubClass */
   0,                         /* bDeviceProtocol */
   APP_CONTROL_MAX_PKT_SIZE,  /* bMaxPacketSize0 */
   0x01,                      /* bNumConfigurations */
   0
};

#define CONFIG_DESC_NUM_INTERFACES  (4)
/* This must be counted manually and updated with the descriptor */
/* 1*Config(9) + 1*Interface(9) + 2*Endpoint(7) + 3 bytes OTG = 35 bytes */
//#define CONFIG_DESC_SIZE            (35)
#define CONFIG_DESC_SIZE            (39)		//leonid
	

uchar_ptr ConfigDesc;

uint_8 ConfigDescData[CONFIG_DESC_SIZE] =
{
   /* Configuration Descriptor - always 9 bytes */
   9,
   /* "Configuration" type of descriptor */
   2,
   /* Total length of the Configuration descriptor */
   //USB_uint_16_low(CONFIG_DESC_SIZE), USB_uint_16_high(CONFIG_DESC_SIZE),
   39, 0,												//leonid+ for socle descriptor
   /* NumInterfaces */
   1,
   /* Configuration Value */
   1,
   /* Configuration Description String Index*/
   //4,
   0,													//leonid+ for socle descriptor
   /* Attributes.  Self-powered */
   0xc0,
   /* Current draw from bus */
   //1,
   50,													//leonid+ for socle descriptor
   /* Interface 0 Descriptor - always 9 bytes */
   9,
   /* "Interface" type of descriptor */
   4,
   /* Number of this interface */
   MASS_STORAGE_INTERFACE,
   /* Alternate Setting */
   0,
   /* Number of endpoints on this interface */
   //2,
   3,													//leonid+ for socle descriptor
   /* Interface Class */
   //0x08,
   0xff,												//leonid+ for socle descriptor
   /* Interface Subclass: SCSI transparent command set */
   //0x06,
   0,													//leonid+ for socle descriptor
   /* Interface Protocol: Bulk only protocol */
   //0x50,
   0,													//leonid+ for socle descriptor
   /* Interface Description String Index */
   0,
   /* Endpoint 1 (Bulk In Endpoint), Interface 0 Descriptor - always 7 bytes*/
   7,
   /* "Endpoint" type of descriptor */
   5,
   /*
   ** Endpoint address.  The low nibble contains the endpoint number and the
   ** high bit indicates TX(1) or RX(0).
   */
   //0x81,
   0x1, 												//leonid+ for socle descriptor
   /* Attributes.  0=Control 1=Isochronous 2=Bulk 3=Interrupt */
   0x02,
   /* Max Packet Size for this endpoint */
   USB_uint_16_low(EP1_HS_MAX_PACKET_SIZE),
   USB_uint_16_high(EP1_HS_MAX_PACKET_SIZE),
   /* Polling Interval (ms) */
   0,
   /* Endpoint 2 (Bulk Out Endpoint), Interface 0 Descriptor - always 7 bytes*/
   7,
   /* "Endpoint" type of descriptor */
   5,
   /*
   ** Endpoint address.  The low nibble contains the endpoint number and the
   ** high bit indicates TX(1) or RX(0).
   */
   //0x01,
   0x82,
   /* Attributes.  0=Control 1=Isochronous 2=Bulk 3=Interrupt */
   0x02,
   /* Max Packet Size for this endpoint */
   USB_uint_16_low(EP1_HS_MAX_PACKET_SIZE),
   USB_uint_16_high(EP1_HS_MAX_PACKET_SIZE),
   /* Polling Interval (ms) */
   0,
   7,
   5,
   0x83,
   3,
   0x20,
   0,
   0xb
};

#define OTHER_SPEED_CONFIG_DESC_SIZE  CONFIG_DESC_SIZE
uchar_ptr  other_speed_config;
uint_8  other_speed_config_data[OTHER_SPEED_CONFIG_DESC_SIZE] =
{
   9,                         /* bLength Length of this descriptor */
   7,                         /* bDescType This is a Other speed config descr */
   OTHER_SPEED_CONFIG_DESC_SIZE & 0xff, 
   OTHER_SPEED_CONFIG_DESC_SIZE >> 8,
   1,
   1,
   4,
   /* Attributes.  Self-powered */
   0xc0,
   /* Current draw from bus */
   1,
   /* Interface 0 Descriptor - always 9 bytes */
   9,
   /* "Interface" type of descriptor */
   4,
   /* Number of this interface */
   MASS_STORAGE_INTERFACE,
   /* Alternate Setting */
   0,
   /* Number of endpoints on this interface */
   2,
   /* Interface Class */
   0x08,
   /* Interface Subclass: SCSI transparent command set */
   0x06,
   /* Interface Protocol: Bulk only protocol */
   0x50,
   /* Interface Description String Index */
   0,
   /* Endpoint 1 (Bulk In Endpoint), Interface 0 Descriptor - always 7 bytes*/
   7,
   /* "Endpoint" type of descriptor */
   5,
   /*
   ** Endpoint address.  The low nibble contains the endpoint number and the
   ** high bit indicates TX(1) or RX(0).
   */
   0x81,
   /* Attributes.  0=Control 1=Isochronous 2=Bulk 3=Interrupt */
   0x02,
   /* Max Packet Size for this endpoint */
   USB_uint_16_low(EP1_HS_MAX_PACKET_SIZE),
   USB_uint_16_high(EP1_HS_MAX_PACKET_SIZE),
   /* Polling Interval (ms) */
   0,
   /* Endpoint 2 (Bulk Out Endpoint), Interface 0 Descriptor - always 7 bytes*/
   7,
   /* "Endpoint" type of descriptor */
   5,
   /*
   ** Endpoint address.  The low nibble contains the endpoint number and the
   ** high bit indicates TX(1) or RX(0).
   */
   0x01,
   /* Attributes.  0=Control 1=Isochronous 2=Bulk 3=Interrupt */
   0x02,
   /* Max Packet Size for this endpoint */
   USB_uint_16_low(EP1_FS_MAX_PACKET_SIZE),
   USB_uint_16_high(EP1_FS_MAX_PACKET_SIZE),
   /* Polling Interval (ms) */
   0,
   /* OTG descriptor */
   3,
   9,    /* OTG type */
   3     /* HNP and SRP support */
};

uchar USB_IF_ALT[4] = {0, 0, 0, 0};

/* number of strings in the table not including 0 or n. */
//const uint_8 USB_STR_NUM = 7;
//const uint_8 USB_STR_NUM = 2;
#define USB_STR_NUM 2

/*
** if the number of strings changes, look for USB_STR_0 everywhere and make
** the obvious changes.  It should be found in 3 places.
*/

const uint_16 USB_STR_0[ 2] = {0x300 + sizeof(USB_STR_0),0x0409};
const uint_16 USB_STR_1[10] = {0x300 + sizeof(USB_STR_1),
      'S','o','c','l','e','T','E','C','H'};
const uint_16 USB_STR_2[11] = {0x300 + sizeof(USB_STR_2),
      'B','0','1','U','S','B',' ','U','D','C'};
const uint_16 USB_STR_n[17] = {0x300 + sizeof(USB_STR_n),
      'B','A','D',' ','S','T','R','I','N','G',' ','I','n','d','e','x'};


const uint_8_ptr USB_STRING_DESC[USB_STR_NUM+2] =
{
   (uint_8_ptr)((pointer)USB_STR_0),
   (uint_8_ptr)((pointer)USB_STR_1),
   (uint_8_ptr)((pointer)USB_STR_2),
   (uint_8_ptr)((pointer)USB_STR_n)
};


/**************************************************************************
MASS STORAGE SPECIFIC GLOBALS
**************************************************************************/

volatile boolean        CBW_PROCESSED = FALSE;
volatile boolean        ZERO_TERMINATE = FALSE;


const MASS_STORAGE_READ_CAPACITY_STRUCT read_capacity = {
   /* Data for the capacity */
   {
      0x00, 0x00, USB_uint_16_high(TOTAL_LOGICAL_ADDRESS_BLOCKS-14),
      USB_uint_16_low(TOTAL_LOGICAL_ADDRESS_BLOCKS-14)
   },
   {
      0x00, 0x00, USB_uint_16_high(LENGTH_OF_EACH_LAB),
      USB_uint_16_low(LENGTH_OF_EACH_LAB)
   }
};



/**************************************************************************
APPLICATION SPECIFIC GLOBALS
**************************************************************************/

volatile uchar          usb_status[2];
volatile uint_8         endpoint, if_status;

SETUP_STRUCT   local_setup_packet;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9GetStatus
* Returned Value : None
* Comments       :
*     Chapter 9 GetStatus command
*     WVALUE=Zero
*     WINDEX=Zero
*     WLENGTH=1
*     DATA=bmERR_STAT
*     The GET_STATUS command is used to read the bmERR_STAT register.
*
*     Return the status based on the bRrequestType bits:
*     device (0) = bit 0 = 1 = self powered
*                  bit 1 = 0 = DEVICE_REMOTE_WAKEUP which can be modified
*     with a SET_FEATURE/CLEAR_FEATURE command.
*     interface(1) = 0000.
*     endpoint(2) = bit 0 = stall.
*     static uint_8_ptr pData;
*
*     See section 9.4.5 (page 190) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9GetStatus
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   uint_16 temp_status;
   
   if (setup) {
      switch (setup_ptr->REQUESTTYPE) {

         case 0x80:
            /* Device request */
            _usb_device_get_status(handle, 
                                    USB_STATUS_DEVICE, 
                                    (uint_16_ptr)&usb_status);
            /* Send the requested data */
            _usb_device_send_data(
                  handle, 
                  0, 
                  (uchar_ptr)&usb_status, 
                  2 /*two bytes */
                  );
            break;

         case 0x81:
            /* Interface request */
            if_status = USB_IF_ALT[setup_ptr->INDEX & 0x00FF];
            /* Send the requested data */
            _usb_device_send_data(handle, 0, (pointer)&if_status, sizeof(if_status));
            break;

         case 0x82:
            /* Endpoint request */
            endpoint = setup_ptr->INDEX & USB_STATUS_ENDPOINT_NUMBER_MASK;
            _usb_device_get_status(handle,
               USB_STATUS_ENDPOINT | endpoint, &temp_status);
               
            /*copy status to usb_status global buffer for uncached access */
            usb_status[0] = temp_status & 0xFF;
            usb_status[1] = (temp_status >> 8)& 0xFF;
            
            /* Send the requested data */
            _usb_device_send_data(handle,
                                 0,
                                 (uchar_ptr) &usb_status, 
                                 2 /* sizeof(usb_status) */
                                 );
            break;

         default:
            /* Unknown request */
            _usb_device_stall_endpoint(handle, 0, 0);
            return;

      } /* Endswitch */

      /* status phase */
      _usb_device_recv_data(handle, 0, 0, 0);
   } /* Endif */
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9ClearFeature
* Returned Value : None
* Comments       :
*     Chapter 9 ClearFeature command
*
*END*--------------------------------------------------------------------*/
void ch9ClearFeature
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   static uint_8           endpoint;
   uint_16                 usb_status;

   _usb_device_get_status(handle, USB_STATUS_DEVICE_STATE, &usb_status);

   if ((usb_status != USB_STATE_CONFIG) && (usb_status != USB_STATE_ADDRESS)) {
      _usb_device_stall_endpoint(handle, 0, 0);
      return;
   } /* Endif */

   if (setup) {
      switch (setup_ptr->REQUESTTYPE) {

         case 0:
            /* DEVICE */
            if (setup_ptr->VALUE == 1) {
               /* clear remote wakeup */
               _usb_device_get_status(handle, USB_STATUS_DEVICE, &usb_status);
               usb_status &= ~USB_REMOTE_WAKEUP;
               _usb_device_set_status(handle, USB_STATUS_DEVICE, usb_status);
            } else {
               _usb_device_stall_endpoint(handle, 0, 0);
               return;
            } /* Endif */
            break;

         case 2:
            /* ENDPOINT */
            if (setup_ptr->VALUE != 0) {
               _usb_device_stall_endpoint(handle, 0, 0);
               return;
            } /* Endif */
            endpoint = setup_ptr->INDEX & USB_STATUS_ENDPOINT_NUMBER_MASK;
            _usb_device_get_status(handle, USB_STATUS_ENDPOINT | endpoint,
               &usb_status);
            /* unstall */
            _usb_device_set_status(handle, USB_STATUS_ENDPOINT | endpoint,
               0);
            break;
         default:
            _usb_device_stall_endpoint(handle, 0, 0);
            return;

      } /* Endswitch */
      /* status phase */
      _usb_device_send_data(handle, 0, 0, 0);
   } /* Endif */
   return;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9SetAddress
* Returned Value : None
* Comments       :
*     Chapter 9 SetAddress command
*     We setup a TX packet of 0 length ready for the IN token
*     Once we get the TOK_DNE interrupt for the IN token, then
*     we change the ADDR register and go to the ADDRESS state.
*     See section 9.4.6 (page 192) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/

#define SET_ADDRESS_HARDWARE_ASSISTANCE		//leonid

void ch9SetAddress
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   static uint_8 new_address;
   uint_32  max_pkt_size;  


   if (setup) {
      new_address = setup_ptr->VALUE;
      /*******************************************************
      if hardware assitance is enabled for set_address (see
      hardware rev for details) we need to do the set_address
      before queuing the status phase.
      *******************************************************/
      #ifdef SET_ADDRESS_HARDWARE_ASSISTANCE      
         _usb_device_set_status(handle, USB_STATUS_ADDRESS, new_address);
      #endif
      
      /* status phase */
      _usb_device_send_data(handle, 0, 0, 0);
      

     /***********************************************************
      Set address is a good place to initialize other endpoints.
      At this point speed of the core is known, reset has
      occured and host is interested in talking.     
     ***********************************************************/
           
         #if 1   
         if(device_speed == USB_SPEED_HIGH) 
         {
            max_pkt_size = EP1_HS_MAX_PACKET_SIZE;
         } else 
         {
            max_pkt_size = EP1_FS_MAX_PACKET_SIZE;
         }

         _usb_device_init_endpoint(handle, 1, max_pkt_size, 0,
            USB_BULK_ENDPOINT, USB_DEVICE_DONT_ZERO_TERMINATE);
         _usb_device_init_endpoint(handle, 2, max_pkt_size, 1,
            USB_BULK_ENDPOINT, USB_DEVICE_DONT_ZERO_TERMINATE);
		in_flag = 0;
		out_flag = 0;


	//leonid+ for iso	 
         _usb_device_init_endpoint(handle, 3, iso_max_packet_length, 0,
            USB_ISOCHRONOUS_ENDPOINT, (iso_out_mult << 1));
         _usb_device_init_endpoint(handle, 4, iso_max_packet_length, 1,
            USB_ISOCHRONOUS_ENDPOINT, (iso_in_mult << 1));
		
		 
         _usb_device_cancel_transfer(handle, 1, USB_RECV);
         
         if (_usb_device_get_transfer_status(handle, 1, USB_RECV) == USB_OK) {
            //_usb_device_recv_data(handle, 1, mass_ep1_buf, 31);
            _usb_device_recv_data(handle, 1, (uchar_ptr)mass_ep1_buf[out_flag], BUF_SIZE);		
         } /* Endif */

	printf("iso_length = 0x%x\n", iso_length);

	//leonid+ for iso	 
         if (_usb_device_get_transfer_status(handle, 3, USB_RECV) == USB_OK) {
            _usb_device_recv_data(handle, 3, (uchar_ptr)mass_ep3_buf[out_flag], iso_length);
		
	#if 0	//leonid+ for iso
		{
			int tmp;
			char tmp_val=0x11;
			char *tmp_buf = mass_ep3_buf;
			for(tmp=0;tmp<1024;){
				*tmp_buf = tmp_val;
				tmp_buf++;
				tmp++;
				if((tmp&0x7F) == 0)
					tmp_val += 0x11;
			}
		}		
        	       //initialize_ep4();
	#endif

         } /* Endif */


			 
      #endif

   } else {
      #ifndef SET_ADDRESS_HARDWARE_ASSISTANCE
         _usb_device_set_status(handle, USB_STATUS_ADDRESS, new_address);
      #endif
   
      _usb_device_set_status(handle, USB_STATUS_DEVICE_STATE,
         USB_STATE_ADDRESS);
   } /* Endif */
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9GetDescription
* Returned Value : None
* Comments       :
*     Chapter 9 GetDescription command
*     The Device Request can ask for Device/Config/string/interface/endpoint
*     descriptors (via WVALUE). We then post an IN response to return the
*     requested descriptor.
*     And then wait for the OUT which terminates the control transfer.
*     See section 9.4.3 (page 189) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9GetDescription
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   uint_32  max_pkt_size;

   if (setup) {
      /* Load the appropriate string depending on the descriptor requested.*/
      switch (setup_ptr->VALUE & 0xFF00) {

         case 0x0100:
		printf("ch9GetDescription:0x0100\n");
            _usb_device_send_data(handle, 0, DevDesc,
               MIN(setup_ptr->LENGTH, DEVICE_DESCRIPTOR_SIZE));
            break;

         case 0x0200:
		printf("ch9GetDescription:0x0200\n");
            /* Set the Max Packet Size in the config and other speed config */
            if(device_speed == USB_SPEED_HIGH) {
               max_pkt_size = EP1_HS_MAX_PACKET_SIZE;
            } else {
               max_pkt_size = EP1_FS_MAX_PACKET_SIZE;
            } /* Endif */

		printf("max_pkt_size = %d \n ",max_pkt_size);
		//ryan
#if 1 		
            ConfigDesc[CFG_DESC_EP1TX_MAX_PACKET_SIZE] =
               USB_uint_16_low(max_pkt_size);
            ConfigDesc[CFG_DESC_EP1TX_MAX_PACKET_SIZE+1] =
               USB_uint_16_high(max_pkt_size);
            ConfigDesc[CFG_DESC_EP1RX_MAX_PACKET_SIZE] =
               USB_uint_16_low(max_pkt_size);
            ConfigDesc[CFG_DESC_EP1RX_MAX_PACKET_SIZE+1] =
               USB_uint_16_high(max_pkt_size);
#endif
            _usb_device_send_data(handle, 0, ConfigDesc,
               MIN(setup_ptr->LENGTH, CONFIG_DESC_SIZE));
            break;
#if 1	//leonid
         case 0x0300:
            if ((setup_ptr->VALUE & 0x00FF) > USB_STR_NUM) {
               _usb_device_send_data(handle, 0, USB_STRING_DESC[USB_STR_NUM+1],
                  MIN(setup_ptr->LENGTH, USB_STRING_DESC[USB_STR_NUM+1][0]));
            } else {
               _usb_device_send_data(handle, 0,
                  USB_STRING_DESC[setup_ptr->VALUE & 0x00FF],
                  MIN(setup_ptr->LENGTH,
                     USB_STRING_DESC[setup_ptr->VALUE & 0x00FF][0]));
            } /* Endif */

            break;
#endif
         case 0x600:
		printf("ch9GetDescription:0x600\n");
            _usb_device_send_data(handle, 0, DevQualifierDesc,
               MIN(setup_ptr->LENGTH, DEVICE_QUALIFIER_DESCRIPTOR_SIZE));
            break;

         case 0x700:
		printf("ch9GetDescription:0x700\n");
            if(device_speed == USB_SPEED_HIGH) {
               max_pkt_size = EP1_FS_MAX_PACKET_SIZE;
            } else {
               max_pkt_size = EP1_HS_MAX_PACKET_SIZE;
            } /* Endif */

            other_speed_config[CFG_DESC_EP1TX_MAX_PACKET_SIZE] =
               USB_uint_16_low(max_pkt_size);
            other_speed_config[CFG_DESC_EP1TX_MAX_PACKET_SIZE+1] =
               USB_uint_16_high(max_pkt_size);
            other_speed_config[CFG_DESC_EP1RX_MAX_PACKET_SIZE] =
               USB_uint_16_low(max_pkt_size);
            other_speed_config[CFG_DESC_EP1RX_MAX_PACKET_SIZE+1] =
               USB_uint_16_high(max_pkt_size);

            _usb_device_send_data(handle, 0, other_speed_config, 
               MIN(setup_ptr->LENGTH, OTHER_SPEED_CONFIG_DESC_SIZE));
            break;

         default:
		printf("ch9GetDescription:setup_ptr->VALUE : \n", setup_ptr->VALUE);
            _usb_device_stall_endpoint(handle, 0, 0);
            return;
      } /* Endswitch */
      /* status phase */
      _usb_device_recv_data(handle, 0, 0, 0);
   } /* Endif */
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9SetDescription
* Returned Value : None
* Comments       :
*     Chapter 9 SetDescription command
*     See section 9.4.8 (page 193) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9SetDescription
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   _usb_device_stall_endpoint(handle, 0, 0);
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9GetConfig
* Returned Value : None
* Comments       :
*     Chapter 9 GetConfig command
*     See section 9.4.2 (page 189) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9GetConfig
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   uint_16 current_config;
   /* Return the currently selected configuration */
   if (setup){
      _usb_device_get_status(handle, USB_STATUS_CURRENT_CONFIG,
         &current_config);
      *epTemp_buf = (uchar)current_config;
      _usb_device_send_data(handle, 0, epTemp_buf, sizeof(uchar));
      /* status phase */
      _usb_device_recv_data(handle, 0, 0, 0);
   } /* Endif */
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9SetConfig
* Returned Value : None
* Comments       :
*     Chapter 9 SetConfig command
*
*END*--------------------------------------------------------------------*/
void ch9SetConfig
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   uint_16 usb_state;

   if (setup) {
      if ((setup_ptr->VALUE & 0x00FF) > 1) {
         /* generate stall */
         _usb_device_stall_endpoint(handle, 0, 0);
         return;
      } /* Endif */
      /* 0 indicates return to unconfigured state */
      if ((setup_ptr->VALUE & 0x00FF) == 0) {
          /*
         _usb_device_get_status(handle, USB_STATUS_DEVICE_STATE, &usb_state);
         if ((usb_state == USB_STATE_CONFIG) ||
            (usb_state == USB_STATE_ADDRESS))
          */
         {
            /* clear the currently selected config value */
            _usb_device_set_status(handle, USB_STATUS_CURRENT_CONFIG, 0);
            _usb_device_set_status(handle, USB_STATUS_DEVICE_STATE,
               USB_STATE_ADDRESS);
            /* status phase */
            _usb_device_send_data(handle, 0, 0, 0);
         } 
         /*
         else {
            _usb_device_stall_endpoint(handle, 0, 0);
         }
         */
         return;
      } /* Endif */


      /*
      ** If the configuration value (setup_ptr->VALUE & 0x00FF) differs
      ** from the current configuration value, then endpoints must be
      ** reconfigured to match the new device configuration
      */
      _usb_device_get_status(handle, USB_STATUS_CURRENT_CONFIG,
         &usb_state);
      if (usb_state != (setup_ptr->VALUE & 0x00FF)) {
         /* Reconfigure endpoints here */
         switch (setup_ptr->VALUE & 0x00FF) {

            default:
               break;
         } /* Endswitch */
         _usb_device_set_status(handle, USB_STATUS_CURRENT_CONFIG,
            setup_ptr->VALUE & 0x00FF);
         _usb_device_set_status(handle, USB_STATUS_DEVICE_STATE,
            USB_STATE_CONFIG);
         /* status phase */
         _usb_device_send_data(handle, 0, 0, 0);
         return;
      } /* Endif */
      _usb_device_set_status(handle, USB_STATUS_DEVICE_STATE,
         USB_STATE_CONFIG);
      /* status phase */
      _usb_device_send_data(handle, 0, 0, 0);
   } /* Endif */
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9GetInterface
* Returned Value : None
* Comments       :
*     Chapter 9 GetInterface command
*     See section 9.4.4 (page 190) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9GetInterface
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   uint_16 usb_state;

   _usb_device_get_status(handle, USB_STATUS_DEVICE_STATE, &usb_state);
   if (usb_state != USB_STATE_CONFIG) {
      _usb_device_stall_endpoint(handle, 0, 0);
      return;
   } /* Endif */

   if (setup) {
      _usb_device_send_data(handle, 0, &USB_IF_ALT[setup_ptr->INDEX & 0x00FF],
         MIN(setup_ptr->LENGTH, sizeof(uint_8)));
      /* status phase */
      _usb_device_recv_data(handle, 0, 0, 0);
   } /* Endif */
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9SetInterface
* Returned Value : None
* Comments       :
*     Chapter 9 SetInterface command
*     See section 9.4.10 (page 195) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9SetInterface
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   if (setup) {
      if (setup_ptr->REQUESTTYPE != 0x01) {
         _usb_device_stall_endpoint(handle, 0, 0);
         return;
      } /* Endif */

      /*
      ** If the alternate value (setup_ptr->VALUE & 0x00FF) differs
      ** from the current alternate value for the specified interface,
      ** then endpoints must be reconfigured to match the new alternate
      */
      if (USB_IF_ALT[setup_ptr->INDEX & 0x00FF]
         != (setup_ptr->VALUE & 0x00FF))
      {
         USB_IF_ALT[setup_ptr->INDEX & 0x00FF] = (setup_ptr->VALUE & 0x00FF);
         /* Reconfigure endpoints here. */

      } /* Endif */

      /* status phase */
      _usb_device_send_data(handle, 0, 0, 0);
   } /* Endif */

   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9SynchFrame
* Returned Value :
* Comments       :
*     Chapter 9 SynchFrame command
*     See section 9.4.11 (page 195) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9SynchFrame
   (
      /* USB handle */
      _usb_device_handle handle,

      /* Is it a Setup phase? */
      boolean setup,

      /* The setup packet pointer */
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */

   if (setup) {
      if (setup_ptr->REQUESTTYPE != 0x02) {
         _usb_device_stall_endpoint(handle, 0, 0);
         return;
      } /* Endif */

      if ((setup_ptr->INDEX & 0x00FF) >=
         ConfigDesc[CONFIG_DESC_NUM_INTERFACES])
      {
         _usb_device_stall_endpoint(handle, 0, 0);
         return;
      } /* Endif */

      /* Get SOF count and send it to the host */
      _usb_device_get_status(handle, USB_STATUS_SOF_COUNT, (uint_16_ptr)epTemp_buf);
      _usb_device_send_data(handle, 0, epTemp_buf,
         MIN(setup_ptr->LENGTH, sizeof(uint_16)));
      /* status phase */
      _usb_device_recv_data(handle, 0, 0, 0);
   } /* Endif */
   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9Vendor
* Returned Value :
* Comments       :
*     Chapter 9 SynchFrame command
*     See section 9.4.11 (page 195) of the USB 1.1 Specification.
*
*END*--------------------------------------------------------------------*/
void ch9Vendor
   (
      _usb_device_handle handle,
      boolean setup,
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */

   return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : ch9Class
* Returned Value : 
* Comments       :
*     Chapter 9 Class specific request
*     See section 9.4.11 (page 195) of the USB 1.1 Specification.
* 
*END*--------------------------------------------------------------------*/
void ch9Class
   (
      _usb_device_handle handle,
      boolean setup,
      uint_8  direction,
      SETUP_STRUCT_PTR setup_ptr
   )
{ /* Body */
   
   if (setup) {
      switch (setup_ptr->REQUEST) {
         case 0xFF:
            /* Bulk-Only Mass Storage Reset: Ready the device for the next 
            ** CBW from the host 
            */
            if ((setup_ptr->VALUE != 0) || 
                (setup_ptr->INDEX != MASS_STORAGE_INTERFACE) ||
                (setup_ptr->LENGTH != 0)) {
                _usb_device_stall_endpoint(handle, 0, 0);
            } else { /* Body */
               CBW_PROCESSED = FALSE;
               ZERO_TERMINATE = FALSE;
               _usb_device_cancel_transfer(handle, 1, USB_RECV);
               _usb_device_cancel_transfer(handle, 1, USB_SEND);
               
               /* unstall bulk endpoint */
               _usb_device_unstall_endpoint(handle, 1, 0);
               _usb_device_unstall_endpoint(handle, 1, 1);
               /* 
               _usb_device_set_status(handle, USB_STATUS_ENDPOINT | 1, 0); */
               
               _usb_device_recv_data(handle, 1, (uchar_ptr)mass_ep1_buf[0], 31);
               /* send zero packet to control pipe */
               _usb_device_send_data(handle, 0, 0, 0);
            } /* Endbody */
            return;
         case 0xFE:
            /* For Get Max LUN use any of these responses*/
            if (setup_ptr->LENGTH == 0) { /* Body */
               _usb_device_stall_endpoint(handle, 0, 0);
            } else if ((setup_ptr->VALUE != 0) ||
                (setup_ptr->INDEX != MASS_STORAGE_INTERFACE) ||
                (setup_ptr->LENGTH != 1)) { /* Body */
                _usb_device_stall_endpoint(handle, 0, 0);
             } else { /* Body */
               /* Send Max LUN = 0 to the the control pipe */
               *epTemp_buf = 0;
               _usb_device_send_data(handle, 0, epTemp_buf, 1);
               /* status phase */
               _usb_device_recv_data(handle, 0, 0, 0);
               
             } /* Endbody */     
            return;
         default :
            _usb_device_stall_endpoint(handle, 0, 0);
            return;
      } /* EndSwitch */
   } 

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : service_ep0
* Returned Value : None
* Comments       :
*     Called upon a completed endpoint 0 (USB 1.1 Chapter 9) transfer
*
*END*--------------------------------------------------------------------*/
void service_ep0
   (
      /* [IN] Handle of the USB device */
      _usb_device_handle   handle,

      /* [IN] Is it a setup packet? */
      boolean              setup,

      /* [IN] Direction of the transfer.  Is it transmit? */
      uint_8               direction,

      /* [IN] Pointer to the data buffer */
      uint_8_ptr           buffer,

      /* [IN] Length of the transfer */
      uint_32              length,

      /* [IN] Error, if any */
      uint_8               error
   )
{ /* Body */

   if (setup) {
      _usb_device_read_setup_data(handle, 0, (uchar_ptr)&local_setup_packet);
   } 

   switch (local_setup_packet.REQUESTTYPE & 0x60) {

      case 0x00:
         switch (local_setup_packet.REQUEST) {

            case 0x0:
               ch9GetStatus(handle, setup, &local_setup_packet);
               break;

            case 0x1:
               ch9ClearFeature(handle, setup, &local_setup_packet);
               break;
#if 0	//leonid
            case 0x3:
               ch9SetFeature(handle, setup, &local_setup_packet);
               break;
#endif
            case 0x5:
		printf("ch9SetAddress\n");
               ch9SetAddress(handle, setup, &local_setup_packet);
               break;

            case 0x6:
               ch9GetDescription(handle, setup, &local_setup_packet);
               break;

            case 0x7:
               ch9SetDescription(handle, setup, &local_setup_packet);
               break;

            case 0x8:
               ch9GetConfig(handle, setup, &local_setup_packet);
               break;

            case 0x9:
               ch9SetConfig(handle, setup, &local_setup_packet);
               break;

            case 0xa:
               ch9GetInterface(handle, setup, &local_setup_packet);
               break;

            case 0xb:
               ch9SetInterface(handle, setup, &local_setup_packet);
               break;

            case 0xc:
               ch9SynchFrame(handle, setup, &local_setup_packet);
               break;

            default:
               _usb_device_stall_endpoint(handle, 0, 0);
               break;

         } /* Endswitch */

         break;

      case 0x20:
         /* class specific request */
         ch9Class(handle, setup, direction, &local_setup_packet);
         return;

      case 0x40:
         /* vendor specific request */
         ch9Vendor(handle, setup, &local_setup_packet);
         break;

      default:
         _usb_device_stall_endpoint(handle, 0, 0);
         break;

   } /* Endswitch */

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : service_ep1
* Returned Value : None
* Comments       :
*     Called upon a completed endpoint 1 (USB 1.1 Chapter 9) transfer
*
*END*--------------------------------------------------------------------*/
void service_ep1
   (
      /* [IN] Handle of the USB device */
      _usb_device_handle   handle,

      /* [IN] Is it a setup packet? */
      boolean              setup,

      /* [IN] Direction of the transfer.  Is it transmit? */
      uint_8               direction,

      /* [IN] Pointer to the data buffer */
      uint_8_ptr           buffer,

      /* [IN] Length of the transfer */
      uint_32              length,

      /* [IN] Error, if any */
      uint_8               error
   )
{ /* Body */
   
	//printf("service_ep1\n");
	out_flag ++ ;
	if(out_flag == BUF_CNT)
		out_flag = 0;
	if((in_init == 0) && (out_flag != in_flag)){
		initialize_ep2();
		in_init = 1;
	}

	_usb_device_recv_data(handle, 1, (uchar_ptr)mass_ep1_buf[out_flag], BUF_SIZE);
	

   return;
} /* Endbody */

void service_ep2
   (
      /* [IN] Handle of the USB device */
      _usb_device_handle   handle,

      /* [IN] Is it a setup packet? */
      boolean              setup,

      /* [IN] Direction of the transfer.  Is it transmit? */
      uint_8               direction,

      /* [IN] Pointer to the data buffer */
      uint_8_ptr           buffer,

      /* [IN] Length of the transfer */
      uint_32              length,

      /* [IN] Error, if any */
      uint_8               error
   )
{ /* Body */
   
	in_flag ++ ;
	if(in_flag == BUF_CNT)
		in_flag = 0;
	if(out_flag == in_flag)
		in_init = 0;
	else	
       	 _usb_device_send_data(handle, 2, (uchar_ptr)mass_ep1_buf[in_flag], BUF_SIZE);	

   return;
} /* Endbody */


//leonid+
void service_ep3
   (
      /* [IN] Handle of the USB device */
      _usb_device_handle   handle,

      /* [IN] Is it a setup packet? */
      boolean              setup,

      /* [IN] Direction of the transfer.  Is it transmit? */
      uint_8               direction,

      /* [IN] Pointer to the data buffer */
      uint_8_ptr           buffer,

      /* [IN] Length of the transfer */
      uint_32              length,

      /* [IN] Error, if any */
      uint_8               error
   )
{ /* Body */
   
	//printf("service_ep1\n");
	
	initialize_ep4();
	
	out_flag ++ ;
	if(out_flag == ISO_BUF_CNT)
		out_flag = 0;
	
	_usb_device_recv_data(handle, 3, (uchar_ptr)mass_ep3_buf[out_flag], iso_length);
	printf("service_ep3 -> mass_ep3_buf[out_flag] = 0x%08x\n", mass_ep3_buf[out_flag]);

   return;
} /* Endbody */


void service_ep4
   (
      /* [IN] Handle of the USB device */
      _usb_device_handle   handle,

      /* [IN] Is it a setup packet? */
      boolean              setup,

      /* [IN] Direction of the transfer.  Is it transmit? */
      uint_8               direction,

      /* [IN] Pointer to the data buffer */
      uint_8_ptr           buffer,

      /* [IN] Length of the transfer */
      uint_32              length,

      /* [IN] Error, if any */
      uint_8               error
   )
{ /* Body */

	_usb_device_send_data(handle, 4, (uchar_ptr)mass_ep3_buf[out_flag], iso_length);	
	printf("service_ep4 -> mass_ep3_buf[out_flag] = 0x%08x\n", mass_ep3_buf[out_flag]);

   return;
} /* Endbody */




/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : detect_speed
* Returned Value : None
* Comments       :
*     Called when the speed is set
*
*END*--------------------------------------------------------------------*/
void detect_speed
   (
      /* [IN] Handle of the USB device */
      _usb_device_handle   handle,

      /* [IN] Unused */
      boolean              setup,

      /* [IN] Unused */
      uint_8               direction,

      /* [IN] Unused */
      uint_8_ptr           buffer,

      /* [IN] speed */
      uint_32              speed,

      /* [IN] Error, if any */
      uint_8               error
   )
{ /* Body */
	printf("device_speed = %d \n",device_speed);
   device_speed = speed;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : reset_ep0
* Returned Value : None
* Comments       :
*     Called upon a bus reset event.  Initialises the control endpoint.
*
*END*--------------------------------------------------------------------*/
void reset_ep0
   (
      /* [IN] Handle of the USB device */
      _usb_device_handle   handle,

      /* [IN] Unused */
      boolean              setup,

      /* [IN] Unused */
      uint_8               direction,

      /* [IN] Unused */
      uint_8_ptr           buffer,

      /* [IN] Unused */
      uint_32              length,

      /* [IN] Error, if any */
      uint_8               error
   )
{ /* Body */

   OKAY_TO_DO_HNP = FALSE;

   _usb_device_init_endpoint(handle, 0, DevDesc[DEV_DESC_MAX_PACKET_SIZE], 0,
      USB_CONTROL_ENDPOINT, 0);
   _usb_device_init_endpoint(handle, 0, DevDesc[DEV_DESC_MAX_PACKET_SIZE], 1,
      USB_CONTROL_ENDPOINT, 0);

   return;
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : initialize_device
* Returned Value : None
* Comments       :
*     Initialized the buffers and descriptors
*
*END*--------------------------------------------------------------------*/
unsigned long initialize_device (void)
{
   uint_32              send_data_buffer_size=0;
   uchar_ptr            temp;
   

   /**************************************************************************
   Best way to handle the Data cache is to allocate a large buffer that is
   cache aligned and keep all data inside it. Flush the line of the cache
   that you have changed. In this program, we have static data such as 
   descriptors which never changes. Such data can be kept in this buffer
   and flushed only once. Note that you can reduce the size of this buffer
   by aligning the addresses in a different way.   
   ***************************************************************************/

   send_data_buffer_size =  (DEVICE_DESCRIPTOR_SIZE +  PSP_CACHE_LINE_SIZE) +
                            (CONFIG_DESC_SIZE + PSP_CACHE_LINE_SIZE) +
                            (DEVICE_QUALIFIER_DESCRIPTOR_SIZE + PSP_CACHE_LINE_SIZE) +
                            (OTHER_SPEED_CONFIG_DESC_SIZE + PSP_CACHE_LINE_SIZE) +
                            (BUFFERSIZE + PSP_CACHE_LINE_SIZE) +
                            (EP_TEMP_BUFFERSIZE + PSP_CACHE_LINE_SIZE);



                               
   Send_Buffer_aligned   = (uchar_ptr) malloc(send_data_buffer_size);
   if (Send_Buffer_aligned == NULL) 
   {
   	printf("Send_Buffer_aligned = NULL \n",Send_Buffer_aligned);
      return USBERR_ALLOC;
   }
   
   /* keep a temporary copy of the aligned address */
   temp = Send_Buffer_aligned;
   

   /**************************************************************************
   Assign pointers to different buffers from it and copy data inside.
   ***************************************************************************/
   DevDesc =  (uchar_ptr) Send_Buffer_aligned;
   USB_memcopy(DevDescData, DevDesc, DEVICE_DESCRIPTOR_SIZE);
   Send_Buffer_aligned += ((DEVICE_DESCRIPTOR_SIZE/PSP_CACHE_LINE_SIZE) + 1)* PSP_CACHE_LINE_SIZE; 
   
   ConfigDesc =  (uchar_ptr) Send_Buffer_aligned;
   USB_memcopy(ConfigDescData, ConfigDesc, CONFIG_DESC_SIZE);
   Send_Buffer_aligned += ((CONFIG_DESC_SIZE/PSP_CACHE_LINE_SIZE) + 1)* PSP_CACHE_LINE_SIZE; 
   
   DevQualifierDesc =  (uchar_ptr) Send_Buffer_aligned;
   USB_memcopy(DevQualifierDescData, DevQualifierDesc, DEVICE_QUALIFIER_DESCRIPTOR_SIZE);
   Send_Buffer_aligned += ((DEVICE_QUALIFIER_DESCRIPTOR_SIZE/PSP_CACHE_LINE_SIZE) + \
                           1)* PSP_CACHE_LINE_SIZE;

   other_speed_config =  (uchar_ptr) Send_Buffer_aligned;
   USB_memcopy(other_speed_config_data, other_speed_config, OTHER_SPEED_CONFIG_DESC_SIZE);
   Send_Buffer_aligned += ((OTHER_SPEED_CONFIG_DESC_SIZE/PSP_CACHE_LINE_SIZE) + 1)* PSP_CACHE_LINE_SIZE; 

   /*buffer to receive data from Bulk OUT */
   //mass_ep1_buf =  (uchar_ptr) Send_Buffer_aligned;
   USB_memzero(mass_ep1_buf,BUFFERSIZE);
   Send_Buffer_aligned += ((BUFFERSIZE/PSP_CACHE_LINE_SIZE) + 1)* PSP_CACHE_LINE_SIZE;
   
   /*buffer for control endpoint to send data */
   epTemp_buf =  (uchar_ptr) Send_Buffer_aligned;
   USB_memzero(epTemp_buf,EP_TEMP_BUFFERSIZE);
   Send_Buffer_aligned += ((EP_TEMP_BUFFERSIZE/PSP_CACHE_LINE_SIZE) + 1)* PSP_CACHE_LINE_SIZE;
   
   
   /**************************************************************************
   Flush the cache to ensure main memory is updated.
   ***************************************************************************/
   USB_dcache_flush_mlines(temp,send_data_buffer_size); 

   return USB_OK;

}


extern _usb_device_handle global_usb_device_state_struct_ptr;

void initialize_ep2()
{
	_usb_device_handle   handle;
	
	handle = global_usb_device_state_struct_ptr;
		
        _usb_device_cancel_transfer(handle, 2, USB_SEND);
         
        if (_usb_device_get_transfer_status(handle, 2, USB_SEND) == USB_OK) {
       	 _usb_device_send_data(handle, 2, (uchar_ptr)mass_ep1_buf[in_flag], BUF_SIZE);
        } /* Endif */
}

//leonid+ for iso

void initialize_ep4()
{
	_usb_device_handle   handle;
	
	handle = global_usb_device_state_struct_ptr;
		
        _usb_device_cancel_transfer(handle, 4, USB_SEND);
         
        if (_usb_device_get_transfer_status(handle, 4, USB_SEND) == USB_OK) {
       	 //_usb_device_send_data(handle, 4, mass_ep3_buf[in_flag], iso_length);
       	 _usb_device_send_data(handle, 4, (uchar_ptr)mass_ep3_buf[out_flag], iso_length);
        } /* Endif */
}

/* EOF */
