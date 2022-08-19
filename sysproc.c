#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0){
    return -1;
  }
  exit(n);
  return 0;  // not reached
}

int
sys_wait(void)
{
  int n;
  if(argint(0, &n) < 0){
    return -1;
  }
  int* status = (int *)n;
  return wait(status);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  
  myproc()->totalAlloc += n;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_helloWorld(void)
{
  return helloWorld();
}

int
sys_numOpenFiles(void)
{
  int pid;
  if(argint(0,&pid)<0)
    return -1;
  return numOpenFiles(pid);
}

int
sys_memAlloc(void)
{
  int pid;
  if(argint(0,&pid)<0)
    return -1;
  return memAlloc(pid);
}

int
sys_getprocesstimedetails(void){
  int pid;
  if(argint(0,&pid)<0)
    return -1;
  return getprocesstimedetails(pid);
}

int
sys_psinfo(void){
  return psinfo();
}