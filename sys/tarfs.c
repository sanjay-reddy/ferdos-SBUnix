#include <sys/defs.h>
#include <sys/tarfs.h>
#include <sys/string.h>
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
}
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
}
