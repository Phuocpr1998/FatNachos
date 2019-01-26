#include "copyright.h"
#include "directory.h"
#include "FATTable.h"
#include "system.h"
#include "disk.h"


Directory::Directory()
{
	for (int i = 0; i < DIRSIZE; i++)
	{
		strcpy(this->table[i].name, "");
	}
	/// vị trí bắt đầu vùng data
	this->sectorStartData = SECTORSTARTDIR + ((DIRSIZE*ENTRYSIZE) / SectorSize + 1) + 1;
}

Directory::Directory(const Directory & directory)
{
	for (int i = 0; i < DIRSIZE; i++)
	{
		copyEntry(table[i], directory.table[i]);
	}
}

int Directory::Find(char *name)
{
	for (int i = 0; i < DIRSIZE; i++)
	{
		if (strCompare(table[i].name, name))
		{
			return table[i].lStartCluster;
		}
	}
	return -1;
}

int Directory::GetSize(char *name)
{
	if (name == NULL)
		return -1; // lỗi

	for (int i = 0; i < DIRSIZE; i++)
	{
		if (strCompare(table[i].name, name))
			return table[i].size;
	}
	return -1;

}

bool Directory::Add(char *name, int size)
{
	FATTable *fatTable = new FATTable();
	fatTable->Fetch();
	// kiểm tra tệp tin đã tồn tại hay chưa
	for (int i = 0; i < DIRSIZE; i++)
	{
		if (strCompare(table[i].name, name))
			return false;
	}

	// tìm phần tử FAT trống
	int cluster = fatTable->Find();
	if (cluster == -1)
	{
		return false;
	}

	// Thêm vào phần tử FAT
	if (!fatTable->Add(cluster, size))
		return false;

	int i = 0;
	for (i = 0; i < DIRSIZE; i++)
	{
		if (table[i].name[0] == 0)
			break;
	}

	if (i == DIRSIZE)
	{
		for (i = 0; i < DIRSIZE; i++)
		{
			if (table[i].name[0] == E5)
				break;
		}
		if (i == DIRSIZE)
		{
			DEBUG('f', "RDET full\n");
			return false;
		}
	}
	int len = strlen(name);
	if (len > MAXLENGHTFILENAME) // tên vượt quá giới hạn lưu trữ
	{
		char count = (char)CountFile(name) + '0';
		// sao chép MAXLENGHTSHORT vào tên file
		int j = 0;
		for (j = 0; j < MAXLENGHTSHORT; j++)
		{
			table[i].name[j] = name[j];
		}
		// sau đó thêm ký tự đánh dấu vào
		table[i].name[MAXLENGHTSHORT] = '~';
		table[i].name[MAXLENGHTSHORT + 1] = count;
		// lấy phần mở rộng, giới hạn phần mở rộng chỉ có 3 ký tự
		for (; j < len; j++)
		{
			if (name[j] == '.')
				break;
		}

		// kiểm tra xem file có phần mở rộng hay không
		if (j != len)
		{
			// vì có dấu chấm nên ta sẽ công cho 1
			int k;
			for (k = 0; k < MAXLENGHTEXTENSION + 1; k++)
			{
				table[i].name[MAXLENGHTSHORT + 2 + k] = name[j + k];
			}
		}

		// đổi lại tên bên ngoài truyền vào ứng với tên file thật tế
		strcpy(name, table[i].name);
	}
	else
	{
		strcpy(table[i].name, name);
	}
	
	table[i].type = FILETYPE; // tập tin 
	table[i].lStartCluster = cluster;
	table[i].size = size;

	
	Write(); // lưu cây thư mục
	fatTable->Write(); // lưu bảng FAT
	return true;
}

bool Directory::Remove(char *name)
{
	FATTable *fatTable = new FATTable();
	fatTable->Fetch();

	if (name == NULL)
		return -1;

	for (int i = 0; i < DIRSIZE; i++)
	{
		if (strCompare(table[i].name, name)) // tìm thấy
		{
			table[i].name[0] = E5; // đánh dấu xóa
			fatTable->Remove(table[i].lStartCluster);//xóa các phần tử FAT
			
			fatTable->Write(); // lưu bảng FAT
			Write(); // lưu cây thư mục
			delete fatTable;
			return true;
		}
	}
	//tệp tin không tồn tại
	return false;
}

void Directory::List()
{
	for (int i = 0; i < DIRSIZE; i++)
	{
		if (table[i].name[0] != 0 && table[i].name[0] != (char)E5)
		{
			printf("%s\n", table[i].name);
		}
	}
}

void Directory::Print()
{
	printf("name\t\tsize\n");
	for (int i = 0; i < DIRSIZE; i++)
	{
		if (table[i].name[0] != 0 && table[i].name[0] != (char)E5)
		{
			printf("%s\t\t%d\n", table[i].name, table[i].size);
		}
	}
}


int Directory::GetSectorStartData()
{
	return sectorStartData;
}

// đọc bảng FAT
void Directory::Fetch()
{
	DEBUG('f', "WRITE DIRECTORYTABLE at sector: %d\n", SECTORSTARTDIR);
	synchDisk->ReadSector(SECTORSTARTDIR, (char *)table);
}

//lưu bảng FAT
void Directory::Write()
{
	DEBUG('f', "WRITE DIRECTORYTABLE at sector: %d\n", SECTORSTARTDIR);
	synchDisk->WriteSector(SECTORSTARTDIR, (char *)table);
}

int Directory::CountFile(char * name)
{
	int count = 0;
	for (int i = 0; i < DIRSIZE; i++)
	{
		if (table[i].name[0] != 0 && table[i].name[0] != (char)E5)
		{
			bool isSame = true;
			// kiểm tra MAXLENGHTSHORT ký tự đầu của hai tên nếu giống nhau coi như trùng
			for (int j = 0; j < MAXLENGHTSHORT; j++)
			{
				if (table[i].name[j] != name[j])
				{
					isSame = false;
					break;
				}
			}
			if (isSame)
				count++;
		}
	}
	return count;
}

bool Directory::strCompare(const char * src, const char * des)
{
	int lenS = strlen(src);
	int lenD = strlen(des);

	if (lenS != lenD && (lenS >= MAXLENGHTFILENAME || lenD >= MAXLENGHTFILENAME)) {
		for (int i = 0; i < MAXLENGHTFILENAME; i++)
		{
			if (src[i] != des[i])
				return false;
		}
		return true;
	}
	else
	{
		if (strcmp(src, des) == 0)
			return true;
		return false;
	}
}

void Directory::copyEntry(DirectoryEntry & des, const DirectoryEntry & src)
{
	//des.C = src.C;
	des.date = src.date;
	strcpy(des.hour, src.hour);
	des.hStartCluster = src.hStartCluster;
	des.lastDateAccess = src.lastDateAccess;
	des.lastHourAccess = src.lastHourAccess;
	des.lastUpdate = src.lastUpdate;
	des.lStartCluster = src.lStartCluster;
	strcpy(des.name, src.name);
	des.size = src.size;
	des.type = src.type;
}
