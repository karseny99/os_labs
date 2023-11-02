#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>

#define RED_COLOR    "\x1b[31m" // child process output color
#define GREEN_COLOR  "\x1b[32m" // parent process output color
#define RESET_COLOR    "\x1b[0m"  // reset color

using namespace std;

const int MAX_NUM_LEN = 40 * sizeof(char);


bool IsPrime(int x) {
    if(x < 0) {
        return true;
    } else if(x == 0 or x == 1) {
        return false;
    } 

    for(int i = 2; i * i <= x; ++i) {
        if(x % i == 0) {
            return false;
        }
    }
    
    return true;
}

int main(int argc, char** argv) {

    sem_t* sme = sem_open(argv[1], 0);
    sem_t* she = sem_open(argv[2], 0);
    
    int fd = shm_open(argv[3], O_RDWR , S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    // cout << argv[3] << endl;
    if(fd < 0) {
        perror("Failed to open shm in child");
        exit(-1);
    }

    char* mmap_pipe = static_cast<char*>(mmap(NULL, MAX_NUM_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    if (mmap_pipe == (caddr_t)-1){
        perror("Mapping Failed\n");
        exit(-1);
    }
    int tmp = -1;
    bool is_EOF = true;

    while(scanf("%d", &tmp) != EOF) {
        // cout << RED_COLOR << tmp << endl;
        // cout << RED_COLOR << "Waiting for parent" << endl;
        sem_wait(sme);  

        if(!IsPrime(tmp)) {
            
            string t;
            t = to_string(tmp);
            mmap_pipe[0] = t.length();
            strcpy(mmap_pipe + 1, t.c_str());
            cout << RED_COLOR << t.size() << ' ' << static_cast<int>(mmap_pipe[0]) << endl;
            cout << RED_COLOR << "Doing child's stuff " <<  mmap_pipe + 1 << endl;
            sem_post(she);
        } else {
            mmap_pipe[0] = -1;
            is_EOF = false;
            sem_post(she);
            break;
        }
    }
 
    if(is_EOF) {
        mmap_pipe[0] = -1;
        sem_post(she);
    }
    
    sem_close(sme);
    sem_close(she);
}