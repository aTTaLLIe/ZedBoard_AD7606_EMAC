/*****************************************************************************
*
* (c) Copyright 2010-13 Xilinx, Inc. All rights reserved.
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
* @file xemacps_example_util.c
*
* This file implements the utility functions for the XEmacPs example code.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a wsy  01/10/10 First release
* 1.00a asa  01/24/12 A new version of EmacPsUtilEnterLoopback is used for
* 		      Zynq boards. Now there are two versions of
* 		      EmacPsUtilEnterLoopback, one for PEEP and one for Zynq.
* 		      If the example is to be run  on a PEEP board, define PEEP
* 		      in xemacps_example.h.
* 1.01a asa  02/27/12 The sleep value after PHY loopback is setup is reduced
*		      for Zynq.
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/

#include "xemacps_example.h"
#include "sleep.h"

/************************** Variable Definitions ****************************/

/*
 * Local MAC address
 */
u8 c_EmacPsMAC_Zynq[6] = { 0x00, 0x0a, 0x35, 0x01, 0x02, 0x03 };
//char c_EmacPsMAC_Source[] = { 0x70, 0x85, 0xc2, 0x45, 0xf4, 0xcd };
u8 c_EmacPsMAC_Source[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

u8 IP_Source[4] = {0xc0, 0xac, 0x00, 0x14};
const u8 IP_Zynq[4] ={0xc0, 0xac, 0x00, 0x50};
const u8 Port_Zynq[2] = {0x04, 0x00};
u8 Port_Answer[2];
/****************************************************************************/
/**
*
* Set the MAC addresses in the frame.
*
* @param    FramePtr is the pointer to the frame.
* @param    DestAddr is the Destination MAC address.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
void EmacPsUtilFrameHdrFormatMAC(EthernetFrame * FramePtr, u8 *DestAddr, u8 *SoursAddr)
{
	char *Frame = (char *) FramePtr;
	int Index;

	/* Destination address */
	for (Index = 0; Index < XEMACPS_MAC_ADDR_SIZE; Index++) {
		*Frame++ = *DestAddr++;
	}

	/* Source address */
	for (Index = 0; Index < XEMACPS_MAC_ADDR_SIZE; Index++) {
		*Frame++ = *SoursAddr++;
	}
}

/****************************************************************************/
/**
*
* Set the frame type for the specified frame.
*
* @param    FramePtr is the pointer to the frame.
* @param    FrameType is the Type to set in frame.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
void EmacPsUtilFrameHdrFormatType(EthernetFrame * FramePtr, u16 FrameType)
{
	char *Frame = (char *) FramePtr;

	/*
	 * Increment to type field
	 */
	Frame = Frame + 12;
	/*
	 * Do endian swap from little to big-endian.
	 */
	FrameType = Xil_EndianSwap16(FrameType);
	/*
	 * Set the type
	 */
	*(u16 *) Frame = FrameType;
}

/****************************************************************************/
/**
* This function places a pattern in the payload section of a frame. The pattern
* is a  8 bit incrementing series of numbers starting with 0.
* Once the pattern reaches 256, then the pattern changes to a 16 bit
* incrementing pattern:
* <pre>
*   0, 1, 2, ... 254, 255, 00, 00, 00, 01, 00, 02, ...
* </pre>
*
* @param    FramePtr is a pointer to the frame to change.
* @param    PayloadSize is the number of bytes in the payload that will be set.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
void EmacPsUtilFrameSetPayloadData(EthernetFrame * FramePtr, int PayloadSize)
{
	unsigned BytesLeft = PayloadSize;
	u8 *Frame;
	u16 Counter = 0;

	/*
	 * Set the frame pointer to the start of the payload area
	 */
	Frame = (u8 *) FramePtr + XEMACPS_HDR_SIZE;

	/*
	 * Insert 8 bit incrementing pattern
	 */
	while (BytesLeft && (Counter < 256)) {
		*Frame++ = (u8) Counter++;
		BytesLeft--;
	}

	/*
	 * Switch to 16 bit incrementing pattern
	 */
	while (BytesLeft) {
		*Frame++ = (u8) (Counter >> 8);	/* high */
		BytesLeft--;

		if (!BytesLeft)
			break;

		*Frame++ = (u8) Counter++;	/* low */
		BytesLeft--;
	}
}

/****************************************************************************/
/**
* This function verifies the frame data against a CheckFrame.
*
* Validation occurs by comparing the ActualFrame to the header of the
* CheckFrame. If the headers match, then the payload of ActualFrame is
* verified for the same pattern Util_FrameSetPayloadData() generates.
*
* @param    CheckFrame is a pointer to a frame containing the 14 byte header
*           that should be present in the ActualFrame parameter.
* @param    ActualFrame is a pointer to a frame to validate.
*
* @return   XST_SUCCESS if successful, else XST_FAILURE.
*
* @note     None.
*****************************************************************************/
int EmacPsUtilFrameVerify(EthernetFrame * CheckFrame,
			   EthernetFrame * ActualFrame)
{
	char *CheckPtr = (char *) CheckFrame;
	char *ActualPtr = (char *) ActualFrame;
	u16 BytesLeft;
	u16 Counter;
	int Index;

	/*
	 * Compare the headers
	 */
	for (Index = 0; Index < XEMACPS_HDR_SIZE; Index++) {
		if (CheckPtr[Index] != ActualPtr[Index]) {
			return XST_FAILURE;
		}
	}

	/*
	 * Get the length of the payload
	 */
	BytesLeft = *(u16 *) &ActualPtr[12];
	/*
	 * Do endian swap from big back to little-endian.
	 */
	BytesLeft = Xil_EndianSwap16(BytesLeft);
	/*
	 * Validate the payload
	 */
	Counter = 0;
	ActualPtr = &ActualPtr[14];

	/*
	 * Check 8 bit incrementing pattern
	 */
	while (BytesLeft && (Counter < 256)) {
		if (*ActualPtr++ != (char) Counter++) {
			return XST_FAILURE;
		}
		BytesLeft--;
	}

	/*
	 * Check 16 bit incrementing pattern
	 */
	while (BytesLeft) {
		if (*ActualPtr++ != (char) (Counter >> 8)) {	/* high */
			return XST_FAILURE;
		}

		BytesLeft--;

		if (!BytesLeft)
			break;

		if (*ActualPtr++ != (char) Counter++) {	/* low */
			return XST_FAILURE;
		}

		BytesLeft--;
	}

	return XST_SUCCESS;
}

/****************************************************************************/
/**
* This function sets all bytes of a frame to 0.
*
* @param    FramePtr is a pointer to the frame itself.
*
* @return   None.
*
* @note     None.
*
*****************************************************************************/
void EmacPsUtilFrameMemClear(EthernetFrame * FramePtr)
{
	u32 *Data32Ptr = (u32 *) FramePtr;
	u32 WordsLeft = sizeof(EthernetFrame) / sizeof(u32);

	/* frame should be an integral number of words */
	while (WordsLeft--) {
		*Data32Ptr++ = 0xDEADBEEF;
	}
}


/****************************************************************************/
/**
*
* This function copys data from source to desitnation for n bytes.
*
* @param    Destination is the targeted string to copy to.
* @param    Source is the source string to copy from.
* @param    n is number of bytes to be copied.
*
* @note     This function is similiar to strncpy(), however strncpy will
*           stop either at null byte or n bytes is been copied.
*           This function will copy n bytes without checking the content.
*
*****************************************************************************/
void EmacPsUtilstrncpy(char *Destination, const char *Source, u32 n)
{
	do {
		*Destination++ = *Source++;
	} while (--n != 0);
}


/****************************************************************************/
/**
*
* This function sets the emacps to loopback mode.
*
* @param    EmacPsInstancePtr is the XEmacPs driver instance.
*
* @note     None.
*
*****************************************************************************/
void EmacPsUtilEnterLocalLoopback(XEmacPs * EmacPsInstancePtr)
{
	u32 reg;

	reg = XEmacPs_ReadReg(EmacPsInstancePtr->Config.BaseAddress,
				XEMACPS_NWCTRL_OFFSET);
	XEmacPs_WriteReg(EmacPsInstancePtr->Config.BaseAddress,
			   XEMACPS_NWCTRL_OFFSET,
			   reg | XEMACPS_NWCTRL_LOOPEN_MASK);
}


/****************************************************************************/
/**
*
* This function detects the PHY address by looking for successful MII status
* register contents.
*
* @param    The XEMACPS driver instance
*
* @return   The address of the PHY (defaults to 32 if none detected)
*
* @note     None.
*
*****************************************************************************/
#define PHY_DETECT_REG1 2
#define PHY_DETECT_REG2 3

u32 XEmacPsDetectPHY(XEmacPs * EmacPsInstancePtr)
{
	u32 PhyAddr;
	int Status;
	u16 PhyReg1;
	u16 PhyReg2;

	for (PhyAddr = 0; PhyAddr <= 31; PhyAddr++) {
		Status = XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr,
					  PHY_DETECT_REG1, &PhyReg1);

		Status |= XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr,
					   PHY_DETECT_REG2, &PhyReg2);

		if ((Status == XST_SUCCESS) &&
		    (PhyReg1 > 0x0000) && (PhyReg1 < 0xffff) &&
		    (PhyReg2 > 0x0000) && (PhyReg2 < 0xffff)) {
			/* Found a valid PHY address */
			return PhyAddr;
		}
	}

	return PhyAddr;		/* default to 32(max of iteration) */
}


/****************************************************************************/
/**
*
* This function sets the PHY to loopback mode.
*
* @param    The XEMACPS driver instance
* @param    Speed is the loopback speed 10/100 Mbit.
*
* @return   XST_SUCCESS if successful, else XST_FAILURE.
*
* @note     None.
*
*****************************************************************************/
#ifdef PEEP /* Define PEEP in xemacps_example.h if the example is run on PEEP*/
#define PHY_REG0_RESET    		0x8000
#define PHY_REG0_LOOPBACK 		0x4000
#define PHY_REG0_10       		0x0100
#define PHY_REG0_100      		0x2100
#define PHY_REG0_1000     		0x0140
#define PHY_R20_DFT_SPD_MASK 		0x0070
#define PHY_R20_DFT_SPD_10   		0x0040
#define PHY_R20_DFT_SPD_100  		0x0050
#define PHY_R20_DFT_SPD_1000 		0x0060
int EmacPsUtilEnterLoopback(XEmacPs * EmacPsInstancePtr, int Speed)
{
	int Status;
	u16 PhyReg0  = 0;
	u16 PhyReg20 = 0;
	u32 PhyAddr;

	/* Detect the PHY address */
	PhyAddr = XEmacPsDetectPHY(EmacPsInstancePtr);
	if (PhyAddr >= 32) {
		EmacPsUtilErrorTrap("Error detect phy");
		return XST_FAILURE;
	}


	Status  = XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 20, &PhyReg20);
	Status  |= XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 0, &PhyReg0);
	PhyReg20 &= ~PHY_R20_DFT_SPD_MASK;

	switch (Speed) {
	case 10:
		PhyReg20 |= PHY_R20_DFT_SPD_10;
		break;
	case 100:
		PhyReg20 |= PHY_R20_DFT_SPD_100;
		break;
	case 1000:
		PhyReg20 |= PHY_R20_DFT_SPD_1000;
		break;
	default:
		EmacPsUtilErrorTrap("Error: speed not recognized ");
		return XST_FAILURE;
	}

	Status  |= XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 20, PhyReg20);
	Status  |= XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 0,
						PhyReg0 | PHY_REG0_RESET);

	/* setup speed and duplex */
	switch (Speed) {
	case 10:
		PhyReg0 = PHY_REG0_10;
		break;
	case 100:
		PhyReg0 = PHY_REG0_100;
		break;
	case 1000:
		PhyReg0 = PHY_REG0_1000;
		break;
	default:
		EmacPsUtilErrorTrap("Error: speed not recognized ");
		return XST_FAILURE;
	}
	Status |= XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 0, PhyReg0);
	Status  |= XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 0,
						(PhyReg0 | PHY_REG0_RESET));
	sleep(1);
	Status |= XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 0, &PhyReg0);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error setup phy speed");
		return XST_FAILURE;
	}

	/* enable loopback */
	PhyReg0 |= PHY_REG0_LOOPBACK;
	Status = XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 0, PhyReg0);

	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error setup phy loopback");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

#else /*For Zynq board*/

#define PHY_REG0_RESET    0x8000
#define PHY_REG0_LOOPBACK 0x4000
#define PHY_REG0_10       0x0100
#define PHY_REG0_100      0x2100
#define PHY_REG0_1000     0x0140
#define PHY_REG21_10      0x0030
#define PHY_REG21_100     0x2030
#define PHY_REG21_1000    0x0070

int EmacPsUtilEnterLoopback(XEmacPs * EmacPsInstancePtr, int Speed)
{
	int Status;
	u16 PhyReg0  = 0;
	u16 PhyReg21  = 0;
	u16 PhyReg22  = 0;
	u32 PhyAddr;

	/*
	 * Detect the PHY address
	 */
	PhyAddr = XEmacPsDetectPHY(EmacPsInstancePtr);

	if (PhyAddr >= 32) {
		EmacPsUtilErrorTrap("Error detect phy");
		return XST_FAILURE;
	}

	/*
	 * Setup speed and duplex
	 */
	switch (Speed) {
	case 10:
		PhyReg0 |= PHY_REG0_10;
		PhyReg21 |= PHY_REG21_10;
		break;
	case 100:
		PhyReg0 |= PHY_REG0_100;
		PhyReg21 |= PHY_REG21_100;
		break;
	case 1000:
		PhyReg0 |= PHY_REG0_1000;
		PhyReg21 |= PHY_REG21_1000;
		break;
	default:
		EmacPsUtilErrorTrap("Error: speed not recognized ");
		return XST_FAILURE;
	}

	Status = XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 0, PhyReg0);
	/*
	 * Make sure new configuration is in effect
	 */
	Status = XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 0, &PhyReg0);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error setup phy speed");
		return XST_FAILURE;
	}

	/*
	 * Switching to PAGE2
	 */
	PhyReg22 = 0x2;
	Status = XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 22, PhyReg22);

	/*
	 * Adding Tx and Rx delay. Configuring loopback speed.
	 */
	Status = XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 21, PhyReg21);
	/*
	 * Make sure new configuration is in effect
	 */
	Status = XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 21, &PhyReg21);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error setting Reg 21 in Page 2");
		return XST_FAILURE;
	}
	/*
	 * Switching to PAGE0
	 */
	PhyReg22 = 0x0;
	Status = XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 22, PhyReg22);

	/*
	 * Issue a reset to phy
	 */
	Status  = XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 0, &PhyReg0);
	PhyReg0 |= PHY_REG0_RESET;
	Status = XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 0, PhyReg0);

	Status = XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 0, &PhyReg0);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error reset phy");
		return XST_FAILURE;
	}

	/*
	 * Enable loopback
	 */
	PhyReg0 |= PHY_REG0_LOOPBACK;
	Status = XEmacPs_PhyWrite(EmacPsInstancePtr, PhyAddr, 0, PhyReg0);

	Status = XEmacPs_PhyRead(EmacPsInstancePtr, PhyAddr, 0, &PhyReg0);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error setup phy loopback");
		return XST_FAILURE;
	}

	/*
	 * Delay loop
	 */
	sleep(1);

	return XST_SUCCESS;
}
#endif /*PEEP*/

/****************************************************************************/
/**
*
* This function is called by example code when an error is detected. It
* can be set as a breakpoint with a debugger or it can be used to print out
* the given message if there is a UART or STDIO device.
*
* @param    Message is the text explaining the error
*
* @return   None
*
* @note     None
*
*****************************************************************************/
void EmacPsUtilErrorTrap(const char *Message)
{
	static int Count = 0;

	Count++;

#ifdef STDOUT_BASEADDRESS
	xil_printf("%s\r\n", Message);
#else
	(void) Message;
#endif
}

/******************************************************************************/
u16 CalculateChecSum(u8 *ProccesArray, u16 Length)
{
	u32 BufferCheckSum = *ProccesArray;
	BufferCheckSum = BufferCheckSum << 8;
	BufferCheckSum = BufferCheckSum + *(ProccesArray+1);

	for(int i = 2; i < Length; i=i+2)
	{
		u16 SecondNum  = *(ProccesArray+i);
		SecondNum = SecondNum << 8;
		SecondNum = SecondNum + *(ProccesArray+i+1);

		BufferCheckSum = BufferCheckSum + SecondNum;
		if(BufferCheckSum > 0xFFFF)
		{
			BufferCheckSum = BufferCheckSum & 0x0000FFFF;
			BufferCheckSum = BufferCheckSum + 1;
		}
	}
	return ~(u16)BufferCheckSum;
}
/*******************************************************************************
 * This function add IPv4 lay to TxFrame:
 * 1 byte - 0100 .... = Version: 4
 * .... 0101 = Header Length: 20 bytes (5)
 * 1 byte - Differentiated Services Field
 * 2 byte - Total Length
 * 2 byte - Identification
 * 2 byte - Fragment Offset: 0
 * 1 byte - Time to Live
 * 1 byte - Protocol: UDP (17)
 * 2 byte - Header Checksum
 * 4 byte - Source Address
 * 4 byte - Destination Address
********************************************************************************/
void EmacPsUtilFrameSetIPHeader(EthernetFrame *FramePtr, u16 TotalLangth, u16 Identificator_Frame, const u8 *IP_Source, const u8 *IP_Destination)
{
	u8 *Frame = (u8 *) FramePtr + 14;

	u8 FirstPartHeader[] = {0x45, 0x00, (u8) (TotalLangth >> 8), (u8) TotalLangth, (u8) (Identificator_Frame >> 8), (u8) Identificator_Frame, 0x00, 0x00, 0x80, 0x11};
	u8 IP_Header[8];
	u8 HeaderWithoutCheckSum[sizeof(FirstPartHeader) + sizeof(IP_Header)];

	memcpy(&IP_Header[0], IP_Source, 4);
	memcpy(&IP_Header[4], IP_Destination, 4);
	memcpy(&HeaderWithoutCheckSum[0], FirstPartHeader, sizeof(FirstPartHeader));
	memcpy(&HeaderWithoutCheckSum[sizeof(FirstPartHeader)], IP_Header, sizeof(IP_Header));

	u16 ResaltCheckSum = CalculateChecSum(&(HeaderWithoutCheckSum[0]), sizeof(HeaderWithoutCheckSum));

	memcpy(Frame, FirstPartHeader, sizeof(FirstPartHeader));
	memcpy(Frame + sizeof(FirstPartHeader), &ResaltCheckSum, sizeof(ResaltCheckSum));
	memcpy(Frame + sizeof(FirstPartHeader) + 2, IP_Header, sizeof(IP_Header));
}
/*******************************************************************************
 * This function add UDP lay and data to TxFrame:
 * 2 byte - Port source
 * 2 byte - Port destination
 * 2 byte - Length
 * 2 byte - Checksum
 * N byte - Data
********************************************************************************/
void EmacPsUtilFrameUDPHeader(EthernetFrame *FramePtr, const u8 *SourcePort, u8 *DestinationPort, const u16 DataLength, const u8 *Data)
{
	u8 *Frame = (u8 *) FramePtr + 14 + 20;
	u16 TotalLangth = DataLength + 8;
	u8 HeaderWithoutCheckSum[6 + DataLength];

	HeaderWithoutCheckSum[0] = *(SourcePort + 1);
	HeaderWithoutCheckSum[1] = *SourcePort;
	HeaderWithoutCheckSum[2] = *(DestinationPort + 1);
	HeaderWithoutCheckSum[3] = *DestinationPort;
	HeaderWithoutCheckSum[4] = (u8)(TotalLangth >> 8);
	HeaderWithoutCheckSum[5] = (u8)TotalLangth;

	memcpy(HeaderWithoutCheckSum + 6, Data, DataLength);

//	u16 ResaltCheckSum = CalculateChecSum(&(HeaderWithoutCheckSum[0]), sizeof(HeaderWithoutCheckSum));
	u16 ResaltCheckSum = 0x0000;
	memcpy(Frame, SourcePort, 2);
	memcpy(Frame + 2, DestinationPort, 2);
	memcpy(Frame + 4, &HeaderWithoutCheckSum[4], 1);
	memcpy(Frame + 5, &HeaderWithoutCheckSum[5], 1);
	memcpy(Frame + 6, &ResaltCheckSum, 2);
	memcpy(Frame + 8, Data, DataLength);
}
/*******************************************************************************
 * This function add ARP lay and data to TxFrame:
 * 2 byte - Port source
 * 2 byte - Port destination
 * 2 byte - Length
 * 2 byte - Checksum
 * N byte - Data
********************************************************************************/
void EmacPsUtilFrameARPHeader(EthernetFrame *FramePtr, u8* Opcode)
{
	u8 *Frame = (u8 *) FramePtr + 14;
	u8 HardwareType[2] = {0x00, 0x01};
	u8 ProtocolType[2] = {0x08, 0x00};
	u8 HardwareSize = 0x06;
	u8 ProtocolSize = 0x04;

	memcpy(Frame, &HardwareType, sizeof(HardwareType));
	memcpy(Frame + 2, &ProtocolType, sizeof(ProtocolType));
	memcpy(Frame + 4, &HardwareSize, sizeof(HardwareSize));
	memcpy(Frame + 5, &ProtocolSize, sizeof(ProtocolSize));
	memcpy(Frame + 6, Opcode, 2*sizeof(u8));

	memcpy(Frame + 8, &c_EmacPsMAC_Zynq, sizeof(c_EmacPsMAC_Zynq));
	memcpy(Frame + 14, &IP_Zynq, sizeof(IP_Zynq));
	memcpy(Frame + 18, &c_EmacPsMAC_Source, sizeof(c_EmacPsMAC_Source));
	memcpy(Frame + 24, &IP_Source, sizeof(IP_Source));
}
/***********************************************************************************
 * This function processing Rx frame and ...
 *
 *
 *
 *
 *
 *
 ************************************************************************************/
int EmacpsRxFrameProcessing(EthernetFrame *RxFrame, int Length, u8 *c_EmacPsMAC_Source, u8 *IP_Source, u8 *Data, u16 *Length_Data)
{
	u8 *Frame = (u8 *) RxFrame;
	u8 MAC_Destination[6];
	u8 MAC_Source[6];
	u8 TypeFrame[2];

	u8 const Protocol_ARP[2] = {0x08, 0x06};
	u8 const Protocol_Internet[2] = {0x08, 0x00};
	u8 const Opcode_Request[] = {0x00, 0x01};
	u8 const Opcode_Response[] = {0x00, 0x02};


	memcpy(&MAC_Destination, Frame, sizeof(MAC_Destination));
	memcpy(&MAC_Source, Frame + 6, sizeof(MAC_Source));
	memcpy(&TypeFrame, Frame + 12, sizeof(TypeFrame));


	if(memcmp(&TypeFrame, &Protocol_ARP, 2) == 0)
	{
		u8 HardwareType[2];
		u8 ProtocolType[2];
		u8 HardwareSize;
		u8 ProtocolSize;
		u8 Opcode[2];
		u8 IP_Target_ARP[4];

		memcpy(&HardwareType, Frame + 14, sizeof(HardwareType));
		memcpy(&ProtocolType, Frame + 16, sizeof(ProtocolType));
		memcpy(&HardwareSize, Frame + 18, sizeof(HardwareSize));
		memcpy(&ProtocolSize, Frame + 19, sizeof(ProtocolSize));
		memcpy(&Opcode, Frame + 20, sizeof(Opcode));
		memcpy(&IP_Target_ARP, Frame + 38, sizeof(IP_Target_ARP));

		if(memcmp(&Opcode, &Opcode_Request, 2) == 0)
		{
			if(memcmp(&IP_Target_ARP, &IP_Zynq, 4) == 0)
			{
				memcpy(c_EmacPsMAC_Source, Frame + 22, 6);
				memcpy(IP_Source, Frame + 28, 4);
				return ARP_REQUEST;
			}
		}
		else if(memcmp(&Opcode, &Opcode_Response, 2) == 0)
		{
			memcpy(c_EmacPsMAC_Source, Frame + 22, 6);
			memcpy(IP_Source, Frame + 28, 4);
			return ARP_RESPONSE;
		}
	}

	if(memcmp(&TypeFrame, &Protocol_Internet, 2) == 0) //UDP
	{
		u8 Header_IPv4[18];
		u8 Protocol;
		u8 Header_Checksum[2];
		u8 IP_Destination[4];


		memcpy(&Header_IPv4[0], Frame + 14, 10);
		memcpy(&Header_IPv4[10], Frame + 26, 8);
		memcpy(&Protocol, Frame + 23, sizeof(Protocol));
		memcpy(&Header_Checksum[0], Frame + 25, 1);
		memcpy(&Header_Checksum[1], Frame + 24, 1);
		memcpy(&IP_Destination, Frame + 30, sizeof(IP_Destination));

		u16 Calculate_Checksum = 0;
		if(Header_Checksum[0] || Header_Checksum[1])
		{
			Calculate_Checksum = CalculateChecSum(Header_IPv4, sizeof(Header_IPv4));
		}

		if((memcmp(&Header_Checksum, &Calculate_Checksum, 2) == 0) && Protocol == 0x11 && memcmp(&IP_Destination, &IP_Zynq, 4) == 0)
		{
			u8 Destination_Port[2];
			u8 Length[2];
			u8 UDP_Checkcum[2];

			memcpy(&Destination_Port, Frame + 36, sizeof(Destination_Port));
			memcpy(&Length[0], Frame + 39, 1);
			memcpy(&Length[1], Frame + 38, 1);
			memcpy(&UDP_Checkcum[0], Frame + 41, 1);
			memcpy(&UDP_Checkcum[1], Frame + 40, 1);

			*Length_Data = Length[0] - 8;
			memcpy(Data, Frame + 42, *Length_Data);
			if(memcmp(Port_Zynq, Destination_Port, 2) == 0)
			{
				memcpy(&Port_Answer, Frame + 34, sizeof(Port_Answer));
				return UDP_FRAME;
			}
		}
	}

	return NOT_PROCESSING;
}
/****************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************************/
int xEmacpsTxFrameARP(XEmacPs *EmacPsInstancePtr, XEmacPs_Bd *MyBdPtr, const u8 *Opcode)
{
	EthernetFrame TxFrame;
	XEmacPs_Bd *BdTxPtr;
	BdTxPtr = MyBdPtr;
	u16 ui_frame_type = 0x0806;
	u16 TotalLength = 0;

//**********Building Frame*************************
	if(*(Opcode + 1) == 0x01)
	{
		u8 EMAC_Default[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		EmacPsUtilFrameHdrFormatMAC(&TxFrame, &EMAC_Default[0], c_EmacPsMAC_Zynq);
		EmacPsUtilFrameHdrFormatType(&TxFrame, ui_frame_type);
		EmacPsUtilFrameARPHeader(&TxFrame, Opcode);
	}
	else
	{
		EmacPsUtilFrameHdrFormatMAC(&TxFrame, c_EmacPsMAC_Source, c_EmacPsMAC_Zynq);
		EmacPsUtilFrameHdrFormatType(&TxFrame, ui_frame_type);
		EmacPsUtilFrameARPHeader(&TxFrame, Opcode);
	}
	TotalLength = 42;
	Xil_DCacheFlushRange((u32)&TxFrame, TotalLength);
//****************************************************
	return xEmacpsTxBdBuilde(EmacPsInstancePtr, BdTxPtr, &TxFrame, TotalLength);
}

/****************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************************/
int xEmacpsTxFrameUDP(XEmacPs *EmacPsInstancePtr, XEmacPs_Bd *MyBdPtr, u8 *Data_Send, u16 Length_Data)
{
	static u16 Identificator_Frame = 0;
	EthernetFrame TxFrame;
	XEmacPs_Bd *BdTxPtr;
	BdTxPtr = MyBdPtr;
	u16 ui_frame_type = 0x0800;
	u16 TotalLength = 0;

//**********Building Frame*************
	Identificator_Frame++;
	EmacPsUtilFrameHdrFormatMAC(&TxFrame, c_EmacPsMAC_Zynq, c_EmacPsMAC_Source);
	EmacPsUtilFrameHdrFormatType(&TxFrame, ui_frame_type);
	EmacPsUtilFrameSetIPHeader(&TxFrame, Length_Data + 28, Identificator_Frame, IP_Zynq, IP_Source);
	EmacPsUtilFrameUDPHeader(&TxFrame, &Port_Zynq[0], &Port_Answer[0], Length_Data, Data_Send);
	TotalLength = Length_Data + 42;
	Xil_DCacheFlushRange((u32)&TxFrame, TotalLength);
//**************************************

	return xEmacpsTxBdBuilde(EmacPsInstancePtr, BdTxPtr, &TxFrame, TotalLength);
}

int xEmacpsTxBdBuilde(XEmacPs *EmacPsInstancePtr, XEmacPs_Bd *BdTxPtr, EthernetFrame *TxFrame, u16 TotalLength)
{
	int Status;
	Status = XEmacPs_BdRingAlloc(&(XEmacPs_GetTxRing(EmacPsInstancePtr)), 1, &BdTxPtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error allocating TxBD");
		return XST_FAILURE;
	}

	XEmacPs_BdSetAddressTx(BdTxPtr, TxFrame);
	XEmacPs_BdSetLength(BdTxPtr, TotalLength);
	XEmacPs_BdClearTxUsed(BdTxPtr);
	XEmacPs_BdSetLast(BdTxPtr);

	/*
	 * Enqueue to HW
	 */
	Status = XEmacPs_BdRingToHw(&(XEmacPs_GetTxRing(EmacPsInstancePtr)), 1, BdTxPtr);
	if (Status != XST_SUCCESS) {
		EmacPsUtilErrorTrap("Error committing TxBD to HW");
		return XST_FAILURE;
	}

	/* Start transmit */
	XEmacPs_Transmit(EmacPsInstancePtr);


	return XST_SUCCESS;
}

void resetrx_on_no_rxdata(XEmacPs *EmacPsInstancePtr)
{
        u32 regctrl;
        u32 tempcntr;
        u32 gigeversion;

        gigeversion = ((Xil_In32(EmacPsInstancePtr->Config.BaseAddress + 0xFC)) >> 16) & 0xFFF;
        if (gigeversion == 2) {
                tempcntr = XEmacPs_ReadReg(EmacPsInstancePtr->Config.BaseAddress, XEMACPS_RXCNT_OFFSET);

                if ((!tempcntr)) {
                        regctrl = XEmacPs_ReadReg(EmacPsInstancePtr->Config.BaseAddress,
                                        XEMACPS_NWCTRL_OFFSET);
                        regctrl &= (~XEMACPS_NWCTRL_RXEN_MASK);
                        XEmacPs_WriteReg(EmacPsInstancePtr->Config.BaseAddress,
                                        XEMACPS_NWCTRL_OFFSET, regctrl);
                        regctrl = XEmacPs_ReadReg(EmacPsInstancePtr->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
                        regctrl |= (XEMACPS_NWCTRL_RXEN_MASK);
                        XEmacPs_WriteReg(EmacPsInstancePtr->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, regctrl);
                }
        }
}


