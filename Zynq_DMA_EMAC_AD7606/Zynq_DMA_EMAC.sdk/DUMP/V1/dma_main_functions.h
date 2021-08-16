/*
 * dma_main_functions.h
 *
 *  Created on: 5 èþë. 2021 ã.
 *      Author: admin
 */

#ifndef DMA_MAIN_FUNCTIONS_H_
#define DMA_MAIN_FUNCTIONS_H_


#include "xaxidma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xdebug.h"

#ifndef DEBUG
extern void xil_printf(const char *format, ...);
#endif

#include "xscugic.h"

/******************** Constant Definitions **********************************/
/*
 * Device hardware build related constants.
 */

#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID
#ifndef DDR_BASE_ADDR
#warning CHECK FOR THE VALID DDR ADDRESS IN XPARAMETERS.H, DEFAULT SET TO 0x01000000
#endif
#define MEM_BASE_ADDR		0x01000000

#define RX_INTR_ID		XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID
#define TX_INTR_ID		XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID

#define RX_BD_SPACE_BASE	(MEM_BASE_ADDR)
#define RX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x0000FFFF)
#define TX_BD_SPACE_BASE	(MEM_BASE_ADDR + 0x00010000)
#define TX_BD_SPACE_HIGH	(MEM_BASE_ADDR + 0x0001FFFF)
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define RX_BUFFER_HIGH		(MEM_BASE_ADDR + 0x004FFFFF)

#define INTC_DEVICE_ID          XPAR_SCUGIC_SINGLE_DEVICE_ID

/* Timeout loop counter for reset
 */
#define RESET_TIMEOUT_COUNTER	10000

/*
 * Buffer and Buffer Descriptor related constant definition
 */
#define MAX_PKT_LEN		0x100
#define MARK_UNCACHEABLE        0x701

/*
 * Number of BDs in the transfer example
 * We show how to submit multiple BDs for one transmit.
 * The receive side gets one completion per transfer.
 */
#define NUMBER_OF_BDS_PER_PKT		12
#define NUMBER_OF_PKTS_TO_TRANSFER 	11
#define NUMBER_OF_BDS_TO_TRANSFER	(NUMBER_OF_PKTS_TO_TRANSFER * \
						NUMBER_OF_BDS_PER_PKT)

/* The interrupt coalescing threshold and delay timer threshold
 * Valid range is 1 to 255
 *
 * We set the coalescing threshold to be the total number of packets.
 * The receive side will only get one completion interrupt for this example.
 */
#define COALESCING_COUNT		NUMBER_OF_PKTS_TO_TRANSFER
#define DELAY_TIMER_COUNT		100

#define INTC		XScuGic
#define INTC_HANDLER	XScuGic_InterruptHandler
/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

int CheckData(int Length, u8 StartValue);
void TxCallBack(XAxiDma_BdRing * TxRingPtr);
void TxIntrHandler(void *Callback);
void RxCallBack(XAxiDma_BdRing * RxRingPtr);
void RxIntrHandler(void *Callback);



int SetupIntrSystem(INTC * IntcInstancePtr,
		   XAxiDma * AxiDmaPtr, u16 TxIntrId, u16 RxIntrId);
void DisableIntrSystem(INTC * IntcInstancePtr,
				u16 TxIntrId, u16 RxIntrId);

int RxSetup(XAxiDma * AxiDmaInstPtr);
int TxSetup(XAxiDma * AxiDmaInstPtr);
int SendPacket(XAxiDma * AxiDmaInstPtr);

/************************** Variable Definitions *****************************/
/*
 * Device instance definitions
 */
XAxiDma AxiDma;


static INTC Intc;	/* Instance of the Interrupt Controller */
/*
 * Flags interrupt handlers use to notify the application context the events.
 */
volatile int TxDone;
volatile int RxDone;
volatile int Error;
/*
 * Buffer for transmit packet. Must be 32-bit aligned to be used by DMA.
 */

#endif /* DMA_MAIN_FUNCTIONS_H_ */
