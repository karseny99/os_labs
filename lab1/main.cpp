#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#define RED_COLOR    "\x1b[31m" // child process output color
#define GREEN_COLOR  "\x1b[32m" // parent process output color
#define RESET_COLOR    "\x1b[0m"  // reset color

#define MAX_PATH_SIZE 15

using namespace std;
using ll = int64_t;

bool IsPrime(int);

int create_process() {
    pid_t pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        exit(-1);
    }
    return pid;
}

/*
10 вариант) В файле записаны команды вида: «число<endline>». Дочерний процесс производит
проверку этого числа на простоту. Если число составное, то дочерний процесс пишет это число в
стандартный поток вывода. Если число отрицательное или простое, то тогда дочерний и
родительский процессы завершаются. Количество чисел может быть произвольным
*/

int main() {

    char str_file_path[MAX_PATH_SIZE];
    cout << "Enter file path:" << endl;
    cin >> str_file_path;
    FILE * input_file = fopen(str_file_path, "r");
    int pipe_fd[2];
    
    if(pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(-1);
    }

    pid_t pid = create_process();

    if(pid == 0) { // Child process (writable) fd(1)
        close(pipe_fd[0]);

        if(dup2(fileno( input_file ), STDIN_FILENO) == -1) {
            perror("dup2 can't redirect stdin to input_file");
            exit(-1);
        }

        if(dup2(pipe_fd[1], STDOUT_FILENO) == -1) {
            perror("dup2 can't redirect stdout to pipe");
            exit(-1);
        }

        int tmp;
        for(int i = 0; i < 10; ++i) {
            cin >> tmp;
            // scanf("%d", &tmp);
            printf("%d", tmp);
        } 
        // while(scanf("%d", &tmp)) {
        //     printf("%d", tmp);
        //     // if(!IsPrime(tmp)) {
        //     //     printf("%d", tmp);
        //     // } else {
        //     //     break;
        //     // }
        // }
        
        close(pipe_fd[1]);

    } else {  // Parent process (readable) fd(0)
        close(pipe_fd[1]);
        int tmp;
        while(read(pipe_fd[0], &tmp, sizeof(int))) {
            cout << GREEN_COLOR << tmp << endl;
        }

        close(pipe_fd[0]);
    }


}

/*
Родительский процесс создает дочерний процесс. Первой строчкой пользователь в консоль
родительского процесса вводит имя файла, которое будет использовано для открытия файла с
таким именем на чтение. Стандартный поток ввода дочернего процесса переопределяется
открытым файлом. Дочерний процесс читает команды из стандартного потока ввода.
Стандартный поток вывода дочернего процесса перенаправляется в pipe1. Родительский процесс
читает из pipe1 и прочитанное выводит в свой стандартный поток вывода. Родительский и
дочерний процесс должны быть представлены разными программами.
*/