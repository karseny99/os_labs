#include "../include/tree.hpp"
#include <zmq.hpp>
#include <iostream>
#include <memory>


int main() {
    node tree(12);
    // std::cout << (*tree.get_node("")).id << std::endl;

    if(tree.add_node("", 14)) {
       std::cout << "im here\n"; 
    }
    if(tree.add_node("c", 16)) {
       std::cout << "im here\n"; 
    }
    if(tree.add_node("c", 18)) {
       std::cout << "im here\n"; 
    }
    if(tree.add_node("c", 20)) {
       std::cout << "im here\n"; 
    }

    std::cout << tree.get_node("c")->id << std::endl;  
    std::cout << tree.get_node("cc")->id << std::endl;  
    std::cout << tree.get_node("ccbb")->id << std::endl;  

}