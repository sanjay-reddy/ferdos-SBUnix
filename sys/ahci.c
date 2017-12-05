#include<sys/defs.h>
#include<sys/ahci.h>
#include<sys/kprintf.h>
#include<string.h>
#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
#define FALSE (1==0)
#define TRUE  (1==1)
static int check_type(hba_port_t *port);
static inline void sysOutLong( unsigned short port, uint32_t val );
static inline uint32_t sysInLong( unsigned short port );
uint64_t checkAllBuses() ;
void port_rebase(hba_port_t *port, int portno);
void start_cmd(hba_port_t *port);
void stop_cmd(hba_port_t *port);
void resetGHC(hba_mem_t *abar);
int ahciwrite(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf);
int read(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf);
int find_cmdslot(hba_port_t *port);
typedef hba_mem_t HBA_MEM;
typedef hba_port_t HBA_PORT;
typedef hba_cmd_header_t HBA_CMD_HEADER;
typedef hba_cmd_tbl_t HBA_CMD_TBL;
uint64_t bar5;
hba_mem_t *abar;
void probe_port(hba_mem_t *abar)
{
	// Search disk in impelemented ports
	DWORD pi = abar->pi;
	int i = 0;
	int sata = 0;
	while (i<32)
	{
		if (pi & 1)
		{//	kprintf("%d",(int)pi);
			int dt = check_type(&abar->ports[i]);
		//	kprintf("%d\n",dt);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n",i);
				sata=i;
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
			//kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i ++;
	}
	resetGHC(abar);
	port_rebase(&abar->ports[sata],sata);
	
uint32_t * buffer = (uint32_t *)0x400000;
uint32_t * newBuf = (uint32_t *)0x500000;
//*buffer=8;
//kprintf("%d",sata);
//write(&abar->ports[sata],0,0,1,(uint16_t *)buffer);
	uint8_t *ptr = (uint8_t *)0x400000;
	int sectors = 0;
	for(int j=0;j<100;j++)
	{
		buffer = (uint32_t *)0x400000;
		ptr = (uint8_t *)0x400000;
		for(int k=0;k<512;k++)
		{
			*ptr = j;
			ptr+=1;
		}
		for(int n=0;n<8;n++)
		{
			ahciwrite(&abar->ports[sata],sectors++,0,1,(uint16_t *)buffer);
		}		
	}
	kprintf("Writing completed!");	
//	read(&abar->ports[sata],0,0,1,(uint16_t *)newBuf);
kprintf("Reading from disk");
for(int sec=0;sec < 800;sec ++)
{
read(&abar->ports[sata],sec,0,1,(uint16_t *)newBuf);
uint8_t * readPtr = (uint8_t *)newBuf;
if(sec%8==0)
kprintf("|Sector %d:",sec/8);
kprintf("%x,",*readPtr);
}
kprintf("Reading completed!");
}
void resetGHC(hba_mem_t *abar)
{
	abar->ghc |= HBA_GHC_AE;
	abar->ghc |= HBA_GHC_IE;
	abar->ghc |= HBA_GHC_HR;
}
// Check device type
static int check_type(hba_port_t *port)
{
	DWORD ssts = port->ssts;
 
	BYTE ipm = (ssts >> 8) & 0x0F;
	BYTE det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}

uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    /* create configuration address as per Figure 1 */

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */

    sysOutLong (0xCF8, address);

    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */

    tmp = (uint16_t)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}


uint32_t ReadWord (uint8_t bus, uint8_t slot,uint8_t func,uint8_t  offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;

    /* create configuration address as per Figure 1 */

    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */

    sysOutLong (0xCF8, address);
    sysOutLong (0xCFC , 0x3EBF1000);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */

    tmp = (uint32_t)(sysInLong (0xCFC));
    return (tmp);
}


static inline void sysOutLong( unsigned short port, uint32_t val ){
    __asm__ volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline uint32_t sysInLong( unsigned short port ){
    uint32_t ret;
    __asm__ volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

uint64_t checkAllBuses() {
    uint8_t bus;
    uint8_t slot;
    uint16_t tmp;
    uint8_t subClass;
    uint8_t baseClass;
    uint16_t vendorID; 
    uint16_t deviceID;
   // uint64_t bar5;
  //  unsigned short vendor,device;
    for(bus = 0; bus < 255; bus++) {
        for(slot = 0; slot < 32; slot++) {
		vendorID=pciConfigReadWord(bus,slot,0,0);
		deviceID=pciConfigReadWord(bus,slot,0,0x02);
		tmp = pciConfigReadWord(bus,slot,0,0x0A);
		baseClass = (tmp & 0xFF00)>>8;
		subClass = (tmp & 0x00FF) ;
		if(subClass == 0x06 && baseClass == 0x01){
		kprintf("Vendor ID : %d, Device ID : %d\n",(int)vendorID,(int)deviceID);
		kprintf("AHCI Found at Bus : %d , Slot : %d \n",(int)bus,(int)slot);
		bar5 = ReadWord(bus,slot,0,0x24);
		//kprintf("%x\n",(uint64_t)bar5);
		probe_port((HBA_MEM *)(0xFFFFFFFF00000000+(uintptr_t)bar5));
//		return 0;
		} 		

            }
        }
    return 0;
}


void port_rebase(hba_port_t *port, int portno)
{
	stop_cmd(port);	// Stop command engine
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno<<10);
	//port->clbu = 0;
	memset((void*)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port	
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
	//port->fbu = 0;
	memset((void*)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		//cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}
 
	start_cmd(port);	// Start command engine
}


// Start command engine


void start_cmd(hba_port_t *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(hba_port_t *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
}
int read(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
	port->is_rwc = (DWORD)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return FALSE;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_TYPE_REG_H2D)/sizeof(DWORD);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 8K bytes (16 sectors) per PRDT
	int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl>>8);
	cmdfis->lba2 = (BYTE)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (BYTE)(startl>>24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth>>8);
 
	cmdfis->count = count;
	cmdfis->count = count;
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return FALSE;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return FALSE;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return FALSE;
	}
 
	return TRUE;
}
 
// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
	// If not set in SACT and CI, the slot is free
	DWORD slots = (port->sact | port->ci);
	int cmdslots= (abar->cap & 0x0f00)>>8 ; // Bit 8-12	
	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
}

int ahciwrite(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
    //   kprintf("Inside Write Function");
        port->is_rwc = (DWORD)-1;               // Clear pending interrupt bits
        int spin = 0; // Spin lock timeout counter
        int slot = find_cmdslot(port);
        if (slot == -1)
                return FALSE;

        hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
        cmdheader += slot;
        cmdheader->cfl = sizeof(FIS_TYPE_REG_H2D)/sizeof(DWORD);        // Command FIS size
        cmdheader->w = 1;               // write from device
        cmdheader->prdtl = (WORD)((count-1)>>4) + 1;    // PRDT entries count

        hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
        memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
                (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

        // 8K bytes (16 sectors) per PRDT
        int i;
        for (i=0; i<cmdheader->prdtl-1; i++)
        {
                cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
                cmdtbl->prdt_entry[i].dbc = 8*1024;     // 8K bytes
                cmdtbl->prdt_entry[i].i = 1;
                buf += 4*1024;  // 4K words
                count -= 16;    // 16 sectors
        }
        // Last entry
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = count<<9;   // 512 bytes per sector
        cmdtbl->prdt_entry[i].i = 1;

        // Setup command
        fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);

        cmdfis->fis_type = FIS_TYPE_REG_H2D;
        cmdfis->c = 1;  // Command
        cmdfis->command = ATA_CMD_WRITE_DMA_EX;

        cmdfis->lba0 = (BYTE)startl;
        cmdfis->lba1 = (BYTE)(startl>>8);
        cmdfis->lba2 = (BYTE)(startl>>16);
        cmdfis->device = 1<<6;  // LBA mode

        cmdfis->lba3 = (BYTE)(startl>>24);
        cmdfis->lba4 = (BYTE)starth;
        cmdfis->lba5 = (BYTE)(starth>>8);

        cmdfis->count = count;
        cmdfis->count = count;
        // The below loop waits until the port is no longer busy before issuing a new command
        while ((port->tfd & (ATA_STATUS_BSY | ATA_STATUS_DRQ)) && spin < 1000000)
        {
                spin++;
        }
        if (spin == 1000000)
        {
                kprintf("Port is hung\n");
                return FALSE;
        }

        port->ci = 1<<slot;     // Issue command

        // Wait for completion
        while (1)
        {
                // In some longer duration reads, it may be helpful to spin on the DPS bit 
                // in the PxIS port field as well (1 << 5)
                if ((port->ci & (1<<slot)) == 0)
                        break;
                if (port->is_rwc & HBA_PxIS_TFES)       // Task file error
                {
                        kprintf("Write disk error\n");
                        return FALSE;
                }
        }

        // Check again
        if (port->is_rwc & HBA_PxIS_TFES)
        {
                kprintf("Write disk error\n");
                return FALSE;
        }
        return TRUE;
//   kprintf("Written to Disk");

}


