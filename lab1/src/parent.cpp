/*
10 вариант) В файле записаны команды вида: «число<endline>». Дочерний процесс производит
проверку этого числа на простоту. Если число составное, то дочерний процесс пишет это число в
стандартный поток вывода. Если число отрицательное или простое, то тогда дочерний и
родительский процессы завершаются. Количество чисел может быть произвольным
*/
/*
Родительский процесс создает дочерний процесс. Первой строчкой пользователь в консоль
родительского процесса вводит имя файла, которое будет использовано для открытия файла с
таким именем на чтение. Стандартный поток ввода дочернего процесса переопределяется
открытым файлом. Дочерний процесс читает команды из стандартного потока ввода.
Стандартный поток вывода дочернего процесса перенаправляется в pipe1. Родительский процесс
читает из pipe1 и прочитанное выводит в свой стандартный поток вывода. Родительский и
дочерний процесс должны быть представлены разными программами.
*/



#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#define RED_COLOR    "\x1b[31m" // child process output color
#define GREEN_COLOR  "\x1b[32m" // parent process output color
#define RESET_COLOR    "\x1b[0m"  // reset color

#define MAX_PATH_SIZE 64

using namespace std;
using ll = int64_t;


int create_process() {
    pid_t pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        exit(-1);
    }
    return pid;
}



int main() {
    char str_file_path[MAX_PATH_SIZE];
    cout << "Enter file path:" << endl;
    cin >> str_file_path;

    int input_file = open(str_file_path, O_RDONLY);
    if(input_file == -1) {
        perror("Can't open file");
        exit(-1); 
    }

    int pipe_fd[2];
    
    if(pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(-1);
    }

    pid_t pid = create_process();

    if(pid == 0) { // Child process (writable) fd(1)
        close(pipe_fd[0]);

        if(dup2(input_file, STDIN_FILENO) == -1) {  // set txt-file as stdin
            perror("dup2 can't redirect stdin to input_file");
            exit(-1);
        }

        if(dup2(pipe_fd[1], STDOUT_FILENO) == -1) { // set pipe fd as stdout
            perror("dup2 can't redirect stdout to pipe");
            exit(-1);
        }

        if(execl("./child", "./child", NULL) == -1) { // exec child process
            perror("can't exec child process");
            exit(-1);
        }
        
        close(pipe_fd[1]);

    } else {  // Parent process (readable) fd(0)
        close(pipe_fd[1]);
        close(input_file); // close unused file

        int tmp;
        while(read(pipe_fd[0], &tmp, sizeof(int)) == sizeof(int)) { // getting numbers from pipe
            cout << GREEN_COLOR << tmp << endl;
        }

        close(pipe_fd[0]);
    }


}
