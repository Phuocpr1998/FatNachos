#include "copyright.h"
#include "bootsector.h"
#include "FATTable.h"
#include "directory.h"
#include "system.h"
#include "disk.h"

BootSector::BootSector(short byteOfSector, char sectorOfCluster, short numSectorPreFat, int numSectorVolume)
{
	// khởi tạo thông tin
	init();

	this->bootInfo.byteOfSector = byteOfSector;
	this->bootInfo.sectorOfCluster = sectorOfCluster;
	this->bootInfo.numSectorPreFat = numSectorPreFat;

	if (numSectorVolume <= 32767)
		this->bootInfo.numSectorVolumeL = (short)numSectorVolume;
	else
		this->bootInfo.numSectorVolumeH = numSectorVolume;
}

BootSector::BootSector(const BootSector & bootSector)
{
	strcpy(this->bootInfo.bootStep, bootSector.bootInfo.bootStep);
	this->bootInfo.byteOfSector = bootSector.bootInfo.byteOfSector;
	strcpy(this->bootInfo.nameCompany, bootSector.bootInfo.nameCompany);
	this->bootInfo.nu = bootSector.bootInfo.nu;
	this->bootInfo.numBytes = bootSector.bootInfo.numBytes;
	this->bootInfo.numEntry = bootSector.bootInfo.numEntry;
	this->bootInfo.numFat = bootSector.bootInfo.numFat;
	this->bootInfo.numReadHead = bootSector.bootInfo.numReadHead;
	this->bootInfo.numSectorFAT = bootSector.bootInfo.numSectorFAT;
	this->bootInfo.numSectorPreFat = bootSector.bootInfo.numSectorPreFat;
	this->bootInfo.numSectorTrack = bootSector.bootInfo.numSectorTrack;
	this->bootInfo.numSectorVolumeH = bootSector.bootInfo.numSectorVolumeH;
	this->bootInfo.numSectorVolumeL = bootSector.bootInfo.numSectorVolumeL;
	strcpy(this->bootInfo.pushOS, bootSector.bootInfo.pushOS);
	this->bootInfo.sectorOfCluster = bootSector.bootInfo.sectorOfCluster;
	this->bootInfo.serialNumber = bootSector.bootInfo.serialNumber;
	this->bootInfo.signEndBootInfo = bootSector.bootInfo.signEndBootInfo;
	this->bootInfo.signOS = bootSector.bootInfo.signOS;
	this->bootInfo.symVolume = bootSector.bootInfo.symVolume;
	this->bootInfo.type = bootSector.bootInfo.type;
	strcpy(this->bootInfo.typeFAT, bootSector.bootInfo.typeFAT);
	strcpy(this->bootInfo.volumeLabel, bootSector.bootInfo.volumeLabel);
}

void BootSector::format()
{
	// cài đặt thông số cho bootsector
	strcpy(this->bootInfo.nameCompany, COMPANYNAME);

	this->bootInfo.numFat = NUMFAT;
	this->bootInfo.numEntry = DIRSIZE;
	this->bootInfo.numSectorFAT = NUMSECTORFAT;
	strcpy(this->bootInfo.typeFAT, FATType);
	this->bootInfo.signEndBootInfo = ENDBOOTSECTOR;

	FATTable *fatTable = new FATTable();
	Directory *directory = new Directory();

	// ghi bảng FAT xuống đĩa
	fatTable->Write();
	// ghi bảng thư mục xuống đĩa
	directory->Write();
	// lưu xuống đĩa
	this->Write();

	delete fatTable;
	delete directory;
}

bool BootSector::checkFormat()
{
	Fetch();
	if (strcmp(this->bootInfo.typeFAT, FATType) == 0)
		return true;
	return false;
}

void BootSector::Write()
{
	DEBUG('f', "Write BOOTSECTOR at sector: %d\n", SectorBoot);
	synchDisk->WriteSector(SectorBoot, (char*)&bootInfo);
}

void BootSector::Fetch()
{
	DEBUG('f', "Write BOOTSECTOR at sector: %d\n", SectorBoot);
	synchDisk->ReadSector(SectorBoot, (char*)&bootInfo);
}

void BootSector::init()
{
	strcpy(bootInfo.bootStep, "");
	strcpy(bootInfo.nameCompany, "");
	bootInfo.byteOfSector = 0;
	bootInfo.nu = 0;
	bootInfo.numBytes = 0;
	bootInfo.numEntry = 0;
	bootInfo.numFat = 0;
	bootInfo.numReadHead = 0;
	bootInfo.numSectorFAT = 0;
	bootInfo.numSectorPreFat = 0;
	bootInfo.numSectorTrack = 0;
	bootInfo.numSectorVolumeH = 0;
	bootInfo.numSectorVolumeL = 0;
	strcpy(bootInfo.pushOS, "");
	bootInfo.sectorOfCluster = 0;
	bootInfo.serialNumber = 0;
	bootInfo.signEndBootInfo = 0;
	bootInfo.signOS = 0;
	bootInfo.symVolume = 0;
	bootInfo.type = 0;
	strcpy(bootInfo.typeFAT, "");
	strcpy(bootInfo.volumeLabel, "");
}

BootInfo::BootInfo(const BootInfo & bootInfo)
{
	strcpy(this->bootStep, bootInfo.bootStep);
	this->byteOfSector = bootInfo.byteOfSector;
	strcpy(this->nameCompany, bootInfo.nameCompany);
	this->nu = bootInfo.nu;
	this->numBytes = bootInfo.numBytes;
	this->numEntry = bootInfo.numEntry;
	this->numFat = bootInfo.numFat;
	this->numReadHead = bootInfo.numReadHead;
	this->numSectorFAT = bootInfo.numSectorFAT;
	this->numSectorPreFat = bootInfo.numSectorPreFat;
	this->numSectorTrack = bootInfo.numSectorTrack;
	this->numSectorVolumeH = bootInfo.numSectorVolumeH;
	this->numSectorVolumeL = bootInfo.numSectorVolumeL;
	strcpy(this->pushOS, bootInfo.pushOS);
	this->sectorOfCluster = bootInfo.sectorOfCluster;
	this->serialNumber = bootInfo.serialNumber;
	this->signEndBootInfo = bootInfo.signEndBootInfo;
	this->signOS = bootInfo.signOS;
	this->symVolume = bootInfo.symVolume;
	this->type = bootInfo.type;
	strcpy(this->typeFAT, bootInfo.typeFAT);
	strcpy(this->volumeLabel, bootInfo.volumeLabel);
}

void BootInfo::operator=(const BootInfo & bootInfo)
{
	strcpy(this->bootStep, bootInfo.bootStep);
	this->byteOfSector = bootInfo.byteOfSector;
	strcpy(this->nameCompany, bootInfo.nameCompany);
	this->nu = bootInfo.nu;
	this->numBytes = bootInfo.numBytes;
	this->numEntry = bootInfo.numEntry;
	this->numFat = bootInfo.numFat;
	this->numReadHead = bootInfo.numReadHead;
	this->numSectorFAT = bootInfo.numSectorFAT;
	this->numSectorPreFat = bootInfo.numSectorPreFat;
	this->numSectorTrack = bootInfo.numSectorTrack;
	this->numSectorVolumeH = bootInfo.numSectorVolumeH;
	this->numSectorVolumeL = bootInfo.numSectorVolumeL;
	strcpy(this->pushOS, bootInfo.pushOS);
	this->sectorOfCluster = bootInfo.sectorOfCluster;
	this->serialNumber = bootInfo.serialNumber;
	this->signEndBootInfo = bootInfo.signEndBootInfo;
	this->signOS = bootInfo.signOS;
	this->symVolume = bootInfo.symVolume;
	this->type = bootInfo.type;
	strcpy(this->typeFAT, bootInfo.typeFAT);
	strcpy(this->volumeLabel, bootInfo.volumeLabel);
}
