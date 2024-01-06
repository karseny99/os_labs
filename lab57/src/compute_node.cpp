#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "../include/zmq_funcs.hpp"
#include "../include/compute_node.hpp"



compute_node::compute_node(int _id, int _parPort, int _parId)
    : id(_id), parentPort(_parPort), parentId(_parId), context(1), parent(context,ZMQ_REP), child(context,ZMQ_REQ) {
    if(_id != -1){
        connect(parent, parentPort);
    }
}

void compute_node::run() {

    while(true) {

        std::string msg, cmd;

        msg = receive_msg(parent);
        std::istringstream request(msg);
        request >> cmd;

        if(cmd == "pid") {

            std::string answer = std::to_string(getpid());
            send_msg(parent, answer);
            
        } else if(cmd == "ping") {

            int ID_element;
            request >> ID_element;

            std::string answer = ping(ID_element);
            send_msg(parent, answer);

        } else if(cmd == "create") {

            int child;
            request >> child;

            std::string answer = create_child(child);
            send_msg(parent, answer);                

        } else if(cmd == "send") {

            int ID_child;
            request >> ID_child;

            std::string str;
            getline(request, str);


            str.erase(0, 1);



            std::string answer = send_str(str, ID_child);

            send_msg(parent, answer);

        } else if(cmd == "exec") {

            std::string str;
            getline(request, str);
            std::string answer = exec(str);
            send_msg(parent, answer);

        } else if(cmd == "stop") {

            if(childId != -2) {
                send_msg(child, "stop");
                disconnect(child, childPort);
                child.close();
            }
            break;

        } else {

            send_msg(parent, "wrong command");

        }

    }

}

std::string compute_node::create_child(int idElem) {
    int port;
    if(childId == -2){
        childId = idElem;
        childPort = bind(child, childId);
    } else {
        return "Error create";
    }
    int pid = fork();
    if(pid == 0){
        execl("./server", "server", std::to_string(childId).c_str(), std::to_string(childPort).c_str(), std::to_string(id).c_str(), (char*)NULL);
        std::cout << "Error execl" << std::endl;
        exit(1);
    }
    else{
        std::string pidChild;
        try{
            child.setsockopt(ZMQ_SNDTIMEO, 3000);
            send_msg(child, "pid");
            pidChild = receive_msg(child);
            return "OK: " + pidChild;
        }
        catch(int){
            return "Error: can't connect to child";
        }
    }
    return "OK: " + std::to_string(childId);
}

std::string compute_node::ping(int ID_element) {
    std::string answer = "OK: 0";
    if(ID_element == id){
        answer = "OK: 1";
        return answer;
    }
    else if(childId == ID_element){
        std::string message = "ping " + std::to_string(ID_element);
        send_msg(child, message);

        message = receive_msg(child);
        if(message == "OK: 1"){
            answer = message;
        }

    }
    return answer;
}

std::string compute_node::send_str(std::string str, int ID_element) {
    std::string answer = "Error: id is not found";

    if(childId == -2){
        return answer;
    }
    else if(childId == ID_element) {
        if(ping(childId) == "OK: 1"){
            send_msg(child, str);
            answer = receive_msg(child);
        }
    } else {                
        if(ping(childId) == "OK: 1"){
            std::string req = "send " + std::to_string(ID_element) + " " + str;
            std::string resp;
            send_msg(child, req);
            try{
                resp = receive_msg(child);
            }
            catch(int){
                resp = "Error: id is not found";
            }
            if(resp != "Error: id is not found"){
                answer = resp;
            }
        }
    }
    return answer;
}
        
std::string compute_node::exec(std::string str) {
    std::string answer = "";
    std::istringstream in(str);

    std::string cmd;
    in >> cmd;
    if(cmd == "start" && !timer_on) {

        timer_on = true;
        start = std::chrono::system_clock::now();
        answer = "Timer started on node[" + std::to_string(id) + "]";

    } else if(cmd == "stop" && timer_on) {
        
        timer_on = false;
        answer = "Time passed is ";
        answer += std::to_string(static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count()));
        answer += " seconds";

    } else if(cmd == "time" && timer_on) {

        answer = "Time passed is ";
        answer += std::to_string(static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count()));
        answer += " seconds";
    } else {
        answer = "Error: wrong command context";
    }
    // std::cout << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() << std::endl;
    // std::cout << timer_on << ' ' << cmd << ' ' << answer << std::endl;
    return answer;
}

