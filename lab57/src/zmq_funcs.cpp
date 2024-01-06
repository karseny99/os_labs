#include "../include/zmq_funcs.hpp"
#include <iostream>

void send_msg(zmq::socket_t &socket, const std::string &msg) {
    try {
        zmq::message_t message(msg.size());
        memcpy(message.data(), msg.c_str(), msg.size());
        socket.send(message);
    } catch (...) {
    }
}

std::string receive_msg(zmq::socket_t &socket) {
    zmq::message_t message;
    int chars_read;
    try {
        chars_read = (int)socket.recv(&message);
    }
    catch (...) {
        chars_read = 0;
    }
    if (chars_read == 0) {
        throw -1;
    }
    std::string received_msg(static_cast<char*>(message.data()), message.size());
    return received_msg;
}

void connect(zmq::socket_t &socket, int port) {
    std::string adress = HOST + std::to_string(port);
    socket.connect(adress);
}

void disconnect(zmq::socket_t &socket, int port) {
    std::string adress = HOST + std::to_string(port);
    socket.disconnect(adress);
}

int bind(zmq::socket_t &socket, int id) {
    int port = (BEGIN_PORT + id);
    while(1){
        std::string adress = HOST + std::to_string(port);
        if (port == 65535){
            throw std::runtime_error("Error: can't bind");
        }
        try{
            socket.bind(adress);
            break;
        }
        catch(...){
            port++;
        }
    }
    return port;
}

void unbind(zmq::socket_t &socket, int port) {
    std::string adress = HOST + std::to_string(port);
    socket.unbind(adress);
}