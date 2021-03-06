/*
 * emac_dma_main_functions.h
 *
 *  Created on: 05.07.2021
 *      Author: Jaroslav K.
 */

#ifndef EMAC_DMA_MAIN_FUNCTIONS_H_
#define EMAC_DMA_MAIN_FUNCTIONS_H_

#include "xemacps_lockal_utils.h"
#include "xil_exception.h"
#include "xil_mmu.h"


#include "xaxidma.h"
#include "xparameters.h"
#include "xdebug.h"
#include "xscugic.h"
//#ifndef DEBUG
//extern void xil_printf(const char *format, ...);
//#endif



/*************************** Constant Definitions ***************************/
#define EMACPS_DEVICE_ID	XPAR_XEMACPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define EMACPS_IRPT_INTR	XPS_GEM0_INT_ID

#define RXBD_CNT       32	/* Number of RxBDs to use */
#define TXBD_CNT       64	/* Number of TxBDs to use */

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
//DMA
#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID
#ifndef DDR_BASE_ADDR
#warning CHECK FOR THE VALID DDR ADDRESS IN XPARAMETERS.H, DEFAULT SET TO 0x01000000
#endif
#define MEM_BASE_ADDR		0x01000000
#define RX_BD_SPACE_BASE	(MEM_BASE_ADDR)
#define RX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x0000FFFF)
#define TX_BD_SPACE_BASE	(MEM_BASE_ADDR + 0x00010000)
#define TX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x0001FFFF)
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x004FFFFF)
/*************************** Variable Definitions ***************************/
/*
 * Buffer descriptors are allocated in uncached memory. The memory is made
 * uncached by setting the attributes appropriately in the MMU table.
 */
#define RX_BD_LIST_START_ADDRESS	0x0FF00000
#define TX_BD_LIST_START_ADDRESS	0x0FF10000
#define FIRST_FRAGMENT_SIZE 64
#define XEMACPS_BD_TO_INDEX(ringptr, bdptr)(((u32)bdptr - (u32)(ringptr)->BaseBdAddr) / (ringptr)->Separation)

#define RESET_TIMEOUT_COUNTER	10000 /* Timeout loop counter for reset*/
/*
 * Buffer and Buffer Descriptor related constant definition
 */
#define MAX_PKT_LEN		0x578
#define MARK_UNCACHEABLE        0x701
/*
 * Number of BDs in the transfer example
 * We show how to submit multiple BDs for one transmit.
 * The receive side gets one completion per transfer.
 */
#define NUMBER_OF_BDS_PER_PKT		12
#define NUMBER_OF_PKTS_TO_TRANSFER 	6
#define NUMBER_OF_BDS_TO_TRANSFER	(NUMBER_OF_PKTS_TO_TRANSFER * NUMBER_OF_BDS_PER_PKT)
/* The interrupt coalescing threshold and delay timer threshold
 * Valid range is 1 to 255
 * We set the coalescing threshold to be the total number of packets.
 * The receive side will only get one completion interrupt for this example.
 */
#define COALESCING_COUNT		NUMBER_OF_PKTS_TO_TRANSFER
#define DELAY_TIMER_COUNT		100

XEmacPs EmacPsInstance;	/* XEmacPs instance used throughout*/
XAxiDma AxiDma; /*Device instance definitions*/
EthernetFrame TxFrame[TXBD_CNT];		/* Transmit buffer */
EthernetFrame RxFrame[RXBD_CNT];		/* Receive buffer */
XEmacPs_Bd *TxBdPtr, *RxBdPtr;
u32 TxFrameLength;

volatile int TxDone;
volatile int RxDone;
volatile int Error;
//typedef struct ethernet_header
//{
//	u_char  dest[6];
//	u_char  source[6];
//	u_short type;
//    u_char  data[2000];
//}   RAW_ETH_FRAME;


int EmacPsInitial(XScuGic * IntcInstancePtr, XEmacPs * EmacPsInstancePtr, u16 EmacPsDeviceId, u16 EmacPsIntrId);
int EmacPsResetDevice(XEmacPs * EmacPsInstancePtr);
void XEmacPs_SetMdioDivisor(XEmacPs *InstancePtr, XEmacPs_MdcDiv Divisor);

int CheckData(int Length, u8 StartValue);
int RxSetup(XAxiDma * AxiDmaInstPtr);
int TxSetup(XAxiDma * AxiDmaInstPtr);
int SendPacketAxiDMA(XAxiDma * AxiDmaInstPtr, u8 *Data, u16 LengthData);

/*************************** Function Prototypes ****************************/
#endif /* EMAC_DMA_MAIN_FUNCTIONS_H_ */
