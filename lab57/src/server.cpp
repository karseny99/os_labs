#include <iostream>
#include "../include/compute_node.hpp"
#include "zmq.hpp"

int main(int argc, char *argv[]) {
    if (argc < 4){
        printf("Wrong launch\n");
        throw -1;
    }
    compute_node node(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
    node.run();
}