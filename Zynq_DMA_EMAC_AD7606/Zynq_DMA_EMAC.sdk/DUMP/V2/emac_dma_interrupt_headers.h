/*
 * emac_dma_interrupt_headers.h
 *
 *  Created on: 5 èþë. 2021 ã.
 *      Author: admin
 */

#ifndef EMAC_DMA_INTERRUPT_HEADERS_H_
#define EMAC_DMA_INTERRUPT_HEADERS_H_

#include "emac_dma_main_functions.h"

#define RX_INTR_ID		XPAR_FABRIC_AXIDMA_0_S2MM_INTROUT_VEC_ID
#define TX_INTR_ID		XPAR_FABRIC_AXIDMA_0_MM2S_INTROUT_VEC_ID
#define INTC_DEVICE_ID          XPAR_SCUGIC_SINGLE_DEVICE_ID
#define INTC		XScuGic
#define INTC_HANDLER	XScuGic_InterruptHandler
static INTC IntcInstance;	/* Instance of the Interrupt Controller */

void TxCallBack(XAxiDma_BdRing * TxRingPtr);
void TxIntrHandler(void *Callback);
void RxCallBack(XAxiDma_BdRing * RxRingPtr);
void RxIntrHandler(void *Callback);

void XEmacPsSendHandler(void *Callback);
void XEmacPsRecvHandler(void *Callback);
void XEmacPsErrorHandler(void *Callback, u8 direction, u32 word);


int SetupIntrSystem(INTC * IntcInstancePtr, XAxiDma * AxiDmaPtr, XEmacPs *EmacPsInstancePtr, u16 EmacPsIntrId, u16 TxIntrId, u16 RxIntrId);
void DisableIntrSystem(INTC * IntcInstancePtr, u16 TxIntrId, u16 RxIntrId,  u16 EmacPsIntrId);


#endif /* EMAC_DMA_INTERRUPT_HEADERS_H_ */
