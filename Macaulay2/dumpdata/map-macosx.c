#include <stdio.h>
#include <assert.h> 
#include <mach/mach.h>
#include <mach/mach_error.h>
#include "warning.h"
#include "map.h"
#include "file.h"
#include "std.h"

int haveDumpdata() {
  return TRUE;
}

#define count(x) (sizeof(x)/sizeof(int))

int nummaps () {
  int i=0, nesting_depth=0;
  vm_address_t address = 0;
  struct vm_region_submap_info_64 info;
  vm_size_t size;
  while (address < 0xc0000000) {
    int infoCnt = count(info);
    if (vm_region_recurse_64(mach_task_self(),&address,&size,&nesting_depth,(int *)&info,&infoCnt) != 0) 
      return ERROR;
    if (info.is_submap) {
      nesting_depth++;
    }
    else {
      i++;
      address += size;
    }
  }
  return i;
}

int getmaps(int nmaps, struct MAP maps[nmaps]) {
  int i=0, nesting_depth=0;
  vm_address_t address = 0;
  struct vm_region_submap_info_64 info;
  vm_size_t size;
  while (address < 0xc0000000) {
    int infoCnt = count(info);
    if (vm_region_recurse_64(mach_task_self(),&address,&size,&nesting_depth,(int *)&info,&infoCnt) != 0) 
      return ERROR;
    if (info.is_submap) {
      nesting_depth++;
    }
    else {
      assert(i < nmaps);
      maps[i].from = (void *)address;
      maps[i].to = (void *)(address + size);
      maps[i].r = (info.protection & VM_PROT_READ) != 0;
      maps[i].w = (info.protection & VM_PROT_WRITE) != 0;
      maps[i].x = (info.protection & VM_PROT_EXECUTE) != 0;
      maps[i].checksum = 0;
      i++;
      address += size;
    }
  }
  return OKAY;
}

int printmaps() {
  int i=0, nesting_depth=0;
  vm_address_t address = 0;
  struct vm_region_submap_info_64 info;
  vm_size_t size;
  while (address < 0xc0000000) {
    int infoCnt = count(info);
    if (vm_region_recurse_64(mach_task_self(),&address,&size,&nesting_depth,(int *)&info,&infoCnt) != 0) 
      return ERROR;
    if (info.is_submap) {
      nesting_depth++;
    }
    else {
      printf("%d: %08x-%08x %c%c%c/%c%c%c nesting_depth=%d\n",i,address,address+size,
	     (info.protection & VM_PROT_READ) ? 'r' : '-',
	     (info.protection & VM_PROT_WRITE) ? 'w' : '-',
	     (info.protection & VM_PROT_EXECUTE) ? 'x' : '-',
	     (info.max_protection & VM_PROT_READ) ? 'r' : '-',
	     (info.max_protection & VM_PROT_WRITE) ? 'w' : '-',
	     (info.max_protection & VM_PROT_EXECUTE) ? 'x' : '-',
	     nesting_depth
	     );
      i++;
      address += size;
    }
  }
  return OKAY;
}
