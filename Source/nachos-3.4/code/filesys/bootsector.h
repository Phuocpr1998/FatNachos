#ifndef _BOOTSECTOR_H
#define _BOOTSECTOR_H

#include "copyright.h"
#include "FATTable.h"
#include "directory.h"
#include "disk.h"

// vùng boot sector được lưu ở sector thứ 0
#define SectorBoot 0
#define FATType "FAT16"
#define COMPANYNAME "1612523"
#define ENDBOOTSECTOR 0xAA55

// lưu thông tin của vùng boost sector
class BootInfo
{
public:

	BootInfo() {};
	BootInfo(const BootInfo& bootInfo);
	void operator=(const BootInfo& bootInfo);

	char bootStep[3]; // nhảy đến đầu đoạn mã boot
	char nameCompany[8]; // tên hệ điều hành
	
	short byteOfSector;// số byte của một sector
	char sectorOfCluster; // số sector của một cluster
	short numSectorPreFat; // số sector trước bảng FAT
	char numFat; // số lượng bảng FAT, thường là 2 nhưng ở đây chỉ dùng 1
	short numEntry; // số entry của RDET
	short numSectorVolumeL; // số sector của ổ đĩa, lưu giá trị khi số sector nhỏ hơn 2^16 - 1 = 65536
	
	char symVolume; // ký hiệu loại volume
	
	short numSectorFAT; // số sector của bảng FAT

	short numSectorTrack; //số sector của track
	short numReadHead; //số lượng đầu đọc
	int numBytes; // khoảng cách từ nơi mô tả vol đến đầu vol

	int numSectorVolumeH;// số sector của ổ đĩa, lưu giá trí lớn

	char type;// lưu loại ổ đĩa: mềm 0, cứng 80h
	char nu;// ký hiệu dành riêng
	char signOS; // dấu hiện nhận diện hệ điều hành
	int serialNumber;// serial Number của volume
	char volumeLabel[11]; // Volume Label 
	char typeFAT[8]; //loại FAT
	char pushOS[SectorSize - 70]; // đoạn mã nạp hệ điều hành
	short signEndBootInfo ; // mã kết thức AA55h
};

class BootSector 
{
private:
	BootInfo bootInfo;
public:
	BootSector(short byteOfSector, char sectorOfCluster, short numSectorPreFat, int numSectorVolume);
	BootSector(const BootSector& bootSector);

	void format(); // format ổ đĩa
	bool checkFormat(); // kiểm tra xem ổ đĩa đã được format theo FAT16 chưa

	// đọc và ghi thông tin BootSector trên đĩa
	void Write();
	void Fetch();

private:
	void init(); // set bootInfo = 0
};

#endif