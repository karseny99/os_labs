#pragma once

#include "zmq_funcs.hpp"
#include "compute_node.hpp"
#include <map>
#include <chrono>
#include <iostream>
#include <sys/wait.h>
#include <list>

struct child_info {
    zmq::socket_t socket;
    std::list<int> childs;
    int port;
    pid_t pid;

    child_info() = default;
    child_info(zmq::context_t& context, int id);

    bool exist(int id) {
        // auto it = std::find(childs.begin(), childs.end(), id);
        // return it != childs.end();

        for(auto i : childs){
            if(i == id){
                return true;
            }
        }
        return false;
    }

    void add(int id) {
        childs.push_back(id);
    }
};

class control_node {
    private:
        zmq::context_t context;
        zmq::socket_t socket;
        std::map<int, child_info> nodes;

    public:
        control_node();

        void run();

        std::string createNode(int ID, int parentID);

        std::string exec_timer(int ID, std::string cmd);

        std::string ping(int ID);

        void close_nodes();
};