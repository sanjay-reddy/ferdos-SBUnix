#ifndef _TARFS_H
#define _TARFS_H

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

#define DIRECTORY 5
#define FILE_TYPE 0
#define MAX_BIN 40

struct tarfs {
	char name[100];
	int file_size;
	char typeflag[1];
	int ref_cnt;
	uint64_t tarfs_start_addr;
	uint64_t bin_start_addr;
	}__attribute__((packed))tarfs_ind[MAX_BIN];

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

//tarfs struct actually used

typedef struct {
    char name[100];
    int size;
    int typeflag;
    uint64_t addr_hdr;
    int par_ind;
} tarfs_entry;

extern tarfs_entry tarfs_fs[100];

void tarfs_init();

//file handling functions

char filebuf[2048];
int num_files;
char cwd[1024];

uint64_t openfile(char * file);
int readfile(uint64_t file_addr, int size, uint64_t buf);
void closefile(uint64_t file_addr);

// directory handling functions

uint64_t opendirect(char * dir);
uint64_t readdirect(char* dir);
void closedirect(uint64_t dir_addr);

//elf functions

uint64_t getElfHeader(char * fileName);
struct file *tarfs_open(const char *, int, uint32_t, int);
#endif
