/*
 * emac_dma_interrupt_headers.c
 *
 *  Created on: 5 ???. 2021 ?.
 *      Author: admin
 */

#include "emac_dma_interrupt_headers.h"

/*****************************************************************************/
/*
*
* This function setups the interrupt system so interrupts can occur for the
* DMA, it assumes INTC component exists in the hardware system.
*
* @param	IntcInstancePtr is a pointer to the instance of the INTC.
* @param	AxiDmaPtr is a pointer to the instance of the DMA engine
* @param	TxIntrId is the TX channel Interrupt ID.
* @param	RxIntrId is the RX channel Interrupt ID.
*
* @return
*		- XST_SUCCESS if successful,
*		- XST_FAILURE.if not succesful
*
* @note		None.
*
******************************************************************************/

int SetupIntrSystem(INTC * IntcInstancePtr, XAxiDma * AxiDmaPtr, XEmacPs *EmacPsInstancePtr, u16 EmacPsIntrId, u16 TxIntrId, u16 RxIntrId)
{
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(AxiDmaPtr);
	XAxiDma_BdRing *RxRingPtr = XAxiDma_GetRxRing(AxiDmaPtr);
	int Status;
	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	XScuGic_SetPriorityTriggerType(IntcInstancePtr, TxIntrId, 0xA0, 0x3);

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, RxIntrId, 0xA0, 0x3);
	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, TxIntrId,
				(Xil_InterruptHandler)TxIntrHandler,
				TxRingPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	Status = XScuGic_Connect(IntcInstancePtr, RxIntrId,
				(Xil_InterruptHandler)RxIntrHandler,
				RxRingPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	Status = XScuGic_Connect(IntcInstancePtr, EmacPsIntrId,
			(Xil_InterruptHandler) XEmacPs_IntrHandler,
			(void *) EmacPsInstancePtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap ("Unable to connect ISR to interrupt controller");
		return XST_FAILURE;
	}

	XScuGic_Enable(IntcInstancePtr, TxIntrId);
	XScuGic_Enable(IntcInstancePtr, RxIntrId);
	XScuGic_Enable(IntcInstancePtr, EmacPsIntrId);
	/* Enable interrupts from the hardware */

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)INTC_HANDLER,
			(void *)IntcInstancePtr);

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function disables the interrupts for DMA engine.
*
* @param	IntcInstancePtr is the pointer to the INTC component instance
* @param	TxIntrId is interrupt ID associated w/ DMA TX channel
* @param	RxIntrId is interrupt ID associated w/ DMA RX channel
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void DisableIntrSystem(INTC * IntcInstancePtr, u16 TxIntrId, u16 RxIntrId,  u16 EmacPsIntrId)
{
	XScuGic_Disconnect(IntcInstancePtr, TxIntrId);
	XScuGic_Disconnect(IntcInstancePtr, RxIntrId);
	XScuGic_Disconnect(IntcInstancePtr, EmacPsIntrId);
}

/****************************************************************************/
/**
*
* This the Transmit handler callback function and will increment a shared
* counter that can be shared by the main thread of operation.
*
* @param	Callback is the pointer to the instance of the EmacPs device.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XEmacPsSendHandler(void *Callback)
{
	XEmacPs *EmacPsInstancePtr = (XEmacPs *) Callback;
	int Status;

	// Return the BD to the free list
	if (XEmacPs_BdRingFromHwTx(&(XEmacPs_GetTxRing(EmacPsInstancePtr)),
						1, &TxBdPtr) == 0) {
		EmacPsUtilErrorTrap("TxBDs were not ready for post processing - Handler");
	}

	Status = XEmacPs_BdRingFree(&(XEmacPs_GetTxRing(EmacPsInstancePtr)),
					 1, TxBdPtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error freeing up TxBDs - Handler");
	}
}


/****************************************************************************/
/**
*
* This is the Receive handler callback function and will increment a shared
* counter that can be shared by the main thread of operation.
*
* @param	Callback is a pointer to the instance of the EmacPs device.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XEmacPsRecvHandler(void *Callback)
{
	XEmacPs *EmacPsInstancePtr = (XEmacPs *) Callback;

	u8 Data_Rx[1500];
	u16 Length_Rx = 0;

	static int Sum_Len = 0;
	static int Rx_Function = 0;
	unsigned int BdIndex = 0;
	const u8 Opcode_Response[2] = {0x00, 0x02};
	XEmacPs_Bd *RxBdPtr_Local, *CurBd_Local, *RxBdPtr_Base;

    u32 gigeversion = ((Xil_In32(EmacPsInstancePtr->Config.BaseAddress+0xFC))>>16) & 0xFFF;

    /*
     *If Reception done interrupt is asserted,call RX call back function
     *to handle the processed BDs and then raise the according flag.
     */
    u32 regval = XEmacPs_ReadReg(EmacPsInstancePtr->Config.BaseAddress,XEMACPS_RXSR_OFFSET);
    XEmacPs_WriteReg(EmacPsInstancePtr->Config.BaseAddress,XEMACPS_RXSR_OFFSET,regval);

    if(gigeversion <=2) {
       resetrx_on_no_rxdata(EmacPsInstancePtr);
    }

	int NumRxBuf = XEmacPs_BdRingFromHwRx(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), RXBD_CNT, &RxBdPtr_Local);
	CurBd_Local = RxBdPtr_Local;
	for(int i = 0; i < NumRxBuf; i++)
	{
		BdIndex = XEMACPS_BD_TO_INDEX(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurBd_Local);
		int RxFrLen = XEmacPs_GetRxFrameSize(EmacPsInstancePtr, CurBd_Local);
		Sum_Len = Sum_Len + RxFrLen;
		Rx_Function = EmacpsRxFrameProcessing(&RxFrame[BdIndex], RxFrLen, &c_EmacPsMAC_Source[0], &IP_Source[0], &Data_Rx[0], &Length_Rx);
		switch(Rx_Function)
		{
		case NOT_PROCESSING:
			break;
		case ARP_REQUEST:
			xEmacpsTxFrameARP(EmacPsInstancePtr, TxBdPtr, &Opcode_Response[0]);
			break;
		case ARP_RESPONSE:
			break;
		case UDP_FRAME:
			xEmacpsTxFrameUDP(EmacPsInstancePtr, TxBdPtr, &Data_Rx[0], Length_Rx);
			SendPacketAxiDMA(&AxiDma, &Data_Rx[0], Length_Rx);
			break;
		}
		Xil_DCacheInvalidateRange((u32)&RxFrame[BdIndex], sizeof(EthernetFrame));
		Xil_DCacheFlushRange((u32)&RxFrame[BdIndex], sizeof(EthernetFrame));
		XEmacPs_BdClearRxNew(CurBd_Local);
		CurBd_Local = XEmacPs_BdRingNext(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurBd_Local);
	}
	XEmacPs_BdRingFree(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), NumRxBuf, RxBdPtr_Local);
	u32 FreeCount = EmacPsInstancePtr->RxBdRing.FreeCnt;
	XEmacPs_BdRingAlloc(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), FreeCount, &RxBdPtr_Base);

	CurBd_Local = RxBdPtr_Base;
	for(int i = 0; i < FreeCount; i++)
	{
		BdIndex = XEMACPS_BD_TO_INDEX(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurBd_Local);
		XEmacPs_BdClearRxNew(CurBd_Local);
		XEmacPs_BdSetAddressRx(CurBd_Local, &RxFrame[BdIndex]);
		CurBd_Local = XEmacPs_BdRingNext(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), CurBd_Local);
	}
	XEmacPs_BdRingToHw(&(XEmacPs_GetRxRing(EmacPsInstancePtr)), FreeCount, RxBdPtr_Base);
}
/****************************************************************************/
/**
*
* This is the Error handler callback function and this function increments
* the error counter so that the main thread knows the number of errors.
*
* @param	Callback is the callback function for the driver. This
*		parameter is not used in this example.
* @param	Direction is passed in from the driver specifying which
*		direction error has occurred.
* @param	ErrorWord is the status register value passed in.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XEmacPsErrorHandler(void *Callback, u8 Direction, u32 ErrorWord)
{
	XEmacPs *EmacPsInstancePtr = (XEmacPs *) Callback;

	/*
	 * Increment the counter so that main thread knows something
	 * happened. Reset the device and reallocate resources ...
	 */

	switch (Direction) {
	case XEMACPS_RECV:
		if (ErrorWord & XEMACPS_RXSR_HRESPNOK_MASK) {
			EmacPsUtilErrorTrap("Receive DMA error");
		}
		if (ErrorWord & XEMACPS_RXSR_RXOVR_MASK) {
			EmacPsUtilErrorTrap("Receive over run");
		}
		if (ErrorWord & XEMACPS_RXSR_BUFFNA_MASK) {
			EmacPsUtilErrorTrap("Receive buffer not available");
		}
		break;
	case XEMACPS_SEND:
		if (ErrorWord & XEMACPS_TXSR_HRESPNOK_MASK) {
			EmacPsUtilErrorTrap("Transmit DMA error");
		}
		if (ErrorWord & XEMACPS_TXSR_URUN_MASK) {
			EmacPsUtilErrorTrap("Transmit under run");
		}
		if (ErrorWord & XEMACPS_TXSR_BUFEXH_MASK) {
			EmacPsUtilErrorTrap("Transmit buffer exhausted");
		}
		if (ErrorWord & XEMACPS_TXSR_RXOVR_MASK) {
			EmacPsUtilErrorTrap("Transmit retry excessed limits");
		}
		if (ErrorWord & XEMACPS_TXSR_FRAMERX_MASK) {
			EmacPsUtilErrorTrap("Transmit collision");
		}
		if (ErrorWord & XEMACPS_TXSR_USEDREAD_MASK) {
			EmacPsUtilErrorTrap("Transmit buffer not available");
		}
		break;
	}
	EmacPsResetDevice(EmacPsInstancePtr);
}



//********************************************DMA***************************************************************************

/*****************************************************************************/
/*
*
* This is the DMA TX callback function to be called by TX interrupt handler.
* This function handles BDs finished by hardware.
*
* @param	TxRingPtr is a pointer to TX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void TxCallBack(XAxiDma_BdRing * TxRingPtr)
{
	int BdCount;
	u32 BdSts;
	XAxiDma_Bd *BdPtr;
	XAxiDma_Bd *BdCurPtr;
	int Status;
	int Index;

	/* Get all processed BDs from hardware */
	BdCount = XAxiDma_BdRingFromHw(TxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	/* Handle the BDs */
	BdCurPtr = BdPtr;
	for (Index = 0; Index < BdCount; Index++) {

		/*
		 * Check the status in each BD
		 * If error happens, the DMA engine will be halted after this
		 * BD processing stops.
		 */
		BdSts = XAxiDma_BdGetSts(BdCurPtr);
		if ((BdSts & XAXIDMA_BD_STS_ALL_ERR_MASK) || (!(BdSts & XAXIDMA_BD_STS_COMPLETE_MASK))) {
			Error = 1;
			break;
		}

		/*
		 * Here we don't need to do anything. But if a RTOS is being
		 * used, we may need to free the packet buffer attached to
		 * the processed BD
		 */

		/* Find the next processed BD */
		BdCurPtr = (XAxiDma_Bd *)XAxiDma_BdRingNext(TxRingPtr, BdCurPtr);
	}

	/* Free all processed BDs for future transmission */
	Status = XAxiDma_BdRingFree(TxRingPtr, BdCount, BdPtr);
	if (Status != XST_SUCCESS) {
		Error = 1;
	}

	if(!Error) {

		TxDone += BdCount;
	}
}

/*****************************************************************************/
/*
*
* This is the DMA TX Interrupt handler function.
*
* It gets the interrupt status from the hardware, acknowledges it, and if any
* error happens, it resets the hardware. Otherwise, if a completion interrupt
* presents, then it calls the callback function.
*
* @param	Callback is a pointer to TX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void TxIntrHandler(void *Callback)
{
	XAxiDma_BdRing *TxRingPtr = (XAxiDma_BdRing *) Callback;
	u32 IrqStatus;
	int TimeOut;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(TxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(TxRingPtr, IrqStatus);

	/* If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {

		return;
	}
	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		XAxiDma_BdRingDumpRegs(TxRingPtr);

		Error = 1;

		/*
		 * Reset should never fail for transmit channel
		 */
		XAxiDma_Reset(&AxiDma);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if (XAxiDma_ResetIsDone(&AxiDma)) {
				break;
			}

			TimeOut -= 1;
		}

		return;
	}
	/*
	 * If Transmit done interrupt is asserted, call TX call back function
	 * to handle the processed BDs and raise the according flag
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		TxCallBack(TxRingPtr);
	}
}

/*****************************************************************************/
/*
*
* This is the DMA RX callback function called by the RX interrupt handler.
* This function handles finished BDs by hardware, attaches new buffers to those
* BDs, and give them back to hardware to receive more incoming packets
*
* @param	RxRingPtr is a pointer to RX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RxCallBack(XAxiDma_BdRing * RxRingPtr)
{
	int BdCount;
	XAxiDma_Bd *BdPtr;
	XAxiDma_Bd *BdCurPtr;
	u32 BdSts;
	int Index;

	/* Get finished BDs from hardware */
	BdCount = XAxiDma_BdRingFromHw(RxRingPtr, XAXIDMA_ALL_BDS, &BdPtr);

	BdCurPtr = BdPtr;
	for (Index = 0; Index < BdCount; Index++) {

		/*
		 * Check the flags set by the hardware for status
		 * If error happens, processing stops, because the DMA engine
		 * is halted after this BD.
		 */
		BdSts = XAxiDma_BdGetSts(BdCurPtr);
		if ((BdSts & XAXIDMA_BD_STS_ALL_ERR_MASK) ||
		    (!(BdSts & XAXIDMA_BD_STS_COMPLETE_MASK))) {
			Error = 1;
			break;
		}

		/* Find the next processed BD */
		BdCurPtr = (XAxiDma_Bd *)XAxiDma_BdRingNext(RxRingPtr, BdCurPtr);
		RxDone += 1;
	}

}

/*****************************************************************************/
/*
*
* This is the DMA RX interrupt handler function
*
* It gets the interrupt status from the hardware, acknowledges it, and if any
* error happens, it resets the hardware. Otherwise, if a completion interrupt
* presents, then it calls the callback function.
*
* @param	Callback is a pointer to RX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void RxIntrHandler(void *Callback)
{
	XAxiDma_BdRing *RxRingPtr = (XAxiDma_BdRing *) Callback;
	u32 IrqStatus;
	int TimeOut;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(RxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(RxRingPtr, IrqStatus);

	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		XAxiDma_BdRingDumpRegs(RxRingPtr);

		Error = 1;

		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(&AxiDma);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if(XAxiDma_ResetIsDone(&AxiDma)) {
				break;
			}

			TimeOut -= 1;
		}

		return;
	}

	/*
	 * If completion interrupt is asserted, call RX call back function
	 * to handle the processed BDs and then raise the according flag.
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		RxCallBack(RxRingPtr);
	}
}
