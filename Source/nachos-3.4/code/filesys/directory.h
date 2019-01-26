#ifndef DIRECTORY_FAT_H
#define DIRECTORY_FAT_H
#include "copyright.h"
#include "FATTable.h"


#define E5 229
#define FILETYPE 0x20
// số lượng tập tin trong ổ đĩa
#define DIRSIZE 10
#define ENTRYSIZE 32
#define SECTORSTARTDIR 2
#define MAXLENGHTFILENAME 12
#define MAXLENGHTSHORT 6
#define MAXLENGHTEXTENSION 3

class DirectoryEntry {
public:
	char name[MAXLENGHTFILENAME]; // tích hợp cả phần mở rộng
	unsigned char type; // trạng thái và loại tập tin
	//unsigned char C;// phần dành riêng vì ko sử dụng nên chuyển 1 byte lên name để lưu dấu chấm
	char hour[3]; // giờ tạo
	unsigned short date; // ngày tạo
	unsigned short lastDateAccess;// ngày truy cập gần nhất
	unsigned short hStartCluster; // cluster bắt đầu (byte cao) // hiện tại chỉ dùng byte thấp chưa dùng tới byte cao
	unsigned short lastHourAccess; // giờ truy cập gần nhất
	unsigned short lastUpdate; // ngày cập nhật gần nhất
	unsigned short lStartCluster; // cluster bắt đầu (byte thấp)
	unsigned int size; // kích thước tập tin
};


class Directory {
private:
	DirectoryEntry table[DIRSIZE];		// Table of entry

	int sectorStartData; //sector bắt đầu vùng data
	
public:
	Directory();
	Directory(const Directory &directory);

	// Tìm cluster bắt đầu của tạp tin tên name
	// nếu không tìm thấy trả về -1
	int Find(char *name); 

	// lấy kích thước của tập tin với tên "name"
	int GetSize(char *name); 

	// lấy sector bắt đầu vùng data
	int GetSectorStartData();

	bool Add(char *name, int size);  // Thêm một file

	bool Remove(char *name);		// Xóa file

	void List();			// In ra danh sách tên file có trong ổ đĩa
	void Print(); // xuất ra tên file và dung lượng của file

	void Fetch(); // đọc bản thư mục gốc lên từ đĩa
	void Write(); // ghi bản thư mục gốc xuống đĩa
private:
	int CountFile(char * name); // hàm giúp đếm số lượng tập tin có tên trùng
	bool strCompare(const char *src, const char *des);
	void copyEntry(DirectoryEntry &des, const DirectoryEntry &src);
};

#endif