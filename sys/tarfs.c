#include <sys/defs.h>
#include <sys/tarfs.h>
#include <string.h>
#include <sys/elf64.h>
#include <sys/kprintf.h>
unsigned int getSize(const char *input)
{   
    unsigned int size = 0;
    unsigned int k;
    unsigned int count = 1;
    
    for (k = 11; k > 0; k--, count *= 8){
        size += ((input[k - 1] - '0') * count);
    }
    
    return size;
}/*
void tarfs_init()
{       
        uint64_t address= (uint64_t)&_binary_tarfs_start;
        unsigned int i;
        for(i =0 ; address <(uint64_t)&_binary_tarfs_end ;i++){
                struct posix_header_ustar *header = (struct posix_header_ustar*)address;
                unsigned int size = getSize(header->size);
                strcpy(tarfs_fs[i].name,header->name);
                tarfs_fs[i].size = size;            
                tarfs_fs[i].typeflag = atoi(header->typeflag);
                tarfs_fs[i].addr_hdr = address;
                if(tarfs_ei.typeflag == FILE_TYPE)
                        tarfs_e.par_ind = get_per_ind_file(&(tarfs_e.name[0]));
                else if(tarfs_e.typeflag == DIRECTORY)
                {
                        tarfs_e.par_ind = get_per_ind(&(tarfs_e.name[0]));
                        // curr_dir = i;
                }     
                address += ((size/512) +1) * 512;
                if(size %512)
                        address+=512;
        }
}*/
uint64_t getElfHeader(char * fileName)
{
        uint64_t address= (uint64_t)&_binary_tarfs_start; // starting from the first binary tarfs address 
        unsigned int i;
        for(i =0 ; address <(uint64_t)&_binary_tarfs_end ;i++){ // traversing till the end of the tarfs header address to find the address we need.
                struct posix_header_ustar *header = (struct posix_header_ustar*)address;
                unsigned int size = getSize(header->size);
                if(strcmp(header->name,fileName)==0) 
                {
                        return (uint64_t) (header+1); // the elf header sits next to the tarf header. So returning the address of the elf header.
                }
                address += ((size/512) +1) * 512; // rounding off the address.
                if(size %512)
                        address+=512;
        }
return (uint64_t)0;
}/*
uint64_t tarfs_open_file(char * filename)
{       
        int i=0;
        while(1)
        {       
                if(tarfs_fs[i].name==0) break;
                if(strcmp(&(tarfs_fs[i].name[0]), filename) == 0 && tarfs_fs[i].typeflag == FILE_TYPE)
                {       
                        return tarfs_fs[i].addr_hdr;
                }
        }
        kprintf("\n No such file ");
        kprintf("%s", filename);
        return 0;
}
int tarfs_read_file(uint64_t fd, int size, char *buf)
{       
        struct posix_header_ustar * header = (struct posix_header_ustar*)fd;
        unsigned int file_size = getSize(header->size);
        if(file_size<size)
        {       
                size = file_size;
        }
        int len = size;
        char *temp = (char *) buf;
        char* file_addr = (char *)(fd + 512);
        int i = 0;
        while(size-- > 0)
        {       
                temp[i++] = *file_addr++;
        }    
        temp[i]='\0';
        return len;
}*/
