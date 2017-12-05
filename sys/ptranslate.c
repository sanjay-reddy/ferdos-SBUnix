/*#include<stdarg.h>
#include<sys/kprintf.h>
#include<sys/defs.h>

uint64_t virtual_addr;
uint64_t pml_val;
uint64_t pdp_val;
uint64_t pd_val;
uint64_t pg_val;

typedef struct{
	uint64_t pdp_index[512];
}pml4;

typedef struct{
	uint64_t pd_index[512];
}pdp;

typedef struct{
	uint64_t pt_idex[512];
}

void parse_v_addr(uint64_t v_addr){
	pml_val = (v_addr>>39)&0x1FF;
        pdp_val = (v_addr>>30)&0x1FF;
        pd_val  = (v_addr>>21)&0x1FF;
        pg_val  = (v_addr>>12)&0x1FF;
        pg_offset = (v_addr)&0x7FF;
}
*/

