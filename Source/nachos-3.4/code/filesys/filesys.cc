// filesys.cc 
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk 
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them 
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "bootsector.h"
#include "disk.h"
#include "filesys.h"
#include "openfile.h"

// xác định một số thông tin cơ bản cho bootsector
// vị trí của bảng FAT
#define FatTableSector	1
// vị trí của bản thư mục
#define SectorOfCluster	2

// số lượng sector của volume
// được lấy thông số từ bên machine
#define SectorVolume NumSectors

FileSystem::FileSystem(bool format)
{ 
	bootSector = new BootSector(SectorSize, SectorOfCluster, FatTableSector, SectorVolume);
	if (format == TRUE)
	{
		printf("FORMAT thanh cong\n");
		bootSector->format();
	}
	else if (!bootSector->checkFormat())
	{
		bootSector->format();
		//printf("FORMATING... OK\n");
		DEBUG('f',"FORMATING... OK\n");
	}
	else
	{
		//printf("FORMATED OK\n");
		DEBUG('f',"FORMATED OK\n");
	}
}


bool FileSystem::Create(char *name, int initialSize)
{
	Directory *directory = new Directory();

	directory->Fetch();

	if (directory->Add(name, initialSize))
	{
		delete directory;
		return true;
	}

	delete directory;
	return false;
}


OpenFile *FileSystem::Open(char *name)
{ 
	Directory *directory = new Directory();

	directory->Fetch();
	if (directory->Find(name) == -1)
		return NULL;
	OpenFile *openF = new OpenFile(name);
	return openF;
}


bool FileSystem::Remove(char *name)
{ 
	Directory *directory = new Directory();

	directory->Fetch();

	bool result = directory->Remove(name);
	delete directory;

	return result;
} 


void FileSystem::List()
{
	Directory *directory = new Directory();

	directory->Fetch();
	directory->List();

	delete directory;
}

void FileSystem::Print()
{
	Directory *directory = new Directory();

	directory->Fetch();
	directory->Print();
	delete directory;
}