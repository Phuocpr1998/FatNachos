// openfile.h 
//	Data structures for opening, closing, reading and writing to 
//	individual files.  The operations supported are similar to
//	the UNIX ones -- type 'man open' to the UNIX prompt.
//
//	There are two implementations.  One is a "STUB" that directly
//	turns the file operations into the underlying UNIX operations.
//	(cf. comment in filesys.h).
//
//	The other is the "real" implementation, that turns these
//	operations into read and write disk sector requests. 
//	In this baseline implementation of the file system, we don't 
//	worry about concurrent accesses to the file system
//	by different threads -- this is part of the assignment.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef OPENFILE_H
#define OPENFILE_H

#include "copyright.h"
#include "utility.h"

#ifdef FILESYS_STUB			// Temporarily implement calls to 
					// Nachos file system as calls to UNIX!
					// See definitions listed under #else
class OpenFile {
  public:
    OpenFile(int f) { file = f; currentOffset = 0; }	// open the file
    ~OpenFile() { Close(file); }			// close the file

    int ReadAt(char *into, int numBytes, int position) { 
    		Lseek(file, position, 0); 
		return ReadPartial(file, into, numBytes); 
		}	
    int WriteAt(char *from, int numBytes, int position) { 
    		Lseek(file, position, 0); 
		WriteFile(file, from, numBytes); 
		return numBytes;
		}	
    int Read(char *into, int numBytes) {
		int numRead = ReadAt(into, numBytes, currentOffset); 
		currentOffset += numRead;
		return numRead;
    		}
    int Write(char *from, int numBytes) {
		int numWritten = WriteAt(from, numBytes, currentOffset); 
		currentOffset += numWritten;
		return numWritten;
		}

    int Length() { Lseek(file, 0, 2); return Tell(file); }
    
  private:
    int file;
    int currentOffset;
};

#else // FILESYS
#include "disk.h"

class FATTable;
class Directory;

class OpenFile
{
private:
	int seekPosition; // vị trí đọc file
	char *name;
	int clusterStart;
	int sectorOfCluster;
	int sectorData;
	int size;

	Directory *directory;
	FATTable *fatTable;
public:
	// mở file theo tên
	OpenFile(char * name);

	// ghi data xuống đĩa với data có độ dài numBytes
	int Write(char * data, int numBytes);
	// đọc data từ đĩa lên lưu vào data có độ dài numBytes
	int Read(char *data, int numBytes);
	// dịch chuyển con trỏ tới vị trí pos
	void Seek(int pos);
	// Trả về kích thước của tập tin
	int GetSize();

	// Read/write bytes from the file,
	// bypassing the implicit position.
	int ReadAt(char *data, int numBytes, int position);
	int WriteAt(char *data, int numBytes, int position);

	~OpenFile()
	{
		if (fatTable != NULL)
			delete fatTable;
		if (directory != NULL)
			delete directory;
	}
};


#endif // FILESYS

#endif // OPENFILE_H
