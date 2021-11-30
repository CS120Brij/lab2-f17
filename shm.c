#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {

//you write this
 acquire(&(shm_table.lock));
 uint sz = PGROUNDUP(myproc()->sz);
 int i = 0;
 int temp = 0;
 for (i = 0; i < 64; i++) { //up to 64 pages of shared memory
  if (temp == 0 && shm_table.shm_pages[i].id == 0) { //found first empty one
      temp = i;
      
  }
  if (id == shm_table.shm_pages[i].id) {
    mappages(myproc()->pgdir, (char *)sz, PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
    shm_table.shm_pages[i].refcnt++;
    break;
  }
 }
  if (temp > 0) {

      shm_table.shm_pages[temp].id = id;
      shm_table.shm_pages[temp].frame = kalloc();
      memset(shm_table.shm_pages[temp].frame, 0, PGSIZE);
      mappages(myproc()->pgdir, (char *)sz, PGSIZE, V2P(shm_table.shm_pages[temp].frame), PTE_W|PTE_U);
      shm_table.shm_pages[temp].refcnt++;
  }


 
  *pointer=(char *)sz;
  release(&(shm_table.lock));




return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
//you write this too!
  acquire(&(shm_table.lock));
  int i;
  for (i = 0; i < 64; ++i) {
    if (shm_table.shm_pages[i].id == id) {
	    shm_table.shm_pages[i].refcnt--;
	  }
	  if (shm_table.shm_pages[i].refcnt == 0) {
	    shm_table.shm_pages[i].id = 0;
      shm_table.shm_pages[i].frame = 0;
	    break;
	  }
  }
  release(&(shm_table.lock));




return 0; //added to remove compiler warning -- you should decide what to return
}
