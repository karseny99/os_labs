#include <iostream>
#include <unistd.h>
using namespace std;


int main(int argc, char* argv[]) {
    int id = atoi(argv[1]);
    // if(id == 5) {
    //     exit(-1);
    // }
    // int id = 1;
    cout << "Node " << id << " started working ..." << endl; 
    sleep(id);
    cout << "Node " << id << " finished work!" << endl; 
}