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
***  This file contains the main VUSB_HS Device Controller interface 
***  functions.
***                                                               
**************************************************************************
**END*********************************************************/
#include <global.h>
#include "devapi.h"
#include "usbprv_dev.h"

#include "dependency.h"

#define SOF_INTERRUPT_DISABLE
#define SUSPEND_INTERRUPT_DISABLE
#define TRIP_WIRE
#define SET_ADDRESS_HARDWARE_ASSISTANCE

unsigned int tmp1, tmp2;

/* in the  OTG mode this need to be a global */
extern USB_DEV_STATE_STRUCT_PTR global_usb_device_state_struct_ptr;

extern u32 otg_base;
extern u32 otg_irq;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_init
   (
      /* [IN] the USB device controller to initialize */
      uint_8                     devnum,

      /* [OUT] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
   uint_32                                      temp;
   uint_32                                      total_memory=0;
   uchar_ptr                                    driver_memory;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_init");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   //cap_dev_ptr = 0x1cb20100;
   //cap_dev_ptr = 0x180a0100;
   cap_dev_ptr = (VUSB20_REG_STRUCT_PTR)otg_base + 0x100;
      
   /* Get the base address of the VUSB_HS registers */
   usb_dev_ptr->DEV_PTR = 
      (VUSB20_REG_STRUCT_PTR)((uint_32)cap_dev_ptr + 
      (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER & 
      EHCI_CAP_LEN_MASK));

      /* Get the maximum number of endpoints supported by this USB controller */
      usb_dev_ptr->MAX_ENDPOINTS = 
         (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.DCC_PARAMS & 
         VUSB20_MAX_ENDPTS_SUPPORTED);
      
      temp = (usb_dev_ptr->MAX_ENDPOINTS * 2);
   
       /****************************************************************   
         Consolidated memory allocation    
       ****************************************************************/   
      total_memory = ((temp * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048) +
                     ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 32) +
                     (sizeof(SCRATCH_STRUCT)*MAX_EP_TR_DESCRS);

      //driver_memory = (uchar_ptr) USB_Uncached_memalloc(total_memory);
      driver_memory = (uchar_ptr) malloc(total_memory);
   
   
      if (driver_memory == NULL) {
         #ifdef _DEVICE_DEBUG_
            DEBUG_OTG_TRACE("_usb_dci_vusb20_init, malloc failed");
         #endif
         return USBERR_ALLOC;
      }
   
    /****************************************************************   
      Zero out the memory allocated
    ****************************************************************/   
   USB_memzero((void *) driver_memory,total_memory );

   /****************************************************************   
     Keep a pointer to driver memory alloctaion
   ****************************************************************/   
   usb_dev_ptr->DRIVER_MEMORY = driver_memory;
                    
   /****************************************************************   
      Assign QH base
   ****************************************************************/   
   usb_dev_ptr->EP_QUEUE_HEAD_BASE = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)\
                                     driver_memory;
   driver_memory += ((temp * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048);

   /* Align the endpoint queue head to 2K boundary */   
   usb_dev_ptr->EP_QUEUE_HEAD_PTR = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      USB_MEM2048_ALIGN((uint_32)usb_dev_ptr->EP_QUEUE_HEAD_BASE);
                                     
   /****************************************************************   
      Assign DTD base
   ****************************************************************/   
   usb_dev_ptr->DTD_BASE_PTR = (VUSB20_EP_TR_STRUCT_PTR) \
                                driver_memory;
   driver_memory += ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 32);
    
   /* Align the dTD base to 32 byte boundary */   
   usb_dev_ptr->DTD_ALIGNED_BASE_PTR = (VUSB20_EP_TR_STRUCT_PTR)
      USB_MEM32_ALIGN((uint_32)usb_dev_ptr->DTD_BASE_PTR);

   /****************************************************************   
      Assign SCRATCH Structure base
   ****************************************************************/   
   /* Allocate memory for internal scratch structure */   
   usb_dev_ptr->SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR)\
                                driver_memory;
   
/*********************************************************************
If there is no need to install ISR, don't install it. Some USB products
would just like to hardcode the function pointers for ISR calls.
*********************************************************************/
   
   usb_dev_ptr->USB_STATE = USB_STATE_UNKNOWN;

   /* Initialize the VUSB_HS controller */   
   _usb_dci_vusb20_chip_initialize((pointer)usb_dev_ptr);
   
   global_usb_device_state_struct_ptr = usb_dev_ptr;

	request_irq(usb_dev_ptr->DEV_VEC, _usb_dci_vusb20_isr, NULL);

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_init, SUCCESSFUL");
   #endif

   return USB_OK;   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_chip_initialize
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_chip_initialize
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_REG_STRUCT_PTR                        cap_dev_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr;
   uint_32                                      i, port_control;
   SCRATCH_STRUCT_PTR                           temp_scratch_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_chip_initialize");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   //cap_dev_ptr = 0x1cb20100;
   //cap_dev_ptr = 0x180a0100;
   cap_dev_ptr = (VUSB20_REG_STRUCT_PTR)otg_base + 0x100;
   
   /* Stop the controller */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &= 
      ~EHCI_CMD_RUN_STOP;
      
   /* Reset the controller to get default values */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD = EHCI_CMD_CTRL_RESET;

   while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD & 
      EHCI_CMD_CTRL_RESET) 
   {
      /* Wait for the controller reset to complete */
   } /* EndWhile */

   /* Program the controller to be the USB device controller */ 
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE = 
      (VUSBHS_MODE_CTRL_MODE_DEV | VUSBHS_MODE_SETUP_LOCK_DISABLE);
   
   /* Initialize the internal dTD head and tail to NULL */   
   usb_dev_ptr->DTD_HEAD = NULL;
   usb_dev_ptr->DTD_TAIL = NULL;

   /* Make sure the 16 MSBs of this register are 0s */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT = 0;
   
   ep_queue_head_ptr = usb_dev_ptr->EP_QUEUE_HEAD_PTR;

   /* Initialize all device queue heads */
   for (i=0;i<(usb_dev_ptr->MAX_ENDPOINTS * 2);i++) {
      /* Interrupt on Setup packet */
      (ep_queue_head_ptr + i)->MAX_PKT_LENGTH = 
         (((uint_32)USB_MAX_CTRL_PAYLOAD << 
         VUSB_EP_QUEUE_HEAD_MAX_PKT_LEN_POS) | VUSB_EP_QUEUE_HEAD_IOS);
      (ep_queue_head_ptr + i)->NEXT_DTD_PTR = 
         VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
   } /* Endfor */

   /* Configure the Endpoint List Address */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR = 
      (uint_32)ep_queue_head_ptr;
      
   if (cap_dev_ptr->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS & 
       VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG) 
   {
      port_control = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0];
      port_control &= (~EHCI_PORTSCX_W1C_BITS & ~EHCI_PORTSCX_PORT_POWER);		//leonid fix
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0] = port_control;
   } /* Endif */
   
   dTD_ptr = usb_dev_ptr->DTD_ALIGNED_BASE_PTR;

   temp_scratch_ptr = usb_dev_ptr->SCRATCH_STRUCT_BASE;

   /* Enqueue all the dTDs */   
   for (i=0;i<MAX_EP_TR_DESCRS;i++) {
      dTD_ptr->SCRATCH_PTR = temp_scratch_ptr;
      dTD_ptr->SCRATCH_PTR->FREE = _usb_dci_vusb20_free_dTD;
      /* Set the dTD to be invalid */
      dTD_ptr->NEXT_TR_ELEM_PTR = VUSBHS_TD_NEXT_TERMINATE;
      /* Set the Reserved fields to 0 */
      dTD_ptr->SIZE_IOC_STS &= ~VUSBHS_TD_RESERVED_FIELDS;
      dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
      _usb_dci_vusb20_free_dTD((pointer)dTD_ptr);
      dTD_ptr++;
      temp_scratch_ptr++;
   } /* Endfor */

   /* Initialize the endpoint 0 properties */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0] = 
      (EHCI_EPCTRL_TX_DATA_TOGGLE_RST | EHCI_EPCTRL_RX_DATA_TOGGLE_RST);
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0] &= 
      ~(EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
   
   /* Enable interrupts don't enable if one is disabled by a compiler switch*/
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR = 
         (
            EHCI_INTR_INT_EN | 
          #ifndef ERROR_INTERRUPT_DISABLE
            EHCI_INTR_ERR_INT_EN | 
          #endif
          #ifndef PORT_CHANGE_INTERRUPT_DISABLE
            EHCI_INTR_PORT_CHANGE_DETECT_EN | 
          #endif  
          #ifndef SOF_INTERRUPT_DISABLE  
            EHCI_INTR_SOF_UFRAME_EN | 
          #endif
          #ifndef SUSPEND_INTERRUPT_DISABLE  
            EHCI_INTR_DEVICE_SUSPEND |
          #endif  
            EHCI_INTR_RESET_EN
          );

    #ifndef SMALL_CODE_SIZE
      usb_dev_ptr->USB_STATE = USB_STATE_UNKNOWN;
    #endif
    
   /* Set the Run bit in the command register */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD = EHCI_CMD_RUN_STOP;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_chip_initialize, SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_free_dTD
*  Returned Value : void
*  Comments       :
*        Enqueues a dTD onto the free DTD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_free_dTD
   (
      /* [IN] the dTD to enqueue */
      pointer  dTD_ptr
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)
      (((VUSB20_EP_TR_STRUCT_PTR)dTD_ptr)->SCRATCH_PTR->PRIVATE);

   #ifdef _DEVICE_DEBUG_
      //DEBUG_LOG_TRACE("_usb_dci_vusb20_free_dTD");
   #endif

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   USB_lock();

   /*
   ** Add the dTD to the free dTD queue (linked via PRIVATE) and
   ** increment the tail to the next descriptor
   */
   EHCI_DTD_QADD(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL, 
      (VUSB20_EP_TR_STRUCT_PTR)dTD_ptr);
   usb_dev_ptr->DTD_ENTRIES++;

   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      //DEBUG_LOG_TRACE("_usb_dci_vusb20_free_dTD, SUCCESSFUL");
   #endif
   

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_add_dTD
*  Returned Value : USB_OK or error code
*  Comments       :
*        Adds a device transfer desriptor(s) to the queue.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_add_dTD
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* [IN] The transfer descriptor address */
      XD_STRUCT_PTR              xd_ptr
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr, temp_dTD_ptr, first_dTD_ptr = NULL;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
   uint_32                                      curr_pkt_len, remaining_len; 
   uint_32                                      curr_offset, temp, bit_pos, temp_ep_stat;
   #ifdef TRIP_WIRE
   boolean                                      read_safe;
   #endif
   /*********************************************************************
   For a optimal implementation, we need to detect the fact that
   we are adding DTD to an empty list. If list is empty, we can
   actually skip several programming steps esp. those for ensuring
   that there is no race condition.The following boolean will be useful
   in skipping some code here.
   *********************************************************************/
   boolean                                      list_empty = FALSE;
	//leonid
	uint_32 *tmp;
	tmp = (uint_32 *)0x1cb201b8;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_add_dTD");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   remaining_len = xd_ptr->WTOTALLENGTH;
   
   curr_offset = 0;
   temp = (2*xd_ptr->EP_NUM + xd_ptr->BDIRECTION);
   bit_pos = (1 << (16 * xd_ptr->BDIRECTION + xd_ptr->EP_NUM));
   
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR + 
      temp;
   /*********************************************************************
   This loops iterates through the length of the transfer and divides
   the data in to DTDs each handling the a max of 0x4000 bytes of data.
   The first DTD in the list is stored in a pointer called first_dTD_ptr.
   This pointer is later linked in to QH for processing by the hardware.   
   *********************************************************************/

   do {
      /* Check if we need to split the transfer into multiple dTDs */
      if (remaining_len > VUSB_EP_MAX_LENGTH_TRANSFER) {
         curr_pkt_len = VUSB_EP_MAX_LENGTH_TRANSFER;
      } else {
         curr_pkt_len = remaining_len;
      } /* Endif */
   
      remaining_len -= curr_pkt_len;

      /* Get a dTD from the queue */   
      EHCI_DTD_QGET(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL, dTD_ptr);
   
      if (!dTD_ptr) {
         #ifdef _DEVICE_DEBUG_
            DEBUG_LOG_TRACE("_usb_dci_vusb20_add_dTD, SUCCESSFUL");
         #endif
         return USBERR_TR_FAILED;
      } /* Endif */

      usb_dev_ptr->DTD_ENTRIES--;

      if (curr_offset == 0) {
         first_dTD_ptr = dTD_ptr;
      } /* Endif */

      #ifdef _DATA_CACHE_
      /**************************************************************
      USB Memzero does not bypass the cache and hence we must use
      DTD pointer to update the memory and bypass the cache. If
      your DTD are allocated from an uncached regigio, you can
      eliminitate this approach and switch back to USB_memzero().
      **************************************************************/
      
         dTD_ptr->NEXT_TR_ELEM_PTR   = 0;
         dTD_ptr->SIZE_IOC_STS       = 0;
         dTD_ptr->BUFF_PTR0          = 0;
         dTD_ptr->BUFF_PTR1          = 0;
         dTD_ptr->BUFF_PTR2          = 0;
         dTD_ptr->BUFF_PTR3          = 0;
         dTD_ptr->BUFF_PTR4          = 0;

      #else
         /* Zero the dTD. Leave the last 4 bytes as that is the scratch pointer */
         USB_memzero((void *) dTD_ptr,(sizeof(VUSB20_EP_TR_STRUCT) - 4));
      #endif   

      /* Initialize the dTD */
      dTD_ptr->SCRATCH_PTR->PRIVATE = handle;
   
      /* Set the Terminate bit */
      dTD_ptr->NEXT_TR_ELEM_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;


      /*************************************************************
      FIX ME: For hig-speed and high-bandwidth ISO IN endpoints,
      we must initialize the multiplied field so that Host can issues
      multiple IN transactions on the endpoint. See the DTD data
      structure for MultiIO field.
      
      S Garg 11/06/2003
      *************************************************************/
      
      /* Fill in the transfer size */
      if (!remaining_len) {
         dTD_ptr->SIZE_IOC_STS = ((curr_pkt_len << 
            VUSBHS_TD_LENGTH_BIT_POS) | (VUSBHS_TD_IOC) | 
            (VUSBHS_TD_STATUS_ACTIVE));
      } else {
         dTD_ptr->SIZE_IOC_STS = ((curr_pkt_len << VUSBHS_TD_LENGTH_BIT_POS) 
            | VUSBHS_TD_STATUS_ACTIVE);
      } /* Endif */
   
      /* Set the reserved field to 0 */
      dTD_ptr->SIZE_IOC_STS &= ~VUSBHS_TD_RESERVED_FIELDS;

      /* 4K apart buffer page pointers */
      dTD_ptr->BUFF_PTR0 = (uint_32)(xd_ptr->WSTARTADDRESS + curr_offset);
      dTD_ptr->BUFF_PTR1 = (dTD_ptr->BUFF_PTR0 + 4096);
      dTD_ptr->BUFF_PTR2 = (dTD_ptr->BUFF_PTR1 + 4096);
      dTD_ptr->BUFF_PTR3 = (dTD_ptr->BUFF_PTR2 + 4096);
      dTD_ptr->BUFF_PTR4 = (dTD_ptr->BUFF_PTR3 + 4096);

      curr_offset += curr_pkt_len;

      /* Maintain the first and last device transfer descriptor per 
      ** endpoint and direction 
      */
      if (!usb_dev_ptr->EP_DTD_HEADS[temp]) 
      {
         usb_dev_ptr->EP_DTD_HEADS[temp] = dTD_ptr;
         /***********************************************
         If list does not have a head, it means that list
         is empty. An empty condition is detected.
         ***********************************************/
         list_empty = TRUE;
      } /* Endif */ 
   
      /* Check if the transfer is to be queued at the end or beginning */
      temp_dTD_ptr = usb_dev_ptr->EP_DTD_TAILS[temp];
      
      /* Remember which XD to use for this dTD */
      dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD = (pointer)xd_ptr;
      
      /* New tail */
      usb_dev_ptr->EP_DTD_TAILS[temp] = dTD_ptr;
      if (temp_dTD_ptr) {
         /* Should not do |=. The Terminate bit should be zero */
         temp_dTD_ptr->NEXT_TR_ELEM_PTR = (uint_32)dTD_ptr;
      } /* Endif */

   } while (remaining_len); /* EndWhile */


   /**************************************************************
   In the loop above DTD has already been added to the list
   However endpoint has not been primed yet. If list is not empty we need safter ways to add DTD to the
   existing list. Else we just skip to adding DTD to QH safely.
   **************************************************************/
   
   if(!list_empty)
   {
         #ifdef TRIP_WIRE
               /*********************************************************
               Hardware v3.2+ require the use of semaphore to ensure that
               QH is safely updated.
               *********************************************************/
               
               /*********************************************************
               Check the prime bit. If set goto done
               *********************************************************/
               if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME & bit_pos) 
               {
                  goto done;
               }

               read_safe = FALSE;                      
               while(!read_safe)
               {

                  /*********************************************************
                  start with setting the semaphores
                  *********************************************************/
                  dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD |= 
                                                 EHCI_CMD_ATDTW_TRIPWIRE_SET;
                      
                  /*********************************************************
                  Read the endpoint status
                  *********************************************************/
                                                 
                  temp_ep_stat = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & bit_pos; 

                  /*********************************************************
                  Reread the ATDTW semaphore bit to check if it is cleared.
                  When hardware see a hazard, it will clear the bit or
                  else we remain set to 1 and we can proceed with priming
                  of endpoint if not already primed.
                  *********************************************************/
                  if(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD & 
                                                 EHCI_CMD_ATDTW_TRIPWIRE_SET);
                  {
                        read_safe = TRUE;
                  }

               }/*end while loop */

               /*********************************************************
               Clear the semaphore
               *********************************************************/
               dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &= 
                                              EHCI_CMD_ATDTW_TRIPWIRE_CLEAR;

               /*********************************************************
               If endpoint is not active, we activate it now.               
               *********************************************************/
               if(!temp_ep_stat)
               {
                  /* No other transfers on the queue */
                  ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)first_dTD_ptr;
                  ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;

                  /* Prime the Endpoint */
                  dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = bit_pos;
               }

         #else   /*workaround old method */
                   
          /* Start CR 1015 */
         /* Prime the Endpoint */
         dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = bit_pos;

         if (!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & 
            bit_pos)) 
         {
   
                     /* old workaround will be compiled */
                     while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME & bit_pos) {
                        /* Wait for the ENDPTPRIME to go to zero */
                     } /* EndWhile */

                     if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & 
                        bit_pos) 
                     {
                        /* The endpoint was not not primed so no other transfers on 
                        ** the queue 
                        */
                          goto done;
                     } /* Endif */
      
         } else {
            goto done;
         } /* Endif */

         /* No other transfers on the queue */
         ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)first_dTD_ptr;
         ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
   
         /* Prime the Endpoint */
         dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = bit_pos;
         
         #endif

   }
   else
   {
         /* No other transfers on the queue */
         ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)first_dTD_ptr;
         ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
   
         /* Prime the Endpoint */
         dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = bit_pos;
      
   }

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_add_dTD, SUCCESSFUL");
   #endif

done:
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_add_dTD, SUCCESSFUL");
   #endif

   return USB_OK;
   /* End CR 1015 */
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_send_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data by adding and executing the dTD. Non-blocking.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_send_data
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* [IN] The transfer descriptor address */
      XD_STRUCT_PTR              xd_ptr
   )
{ /* Body */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_send_data, SUCCESSFUL");
   #endif

   /* Add and execute the device transfer descriptor */
   return(_usb_dci_vusb20_add_dTD(handle, xd_ptr));
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_recv_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Receives data by adding and executing the dTD. Non-blocking.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_recv_data
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* [IN] The transfer descriptor address */
      XD_STRUCT_PTR              xd_ptr
   )
{ /* Body */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_recv_data, SUCCESSFUL");
   #endif
   /* Add and execute the device transfer descriptor */
   return(_usb_dci_vusb20_add_dTD(handle, xd_ptr));
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_tr_complete
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_tr_complete
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr; 
   VUSB20_EP_TR_STRUCT_PTR                      temp_dTD_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
   uint_32                                      temp, i, ep_num, direction, bit_pos;
   uint_32                                      remaining_length = 0;
   uint_32                                      actual_transfer_length = 0;
   uint_32                                      errors = 0;
   XD_STRUCT_PTR                                xd_ptr;
   XD_STRUCT_PTR                                temp_xd_ptr = NULL;
   uchar_ptr                                    buff_start_address = NULL;
   boolean                                      endpoint_detected = FALSE;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   #ifdef _DEVICE_DEBUG_
      printf("_usb_dci_vusb20_process_tr_complete");
   #endif
   
   /* We use separate loops for ENDPTSETUPSTAT and ENDPTCOMPLETE because the 
   ** setup packets are to be read ASAP 
   */
   
   /* Process all Setup packet received interrupts */
   bit_pos = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT;
   
   if (bit_pos) {
      for (i=0;i<USB_MAX_CONTROL_ENDPOINTS;i++) {
         if (bit_pos & (1 << i)) {
		//printf("_usb_device_call_service\n");
            _usb_device_call_service(handle, i, TRUE, 0, 0, 8, 0);
         } /* Endif */
      } /* Endfor */
   } /* Endif */
   
   /* Don't clear the endpoint setup status register here. It is cleared as a 
   ** setup packet is read out of the buffer 
   */

   /* Process non-setup transaction complete interrupts */   
   bit_pos = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE;

   /* Clear the bits in the register */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE = bit_pos;
   
   if (bit_pos) {
      /* Get the endpoint number and the direction of transfer */
      
      for (i=0;i<USB_MAX_ENDPOINTS*2;i++) {

         endpoint_detected = FALSE;
         if ((i < USB_MAX_ENDPOINTS) && (bit_pos & (1 << i)))
         {
            ep_num = i;
            direction = 0;
            endpoint_detected = TRUE;
         }
         else if((i >= USB_MAX_ENDPOINTS) && (bit_pos & (1 << (i+16-USB_MAX_ENDPOINTS))))
         {
            ep_num = (i - USB_MAX_ENDPOINTS);
            direction = 1;
            endpoint_detected = TRUE;
         }
         
         if(endpoint_detected)
         {
 
            temp = (2*ep_num + direction);

            /* Get the first dTD */      
            dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];
            
            ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
               dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR + 
               temp;

            /* Process all the dTDs for respective transfers */
            while (dTD_ptr) {
            
               if (dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_STATUS_ACTIVE) {
                  /* No more dTDs to process. Next one is owned by VUSB */
                  break;
               } /* Endif */
               
               /* Get the correct internal transfer descriptor */
               xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
               if (xd_ptr) {
                  buff_start_address = xd_ptr->WSTARTADDRESS;
                  actual_transfer_length = xd_ptr->WTOTALLENGTH;
                  temp_xd_ptr = xd_ptr;
               } /* Endif */
               
               /* Get the address of the next dTD */
               temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)
                  (dTD_ptr->NEXT_TR_ELEM_PTR & VUSBHS_TD_ADDR_MASK);
                  
               /* Read the errors */
               errors = (dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_ERROR_MASK);
                  
               if (!errors) {
                  /* No errors */
                  /* Get the length of transfer from the current dTD */   
                  remaining_length += ((dTD_ptr->SIZE_IOC_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
                  actual_transfer_length -= remaining_length;
               } else {
                  if (errors & VUSBHS_TD_STATUS_HALTED) {
                     /* Clear the errors and Halt condition */
                     ep_queue_head_ptr->SIZE_IOC_INT_STS &= ~errors;
                  } /* Endif */
               } /* Endif */
               
               /* Retire the processed dTD */
               _usb_dci_vusb20_cancel_transfer(handle, ep_num, direction);
               if (temp_dTD_ptr) {
                  if ((uint_32)temp_dTD_ptr->SCRATCH_PTR->\
                     XD_FOR_THIS_DTD != (uint_32)temp_xd_ptr) 
                  {
                     /* Transfer complete. Call the register service function for the 
                     ** endpoint 
                     */
                     _usb_device_call_service(handle, ep_num, FALSE, direction, 
                        buff_start_address, actual_transfer_length, errors);
                     remaining_length = 0;
                  } /* Endif */
               } else {
                  /* Transfer complete. Call the register service function for the 
                  ** endpoint 
                  */
                  _usb_device_call_service(handle, ep_num, FALSE, direction, 
                     buff_start_address, actual_transfer_length, errors);
               } /* Endif */
               dTD_ptr = temp_dTD_ptr;
               errors = 0;
            } /* Endwhile */
         } /* Endif */
      } /* Endfor */
   } /* Endif */
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_tr_complete, SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_isr
*  Returned Value : None
*  Comments       :
*        Services all the VUSB_HS interrupt sources
*
*END*-----------------------------------------------------------------*/

//void _usb_dci_vusb20_isr(void)
void _usb_dci_vusb20_isr(void *param)		//jerry modify 2010/03/01
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   uint_32                                      status;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)global_usb_device_state_struct_ptr;

   #ifdef _DEVICE_DEBUG_
      //DEBUG_LOG_TRACE("\n_usb_dci_vusb20_isr");
      //printf("\n_usb_dci_vusb20_isr\n");
   #endif

   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   for (;;) {
      status = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS;
	//printf("status = 0x%08x\n", status);
	//printf("portscx = 0x%08x\n", dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0]);     
 
      if (!(status & dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR)) {
	break;
      } /* Endif */

      /* Clear all the interrupts occured */
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS = status;
      
      if (status & EHCI_STS_RESET) {
         _usb_dci_vusb20_process_reset((pointer)usb_dev_ptr);
     	//printf("EHCI_STS_RESET\n");
	} /* Endif */

      #ifndef PORT_CHANGE_INTERRUPT_DISABLE   
      if (status & EHCI_STS_PORT_CHANGE) {
         _usb_dci_vusb20_process_port_change((pointer)usb_dev_ptr);
      } /* Endif */
      #endif

      #ifndef ERROR_INTERRUPT_DISABLE   
      if (status & EHCI_STS_ERR) {
         _usb_dci_vusb20_process_error((pointer)usb_dev_ptr);
      } /* Endif */
      #endif

      #ifndef SOF_INTERRUPT_DISABLE   
      if (status & EHCI_STS_SOF) {
         _usb_dci_vusb20_process_SOF((pointer)usb_dev_ptr);
      } /* Endif */
      #endif
         
      if (status & EHCI_STS_INT) {
         _usb_dci_vusb20_process_tr_complete((pointer)usb_dev_ptr);
	//printf("tr_complete\n");
      } /* Endif */

      #ifndef SUSPEND_INTERRUPT_DISABLE   
      if (status & EHCI_STS_SUSPEND) {
         _usb_dci_vusb20_process_suspend((pointer)usb_dev_ptr);
         return;
      } /* Endif */
      #endif
      
   } /* Endfor */

   #ifdef _DEVICE_DEBUG_
      //DEBUG_LOG_TRACE("_usb_dci_vusb20_isr, SUCCESSFUL");
      printf("_usb_dci_vusb20_isr, SUCCESSFUL\n");
   #endif

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_reset
*  Returned Value : None
*  Comments       :
*        Services reset interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_reset
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   uint_32                                      temp;
   uint_8                                       i;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_reset");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   
   /* Inform the application so that it can cancel all previously queued transfers */
   _usb_device_call_service(usb_dev_ptr, USB_SERVICE_BUS_RESET, 0, 0, 0, 0, 0);
   
   /* The address bits are past bit 25-31. Set the address */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR &= ~0xFE000000;
   
   /* Clear all the setup token semaphores */
   temp = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT;
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT = temp;

   /* Clear all the endpoint complete status bits */   
   temp = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE;
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE = temp;
   
   while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME & 0xFFFFFFFF) {
      /* Wait until all ENDPTPRIME bits cleared */
   } /* Endif */
   
   /* Write 1s to the Flush register */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = 0xFFFFFFFF;

   /* Unstall all endpoints */
   for (i=0;i<usb_dev_ptr->MAX_ENDPOINTS;i++) {
      _usb_dci_vusb20_unstall_endpoint(handle, i, 0);
      _usb_dci_vusb20_unstall_endpoint(handle, i, 1);
   } /* Endfor */
   
   if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0] & 
      EHCI_PORTSCX_PORT_RESET) 
   {
      usb_dev_ptr->BUS_RESETTING = TRUE;
      usb_dev_ptr->USB_STATE = USB_STATE_POWERED;
   } else { 
      /* re-initialize */      
      _usb_dci_vusb20_chip_initialize((pointer)usb_dev_ptr);
	printf("_usb_dci_vusb20_process_reset, SUCCESSFUL reinit hw");
      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_dci_vusb20_process_reset, SUCCESSFUL reinit hw");
      #endif
      return;
   } /* Endif */
   
   _usb_device_call_service(usb_dev_ptr, USB_SERVICE_BUS_RESET, 0, 0, 0, 0, 0);

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_reset, SUCCESSFUL");
   #endif
   
} /* EndBody */

#ifndef SUSPEND_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_suspend
*  Returned Value : None
*  Comments       :
*        Services suspend interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_suspend
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_suspend");
   #endif

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
   usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->USB_STATE;
   
   usb_dev_ptr->USB_STATE = USB_STATE_SUSPEND;

   /* Inform the upper layers */
   _usb_device_call_service(usb_dev_ptr, USB_SERVICE_SLEEP, 0, 0, 0, 0, 0);

   #ifdef __MISSING_RESUME_INTERRUPT_WORKAROUND__
   sof_after_suspend =0;
   #endif /* __MISSING_RESUME_INTERRUPT_WORKAROUND__ */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_suspend, SUCCESSFUL");
   #endif
   
} /* EndBody */
#endif

#ifndef SOF_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_SOF
*  Returned Value : None
*  Comments       :
*        Services SOF interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_SOF
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_SOF");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Inform the upper layer */   
   _usb_device_call_service(usb_dev_ptr, USB_SERVICE_SOF, 0, 0, 0, 
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_FRINDEX, 0);

  #ifdef __MISSING_RESUME_INTERRUPT_WORKAROUND__
  #ifdef __USB_OTG__
   /* 10 SOFs after the suspend means that we should provide
   a resume callback */
   if ((usb_dev_ptr->USB_STATE == USB_STATE_SUSPEND) &&
      (sof_after_suspend > TB_A_SUSPEND))
   {
       usb_dev_ptr->USB_STATE = usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND;
       /* Inform the upper layers */
       _usb_device_call_service(usb_dev_ptr, USB_SERVICE_RESUME, 0, 0, 0, 0, 0);
   }
   #endif
   #endif /* __MISSING_RESUME_INTERRUPT_WORKAROUND__ */
   

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_SOF, SUCCESSFUL");
   #endif

} /* EndBody */
#endif

#ifndef PORT_CHANGE_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_port_change
*  Returned Value : None
*  Comments       :
*        Services port change detect interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_port_change
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_port_change");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   if (usb_dev_ptr->BUS_RESETTING) {
      /* Bus reset operation complete */
      usb_dev_ptr->BUS_RESETTING = FALSE;
   } /* Endif */

#ifdef __MISSING_RESUME_INTERRUPT_WORKAROUND__
   port_change_log[port_change_counter] = \
       dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0];
   port_change_counter++;
#endif /* __MISSING_RESUME_INTERRUPT_WORKAROUND__ */

   if (!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0] & 
         EHCI_PORTSCX_PORT_RESET)) 
   {
      /* Get the speed */
      if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0] & 
         EHCI_PORTSCX_PORT_HIGH_SPEED) 
      {
         usb_dev_ptr->SPEED = USB_SPEED_HIGH;
      } else {
         usb_dev_ptr->SPEED = USB_SPEED_FULL;
      } /* Endif */

      /* Inform the upper layers of the speed of operation */      
      _usb_device_call_service(usb_dev_ptr, USB_SERVICE_SPEED_DETECTION, 0, 0, 
         0, usb_dev_ptr->SPEED, 0);
   
   } /* Endif */
      
#ifndef SMALL_CODE_SIZE      
   if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0] & 
      EHCI_PORTSCX_PORT_SUSPEND) 
   {
   
      usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->USB_STATE;
      usb_dev_ptr->USB_STATE = USB_STATE_SUSPEND;

      /* Inform the upper layers */
      _usb_device_call_service(usb_dev_ptr, USB_SERVICE_SUSPEND, 0, 0, 0, 0, 0);
   } /* Endif */
#endif
   
   if (!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[0] & 
      EHCI_PORTSCX_PORT_SUSPEND) && 
      (usb_dev_ptr->USB_STATE == USB_STATE_SUSPEND)) 
   {
      usb_dev_ptr->USB_STATE = usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND;
      /* Inform the upper layers */
      _usb_device_call_service(usb_dev_ptr, USB_SERVICE_RESUME, 0, 0, 0, 0, 0);

      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_dci_vusb20_process_port_change, SUCCESSFUL, resumed");
      #endif
      return;
   } /* Endif */

   usb_dev_ptr->USB_STATE = USB_STATE_DEFAULT;
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_port_change, SUCCESSFUL");
   #endif
   
} /* EndBody */
#endif

#ifndef ERROR_INTERRUPT_DISABLE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_error
*  Returned Value : None
*  Comments       :
*        Services error interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_error
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_error");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   
   /* Increment the error count */
   usb_dev_ptr->ERRORS++;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_process_error, SUCCESSFUL");
   #endif
   
} /* EndBody */
#endif
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_set_speed_full
*  Returned Value : None
*  Comments       :
*        Force the controller port in full speed mode.
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_set_speed_full
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* The port number on the device */
      uint_8                     port_number
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   uint_32                                      port_control;
   
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_speed_full");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   port_control = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[port_number];   
   port_control |= EHCI_PORTSCX_FORCE_FULL_SPEED_CONNECT;
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[port_number] = port_control;

   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_speed_full, SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_suspend_phy
*  Returned Value : None
*  Comments       :
*        Suspends the PHY in low power mode
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_suspend_phy
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* The port number on the device */
      uint_8                     port_number
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   uint_32                                      port_control;
   
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_speed_full");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   port_control = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[port_number];   
   port_control |= EHCI_PORTSCX_PHY_CLOCK_DISABLE;
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX[port_number] = port_control;

   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_speed_full, SUCCESSFUL");
   #endif
   
} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_set_address
*  Returned Value : None
*  Comments       :
*        Sets the newly assigned device address
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_set_address
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* Address of the device assigned by the host */
      uint_8                     address
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_address");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   
	printf("addr = 0x%08x\n", address);

   #ifdef SET_ADDRESS_HARDWARE_ASSISTANCE   
   /***********************************************************
   Hardware Rev 4.0 onwards have special assistance built in
   for handling the set_address command. As per the USB specs
   a device should be able to receive the response on a new
   address, within 2 msecs after status phase of set_address is
   completed. Since 2 mili second may be a very small time window
   (on high interrupt latency systems) before software could 
   come to the code below and write the device register,
   this routine will be called in advance when status phase of
   set_address is still not finished. The following line in the
   code will set the bit 24 to '1' and hardware will take
   the address and queue it in an internal buffer. From which
   it will use it to decode the next USB token. Please look
   at hardware rev details for the implementation of this
   assistance.
   
   Also note that writing bit 24 to 0x01 will not break
   any old hardware revs because it was an unused bit.
   ***********************************************************/
   /* The address bits are past bit 25-31. Set the address 
   also set the bit 24 to 0x01 to start hardware assitance*/
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR = 
      ((uint_32)address << VUSBHS_ADDRESS_BIT_SHIFT) | 
      (0x01 << (VUSBHS_ADDRESS_BIT_SHIFT -1)); 
   #else
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR = 
      (uint_32)address << VUSBHS_ADDRESS_BIT_SHIFT; 
   #endif
   
   usb_dev_ptr->USB_STATE = USB_STATE_ADDRESS;
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_address, SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_setup_data
*  Returned Value : None
*  Comments       :
*        Reads the Setup data from the 8-byte setup buffer
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_get_setup_data
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [OUT] address of the buffer to read the setup data into */
      uchar_ptr                  buffer_ptr
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
   #ifdef TRIP_WIRE
   boolean                                      read_safe;
   #endif

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_setup_data");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Get the endpoint queue head */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR + 2*ep_num + 
      USB_RECV;

        //leonid
   /* Clear the bit in the ENDPTSETUPSTAT */
   //dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT =
   //   (1 << ep_num);



   /********************************************************************
   CR 1219. Hardware versions 2.3+ have a implementation of tripwire 
   semaphore mechanism that requires that we read the contents of 
   QH safely by using the semaphore. Read the USBHS document to under
   stand how the code uses the semaphore mechanism. The following are
   the steps in brief
   
   1. USBCMD Write ??to Setup Tripwire in register.
   2. Duplicate contents of dQH.StatusBuffer into local software byte
      array.
   3  Read Setup TripWire in register. (if set - continue; if
      cleared goto 1.)
   4. Write '0' to clear Setup Tripwire in register.
   5. Process setup packet using local software byte array copy and
      execute status/handshake phases.
           
   ********************************************************************/
  #ifdef TRIP_WIRE  /*if semaphore mechanism is used the following code
                     is compiled in*/
  read_safe = FALSE;                      
  while(!read_safe)
  {
      /*********************************************************
      start with setting the semaphores
      *********************************************************/

      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD |= 
                                                EHCI_CMD_SETUP_TRIPWIRE_SET;

      /*********************************************************
      Duplicate the contents of SETUP buffer to our buffer
      *********************************************************/
        #ifdef _DATA_CACHE_
        for(int i=0; i <8; i++)
        {
              *(buffer_ptr + i) = ep_queue_head_ptr->SETUP_BUFFER[i];
        }
        #else    
         /* Copy the setup packet to private buffer */
         USB_memcopy((char *)ep_queue_head_ptr->SETUP_BUFFER, (char *)buffer_ptr, 8);
        #endif 
      /*********************************************************
      If setup tripwire semaphore is cleared by hardware it means
      that we have a danger and we need to restart.
      else we can exit out of loop safely.
      *********************************************************/
      if(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &
                                                   EHCI_CMD_SETUP_TRIPWIRE_SET)
      {
          read_safe = TRUE; /* we can proceed exiting out of loop*/
      }
  }
  
   /*********************************************************
   Clear the semaphore bit now
   *********************************************************/
  dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &=
    EHCI_CMD_SETUP_TRIPWIRE_CLEAR;                         
  
  
  #else   /*when semaphore is not used */    
     #ifdef _DATA_CACHE_
        for(int i=0; i <8; i++)
        {
           *(buffer_ptr + i) = ep_queue_head_ptr->SETUP_BUFFER[i];
        }
     #else    
      /* Copy the setup packet to private buffer */
      USB_memcopy((uchar_ptr)ep_queue_head_ptr->SETUP_BUFFER, buffer_ptr, 8);
     #endif 
  #endif   
  
   /* Clear the bit in the ENDPTSETUPSTAT */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT = 
      (1 << ep_num);

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_setup_data, SUCCESSFUL");
   #endif
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_init_endpoint
*  Returned Value : None
*  Comments       :
*        Initializes the specified endpoint and the endpoint queue head
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_init_endpoint
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the transaction descriptor address */
      XD_STRUCT_PTR              xd_ptr
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_            ep_queue_head_ptr;
   uint_32                                      bit_pos;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_init_endpoint");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Get the endpoint queue head address */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR + 
      2*xd_ptr->EP_NUM + xd_ptr->BDIRECTION;
      
   bit_pos = (1 << (16 * xd_ptr->BDIRECTION + xd_ptr->EP_NUM));
   
   /* Check if the Endpoint is Primed */
   if ((!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME & bit_pos)) && 
      (!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & bit_pos))) 
   { 
      /* Set the max packet length, interrupt on Setup and Mult fields */
      if (xd_ptr->EP_TYPE == USB_ISOCHRONOUS_ENDPOINT) {
         /* Mult bit should be set for isochronous endpoints */
         ep_queue_head_ptr->MAX_PKT_LENGTH = ((xd_ptr->WMAXPACKETSIZE << 16) | 
            ((xd_ptr->MAX_PKTS_PER_UFRAME ?  xd_ptr->MAX_PKTS_PER_UFRAME : 1) << 
            VUSB_EP_QUEUE_HEAD_MULT_POS));
      } else {
         if (xd_ptr->EP_TYPE != USB_CONTROL_ENDPOINT) {
            ep_queue_head_ptr->MAX_PKT_LENGTH = ((xd_ptr->WMAXPACKETSIZE << 16) | 
               (xd_ptr->DONT_ZERO_TERMINATE ? 
               VUSB_EP_QUEUE_HEAD_ZERO_LEN_TER_SEL : 0));
         } else {
            ep_queue_head_ptr->MAX_PKT_LENGTH = ((xd_ptr->WMAXPACKETSIZE << 16) | 
               VUSB_EP_QUEUE_HEAD_IOS);
         } /* Endif */
      } /* Endif */
      
      /* Enable the endpoint for Rx and Tx and set the endpoint type */
	tmp1 = (xd_ptr->BDIRECTION ? (EHCI_EPCTRL_TX_ENABLE | EHCI_EPCTRL_TX_DATA_TOGGLE_RST) : (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST));
	tmp2 = (xd_ptr->EP_TYPE << (xd_ptr->BDIRECTION ? EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT));

	tmp1 |= tmp2;

      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM] |= tmp1;

#if 0
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM] |= 
         ((xd_ptr->BDIRECTION ? (EHCI_EPCTRL_TX_ENABLE | 
         EHCI_EPCTRL_TX_DATA_TOGGLE_RST) : 
         (EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST)) | 
         (xd_ptr->EP_TYPE << (xd_ptr->BDIRECTION ? 
         EHCI_EPCTRL_TX_EP_TYPE_SHIFT : EHCI_EPCTRL_RX_EP_TYPE_SHIFT)));
#endif

   } else 
   { 
      #ifdef _DEVICE_DEBUG_
         DEBUG_LOG_TRACE("_usb_dci_vusb20_init_endpoint, error ep init");
      #endif
      return USBERR_EP_INIT_FAILED;
   } /* Endif */
      
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_init_endpoint, SUCCESSFUL");
   #endif

   return USB_OK;
   
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_transfer_status
*  Returned Value : USB_OK or error code
*  Comments       :
*        Gets the status of a transfer
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_get_transfer_status
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr;
   XD_STRUCT_PTR                                xd_ptr;
   uint_8                                       status;
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_transfer_status");
   #endif
   
   /* Unlink the dTD */
   dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[2*ep_num + direction];

   if (dTD_ptr) {
      /* Get the transfer descriptor for the dTD */
      xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
      status = xd_ptr->BSTATUS;
   } else {
      status = USB_STATUS_IDLE;
   } /* Endif */
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_transfer_status, SUCCESSFUL");
   #endif

   return (status);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_transfer_details
*  Returned Value : pointer to structure that has details for transfer
*        Gets the status of a transfer
*
*END*-----------------------------------------------------------------*/
XD_STRUCT_PTR  _usb_dci_vusb20_get_transfer_details
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr, temp_dTD_ptr;
   XD_STRUCT_PTR                                xd_ptr;
   uint_32                                      temp, remaining_bytes;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;

   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   temp = (2*ep_num + direction);

   /* get a pointer to QH for this endpoint */   
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR + 
      temp;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_transfer_status");
   #endif
   
   /* Unlink the dTD */
   dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[2*ep_num + direction];

   if (dTD_ptr) 
   {
      
      /* Get the transfer descriptor for the dTD */
      xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
      if(!xd_ptr) return NULL;
      
      /* Initialize the transfer length field */
      xd_ptr->WSOFAR =0;
      remaining_bytes =0;
      
      /*if length of this transfer is greater than 20K
      we have multiple DTDs to count */
      if(xd_ptr->WTOTALLENGTH > VUSB_EP_MAX_LENGTH_TRANSFER)
      {
         /* it is a valid DTD. We should parse all DTDs for this XD
         and find the total bytes used so far */
         temp_dTD_ptr = dTD_ptr;
      
         /*loop through the list of DTDS until an active DTD is found
         or list has finished */
         while(!(dTD_ptr->NEXT_TR_ELEM_PTR & VUSBHS_TD_NEXT_TERMINATE))         
         {
            
            /**********************************************************
            If this DTD has been overlayed, we take the actual length 
            from QH.
            **********************************************************/

            if ((uint_32)(ep_queue_head_ptr->CURR_DTD_PTR & VUSBHS_TD_ADDR_MASK) ==
                  (uint_32)temp_dTD_ptr)
            {
                remaining_bytes += 
                  ((ep_queue_head_ptr->SIZE_IOC_INT_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
            }
            else
            {
               /* take the length from DTD itself */
                remaining_bytes += 
                  ((temp_dTD_ptr->SIZE_IOC_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
            }
   
            dTD_ptr = temp_dTD_ptr;
             
            /* Get the address of the next dTD */
            temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)
                  (temp_dTD_ptr->NEXT_TR_ELEM_PTR & VUSBHS_TD_ADDR_MASK);
         }
         xd_ptr->WSOFAR = xd_ptr->WTOTALLENGTH - remaining_bytes;

      }
      else
      {
         /*look at actual length from QH*/
         xd_ptr->WSOFAR = xd_ptr->WTOTALLENGTH - 
            ((ep_queue_head_ptr->SIZE_IOC_INT_STS & VUSB_EP_TR_PACKET_SIZE) >> 16);
         
      }
      
   } else {
      xd_ptr = NULL;
   } /* Endif */
   
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_transfer_status, SUCCESSFUL");
   #endif

   return (xd_ptr);

} /* EndBody */



/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        Cancels a transfer
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_cancel_transfer
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
     
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr, check_dTD_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
   XD_STRUCT_PTR                                xd_ptr;
   uint_32                                      temp, bit_pos;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_cancel_transfer");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   
   bit_pos = (1 << (16 * direction + ep_num));
   temp = (2*ep_num + direction);
   
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR + 
      temp;
   
   /* Unlink the dTD */
   dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];
   
   if (dTD_ptr) {
      check_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)((uint_32)dTD_ptr->NEXT_TR_ELEM_PTR & 
         VUSBHS_TD_ADDR_MASK);

      if (dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_STATUS_ACTIVE) {
         /* Flushing will halt the pipe */
         /* Write 1 to the Flush register */
         dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = bit_pos;

         /* Wait until flushing completed */
         while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH & bit_pos) {
            /* ENDPTFLUSH bit should be cleared to indicate this 
            ** operation is complete 
            */
         } /* EndWhile */
         
         while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & 
            bit_pos) 
         {
            /* Write 1 to the Flush register */
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = bit_pos;
         
            /* Wait until flushing completed */
            while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH & bit_pos) {
               /* ENDPTFLUSH bit should be cleared to indicate this 
               ** operation is complete 
               */
            } /* EndWhile */
         } /* EndWhile */
      } /* Endif */
      
      /* Retire the current dTD */
      dTD_ptr->SIZE_IOC_STS = 0;
      dTD_ptr->NEXT_TR_ELEM_PTR = VUSBHS_TD_NEXT_TERMINATE;
      
      /* The transfer descriptor for this dTD */
      xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
      dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
      /* Free the dTD */
      _usb_dci_vusb20_free_dTD((pointer)dTD_ptr);
      
      /* Update the dTD head and tail for specific endpoint/direction */
      if (!check_dTD_ptr) {
         usb_dev_ptr->EP_DTD_HEADS[temp] = NULL;
         usb_dev_ptr->EP_DTD_TAILS[temp] = NULL;
         if (xd_ptr) {
            xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
            /* Free the transfer descriptor */
            _usb_device_free_XD((pointer)xd_ptr);
         } /* Endif */
         /* No other transfers on the queue */
         ep_queue_head_ptr->NEXT_DTD_PTR = VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE;
         ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
      } else {
         usb_dev_ptr->EP_DTD_HEADS[temp] = check_dTD_ptr;
            
         if (xd_ptr) {
            if ((uint_32)check_dTD_ptr->SCRATCH_PTR->\
               XD_FOR_THIS_DTD != (uint_32)xd_ptr) 
            {
               xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
               /* Free the transfer descriptor */
               _usb_device_free_XD((pointer)xd_ptr);
            } /* Endif */
         } /* Endif */
         
         if (check_dTD_ptr->SIZE_IOC_STS & VUSBHS_TD_STATUS_ACTIVE) {
         
            /* Start CR 1015 */
            /* Prime the Endpoint */
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = bit_pos;

            if (!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & 
               bit_pos)) 
            {
               while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME & bit_pos) {
                  /* Wait for the ENDPTPRIME to go to zero */
               } /* EndWhile */

               if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS & 
                  bit_pos) 
               {
                  /* The endpoint was not not primed so no other transfers on 
                  ** the queue 
                  */
                  goto done;
               } /* Endif */
            } else {
               goto done;
            } /* Endif */

            /* No other transfers on the queue */
            ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)check_dTD_ptr;
            ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
   
            /* Prime the Endpoint */
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = bit_pos;
         } /* Endif */
      } /* Endif */
   } /* Endif */
   
done:
   
   return USB_OK;

   /* End CR 1015 */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_cancel_transfer, SUCCESSFUL");
   #endif
   
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_stall_endpoint
*  Returned Value : None
*  Comments       :
*        Stalls the specified endpoint
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_stall_endpoint
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_            ep_queue_head_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_stall_endpoint");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   
   /* Get the endpoint queue head address */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR + 
      2*ep_num + direction;
   
   /* Stall the endpoint for Rx or Tx and set the endpoint type */
   if (ep_queue_head_ptr->MAX_PKT_LENGTH & VUSB_EP_QUEUE_HEAD_IOS) {
      /* This is a control endpoint so STALL both directions */
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |= 
         (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
   } else {   
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |= 
         (direction ? EHCI_EPCTRL_TX_EP_STALL : 
         EHCI_EPCTRL_RX_EP_STALL);
   } /* Endif */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_stall_endpoint, SUCCESSFUL");
   #endif
      
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_unstall_endpoint
*  Returned Value : None
*  Comments       :
*        Unstall the specified endpoint in the specified direction
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_unstall_endpoint
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,
            
      /* [IN] the Endpoint number */
      uint_8                     ep_num,
            
      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_unstall_endpoint");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   
   /* Enable the endpoint for Rx or Tx and set the endpoint type */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] &= 
      (direction ? ~EHCI_EPCTRL_TX_EP_STALL : ~EHCI_EPCTRL_RX_EP_STALL);

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_unstall_endpoint, SUCCESSFUL");
   #endif
      
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_dci_vusb20_get_endpoint_status
* Returned Value : None
* Comments       :
*     Gets the endpoint status
* 
*END*--------------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_get_endpoint_status
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,
      
      /* [IN] Endpoint to get */
      uint_8               ep
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   uint_8                            ep_num = ep & 0x0f;
   uint_8                            ep_dir = ep & 0x80; 


   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_endpoint_status");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_get_endpoint_status, SUCCESSFUL");
   #endif
   
   return ((dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] & 
      (ep_dir ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL)) ? 1 : 0);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : _usb_dci_vusb20_set_endpoint_status
* Returned Value : None
* Comments       :
*     Sets the endpoint registers e.g. to enable TX, RX, control
* 
*END*--------------------------------------------------------------------*/
void _usb_dci_vusb20_set_endpoint_status
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,
      
      /* [IN] Endpoint to set */
      uint_8               ep,
      
      /* [IN] Endpoint characteristics */
      uint_8               stall
   )
{ /* Body */

   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_endpoint_status");
   #endif
   
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   if (stall) {
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep] |= 
         (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
   } else {
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep] &= 
         ~(EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
   } /* Endif */

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_set_endpoint_status, SUCCESSFUL");
   #endif
   
} /* EndBody */

#if 0
extern USB_DEV_STATE_STRUCT_PTR
_usb_dci_get_cur_device_handle()
{
	USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;

	usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)global_usb_device_state_struct_ptr;

   	return usb_dev_ptr;
}
#endif

