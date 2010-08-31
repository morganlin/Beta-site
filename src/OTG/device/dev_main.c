/*******************************************************************************
** File          : $HeadURL$ 
** Author        : $Author$
** Project       : HSCTRL 
** Instances     : 
** Creation date : 
********************************************************************************
********************************************************************************
** ChipIdea Microelectronica - IPCS
** TECMAIA, Rua Eng. Frederico Ulrich, n 2650
** 4470-920 MOREIRA MAIA
** Portugal
** Tel: +351 229471010
** Fax: +351 229471011
** e_mail: chipidea.com
********************************************************************************
** ISO 9001:2000 - Certified Company
** (C) 2005 Copyright Chipidea(R)
** Chipidea(R) - Microelectronica, S.A. reserves the right to make changes to
** the information contained herein without notice. No liability shall be
** incurred as a result of its use or application.
********************************************************************************
** Modification history:
** $Date$
** $Revision$
*******************************************************************************
*** Description:      
***  This file contains the main USB device API functions that will be 
***  used by most applications.
***                                                               
**************************************************************************
**END*********************************************************/
#include <platform.h>
#include "irqs.h"
#include "dependency.h"

#include "devapi.h"
#include "usbprv_dev.h"
#include "demo.h"

extern u32 otg_base;
extern u32 otg_irq;

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_free_XD
*  Returned Value : void
*  Comments       :
*        Enqueues a XD onto the free XD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_device_free_XD
   (
      /* [IN] the dTD to enqueue */
      pointer  xd_ptr
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)
      (((XD_STRUCT_PTR)xd_ptr)->SCRATCH_PTR->PRIVATE);

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_free_XD");
   #endif

   USB_lock();

   /*
   ** Add the XD to the free XD queue (linked via PRIVATE) and
   ** increment the tail to the next descriptor
   */
   USB_XD_QADD(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, 
      (XD_STRUCT_PTR)xd_ptr);
   usb_dev_ptr->XD_ENTRIES++;

   USB_unlock();
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_free_XD, SUCCESSFUL");
   #endif

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device specific data structures and calls 
*  the low-level device controller chip initialization routine.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_init
   (
      /* [IN] the USB device controller to initialize */
      uint_8                    devnum,

      /* [OUT] the USB_USB_dev_initialize state structure */
      _usb_device_handle _PTR_  handle,
            
      /* [IN] number of endpoints to initialize */
      uint_8                    endpoints 
   )
{ /* Body */

   USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
   XD_STRUCT_PTR                    xd_ptr;
   uint_8                           temp, i, error;
   SCRATCH_STRUCT_PTR               temp_scratch_ptr;
   //uint_32                          total_memory;
   //uint_8_ptr                       stack_mem_ptr;
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_init");
   #endif   
   
   #ifndef REDUCED_ERROR_CHECKING                   
      
      if (devnum > MAX_USB_DEVICES) 
      {
         #ifdef _DEVICE_DEBUG_
            DEBUG_LOG_TRACE("_usb_device_init, error invalid device number");
         #endif
         return USBERR_INVALID_DEVICE_NUM;
      } /* Endif */
   #endif
      
   /**************************************************************
   All memory allocations should be consolidated in one.
   **************************************************************/
                                       
         /* Allocate memory for the state structure */
         //usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)
         //   USB_memalloc(sizeof(USB_DEV_STATE_STRUCT));
         usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)
            malloc(sizeof(USB_DEV_STATE_STRUCT));
      
         if (usb_dev_ptr == NULL) 
         {
            #ifdef _DEVICE_DEBUG_
               DEBUG_LOG_TRACE("_usb_device_init, error NULL device handle");
            #endif
            return USBERR_ALLOC_STATE;
         } /* Endif */
   
         /* Zero out the internal USB state structure */
         USB_memzero((char *)usb_dev_ptr,sizeof(USB_DEV_STATE_STRUCT));
  
   /**************************************************************
   **************************************************************/
        
   /* Multiple devices will have different base addresses and 
   ** interrupt vectors (For future)
   */   
   //usb_dev_ptr->DEV_PTR = 0x1cb20100;
   usb_dev_ptr->DEV_PTR = (VUSB20_REG_STRUCT_PTR)otg_base + 0x100;
   usb_dev_ptr->DEV_VEC = otg_irq;
   usb_dev_ptr->USB_STATE = USB_STATE_UNKNOWN;
   
   usb_dev_ptr->MAX_ENDPOINTS = endpoints;

   #ifndef SMALL_CODE_SIZE                   
      temp = (usb_dev_ptr->MAX_ENDPOINTS * 2);
   #endif
                
      /* Allocate MAX_XDS_FOR_TR_CALLS */
      xd_ptr = (XD_STRUCT_PTR)
         malloc(sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);
         //USB_memalloc(sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);
      if (xd_ptr == NULL) 
      {
         #ifdef _DEVICE_DEBUG_
            DEBUG_LOG_TRACE("_usb_device_init, malloc error");
         #endif
         return USBERR_ALLOC_TR;
      }

      USB_memzero((char *)xd_ptr, sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);

      /* Allocate memory for internal scratch structure */   
      usb_dev_ptr->XD_SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR)
         malloc(sizeof(SCRATCH_STRUCT)*MAX_XDS_FOR_TR_CALLS);
         //USB_memalloc(sizeof(SCRATCH_STRUCT)*MAX_XDS_FOR_TR_CALLS);
   
      if (usb_dev_ptr->XD_SCRATCH_STRUCT_BASE == NULL) 
      {
         #ifdef _DEVICE_DEBUG_
            DEBUG_LOG_TRACE("_usb_device_init, malloc error");
         #endif
         return USBERR_ALLOC;
      } /* Endif */
   
   usb_dev_ptr->XD_BASE = xd_ptr;
   

   temp_scratch_ptr = usb_dev_ptr->XD_SCRATCH_STRUCT_BASE;
   
   #ifndef SMALL_CODE_SIZE
      usb_dev_ptr->XD_HEAD = NULL;
      usb_dev_ptr->XD_TAIL = NULL;
      usb_dev_ptr->XD_ENTRIES = 0;
   #endif
   
   /* Enqueue all the XDs */   
   for (i=0;i<MAX_XDS_FOR_TR_CALLS;i++) {
      xd_ptr->SCRATCH_PTR = temp_scratch_ptr;
      xd_ptr->SCRATCH_PTR->FREE = _usb_device_free_XD;
      xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
      _usb_device_free_XD((pointer)xd_ptr);
      xd_ptr++;
      temp_scratch_ptr++;
   } /* Endfor */

      usb_dev_ptr->TEMP_XD_PTR = (XD_STRUCT_PTR)malloc(sizeof(XD_STRUCT));
      USB_memzero((char *)usb_dev_ptr->TEMP_XD_PTR, sizeof(XD_STRUCT));
      
   /* Initialize the USB controller chip */

   //printf("before _usb_dci_vusb20_init\n");//morganlin
   error = _usb_dci_vusb20_init(devnum, usb_dev_ptr);
   //printf("after _usb_dci_vusb20_init\n");//morganlin     
   
   if (error) 
   {
      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_device_init, init failed");
      #endif
      return USBERR_INIT_FAILED;
   } /* Endif */
   
   *handle = usb_dev_ptr;
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_init, SUCCESSFUL");
   #endif
   return USB_OK;
   
} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_device_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered. Used internally only.
* 
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_call_service
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,

      /* [OUT] Type of service or endpoint */
      uint_8               type,
      
      /* [OUT] Is it a Setup transfer? */
      boolean              setup,
      
      /* [OUT] Direction of transmission; is it a Transmit? */
      boolean              direction,

      /* [OUT] Pointer to the data */
      uint_8_ptr           buffer_ptr,
      
      /* [OUT] Number of bytes in transmission */
      uint_32              length,

      /* [OUT] Any errors */
      uint_8               errors
   )
{ /* Body */

   switch(type)
   {
         case  USB_SERVICE_EP0:
		//printf("USB_SERVICE_EP0\n");		//leonid
            service_ep0(handle, setup, direction, buffer_ptr, length, errors);
            break;         
         case  USB_SERVICE_EP1:
            service_ep1(handle, setup, direction, buffer_ptr, length, errors);
            break;         
         case  USB_SERVICE_EP2:		 	
            service_ep2(handle, setup, direction, buffer_ptr, length, errors);
            break;         
         case  USB_SERVICE_EP3: 	
            service_ep3(handle, setup, direction, buffer_ptr, length, errors);
            break;      
         case  USB_SERVICE_EP4: 	
            service_ep4(handle, setup, direction, buffer_ptr, length, errors);
            break;   
         case  USB_SERVICE_BUS_RESET:
            /* call your application routine here */
            reset_ep0(handle, setup, direction, buffer_ptr, length, errors);
            break;         
         case  USB_SERVICE_SUSPEND:
            break;         
         case  USB_SERVICE_SOF:
            break;         
         case  USB_SERVICE_RESUME:
            break;         
         case  USB_SERVICE_SLEEP:
           /*bus_suspend(handle, setup, direction, buffer_ptr, length, errors); */
            break;   
         case  USB_SERVICE_SPEED_DETECTION:
            //service_speed(handle, setup, direction, buffer_ptr, length, errors);
            detect_speed(handle, setup, direction, buffer_ptr, length, errors);
            break;         
         case  USB_SERVICE_ERROR:
            break;         
         case  USB_SERVICE_STALL:
            break;         
   }

 return USB_OK;  
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_init_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Initializes the endpoint and the data structures associated with the 
*  endpoint
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_init_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] MAX Packet size for this endpoint */
      uint_16                    max_pkt_size,
            
      /* [IN] Direction */
      uint_8                     direction,
            
      /* [IN] Type of Endpoint */
      uint_8                     type,
            
      /* [IN] After all data is transfered, should we terminate the transfer 
      ** with a zero length packet if the last packet size == MAX_PACKET_SIZE? 
      */
      uint_8                     flag   
   )
{ /* Body */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_init_endpoint");
   #endif

   uint_8         error = 0;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   /* Initialize the transfer descriptor */
   usb_dev_ptr->TEMP_XD_PTR->EP_NUM = ep_num;
   usb_dev_ptr->TEMP_XD_PTR->BDIRECTION = direction;
   usb_dev_ptr->TEMP_XD_PTR->WMAXPACKETSIZE = max_pkt_size;
   usb_dev_ptr->TEMP_XD_PTR->EP_TYPE = type;
   usb_dev_ptr->TEMP_XD_PTR->DONT_ZERO_TERMINATE = flag;
   usb_dev_ptr->TEMP_XD_PTR->MAX_PKTS_PER_UFRAME = 
      ((flag & USB_MAX_PKTS_PER_UFRAME) >> 1);

   error = _usb_dci_vusb20_init_endpoint(handle, usb_dev_ptr->TEMP_XD_PTR);
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_init_endpoint, SUCCESSFUL");
   #endif
   return error;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_transfer_status
*  Returned Value : Status of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_get_transfer_status
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   uint_8   status;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_transfer_status");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
   USB_lock();

	status = _usb_dci_vusb20_get_transfer_status(handle, ep_num, direction);

   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_transfer_status, SUCCESSFUL");
   #endif
   
   /* Return the status of the last queued transfer */
   return (status);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_transfer_details
*  Returned Value : Status of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
void _usb_device_get_transfer_details
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction,
      
      /* [OUT] transfer detail data structure maintained by driver*/
      uint_32_ptr _PTR_           xd_ptr_ptr                   
   )
{ /* Body */
   //uint_8   status;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   XD_STRUCT_PTR                 temp;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_transfer_status");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
//   USB_lock();

   temp = _usb_dci_vusb20_get_transfer_details(handle, ep_num, direction);
   (*xd_ptr_ptr) = (uint_32_ptr) temp;
   
//   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_transfer_status, SUCCESSFUL");
   #endif
   
   return;

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_read_setup_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Reads the setup data from the hardware
*
*END*-----------------------------------------------------------------*/
void _usb_device_read_setup_data
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] buffer for receiving Setup packet */
      uchar_ptr                  buff_ptr
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_read_setup_data");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

  _usb_dci_vusb20_get_setup_data(handle, ep_num, buff_ptr);

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_read_setup_data, SUCCESSFUL");
   #endif

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_set_address
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sets the device address as assigned by the host during enumeration
*
*END*-----------------------------------------------------------------*/
void _usb_device_set_address
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
      
      /* [IN] the USB address to be set in the hardware */
      uint_8                     address
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_set_address");
   #endif

  _usb_dci_vusb20_set_address(handle, address);

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_set_address, SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_send_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_send_data
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] buffer to send */
      uchar_ptr                  buff_ptr,
            
      /* [IN] length of the transfer */
      uint_32                    size
   )
{ /* Body */

   uint_8                           error;
   XD_STRUCT_PTR                    xd_ptr;
   USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
   USB_lock();

   if (!usb_dev_ptr->XD_ENTRIES) 
   {
      USB_unlock();
      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_device_send_data, transfer in progress");
      #endif
      return USB_STATUS_TRANSFER_IN_PROGRESS;
   } /* Endif */

   /* Get a transfer descriptor */
   USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);

   usb_dev_ptr->XD_ENTRIES--;

   /* Initialize the new transfer descriptor */      
   xd_ptr->EP_NUM = ep_num;
   xd_ptr->BDIRECTION = USB_SEND;
   xd_ptr->WTOTALLENGTH = size;
   xd_ptr->WSOFAR = 0;
   xd_ptr->WSTARTADDRESS = buff_ptr;
   
   xd_ptr->BSTATUS = USB_STATUS_TRANSFER_ACCEPTED;
   
   error = _usb_dci_vusb20_send_data(handle, xd_ptr);

   USB_unlock();
   
   if (error) 
   {
      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_device_send_data, transfer failed");
      #endif
      return USBERR_TX_FAILED;
   } /* Endif */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_send_data, SUCCESSFUL");
   #endif
   return error;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_recv_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Receives data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_recv_data
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] buffer to receive data */
      uchar_ptr                  buff_ptr,
            
      /* [IN] length of the transfer */
      uint_32                    size
   )
{ /* Body */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_recv_data");
   #endif

   uint_8                           error = USB_OK;
   XD_STRUCT_PTR                    xd_ptr;
   USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
   USB_lock();

   if (!usb_dev_ptr->XD_ENTRIES) 
   {
      USB_unlock();
      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_device_recv_data, transfer in progress");
      #endif
      return USB_STATUS_TRANSFER_IN_PROGRESS;
   } /* Endif */

   /* Get a transfer descriptor for the specified endpoint 
   ** and direction 
   */
   USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);
   
   usb_dev_ptr->XD_ENTRIES--;

   /* Initialize the new transfer descriptor */      
   xd_ptr->EP_NUM = ep_num;
   xd_ptr->BDIRECTION = USB_RECV;
   xd_ptr->WTOTALLENGTH = size;
   xd_ptr->WSOFAR = 0;
   xd_ptr->WSTARTADDRESS = buff_ptr;
   
   xd_ptr->BSTATUS = USB_STATUS_TRANSFER_ACCEPTED;

  error = _usb_dci_vusb20_recv_data(handle, xd_ptr);

   USB_unlock();
   
   if (error) 
   {
      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_device_recv_data, receive failed");
      #endif
      return USBERR_RX_FAILED;
   } /* Endif */
   
   return error;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_cancel_transfer
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   uint_8                        error = USB_OK;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_cancel_transfer");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
   USB_lock();

   /* Cancel transfer on the specified endpoint for the specified 
   ** direction 
   */

   error = _usb_dci_vusb20_cancel_transfer(handle, ep_num, direction);


   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_cancel_transfer, SUCCESSFUL");
   #endif
   return error;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_unstall_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Unstalls the endpoint in specified direction
*
*END*-----------------------------------------------------------------*/
void _usb_device_unstall_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   /* uint_8         error = 0; */
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_unstall_endpoint");
   #endif
   
   USB_lock();
   
   _usb_dci_vusb20_unstall_endpoint(handle, ep_num, direction);
   
   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_unstall_endpoint, SUCCESSFULL");
   #endif
   
} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_stall_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Stalls the endpoint.
*
*END*-----------------------------------------------------------------*/
void _usb_device_stall_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   /* uint_8         error = 0; */
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_stall_endpoint");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   _usb_dci_vusb20_stall_endpoint(handle, ep_num, direction);

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_stall_endpoint, SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_device_get_status
* Returned Value : USB_OK or error code
* Comments       :
*     Provides API to access the USB internal state.
* 
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_get_status
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,
      
      /* [IN] What to get the status of */
      uint_8               component,
      
      /* [OUT] The requested status */
      uint_16_ptr          status
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_status, SUCCESSFULL");
   #endif
   
   USB_lock();
   switch (component) {

      case USB_STATUS_DEVICE_STATE:
         *status = usb_dev_ptr->USB_STATE;
         break;

      case USB_STATUS_DEVICE:
         *status = usb_dev_ptr->USB_DEVICE_STATE;
         break;
         #ifndef SMALL_CODE_SIZE
      case USB_STATUS_INTERFACE:
         break;
         #endif         
      case USB_STATUS_ADDRESS:
         *status = usb_dev_ptr->DEVICE_ADDRESS;
         break;
         
      case USB_STATUS_CURRENT_CONFIG:
         *status = usb_dev_ptr->USB_CURR_CONFIG;
         break;

      case USB_STATUS_SOF_COUNT:
         *status = usb_dev_ptr->USB_SOF_COUNT;
         break;
         
      default:
         if (component & USB_STATUS_ENDPOINT) {
            *status = _usb_dci_vusb20_get_endpoint_status(handle,
               component & USB_STATUS_ENDPOINT_NUMBER_MASK);
         } else {
            USB_unlock();
            #ifdef _DEVICE_DEBUG_
               DEBUG_LOG_TRACE("_usb_device_get_status, bad status");
            #endif
            return USBERR_BAD_STATUS;
         } /* Endif */
         break;

   } /* Endswitch */
   
   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_status, SUCCESSFUL");
   #endif
   
   return USB_OK;   
} /* EndBody */  
 
/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_device_set_status
* Returned Value : USB_OK or error code
* Comments       :
*     Provides API to set internal state
* 
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_set_status
   (
      /* [IN] Handle to the usb device */
      _usb_device_handle   handle,
      
      /* [IN] What to set the status of */
      uint_8               component,
      
      /* [IN] What to set the status to */
      uint_16              setting
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_set_status");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   USB_lock();
   switch (component) {

      case USB_STATUS_DEVICE_STATE:
         usb_dev_ptr->USB_STATE = setting;
         break;

      case USB_STATUS_DEVICE:
         usb_dev_ptr->USB_DEVICE_STATE = setting;
         break;

      case USB_STATUS_INTERFACE:
         break;

      case USB_STATUS_CURRENT_CONFIG:
         usb_dev_ptr->USB_CURR_CONFIG = setting;
         break;

      case USB_STATUS_SOF_COUNT:
         usb_dev_ptr->USB_SOF_COUNT = setting;
         break;
         
      case USB_FORCE_FULL_SPEED:
         _usb_dci_vusb20_set_speed_full((pointer)usb_dev_ptr, setting);
         break;
      case USB_PHY_LOW_POWER_SUSPEND:
         _usb_dci_vusb20_suspend_phy((pointer)usb_dev_ptr, setting);
         break;

      case USB_STATUS_ADDRESS:
         usb_dev_ptr->DEVICE_ADDRESS = setting;
         _usb_dci_vusb20_set_address((pointer)usb_dev_ptr, setting);
         break;
#if 0      
      case USB_STATUS_TEST_MODE:
         _usb_dci_vusb20_set_test_mode(handle, 
            setting);
         break;
#endif         
      default:
         if (component & USB_STATUS_ENDPOINT) {
            _usb_dci_vusb20_set_endpoint_status(handle,
               component & USB_STATUS_ENDPOINT_NUMBER_MASK,
               setting);
         } else {
            USB_unlock();
            #ifdef _DEVICE_DEBUG_
               DEBUG_LOG_TRACE("_usb_device_set_status, bad status");
            #endif
            return USBERR_BAD_STATUS;
         } /* Endif */
         break;

   } /* Endswitch */

   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_set_status, SUCCESSFUL");
   #endif
   
   return USB_OK;   
} /* EndBody */



#if 0
extern _usb_device_handle
_usb_device_get_cur_device_handle()
{
	_usb_device_handle   handle;
	
	handle = _usb_dci_get_cur_device_handle();

	return handle;
}
#endif


