/*****************************************************************************
*
* (c) Copyright 2010-2013 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
*****************************************************************************/
/****************************************************************************/
/**
*
* @file xemacps_example_intr_dma.c
*
* Implements examples that utilize the EmacPs's interrupt driven DMA
* packet transfer mode to send and receive frames.
*
* These examples demonstrate:
*
* - How to perform simple send and receive.
* - Interrupt
* - Error handling
* - Device reset
*
* Functional guide to example:
*
* - EmacPsDmaSingleFrameIntrExample demonstrates the simplest way to send and
*   receive frames in in interrupt driven DMA mode.
*
* - EmacPsErrorHandler() demonstrates how to manage asynchronous errors.
*
* - EmacPsResetDevice() demonstrates how to reset the driver/HW without
*   loosing all configuration settings.
*
* NOTE:
* Define PEEP in xemacps_example.h to run the example on a PEEP board.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a wsy  01/10/10 First release
* 1.00a asa  11/25/11 The cache disable routines are removed. So now both
*		      I-cache and D-cache are enabled. The array RxBuffer is
*		      removed to avoid an extra copy from RxBuffer to RxFrame.
*		      Now the address of RxFrame is submitted to the Rx BD
*		      instead of the address of RxBuffer.
*		      In function EmacPsDmaSingleFrameIntrExample, RxBdPtr
*		      is made as a pointer instead of array of pointers. This
*		      is done since on the Rx path we now submit a single BD
*		      instead of all 32 BDs. Because of this change, relevant
*		      changes are made throughout the function
*		      EmacPsDmaSingleFrameIntrExample.
*		      Cache invalidation is now being done for the RxFrame
*		      buffer.
*		      The unnecessary cache flush (Xil_DCacheFlushRange) is
*		      removed. This was being done towards the end of the
*		      example which was unnecessary.
* 1.00a asa 01/24/12  Support for Zynq board is added. The SLCR divisors are
* 		      different for Zynq. Changes are made for the same.
* 		      Presently the SLCR GEM clock divisors are hard-coded
*		      assuming that IO PLL output frequency is 1000 MHz.
* 		      The BDs are allocated at the address 0xFF00000 and the
* 		      1 MB address range starting from this address is made
* 		      uncached. This is because, for GEM the BDs need to be
* 		      placed in uncached memory. The RX BDs are allocated at
* 		      address 0xFF00000 and TX BDs are allocated at address
* 		      0xFF10000.
* 		      The MDIO divisor used of 224 is used for Zynq board.
* 1.01a asa 02/27/12  The hardcoded SLCR divisors for Zynq are removed. The
*		      divisors are obtained from xparameters.h.c. The sleep
*		      values are reduced for Zynq. One sleep is added after
*		      MDIO divisor is set. Some of the prints are removed.
* 1.01a asa 03/14/12  The SLCR divisor support for ENET1 is added.
* 1.01a asa 04/15/12  The funcation calls to Xil_DisableMMU and Xil_EnableMMU
*		      are removed for setting the translation table
*		      attributes for the BD memory region.
* 1.05a asa 09/22/13 Cache handling is changed to fix an issue (CR#663885).
*			  The cache invalidation of the Rx frame is now moved to
*			  XEmacPsRecvHandler so that invalidation happens after the
*			  received data is available in the memory. The variable
*			  TxFrameLength is now made global.
*
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/
#include "emac_dma_main_functions.h"
#include "emac_dma_interrupt_headers.h"
/****************************************************************************/
/**
*
* This is the main function for the EmacPs example. This function is not
* included if the example is generated from the TestAppGen test tool.
*
* @param	None.
*
* @return	XST_SUCCESS to indicate success, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
#ifndef TESTAPP_GEN
int main(void)
{
	xil_printf("Entering into main() \r\n");
	int Status;
	XEmacPs_Bd *CurBd;
	/*
	 * Call the EmacPs DMA interrupt example , specify the parameters
	 * generated in xparameters.h
	 */
	Status = EmacPsInitial(&IntcInstance, &EmacPsInstance, EMACPS_DEVICE_ID, EMACPS_IRPT_INTR);

	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Failure initial");
		return XST_FAILURE;
	}
	/*
	 * Clear out receive packet memory area
	 */
	for(int i = 0; i < RXBD_CNT; i++)
	{
		EmacPsUtilFrameMemClear(&RxFrame[i]);
		Xil_DCacheFlushRange((u32)&RxFrame[i], sizeof(EthernetFrame));
	}
	/*
	 * Allocate RxBDs since we do not know how many BDs will be used
	 * in advance, use RXBD_CNT here.
	 */
	Status = XEmacPs_BdRingAlloc(&(XEmacPs_GetRxRing(&EmacPsInstance)), RXBD_CNT, &RxBdPtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error allocating RxBD");
		return XST_FAILURE;
	}
	/*
	 * Setup the BD. The XEmacPs_BdRingClone() call will mark the
	 * "wrap" field for last RxBD. Setup buffer address to associated
	 * BD.
	 */
	CurBd = RxBdPtr;
	for(int i = 0; i < RXBD_CNT-1; i++)
	{
		XEmacPs_BdClearLast(CurBd);
		XEmacPs_BdSetAddressRx(CurBd, (&RxFrame[i]));
		XEmacPs_BdClearRxNew(CurBd);
		CurBd = XEmacPs_BdRingNext(&(XEmacPs_GetRxRing(&EmacPsInstance)), CurBd);
	}
	XEmacPs_BdSetLast(CurBd);
	XEmacPs_BdSetAddressRx(CurBd, (&RxFrame[RXBD_CNT-1]));
	XEmacPs_BdClearRxNew(CurBd);
	XEmacPs_BdWrite((CurBd), XEMACPS_BD_ADDR_OFFSET, XEmacPs_BdRead((CurBd), XEMACPS_BD_ADDR_OFFSET) | XEMACPS_RXBUF_WRAP_MASK);// WRAP
	XEmacPs_SetQueuePtr(&EmacPsInstance, EmacPsInstance.RxBdRing.BaseBdAddr, 0, XEMACPS_RECV);
	/*
	 * Enqueue to HW
	 */
	Status = XEmacPs_BdRingToHw(&(XEmacPs_GetRxRing(&EmacPsInstance)), RXBD_CNT, RxBdPtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error XEmacPs_BdRingToHw ");
		return XST_FAILURE;
	}
//**********************DMA***************************************************************************************************************
	/* Initialize flags before start transfer test  */
	TxDone = 0;
	RxDone = 0;
	Error = 0;
//****************************************************************************************************************************************
// Start the device
	XEmacPs_Start(&EmacPsInstance);
	u8 Opcode_Request[2] = {0x00, 0x01};
	xEmacpsTxFrameARP(&EmacPsInstance, TxBdPtr, &Opcode_Request[0]);
	while(1)
	{

	}
	/*
	 * Disable the interrupts for the EmacPs device
	 */
	DisableIntrSystem(&IntcInstance, TX_INTR_ID, RX_INTR_ID, EMACPS_IRPT_INTR);
	/*
	 * Stop the device
	 */
	XEmacPs_Stop(&EmacPsInstance);
	return XST_SUCCESS;
}
#endif
