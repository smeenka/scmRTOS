/*****************************************************************************
			 /-o-o-o
	   -o-o-/--o-o-o

Copyright 2012 Anton Smeenk

This file is part of project scmRTOS_CortexM3.

scmRTOS_CortexM3 is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.
scmRTOS_CortexM3 is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
scmRTOS_CortexM3. If not, see http://www.gnu.org/licenses/.
*****************************************************************************/

#include <ethmac.h>
#include <LPC17xx.h>

unsigned phy_in_use = 0;
EthConnectionStatus_t ethStatus = DISCONNECTED;

// function added to write to external ethernet PHY chip
void WriteToPHY (int reg, int writeval)
{
  unsigned int loop;
  // Set up address to access in MII Mgmt Address Register
  LPC_EMAC->MADR = DP83848C_DEF_ADR | reg;
  // Write value into MII Mgmt Write Data Register
  LPC_EMAC->MWTD = writeval;
  // Loop whilst write to PHY completes
  for (loop = 0; loop < MII_WR_TOUT; loop++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0) { break; }
  }
}

// function added to read from external ethernet PHY chip
unsigned short ReadFromPHY (unsigned char reg) 
{
  unsigned int loop;
  // Set up address to access in MII Mgmt Address Register
  LPC_EMAC->MADR = DP83848C_DEF_ADR | reg;
  // Trigger a PHY read via MII Mgmt Command Register
  LPC_EMAC->MCMD = MCMD_READ;
  // Loop whilst read from PHY completes
  for (loop = 0; loop < MII_RD_TOUT; loop++) {
    if ((LPC_EMAC->MIND & MIND_BUSY) == 0)  { break; }
  }
  LPC_EMAC->MCMD = 0; // Cancel read
  // Returned value is in MII Mgmt Read Data Register
  return (LPC_EMAC->MRDD);
}




// Ethernet power/clock control bit in PCONP register
#define PCENET 0x40000000
// Ethernet configuration for PINSEL2, as per user guide section 5.3
#define ENET_PINSEL2_CONFIG 0x50150105
// Ethernet configuration for PINSEL3, as per user guide section 5.4
#define ENET_PINSEL3_CONFIG 0x00000005
// Only bottom byte of PINSEL3 relevant to Ethernet
#define ENET_PINSEL3_MASK 0x0000000F

void initEthMAC(void)
{
  unsigned int value, phyid1, phyid2;
  volatile unsigned int loop;

  
  // Set Ethernet power/clock control bit
  LPC_SC->PCONP |= PCENET; 

  //Enable Ethernet pins through PINSEL registers
  LPC_PINCON->PINSEL2 = ENET_PINSEL2_CONFIG; 
  LPC_PINCON->PINSEL3 = (LPC_PINCON->PINSEL3 & ~(ENET_PINSEL3_MASK)) | ENET_PINSEL3_CONFIG;

  // Set up MAC Configuration Register 1
  LPC_EMAC->MAC1 = MAC1_RES_TX | MAC1_RES_MCS_TX | MAC1_RES_RX | 
         MAC1_RES_MCS_RX |MAC1_SIM_RES | MAC1_SOFT_RES;

  // Set up MAC Command Register
  LPC_EMAC->Command = CR_REG_RES | CR_TX_RES | CR_RX_RES | CR_PASS_RUNT_FRM;
  
  // Short delay
  for (loop = 100; loop; loop--);

  // Set up MAC Configuration Register 1 to pass all receive frames
  LPC_EMAC->MAC1 = MAC1_PASS_ALL;
  // Set up MAC Configuration Register 2 to append CRC and pad out frames
  LPC_EMAC->MAC2 = MAC2_CRC_EN | MAC2_PAD_EN;

  // Set Ethernet Maximum Frame Register
  LPC_EMAC->MAXF = ETH_MAX_FLEN;
  // Set Collision Window / Retry Register
  LPC_EMAC->CLRT = CLRT_DEF;
  // Set Non Back-to-Back Inter-Packet-Gap Register
  LPC_EMAC->IPGR = IPGR_DEF;

     /* Enable Reduced MII interface. */
    LPC_EMAC->MCFG = MCFG_CLK_DIV64 | MCFG_RES_MII;
     for (loop = 100; loop; loop--);
     LPC_EMAC->MCFG = MCFG_CLK_DIV64;


   // Set MAC Command Register to enable Reduced MII interface
   // and prevent runt frames being filtered out
  LPC_EMAC->Command = CR_RMII | CR_PASS_RUNT_FRM | CR_PASS_RX_FILT;


  // Put DP83848C PHY into reset mode
  WriteToPHY (PHY_REG_BMCR, 0x8000);

  // Loop until hardware reset completes
  for (loop = 0; loop < 0x100000; loop++) {
    value = ReadFromPHY (PHY_REG_BMCR);
    if (!(value & 0x8000)) {
      // Reset has completed
      break;
    }
  }

  // Just check this actually is a DP83848C PHY
  phyid1 = ReadFromPHY (PHY_REG_IDR1);
  phyid2 = ReadFromPHY (PHY_REG_IDR2);

  if (((phyid1 << 16) | (phyid2 & 0xFFF0)) == DP83848C_ID) {
	  phy_in_use =  DP83848C_ID;
  }
  else if (((phyid1 << 16) | (phyid2 & 0xFFF0)) == LAN8720_ID) {
	  phy_in_use = LAN8720_ID;
  }
  ethStatus = DISCONNECTED;

  // Now set the Ethernet MAC Address registers
  // NOTE - MAC address must be unique on the network!
  LPC_EMAC->SA0 = (MYMAC_1 << 8) | MYMAC_2; // Station address 0 Reg
  LPC_EMAC->SA1 = (MYMAC_3 << 8) | MYMAC_4; // Station address 1 Reg
  LPC_EMAC->SA2 = (MYMAC_5 << 8) | MYMAC_6; // Station address 2 Reg

  // Receive Broadcast and Perfect Match Packets
  LPC_EMAC->RxFilterCtrl = RFC_BCAST_EN | RFC_PERFECT_EN;					 
 
  // Enable interrupts MAC Module Control Interrupt Enable Register
  LPC_EMAC->IntEnable = INT_RX_DONE | INT_TX_DONE;

  // Reset all ethernet interrupts in MAC module
  LPC_EMAC->IntClear  = 0xFFFF;

  // Finally enable receive and transmit mode in ethernet core
  LPC_EMAC->Command  |= (CR_RX_EN | CR_TX_EN);
  LPC_EMAC->MAC1     |= MAC1_REC_EN;
}


EthConnectionStatus_t getConnectionStatus(){
	 uint32_t  phy_linkstatus_reg = 0;
	 uint32_t  phy_linkstatus_mask = 0;
	 switch (phy_in_use)
	 {
	 case LAN8720_ID:
		  phy_linkstatus_reg = PHY_REG_BMSR;
		  phy_linkstatus_mask = 0x0004;
		  break;
	 case DP83848C_ID:
		 phy_linkstatus_reg = PHY_REG_STS;
		 phy_linkstatus_mask = 0x0001;
		 break;
	 default:
		 ethStatus = NO_PHY_CHIP;
		 return ethStatus;
	  }

	 unsigned short value = ReadFromPHY (phy_linkstatus_reg);


	 switch (ethStatus){
	 case NO_PHY_CHIP:
		 break;
	 case DISCONNECTED:
		 // check for new connection established
		 if (value & phy_linkstatus_mask) {
			 // Set PHY to auto negotiation link speed
			 WriteToPHY (PHY_REG_BMCR, PHY_AUTO_NEG);
			 ethStatus =  BUSY_AUTONEG;
		 }
		 break;
	 case BUSY_AUTONEG:
	      if (value & 0x0020) {
	         // Autonegotiation has completed
			 // configure after new connection discovered
			 // Now configure for full/half duplex mode
			  if (value & 0x0004) {
				// We are in full duplex is enabled mode
				  LPC_EMAC->MAC2    |= MAC2_FULL_DUP;
				  LPC_EMAC->Command |= CR_FULL_DUP;
				  LPC_EMAC->IPGT     = IPGT_FULL_DUP;
			  }
			  else{	// Otherwise we are in half duplex mode
				  LPC_EMAC->IPGT = IPGT_HALF_DUP;
			  }

			  // Now configure 100MBit or 10MBit mode
			  if (value & 0x0002) {// 10MBit mode
				  LPC_EMAC->SUPP = 0;
			  }
			  else {// 100MBit mode
				  LPC_EMAC->SUPP = SUPP_SPEED;
			  }
			  switch (value & 0x6){
			  case 0: ethStatus =  HALF_100MHZ;   	break;
			  case 2: ethStatus =  HALF_10MHZ;   	break;
			  case 4: ethStatus =  FULL_100MHZ;   	break;
			  case 6: ethStatus =  FULL_10MHZ;   	break;
	          }
	     }
	     break;
	 default:
		 // already configured, check if connection is lost
		 if ((value & phy_linkstatus_mask) == 0) {
			 ethStatus = DISCONNECTED;
		 }
		 break;
	 }
	 return ethStatus;
}












