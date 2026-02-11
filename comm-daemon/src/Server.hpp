#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include "Client.hpp"


class Server {
public:
Server(uint16_t port) : m_running(false),
            m_server_fd(socket(AF_INET, SOCK_STREAM, 0)),
            m_server_addr({.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port=htons(port)}) {
    int opt = 1;
    setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
}

int run() {
    if(m_server_fd < 0) {
        std::cerr << "socket server fd failed" << std::endl;
        return 1;
    }

    if (bind(m_server_fd, (sockaddr*)&m_server_addr, sizeof(m_server_addr)) < 0) {
        std::cerr << "failed to bind socket fd " << m_server_fd << std::endl;
        close(m_server_fd);
        return 1;
    }

    if (listen(m_server_fd, 10) < 0) {
        std::cerr << "failed to listen to fd " << m_server_fd << std::endl;
        close(m_server_fd);
        return 1;
    }
    m_server_poll_fd = {m_server_fd, POLLIN, 0};
    m_running = true;
    m_polls.push_back(m_server_poll_fd);
    while(m_running) {
        int read_fd = poll(m_polls.data(), m_polls.size(), -1);

        if (m_polls[0].revents & POLLIN) {
            int client_fd = accept(m_server_fd, nullptr, nullptr);
            std::cout << "Client connected: fd=" << client_fd << std::endl;
            m_polls.push_back({client_fd, POLLIN, 0});
            m_clients.emplace_back(client_fd);
        }
        for(int i=0; i<m_clients.size(); i++) {
            if (m_polls[i+1].revents & POLLIN) {
                if(!m_clients[i].run()) {
                    m_clients.erase(m_clients.begin()+i);
                    m_polls.erase(m_polls.begin()+i+1);
                    i--;
                }
            }
        }

    }

}

private :
    bool m_running;
    int m_server_fd;
    sockaddr_in m_server_addr{};
    pollfd m_server_poll_fd;
    std::vector<Client> m_clients;
    std::vector<pollfd> m_polls;
};