#include "types.h"
#include "user.h"

int main(int argc,char* argv[]){
    int pid = atoi(argv[1]);
    if(pid < 1) {
        printf(1,"Enter Valid pid\n");
        exit(1);
    }
    int t = numOpenFiles(pid);
    if(t<0) {
        printf(1,"Something went wrong\n");
        exit(1);
    }
    printf(1,"Number of files opened: %d\n",t);
    t = memAlloc(pid);
    if(t<0) {
        printf(1,"Something went wrong\n");
        exit(1);
    }
    printf(1, "Memory allocated: %d\n",t);
    getprocesstimedetails(pid);
    exit(0);
}