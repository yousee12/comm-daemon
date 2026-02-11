#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string>

#define PORT 8080
#define BUFFER_SIZE 1024
#define REGISTER_ID_INDEX 4
#define REGISTER_NAME_INDEX 8
class Client
{
private:
    /* data */
public:
    Client(int fd) : m_client_fd(fd), m_register_id(-1) {}
    bool run() {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = read(m_client_fd, buffer, BUFFER_SIZE);
        if (bytes <= 0) {
            std::cerr << "Client disconnected: fd=" << m_client_fd << std::endl;
            close(m_client_fd);
            return false;
        } else {
            std::cout << "Client " << m_client_fd << std::endl;
            return parse(buffer, bytes);
        }
        return true;
    }

    ~Client() {
        std::cout << "closing client " << m_client_fd << std::endl;
    }
private:
    bool parse(char buffer[BUFFER_SIZE], ssize_t bytes) {
        if(m_register_name.empty() || m_register_id == -1) {

            uint32_t message_length = buffer_to_int<uint32_t>(buffer);
            m_register_id = buffer_to_int<uint32_t>(buffer + REGISTER_ID_INDEX);
            m_register_name.assign(buffer + REGISTER_NAME_INDEX);
            std::cout << "client id " << m_register_id << " is registerd with name " << m_register_name << std::endl;
            buffer[0] = 1;
            send(m_client_fd, buffer, 1, 0);
        } else {
            send(m_client_fd, buffer, bytes, 0);
        }
        return true;
    }

    template<typename T>
    T buffer_to_int(char* buffer) {
            T ret;
            memcpy(&ret, buffer, sizeof(T));
            return static_cast<T>(ntohl(ret));
    }

    int m_client_fd;
    int32_t m_register_id;
    std::string m_register_name;
    
};
