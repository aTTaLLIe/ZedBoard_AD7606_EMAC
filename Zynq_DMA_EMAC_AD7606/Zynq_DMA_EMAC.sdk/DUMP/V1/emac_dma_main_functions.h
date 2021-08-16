/*
 * emac_dma_main_functions.h
 *
 *  Created on: 05.07.2021
 *      Author: Jaroslav K.
 */

#ifndef EMAC_DMA_MAIN_FUNCTIONS_H_
#define EMAC_DMA_MAIN_FUNCTIONS_H_

#include "xemacps_example.h"
#include "xil_exception.h"
#include "xil_mmu.h"


/*************************** Constant Definitions ***************************/
#define EMACPS_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM0_INT_ID

#define RXBD_CNT       32	/* Number of RxBDs to use */
#define TXBD_CNT       32	/* Number of TxBDs to use */

/*
 * SLCR setting
 */
#define SLCR_LOCK_ADDR			(XPS_SYS_CTRL_BASEADDR + 0x4)
#define SLCR_UNLOCK_ADDR		(XPS_SYS_CTRL_BASEADDR + 0x8)
#define SLCR_GEM0_CLK_CTRL_ADDR		(XPS_SYS_CTRL_BASEADDR + 0x140)
#define SLCR_GEM1_CLK_CTRL_ADDR		(XPS_SYS_CTRL_BASEADDR + 0x144)

#define SLCR_LOCK_KEY_VALUE		0x767B
#define SLCR_UNLOCK_KEY_VALUE		0xDF0D
#define SLCR_ADDR_GEM_RST_CTRL		(XPS_SYS_CTRL_BASEADDR + 0x214)
/*************************** Variable Definitions ***************************/
/*
 * Buffer descriptors are allocated in uncached memory. The memory is made
 * uncached by setting the attributes appropriately in the MMU table.
 */
#define RX_BD_LIST_START_ADDRESS	0x0FF00000
#define TX_BD_LIST_START_ADDRESS	0x0FF10000

#define FIRST_FRAGMENT_SIZE 64
#define XEMACPS_BD_TO_INDEX(ringptr, bdptr)(((u32)bdptr - (u32)(ringptr)->BaseBdAddr) / (ringptr)->Separation)
/*
 * Counters to be incremented by callbacks
 */

XEmacPs EmacPsInstance;	/* XEmacPs instance used throughout examples */
static XScuGic IntcInstance;
EthernetFrame TxFrame;		/* Transmit buffer */
EthernetFrame RxFrame[RXBD_CNT];		/* Receive buffer */
XEmacPs_Bd *TxBdPtr, *RxBdPtr;
u32 TxFrameLength;

//typedef struct ethernet_header
//{
//	u_char  dest[6];
//	u_char  source[6];
//	u_short type;
//    u_char  data[2000];
//}   RAW_ETH_FRAME;


int EmacPsInitial(XScuGic * IntcInstancePtr, XEmacPs * EmacPsInstancePtr, u16 EmacPsDeviceId, u16 EmacPsIntrId);
int send_test(XEmacPs *EmacPsInstancePtr);
int main_test(XEmacPs * EmacPsInstancePtr);
/*
 * Interrupt setup and Callbacks for examples
 */
int EmacPsSetupIntrSystem(XScuGic * IntcInstancePtr, XEmacPs * EmacPsInstancePtr, u16 EmacPsIntrId);
void EmacPsDisableIntrSystem(XScuGic * IntcInstancePtr, u16 EmacPsIntrId);

void XEmacPsSendHandler(void *Callback);
void XEmacPsRecvHandler(void *Callback);
void XEmacPsErrorHandler(void *Callback, u8 direction, u32 word);

int EmacPsResetDevice(XEmacPs * EmacPsInstancePtr);
void XEmacPs_SetMdioDivisor(XEmacPs *InstancePtr, XEmacPs_MdcDiv Divisor);

/*************************** Function Prototypes ****************************/
#endif /* EMAC_DMA_MAIN_FUNCTIONS_H_ */
