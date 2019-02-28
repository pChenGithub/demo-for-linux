#include "w25q.h"





unsigned char W25Q_TxRxByte(unsigned char TxByte)

{

  return HW_SPI_TxRxByte(HW_SPI_W25Q, TxByte );

}



void W25Q_ChipSelect(unsigned int PinLevel)

{

  HW_SPI_ChipSelect(HW_SPI_W25Q, PinLevel );

}



/*

Write Enable (06h)

The Write Enable instruction (Figure 4) sets the Write Enable Latch (WEL) bit

in the Status Register to a 1.



The WEL bit must be set prior to every Page Program, Sector Erase,

Block Erase, Chip Erase and Write Status Register instruction.



The Write Enable instruction is entered by driving /CS low,

shifting the instruction code ?06h? into the Data Input (DI) pin

on the rising edge of CLK, and then driving /CS high.



*/

void W25Q_WriteEnable(void)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x06 );

  W25Q_ChipSelect(1);

}



/*

Write Disable (04h)

The Write Disable instruction (Figure 5) resets the Write Enable Latch (WEL) bit

in the Status Register to a 0.



The Write Disable instruction is entered by driving /CS low,

shifting the instruction code ?04h? into the DI pin and then driving /CS high.



Note that the WEL bit is automatically reset after Power-up and

upon completion of the Write Status Register, Page Program, Sector Erase,

Block Erase and Chip Erase instructions.



*/



void W25Q_WriteDisable(void)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x04 );

  W25Q_ChipSelect(1);

}



/*



Read Status Register-1 (05h) and Read Status Register-2 (35h)



The Read Status Register instructions allow the 8-bit Status Registers to be read.

The instruction is entered by driving /CS low and

shifting the instruction code ?05h?  for Status Register-1

and ?35h? for Status Register-2 into the DI pin on the rising edge of CLK.



The status register bits are then shifted out on the DO pin

at the falling edge of CLK with most significant bit (MSB) first as shown in figure 6.



The Status Register bits are shown in figure 3a and 3b and

include the BUSY, WEL, BP2-BP0, TB, SEC, SRP0, SRP1, QE and SUS bits

(see description of the Status Register earlier in this datasheet).



The Read Status Register instruction may be used at any time,

even while a Program, Erase or Write Status Register cycle is in progress.



This allows the BUSY status bit to be checked to determine when the cycle is complete

and if the device can accept another instruction.



The Status Register can be read continuously, as shown in Figure 6.

The instruction is completed by driving /CS high.



*/





unsigned char W25Q_ReadStatusRegisterLow(void)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x05 );

  unsigned char StatusRegisterLow = W25Q_TxRxByte( 0xFF );

  W25Q_ChipSelect(1);

  return StatusRegisterLow;

}



unsigned char W25Q_ReadStatusRegisterHigh(void)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x35 );

  unsigned char StatusRegisterHigh = W25Q_TxRxByte( 0xFF );

  W25Q_ChipSelect(1);

  return StatusRegisterHigh;

}



/*

Write Status Register (01h) :: 01 : S7..S0 [ : S15..S8 ]



The Write Status Register instruction allows the Status Register to be written.



A Write Enable instruction must previously have been executed for the device

to accept the Write Status Register Instruction (Status Register bit WEL must equal 1).



Once write enabled, the instruction is entered by driving /CS low,

sending the instruction code ?01h?, and then writing the status register data byte as illustrated in figure 7.



The Status Register bits are shown in figure 3 and described earlier in this datasheet.



Only non-volatile Status Register bits SRP0, SEC, TB, BP2, BP1, BP0 (bits 7, 5, 4, 3, 2 of Status Register-1)

and QE, SRP1(bits 9 and 8 of Status Register-2) can be written to.



All other Status Register bit locations are read-only and will not be affected by the Write Status Register instruction.



The /CS pin must be driven high after the eighth or sixteenth bit of data that is clocked in.



If this is not done the Write Status Register instruction will not be executed.



If /CS is driven high after the eighth clock (compatible with the 25X series)

the QE and SRP1 bits will be cleared to 0.

    ***********



After /CS is driven high, the self-timed Write Status Register cycle will commence

for a time duration of t W (See AC Characteristics).



While the Write Status Register cycle is in progress,

the Read Status Register instruction may still be accessed to check the status of the BUSY bit.



The BUSY bit is a 1 during the Write Status Register cycle and

a 0 when the cycle is finished and ready to accept other instructions again.



After the Write Register cycle has finished the Write Enable Latch (WEL) bit

in the Status Register will be cleared to 0.



The Write Status Register instruction allows the Block Protect bits

(SEC, TB, BP2, BP1 and BP0) to be set for protecting all, a portion, or none of the memory

from erase and program instructions.



Protected areas become read-only (see Status Register Memory Protection table and description).



The Write Status Register instruction also allows the Status Register Protect bits (SRP0, SRP1) to be set.

Those bits are used in conjunction with the Write Protect (/WP) pin, Lock out or OTP features

to disable writes to the status register.



Please refer to 11.1.6 for detailed descriptions regarding Status Register protection methods.

Factory default for all status Register bits are 0.



*/



void W25Q_WriteStatusRegisterLow(unsigned char StatusRegisterLow)

{

  W25Q_WriteEnable();

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x01 );

  W25Q_TxRxByte( StatusRegisterLow );

  W25Q_ChipSelect(1);

}



void W25Q_WriteStatusRegister(unsigned char StatusRegisterLow, unsigned char StatusRegisterHigh)

{

  W25Q_WriteEnable();

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x01 );

  W25Q_TxRxByte( StatusRegisterLow );

  W25Q_TxRxByte( StatusRegisterHigh );

  W25Q_ChipSelect(1);

}



/*



Read Data (03h)



The Read Data instruction allows one more data bytes to be sequentially read from the memory.



The instruction is initiated by driving the /CS pin low and

then shifting the instruction code ?03h? followed by a 24-bit address (A23-A0) into the DI pin.



The code and address bits are latched on the rising edge of the CLK pin.

After the address is received, the data byte of the addressed memory location will be

shifted out on the DO pin at the falling edge of CLK with most significant bit (MSB) first.



The address is automatically incremented to the next higher address

after each byte of data is shifted out allowing for a continuous stream of data.



This means that the entire memory can be accessed

with a single instruction as long as the clock continues.



The instruction is completed by driving /CS high.



The Read Data instruction sequence is shown in figure 8.

If a Read Data instruction is issued while an Erase, Program or Write cycle is in process (BUSY=1)

the instruction is ignored and will not have any effects on the current cycle.



The Read Data instruction allows clock rates from D.C. to a maximum of f R (see AC Electrical Characteristics).



*/



void W25Q_Read(unsigned int address, unsigned int count, unsigned char * buffer)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x03 );

  W25Q_TxRxByte( address >> 16 );

  W25Q_TxRxByte( address >> 8 );

  W25Q_TxRxByte( address >> 0 );



  for (unsigned int i=0; i<count; i++)

    buffer[i] = W25Q_TxRxByte( 0xFF );



  W25Q_ChipSelect(1);

}



/*

Fast Read (0Bh)



The Fast Read instruction is similar to the Read Data instruction

except that it can operate at the highest possible frequency of F R (see AC Electrical Characteristics).



This is accomplished by adding eight ?dummy? clocks after the 24-bit address as shown in figure 9.



The dummy clocks allow the devices internal circuits additional time for setting up the initial address.



During the dummy clocks the data value on the DO pin is a ?don?t care?.



*/





void W25Q_FastRead(unsigned int address, unsigned int count, unsigned char * buffer)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x03 );

  W25Q_TxRxByte( address >> 16 );

  W25Q_TxRxByte( address >> 8 );

  W25Q_TxRxByte( address >> 0 );

  W25Q_TxRxByte( 0xFF );



  for (unsigned int i=0; i<count; i++)

    buffer[i] = W25Q_TxRxByte( 0xFF );



  W25Q_ChipSelect(1);

}



/*



Page Program (02h)



The Page Program instruction allows from one byte to 256 bytes (a page) of data

to be programmed at previously erased (FFh) memory locations.



A Write Enable instruction must be executed

before the device will accept the Page Program Instruction (Status Register bit WEL= 1).



The instruction is initiated by driving the /CS pin low

then shifting the instruction code ?02h? followed by

a 24-bit address (A23-A0) and

at least one data byte, into the DI pin.



The /CS pin must be held low for the entire length of the instruction

while data is being sent to the device.



The Page Program instruction sequence is shown in figure 16.



If an entire 256 byte page is to be programmed,

the last address byte (the 8 least significant address bits) should be set to 0.



If the last address byte is not zero, and the number of clocks exceed the remaining page length,

the addressing will wrap to the beginning of the page.

                    *********************************



In some cases, less than 256 bytes (a partial page) can be programmed

without having any effect on other bytes within the same page.



One condition to perform a partial page program is that

the number of clocks can not exceed the remaining page length.



If more than 256 bytes are sent to the device

the addressing will wrap to the beginning of the page and

                    *********************************



overwrite previously sent data.



As with the write and erase instructions, the /CS pin must be driven high

after the eighth bit of the last byte has been latched.



If this is not done the Page Program instruction will not be executed.

After /CS is driven high, the self-timed Page Program instruction

will commence for a time duration of tpp (See AC Characteristics).



While the Page Program cycle is in progress,

the Read Status Register instruction may still be accessed

for checking the status of the BUSY bit.



The BUSY bit is a 1 during the Page Program cycle and

becomes a 0 when the cycle is finished and the device is ready to accept other instructions again.



After the Page Program cycle has finished the Write Enable Latch (WEL) bit

in the Status Register is cleared to 0.



The Page Program instruction will not be executed

if the addressed page is protected by the Block Protect (BP2, BP1, and BP0) bits.



*/





void W25Q_PageProgram(unsigned int address, unsigned int count, unsigned char * buffer)

{

  W25Q_ChipSelect(0);



  W25Q_TxRxByte( 0x02 );

  W25Q_TxRxByte( address >> 16 );

  W25Q_TxRxByte( address >> 8 );

  W25Q_TxRxByte( address >> 0 );



  for (unsigned int i=0; i<count; i++)

     W25Q_TxRxByte( buffer[i] );



  W25Q_ChipSelect(1);

}











/*



Sector Erase (20h)



The Sector Erase instruction sets all memory within a specified sector (4K-bytes)

to the erased state of all 1s (FFh).



A Write Enable instruction must be executed before the device will

accept the Sector Erase Instruction (Status Register bit WEL must equal 1).



The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?20h? followed a 24-bit sector address (A23-A0) (see Figure 2).



The Sector Erase instruction sequence is shown in figure 18.



The /CS pin must be driven high after the eighth bit of the last byte has been latched.

If this is not done the Sector Erase instruction will not be executed.



After /CS is driven high, the self-timed Sector Erase instruction will

commence for a time duration of t SE (See AC Characteristics).



While the Sector Erase cycle is in progress,

the Read Status Register instruction may still be accessed for

checking the status of the BUSY bit.



The BUSY bit is a 1 during the Sector Erase cycle and becomes

a 0 when the cycle is finished and the device is ready to accept other instructions again.



After the Sector Erase cycle has finished the Write Enable Latch (WEL) bit

in the Status Register is cleared to 0.



The Sector Erase instruction will not be executed

if the addressed page is protected by the Block Protect (SEC, TB, BP2, BP1, and BP0) bits

(see Status Register Memory Protection table).



*/



void W25Q_SectorErase(unsigned int address)

{

  W25Q_ChipSelect(0);



  W25Q_TxRxByte( 0x20 );

  W25Q_TxRxByte( address >> 16 );

  W25Q_TxRxByte( address >> 8 );

  W25Q_TxRxByte( address >> 0 );



  W25Q_ChipSelect(1);

}



/*



32KB Block Erase (52h)



The Block Erase instruction sets all memory within a specified block (32K-bytes)

to the erased state of all 1s (FFh).



A Write Enable instruction must be executed before the device will

accept the Block Erase Instruction (Status Register bit WEL must equal 1).



The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?52h? followed a 24-bit block address (A23-A0) (see Figure 2).



The Block Erase instruction sequence is shown in figure 19.



The /CS pin must be driven high after the eighth bit of the last byte has been latched.

If this is not done the Block Erase instruction will not be executed.



After /CS is driven high, the self-timed Block Erase instruction will

commence for a time duration of t BE 1 (See AC Characteristics).



While the Block Erase cycle is in progress,

the Read Status Register instruction may still be accessed for

checking the status of the BUSY bit.



The BUSY bit is a 1 during the Block Erase cycle and becomes

a 0 when the cycle is finished and the device is ready to accept other instructions again.



After the Block Erase cycle has finished the Write Enable Latch (WEL) bit

in the Status Register is cleared to 0.



The Block Erase instruction will not be executed

if the addressed page is protected by the Block Protect (SEC, TB, BP2, BP1, and BP0) bits

(see Status Register Memory Protection table).



*/



void W25Q_BlockErase32K(unsigned int address)

{

  W25Q_ChipSelect(0);



  W25Q_TxRxByte( 0x52 );

  W25Q_TxRxByte( address >> 16 );

  W25Q_TxRxByte( address >> 8 );

  W25Q_TxRxByte( address >> 0 );



  W25Q_ChipSelect(1);

}





/*



64KB Block Erase (D8h)



The Block Erase instruction sets all memory within a specified block (64K-bytes)

to the erased state of all 1s (FFh).



A Write Enable instruction must be executed before the device will

accept the Block Erase Instruction (Status Register bit WEL must equal 1).



The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?D8h? followed a 24-bit block address (A23-A0) (see Figure 2).



The Block Erase instruction sequence is shown in figure 20.



The /CS pin must be driven high after the eighth bit of the last byte has been latched.

If this is not done the Block Erase instruction will not be executed.



After /CS is driven high, the self-timed Block Erase instruction will

commence for a time duration of t BE 1 (See AC Characteristics).



While the Block Erase cycle is in progress,

the Read Status Register instruction may still be accessed for

checking the status of the BUSY bit.



The BUSY bit is a 1 during the Block Erase cycle and becomes

a 0 when the cycle is finished and the device is ready to accept other instructions again.



After the Block Erase cycle has finished the Write Enable Latch (WEL) bit

in the Status Register is cleared to 0.



The Block Erase instruction will not be executed

if the addressed page is protected by the Block Protect (SEC, TB, BP2, BP1, and BP0) bits

(see Status Register Memory Protection table).



*/



void W25Q_BlockErase64K(unsigned int address)

{

  W25Q_ChipSelect(0);



  W25Q_TxRxByte( 0xD8 );

  W25Q_TxRxByte( address >> 16 );

  W25Q_TxRxByte( address >> 8 );

  W25Q_TxRxByte( address >> 0 );



  W25Q_ChipSelect(1);



}





/*

Chip Erase (C7h / 60h)



The Chip Erase instruction sets all memory within the device

to the erased state of all 1s (FFh).



A Write Enable instruction must be executed before the device will

accept the Block Erase Instruction (Status Register bit WEL must equal 1).



The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?C7h? or ?60h?.



The Chip Erase instruction sequence is shown in figure 21.



The /CS pin must be driven high after the eighth bit of the last byte has been latched.

If this is not done the Chip Erase instruction will not be executed.



After /CS is driven high, the self-timed Chip Erase instruction will

commence for a time duration of t BE 1 (See AC Characteristics).



While the Chip Erase cycle is in progress,

the Read Status Register instruction may still be accessed for

checking the status of the BUSY bit.



The BUSY bit is a 1 during the Chip Erase cycle and becomes

a 0 when the cycle is finished and the device is ready to accept other instructions again.



After the Block Erase cycle has finished the Write Enable Latch (WEL) bit

in the Status Register is cleared to 0.



The Block Erase instruction will not be executed

if the addressed page is protected by the Block Protect (SEC, TB, BP2, BP1, and BP0) bits

(see Status Register Memory Protection table).



*/





void W25Q_ChipErase(void)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0xC7 );

  W25Q_ChipSelect(1);

}



/*

Device ID (ABh)



It can be used to obtain the devices electronic identification (ID) number.



the instruction is initiated by driving the /CS pin low and

shifting the instruction code ?ABh? followed by 3-dummy bytes.



The Device ID bits are then shifted out on the falling edge of CLK

with most significant bit (MSB) first as shown in figure 25b.



The Device ID values for the W25Q16BV is listed in Manufacturer and Device Identification table.

The Device ID can be read continuously.



The instruction is completed by driving /CS high.



*/



unsigned char W25Q_ReadDeviceId(void)

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0xAB );

  W25Q_TxRxByte( 0xFF );

  W25Q_TxRxByte( 0xFF );

  W25Q_TxRxByte( 0xFF );

  unsigned char DeviceId = W25Q_TxRxByte( 0xFF );

  W25Q_ChipSelect(1);

  return DeviceId;

}





/*

Read Manufacturer / Device ID (90h)



The Read Manufacturer/Device ID instruction is an alternative

to the Release from Power-down / Device ID instruction that

provides both the JEDEC assigned manufacturer ID and the specific device ID.



The Read Manufacturer/Device ID instruction is very similar

to the Release from Power-down / Device ID instruction.



The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?90h? followed by a 24-bit address (A23-A0) of 000000h.



After which, the Manufacturer ID for Winbond (EFh) and the Device ID are

shifted out on the falling edge of CLK with most significant bit (MSB) first as shown in figure 26.



The Device ID values for the W25Q16BV is listed in Manufacturer and Device Identification table.



If the 24-bit address is initially set to 000001h the Device ID will be read first

and then followed by the Manufacturer ID.



The Manufacturer and Device IDs can be read continuously,

alternating from one to the other.



The instruction is completed by driving /CS high.





*/

unsigned short W25Q_ReadManfDeviceId(void) // Manf -- Device

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x90 );

  W25Q_TxRxByte( 0x00 );  // Address = 0x000000 : ManfId

  W25Q_TxRxByte( 0x00 );  // Address = 0x000001 : DeviceId

  W25Q_TxRxByte( 0x00 );

  unsigned char ManfId = W25Q_TxRxByte( 0xFF );

  unsigned char DeviceId = W25Q_TxRxByte( 0xFF );

  W25Q_ChipSelect(1);

  return ( ( ManfId<<8) + DeviceId );

}





/*



Read JEDEC ID (9Fh)



For compatibility reasons, the W25Q16BV provides several instructions

to electronically determine the identity of the device.



The Read JEDEC ID instruction is compatible with the JEDEC standard

for SPI compatible serial memories that was adopted in 2003.



The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?9Fh?.



The JEDEC assigned Manufacturer ID byte for Winbond (EFh)

and two Device ID bytes, Memory Type (ID15-ID8) and Capacity (ID7-ID0) are

then shifted out on the falling edge of CLK with most significant bit (MSB) first as shown in figure 30.



For memory type and capacity values refer to Manufacturer and Device Identification table.



*/



unsigned int W25Q_ReadJEDEDId(void) // 00--Manf--Type--Capacity

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x9F );

  unsigned char ManfId = W25Q_TxRxByte( 0xFF );

  unsigned char Type = W25Q_TxRxByte( 0xFF );

  unsigned char Capacity = W25Q_TxRxByte( 0xFF );

  W25Q_ChipSelect(1);

  return ( ( ManfId<<16 ) + ( Type<<8 ) + Capacity );



}





/*



Read Unique ID Number (4Bh)



The Read Unique ID Number instruction accesses a factory-set

read-only 64-bit number that is unique to each W25Q16BV device.



The ID number can be used in conjunction with user software methods

to help prevent copying or cloning of a system.



The Read Unique ID instruction is initiated by driving the /CS pin low and

shifting the instruction code ?4Bh? followed by a four bytes of dummy clocks.



After which, the 64-bit ID is shifted out on the falling edge of CLK as shown in figure 29.



*/



void W25Q_ReadUniqueId(unsigned char * UniqueId) // DO :: 63..0 :: [0..7]

{

  W25Q_ChipSelect(0);

  W25Q_TxRxByte( 0x4B );

  W25Q_TxRxByte( 0xFF );

  W25Q_TxRxByte( 0xFF );

  W25Q_TxRxByte( 0xFF );

  W25Q_TxRxByte( 0xFF );



  for (unsigned int i=0; i<8; i++)

    UniqueId[i] = W25Q_TxRxByte( 0xFF );



  W25Q_ChipSelect(1);

}



unsigned int W25Q_Present(void)

{

  unsigned char DeviceId = W25Q_ReadDeviceId();

  unsigned short ManfDeviceId = W25Q_ReadManfDeviceId();

  unsigned int JEDEDId = W25Q_ReadJEDEDId();

  unsigned char UniqueId[8];



  if ( DeviceId != W25Q_DEVICE_ID ) return 0;

  if ( ManfDeviceId != W25Q_MANF_DEVICE_ID ) return 0;

  if ( JEDEDId != W25Q_JEDED_ID ) return 0;

  W25Q_ReadUniqueId( UniqueId );



  return 1;

}



void W25Q_ReadMemory(unsigned int dwAddr,

  unsigned int dwSize , unsigned char * Memory)

{

  //W25Q_Read( dwAddress, dwSize , Memory );

  W25Q_FastRead( dwAddr, dwSize , Memory );

}



unsigned int W25Q_WaitComplete(unsigned int dwTimeout)

{

  for (unsigned int x=0; x<dwTimeout; x++)

  {

    HW_TimeoutStart(1);

    while (! HW_TimeoutStop() )

    {

      if ( ! ( (1<<0) & W25Q_ReadStatusRegisterLow() ) )

        return 1;

    }

  }

  return 0;

}



unsigned int W25Q_WaitStart(unsigned int dwTimeout)

{

  for (unsigned int x=0; x<dwTimeout; x++)

  {

    HW_TimeoutStart(1);

    while (! HW_TimeoutStop() )

    {

      if ( (1<<1) & W25Q_ReadStatusRegisterLow() )

        return 1;

    }

  }

  return 0;

}



/*



dwAddr in Sector, Blk32K, Blk64K ( LSBs will be ignore



Sector Erase (20h)



The Sector Erase instruction sets all memory within a specified sector (4K-bytes)

to the erased state of all 1s (FFh).

The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?20h? followed a 24-bit sector address (A23-A0)



32KB Block Erase (52h)



The Block Erase instruction sets all memory within a specified block (32K-bytes)

to the erased state of all 1s (FFh).

The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?52h? followed a 24-bit block address (A23-A0)



64KB Block Erase (D8h)



The Block Erase instruction sets all memory within a specified block (64K-bytes)

to the erased state of all 1s (FFh).

The instruction is initiated by driving the /CS pin low and

shifting the instruction code ?D8h? followed a 24-bit block address (A23-A0)



Chip Erase (C7h / 60h)



The Chip Erase instruction sets all memory within the device

to the erased state of all 1s (FFh).



caller must read and save data, then restore it after erase



dwSize += ( dwAddr & W25Q_SEC_MASK ); // adjust dwSize

dwAddr &= ( ~W25Q_SEC_MASK );         // adjust dwAddr



// set dwSize = N * W25Q_SEC_SIZE

dwSize += W25Q_SEC_MASK;

dwSize /= W25Q_SEC_SIZE;

dwSize *= W25Q_SEC_SIZE;



*/



void W25Q_ErasaMemory(unsigned int dwAddr,unsigned int dwSize)

{

  if ( dwSize == 0 )  return;



  if ( (int)dwSize == -1 )  // Chip Erase

  {

    W25Q_WriteEnable();

    if ( ! W25Q_WaitStart(W25Q16_WRITE_LATCH_TIME) )

      return;



    W25Q_ChipErase();

    W25Q_WaitComplete( W25Q16_CHIP_ERASE_TIME );

    return;

  }



  unsigned int dwSizeErased = 0;

  while ( (int)dwSize > 0 )

  {

    dwAddr += dwSizeErased;

    dwSize -= dwSizeErased;



    if ( !( dwAddr & W25Q_BLK64_MASK ) && ( dwSize >= W25Q_BLK64_SIZE) )

    {

      W25Q_WriteEnable();

      if ( ! W25Q_WaitStart(W25Q16_WRITE_LATCH_TIME) )

        return;



      W25Q_BlockErase64K(dwAddr);

      if ( ! W25Q_WaitComplete( W25Q16_64K_BLOCK_ERASE_TIME ) )

        return;



      dwSizeErased = W25Q_BLK64_SIZE;

      continue;

    }



    if ( !( dwAddr & W25Q_BLK32_MASK ) && ( dwSize >= W25Q_BLK32_SIZE) )

    {

      W25Q_WriteEnable();

      if ( ! W25Q_WaitStart(W25Q16_WRITE_LATCH_TIME) )

        return;



      W25Q_BlockErase32K(dwAddr);

      if ( ! W25Q_WaitComplete( W25Q16_32K_BLOCK_ERASE_TIME ) )

        return;



      dwSizeErased = W25Q_BLK32_SIZE;

      continue;

    }



    W25Q_WriteEnable();

    if ( ! W25Q_WaitStart(W25Q16_WRITE_LATCH_TIME) )

      return;



    W25Q_SectorErase(dwAddr);

    if ( ! W25Q_WaitComplete( W25Q16_SECTOR_ERASE_TIME ) )

      return;



    dwSizeErased = W25Q_SEC_SIZE - ( dwAddr & W25Q_SEC_MASK );



  }

}



void W25Q_WriteMemory(unsigned int dwAddr,

  unsigned int dwSize , unsigned char * Memory)

{

  unsigned int dwSizeInPage;



  if ( dwAddr + dwSize > W25Q_CHIP_SIZE ) return;



  while ( dwSize > 0 )

  {

    dwSizeInPage = W25Q_PAGE_SIZE - ( dwAddr & W25Q_PAGE_MASK );

    if ( dwSizeInPage > dwSize ) dwSizeInPage = dwSize;



    W25Q_WriteEnable();

    if ( ! W25Q_WaitStart(W25Q16_WRITE_LATCH_TIME) )

      return;



    W25Q_PageProgram( dwAddr,  dwSizeInPage,  Memory );

    if ( ! W25Q_WaitComplete( W25Q16_PAGE_PROG_TIME ) )

      return;



    dwAddr += dwSizeInPage;

    dwSize -= dwSizeInPage;

    Memory += dwSizeInPage;

    // now it must be page alignment

  }

}
