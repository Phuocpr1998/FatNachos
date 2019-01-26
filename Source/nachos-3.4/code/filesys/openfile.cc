#include "openfile.h"
#include "directory.h"
#include "FATTable.h"
#include "system.h"

OpenFile::OpenFile(char * name)
{
	this->directory = new Directory();
	this->fatTable = new FATTable();

	this->directory->Fetch();
	
	this->size = directory->GetSize(name);
	this->name = name;
	this->seekPosition = 0;
	this->sectorOfCluster = SECTOROFCLUSTER;
	this->clusterStart = directory->Find(name);
	this->sectorData = directory->GetSectorStartData();
	this->directory->Write();
}

// ghi data xuống đĩa với data có độ dài numBytes
int OpenFile::Write(char * data, int numBytes)
{
	this->fatTable->Fetch();
	DEBUG('f', "FETCH OK\n");
	int result = WriteAt(data, numBytes, seekPosition);
	seekPosition += result;
	return result;
}
// đọc data từ đĩa lên lưu vào data có độ dài numBytes
int OpenFile::Read(char *data, int numBytes)
{
	if (seekPosition >= size)
		return 0;
	this->fatTable->Fetch();
	int result = ReadAt(data, numBytes, seekPosition);
	seekPosition += result;
	return result;
}

void OpenFile::Seek(int pos)
{
	this->seekPosition = pos;
}
// Trả về kích thước của tập tin
int OpenFile::GetSize()
{
	this->directory->Fetch();
	return directory->GetSize(name);
}


int OpenFile::ReadAt(char *data, int numBytes, int position)
{
	int numCluster, mClusterStart;
	char *buf;
	int clusterSize = SectorSize * this->sectorOfCluster;

	if (numBytes <= 0)
		return 0;

	// tính số cluster phải đọc
	numCluster = divRoundUp(position%clusterSize + numBytes, clusterSize);

	// cluster bắt đầu đọc
	int pos = divRoundDown(position, clusterSize);
	mClusterStart = this->clusterStart;
	for (int i = 0; i < pos; i++)
	{
		mClusterStart = fatTable->GetNext(mClusterStart);
	}

	// tạo buffer lưu data
	buf = new char[numCluster*clusterSize];

	int indexSec = 0;
	for (int i = 0; i < numCluster; i++)
	{
		if (mClusterStart == EOF)
			break;
		DEBUG('f', "READ FILE... at cluster: %d\n", mClusterStart);

		for (int j = 0; j < this->sectorOfCluster; j++)
		{
			synchDisk->ReadSector(this->sectorData + (mClusterStart - 2)*this->sectorOfCluster + j, &buf[indexSec * SectorSize]);
			DEBUG('f', "READ FILE... at sector: %d\n", this->sectorData + (mClusterStart - 2)*this->sectorOfCluster + j);
			indexSec++;
		}
		mClusterStart = fatTable->GetNext(mClusterStart);
	}

	// sao chép data phần cần lấy
	bcopy(&buf[position%clusterSize], data, numBytes);
	delete[] buf;
	return numBytes;
}

int OpenFile::WriteAt(char *data, int numBytes, int position)
{
	int numCluster, mClusterStart;
	char *buf;
	int clusterSize = SectorSize * this->sectorOfCluster;

	if (numBytes <= 0)
		return 0;

	// tính số cluster phải ghi
	numCluster = divRoundUp(position%clusterSize + numBytes, clusterSize);

	// cluster bắt đầu ghi
	int pos = divRoundDown(position, clusterSize);
	mClusterStart = this->clusterStart;
	for (int i = 0; i < pos; i++)
	{
		mClusterStart = fatTable->GetNext(mClusterStart);
	}

	DEBUG('f', "Init OK\n");
	// tạo buffer lưu data
	buf = new char[numCluster * clusterSize];
	// đưa tất cả các phần tử của buf về 0
	for (int i = 0; i < numCluster * clusterSize;buf[i] = 0, i++);

	DEBUG('f', "NEW OK: %d\n", numCluster * clusterSize);
	// trường hợp vị trí ghi không phải nằm ở đầu cluster
	// thì ta phải đọc dữ liệu trước đó trước khi add dữ liệu mới vào
	int firstAligned = position % clusterSize;
	if (firstAligned != 0)
	{
		ReadAt(buf,clusterSize , (position / clusterSize)*clusterSize);
		DEBUG('f',"READ OK: %d\n ", (position / clusterSize)*clusterSize);
	}

	// sao chép data phần cần ghi xuống
	bcopy(data, &buf[position % clusterSize], numBytes);
	DEBUG('f', "COPY OK: %s\n", buf);
	
	int indexSec = 0, cluster1 = mClusterStart;
	DEBUG('f', "SAVING... %d\n", mClusterStart);
	for (int i = 0; i < numCluster; i++)
	{
		// cấp thêm phần tử FAT nếu file lớn
		if (cluster1 == EOF && i != numCluster -1)
		{
			int cluster2 = fatTable->Find();
			if (cluster2 == -1) // hết vùng trống
			{
				fatTable->Write();
				delete[] buf;
				return (i - 1)*clusterSize;
			}

			fatTable->AddNext(cluster2, EOF);
			fatTable->AddNext(cluster1, cluster2);
		}

		DEBUG('f',"WRITE AT CLUSTER: %d\n", mClusterStart);

		for (int j = 0; j < this->sectorOfCluster; j++)
		{
			synchDisk->WriteSector(this->sectorData + (cluster1 - 2)*this->sectorOfCluster + j, &buf[indexSec * SectorSize]);
			DEBUG('f', "WRITE FILE... at sector: %d\n", this->sectorData + (cluster1 - 2)*this->sectorOfCluster + j);
			indexSec++;
		}
		
		// chuyển tới cluster tiếp theo
		cluster1 = fatTable->GetNext(cluster1);
	}
	DEBUG('f', "COPY FILE OK\n");
	// lưu lại bảng FAT
	fatTable->Write();
	DEBUG('f', "SAVE FAT OK\n");
	delete[] buf;
	return numBytes;
}