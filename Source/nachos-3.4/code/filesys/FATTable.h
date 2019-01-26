#ifndef _FAT_TABLE_H
#define _FAT_TABLE_H
#include "copyright.h"
#include "disk.h"

#define NUMFAT 1
#define NUMSECTORFAT 1
#define SECTOROFCLUSTER 2
#define SECTORSTARTFAT 1
#define FATSIZE (NUMSECTORFAT*SectorSize)/2

// ở đây chỉ dùng 1 bản FAT
class FATTable
{
private: //số phần tử FAT và mỗi phần tử FAT sẽ có 2 byte	
	short fatItems[FATSIZE];
	
public:

	FATTable();
	FATTable(const FATTable &fatTable);

	bool Add(int cluster, int bytes); // cluster bắt đầu
									// dung lượng của tập tin đó
	// 
	bool AddNext(int cluster, int clusterNext);
	
	bool Remove(int cluster); //cluster bắt đầu

	int Find(); // trả về vị trí mà phần tử FAT còn trống
				// return -1 là hết vị trí cần tìm
	short GetNext(int posFAT); // trả về giá trị trong bảng FAT tại vị trí posFAT

	void Fetch(); 	// đọc bảng FAT từ đĩa lên tại vị trí sectorStart
	void Write(); // ghi bảng FAT xuống đĩa tại vị trí sectorStart

};

#endif // !_FAT_TABLE_H
