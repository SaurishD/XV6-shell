#include "types.h"
#include "user.h"

int main(){
    //cprintf("Hello world is executing");
    helloWorld();
    numOpenFiles(-1);
    memAlloc(-1);
    sbrk(1000);
    memAlloc(-1);
    getprocesstimedetails(-1);
    exit(0);
}