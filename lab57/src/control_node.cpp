#include "../include/zmq_funcs.hpp"
#include "../include/compute_node.hpp"
#include <map>
#include <chrono>
#include <iostream>
#include <sys/wait.h>
#include <list>



control_node::control_node() : context(1), socket(context, ZMQ_REP) {}

child_info::child_info(zmq::context_t& context, int id) {
    socket = zmq::socket_t(context, ZMQ_REQ);
    port = bind(socket, id);
    pid = fork();
    if(pid == 0){
        execl("./server", "./server", std::to_string(id).c_str(), std::to_string(port).c_str(), std::to_string(-1).c_str(), NULL);
        throw std::runtime_error("Error can't execl child");
    }
}


void control_node::run() {
    std::string cmd;

    std::cout << "control node started" << std::endl;

    while(std::cin >> cmd) {
        if(cmd == "create") {

            int id, pId;

            try {
                std::cin >> id >> pId;
            }
            catch (std::exception& e) {
                std::cout << "Error: expected id(int) and parentId(int)" << std::endl;
                continue;
            }

            std::cout << createNode(id, pId) << std::endl;

        } else if(cmd == "exec") {

            int id;
            std::string command;
            std::cin >> id >> command;
            std::cout << exec_timer(id, command) << std::endl;

        } else if(cmd == "ping") {

            int _id;
            std::cin >> _id;
            std::cout << ping(_id) << std::endl;

        } else if(cmd == "exit") {

            close_nodes();
            break;

        }
    }
}

std::string control_node::createNode(int idElem, int idParent){
    std::string ans = "Error: ";
    if (idParent == -1){ // create main node
        if (nodes.count(idElem) == 0){
            try {
                nodes[idElem] = child_info(context, idElem);
            }
            catch (std::exception& e) {
                ans += e.what();
            }
            std::string pidChild;
            try{
                nodes[idElem].socket.setsockopt(ZMQ_SNDTIMEO, 3000);
                send_msg(nodes[idElem].socket, "pid");
                pidChild = receive_msg(nodes[idElem].socket);
                ans = "OK: " + pidChild;
            }
            catch(int){
                ans += "can't connect to child";
            }
        } else {
            ans += "node already exist";
        }    
    } else { // create child node
        if (nodes.count(idParent)){
            child_info& parent = nodes[idParent];
            if (!parent.exist(idElem)){
                try {
                    send_msg(parent.socket, "create " + std::to_string(idElem));
                    parent.add(idElem);
                }
                catch (std::exception& e) {
                    ans += e.what();
                }
                try{
                    send_msg(parent.socket, "send " + std::to_string(idElem) + " pid");
                    std::string resp = receive_msg(parent.socket);
                    ans = resp;
                }
                catch(int){
                    ans += "can't connect to child";
                }
            } else {
                ans += "node already exist";
            }
        } else {
            bool parentExist = false;
            for (auto& i : nodes){
                if (i.second.exist(idParent)){
                    parentExist = true;
                    try {
                        send_msg(i.second.socket, "send " + std::to_string(idParent) + " create " + std::to_string(idElem));
                        std::string resp = receive_msg(i.second.socket);
                        ans = resp;
                    }
                    catch (std::exception& e) {
                        ans += e.what();
                    }
                    if (ans.substr(0, 2) == "OK"){
                        i.second.add(idElem);
                    }
                    break;
                }
            }
            if (!parentExist){
                ans += "parent node doesn't exist";
            }
        }
    }
    return ans;
}

std::string control_node::exec_timer(int ID, std::string cmd) {
    std::string ans = "OK";
    if (nodes.count(ID)){
        child_info& node = nodes[ID];
        try {
            send_msg(node.socket, "exec " + cmd);
            ans = receive_msg(node.socket);
        }
        catch (std::exception& e) {
            ans = e.what();
        }
    } else {
        bool is_exist = false;
        for (auto& node : nodes){
            if (node.second.exist(ID)){
                is_exist = true;
                try {
                    send_msg(node.second.socket, "send " + std::to_string(ID) + " exec " + cmd);
                    ans = receive_msg(node.second.socket);
                }
                catch (std::exception& e) {
                    ans = e.what();
                }
                break;
            }
        }
        if (!is_exist){
            ans = "Node does not exist";
        }
    }
    return ans;
}

std::string control_node::ping(int ID) {
    std::string ans = "OK";
    if (nodes.count(ID)){
        child_info& node = nodes[ID];
        try {
            send_msg(node.socket, "ping " + std::to_string(ID));
            ans = receive_msg(node.socket);
        }
        catch (std::exception& e) {
            ans = e.what();
        }
    } else {
        bool is_exist = false;
        for (auto& node : nodes){
            if (node.second.exist(ID)){
                is_exist = true;
                try {
                    send_msg(node.second.socket, "send " + std::to_string(ID) + " ping");
                    ans = receive_msg(node.second.socket);
                }
                catch (std::exception& e) {
                    ans = e.what();
                }
                break;
            }
        }
        if (!is_exist){
            ans = "Node does not exist";
        }
    }
    return ans;
}

void control_node::close_nodes() {
    std::cout << "Stopping..." << std::endl;
    for (auto& node : nodes){
        try {
            send_msg(node.second.socket, "stop");
            disconnect(node.second.socket, node.second.port);
            node.second.socket.close();
            waitpid(node.second.pid, NULL, 0);
        }
        catch (...) {
        }
    }
    std::cout << "Stopped" << std::endl;
}
