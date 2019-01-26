#include "copyright.h"
#include "FATTable.h"
#include "disk.h"
#include "system.h"


FATTable::FATTable()
{
	//bỏ hai phần tử FAT 0,1
	this->fatItems[0] = -2;
	this->fatItems[1] = -2;
	for (int i = 2; i < FATSIZE; i++)
	{
		fatItems[i] = 0;
	}
}

FATTable::FATTable(const FATTable & fatTable)
{
	for (int i = 0; i < FATSIZE; i++)
	{
		this->fatItems[i] = fatTable.fatItems[i];
	}
}

bool FATTable::Add(int cluster, int bytes)
{
	int n = bytes / (SECTOROFCLUSTER*SectorSize);

	if (n == 0)
	{
		fatItems[cluster] = EOF;
	}
	else
	{
		if (n + cluster > FATSIZE)
			return false; // không đủ bộ nhớ

		int i = cluster;
		for (; i < cluster + n; i++)
		{
			if (fatItems[i] == 0)
			{
				fatItems[i] = i + 1;
			}
			else
			{
				return false; // không đủ bộ nhớ
			}
		}
		fatItems[i] = EOF;
	}
	return true;
}

bool FATTable::Remove(int cluster)
{
	int index = cluster;
	while (true)
	{
		if (fatItems[index] == EOF)
		{
			fatItems[index] = 0;
			break;
		}

		int tmp = fatItems[index];
		fatItems[index] = 0;
		index = tmp;
	}

	return 0;
}


int FATTable::Find()
{
	if (fatItems[FATSIZE - 1] != 0)
	{
		return -1;
	}

	for (int i = FATSIZE - 1; i > 0; i--)
	{
		if (fatItems[i] != 0 && fatItems[i + 1] == 0)
			return i + 1;
	}
	DEBUG('f', "NOT FOUND: %d\n", FATSIZE);
	for (int i = 0; i < FATSIZE; i++)
	{
		DEBUG('f', "NOT FOUND: %d\n", this->fatItems[i]);
	}
	return -1;
}

bool FATTable::AddNext(int cluster, int clusterNext)
{
	if (fatItems[cluster] == 0 || fatItems[cluster] == EOF)
	{
		fatItems[cluster] = clusterNext;
		return true;
	}
	return false;
}

short FATTable::GetNext(int posFAT)
{
	return fatItems[posFAT];
}

// đọc bảng FAT
void FATTable::Fetch()
{
	DEBUG('f', "FETCH FATTABLE at sector: %d\n", SECTORSTARTFAT);
	synchDisk->ReadSector(SECTORSTARTFAT, (char*) &fatItems);
}

//lưu bảng FAT
void FATTable::Write()
{
	DEBUG('f', "WRITE FATTABLE at sector: %d\n", SECTORSTARTFAT);
	synchDisk->WriteSector(SECTORSTARTFAT, (char*)&fatItems);
}
