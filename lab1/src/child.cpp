#include <iostream>
#include <unistd.h>

using namespace std;

bool IsPrime(int);

int main() {
    int tmp;
    while(scanf("%d", &tmp) != EOF) {
        if(!IsPrime(tmp)) {
            write(STDOUT_FILENO, &tmp, sizeof(int));
        } else {
            break;
        }
    }
}