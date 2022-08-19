#include "types.h"
#include "user.h"
#include "fcntl.h"

struct command{
    int argc;
    char* argv[10];
    char input[20];
    char output[20];
};

struct command commands[2];
char operators[5];

int isOperator(char c){
    if(c=='|' || c==';' || c=='&') return 1;
    return 0;
}

int execute(int p){
    
    
    if(strlen(commands[0].input)>0){
        close(0);
        if(open(commands[p].input, O_RDONLY)<0){
            printf(1,"cannot open file %s\n",commands[p].input);
            exit(1);
        }
    }
    if(strlen(commands[p].output)>0){
        close(1);
        if(open(commands[p].output, O_WRONLY|O_CREATE)<0){
            printf(1,"cannot open file %s\n",commands[p].output);
            exit(1);
        }
    }
    exec(commands[p].argv[0],commands[p].argv);
    exit(0);
}

int parallel(int p, int q){
    int pid1,pid2;
    if((pid1=fork()) ==0){
        execute(p);
    }
    if((pid2=fork())==0){
        execute(q);
    }
    wait(0);
    wait(0);
    return 0;
}

int and(int p, int q){
    
    int pid1,pid2;
    if((pid1=fork())==0){
        execute(p);
        exit(0);
    }
    int ext1;
    wait(&ext1);
    if(ext1==0){
        if((pid2=fork())==0){
            execute(q);
        }
        wait(0);
    }
    
    return 0;
}

int or(int p, int q){
    int pid1,pid2;
    if((pid1=fork())==0){
        execute(p);
    }
    int ext1;
    wait(&ext1);
    if(ext1!=0){
        if((pid2=fork())==0){
            execute(q);
        }
        wait(0);
    }
    return 0;
}

int doPipe(int p, int q){
    int fd[2];
    pipe(fd);
    if(fork()==0){
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        execute(p);
    }
    if(fork()==0){
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        execute(q);
    }
    close(fd[0]);
    close(fd[1]);
    wait(0);
    wait(0);
    return 0;
}

int parse(char* input){
    int cmdCount = 0;
    int n = strlen(input);
    for(int i=0;i<n;){
        if(input[i]==' ') i++;
        if(input[i]!='<' && input[i]!='>' && !isOperator(input[i]) && input[i]!=' '){
            //char token[100];
            int j=0;
            int argc = commands[cmdCount].argc++;
            char token[100];
            do{
                token[j++] = input[i++];
            }while(i<n && input[i]!=' ' && !isOperator(input[i]));
            token[j]='\0';
            commands[cmdCount].argv[argc] = malloc(strlen(token));
            strcpy(commands[cmdCount].argv[argc],token);
        }else if(input[i]=='<'){
            do{
                i++;
            }while(i<n && input[i]==' ');
            int j=0;
            while(i<n && input[i]!=' ' && !isOperator(input[i]) ){
                commands[cmdCount].input[j++] = input[i++];
            }
            commands[cmdCount].input[j] = '\0';
        }else if(input[i]=='>'){
            do{
                i++;
            }while(i<n && input[i]==' ');
            int j=0;
            while(i<n && input[i]!=' ' && !isOperator(input[i])){
                commands[cmdCount].output[j++] = input[i++];
            }
            commands[cmdCount].output[j] = '\0';
        }else if(isOperator(input[i])){
            cmdCount++;
            int j=0;
            do{
                operators[j++] = input[i++];
            }while(i<n && isOperator(input[i]));
            operators[j] = '\0';
        }
    }
    return cmdCount+1;
}

void sh(char* input){
    
    for(int i=0;i<2;i++){
        memset(&commands[i],0,sizeof(struct command));
    }
    memset(operators,0,5);
    int n = parse(input);
    if(strcmp(commands[0].argv[0],"exit")==0){
        exit(0);
    }
    if(strcmp(commands[0].argv[0],"ps")==0){
        psinfo();
        return;
    }
    
    
    if(strcmp(commands[0].argv[0],"executeCommands")==0){
        char * filename = commands[0].argv[1];
        int fd,n;
        if((fd = open(filename,O_RDONLY))<0){
            printf(1,"Cannot Open file %s\n",filename);
            return;
        }
        memset(input,0,100);
        char stor[1000];
        int ii=0;
        int lines = 0;
        char inputs[15][40];
        memset(inputs,0,500);
        while((n=read(fd,stor,1000))>0){
            for(int itr=0;itr<1000 && stor[itr]!=0;itr++){
                if(stor[itr]!='\n') inputs[lines][ii++] = stor[itr];
                else{
                    inputs[lines++][ii] = '\0';
                    ii=0;
                }
            }
        }
        for(int itr=0;itr<lines;itr++){
            for(int i=0;i<2;i++){
                memset(&commands[i],0,sizeof(struct command));
            }
            memset(operators,0,5);
            sh(inputs[itr]);
            
        }
        return;
    }


    
    if(n==1){
        if(fork()==0){
            execute(0);
            exit(0);
        }
        wait(0);
    }else if(n>1){
        for(int i=1;i<n;i++){
            if(commands[i-1].argv[0][0]==0 || commands[i].argv[0][0]==0){
                printf(1,"Invalid Agruments\n");
                return;
            }
            if(strcmp(operators,"||")==0){
                or(i-1,i);
            }else if(strcmp(operators,"&&")==0){
                and(i-1,i);
            }else if(strcmp(operators,"|")==0){
                doPipe(i-1,i);
            }else if(strcmp(operators,";")==0){
                parallel(i-1,i);
            }else{
                printf(1,"Invalid operation\n");
            }
        }
    }else{
        printf(1,"Invalid Operator\n");
    }
}

int main(){
    while(1){
        printf(1,"MyShell>");
        char input[100];
        memset(input,0,100);
        gets(input,100);
        input[strlen(input)-1] = '\0';
        sh(input);
    }
    exit(0);
}