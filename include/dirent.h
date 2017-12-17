#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255
#define RD_ONLY 1
#define WR_ONLY 2
#define RD_WR  3
#define stdin 0
#define stdout 1
#define stderr 2

struct dirent {
 char d_name[NAME_MAX+1];
}dirent;

struct dir {
//        file_t* node;
        uint64_t current;
//        dentry current_dentry;
	int fd;
	char buf[20];
}dir;


typedef struct DIR DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#endif

