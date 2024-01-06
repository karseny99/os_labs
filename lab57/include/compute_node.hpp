#pragma once

#include <iostream>
#include <chrono>
#include "zmq_funcs.hpp"
#include "control_node.hpp"


class compute_node {
    private:
        int id, childId = -2, parentId;
        int childPort, parentPort;
        zmq::context_t context;
        zmq::socket_t parent, child;

        bool timer_on = false;
        std::chrono::time_point<std::chrono::system_clock> start;
        std::chrono::time_point<std::chrono::system_clock> end;



    public:

        compute_node(int _id, int _parent_port, int _parent_id);

        void run();

        std::string create_child(int ID_element);

        std::string ping(int ID_element);

        std::string send_str(std::string str, int ID_element);
        
        std::string exec(std::string str);


};