#pragma once

#include <string>
#include <iostream>
#include <zmq.hpp>


const int BEGIN_PORT = 49152;
const std::string HOST = "tcp://127.0.0.1:";

void send_msg(zmq::socket_t &socket, const std::string &msg);

std::string receive_msg(zmq::socket_t &socket);

void connect(zmq::socket_t &socket, int port);

void disconnect(zmq::socket_t &socket, int port);

int bind(zmq::socket_t &socket, int id);

void unbind(zmq::socket_t &socket, int port);