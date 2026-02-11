#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>

#define PORT 8080
#define BUFFER_SIZE 1024

#define REGISTER_ID_INDEX 4
#define REGISTER_NAME_INDEX 8

class Client{
public:
Client() : m_server_ip("127.0.0.1"), m_sock_fd(-1) {
    // 1️⃣ Create socket
    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::srand(std::time(nullptr)); // seed once
    m_client_id = std::rand() % 100000 + 1; // 1–100

    // 2️⃣ Server address
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &m_server_addr.sin_addr);
}
template<typename T>
void writeBigEndian(uint8_t* buffer, T value) {
    for(int i=0; i<sizeof(T); i++) {
        buffer[sizeof(T)-i-1] = (value >> (8*i)) & 0xFF;
    }
}
template<typename T>
void writeBigEndian(char* buffer, T value) {
    for(int i=0; i<sizeof(T); i++) {
        buffer[sizeof(T)-i-1] = (value >> (8*i)) & 0xFF;
    }
}

bool register_client() {
    std::cout << "Client " << m_client_id << " will start registering" << std::endl;

    int32_t message_size = 0;
    uint8_t buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    writeBigEndian(buffer+REGISTER_ID_INDEX, m_client_id);
    memcpy(buffer+REGISTER_NAME_INDEX, "7amada", 7);
    message_size = 4+7;
    writeBigEndian(buffer, message_size);
    ssize_t sent = send(m_sock_fd, buffer, message_size+4, 0);
    if (sent < 0) {
        std::cerr << "failed to send message" << std::endl;;
        return false;
    }
    memset(buffer, 0, message_size+4);

    ssize_t bytes = read(m_sock_fd, buffer, BUFFER_SIZE - 1);
    if (bytes <= 0) {
        std::cerr << "Server disconnected." << std::endl;
        return false;
    }
    if(buffer[0] == 0) {

        std::cerr << "could not register" << std::endl;
        return false;
    }
    return true;
}

bool run() {
    if (m_sock_fd < 0) {
        std::cerr << "socket failed" << std::endl;
        return false;
    }

    if (connect(m_sock_fd, (sockaddr*)&m_server_addr, sizeof(m_server_addr)) < 0) {
        std::cerr << "connect failed" << std::endl;
        close(m_sock_fd);
        return false;
    }

    if (!register_client()) {
        std::cout << "registering client failed" << std::endl;
        close(m_sock_fd);
        return false;
    }
    std::cout << "client " << m_client_id << " is registered successfully" << std::endl;
    while (true) {
        std::string msg;
        std::getline(std::cin, msg); // read input from user
        char buffer[BUFFER_SIZE];
        if (msg == "quit") {
            std::cout << "Exiting.\n";
            break;
        }

        // Send message to server
        ssize_t sent = send(m_sock_fd, msg.c_str(), msg.size(), 0);
        if (sent < 0) {
            perror("send");
            break;
        }

        // Receive response from server
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes = read(m_sock_fd, buffer, BUFFER_SIZE - 1);
        if (bytes <= 0) {
            std::cout << "Server disconnected.\n";
            break;
        }

        buffer[bytes] = '\0';
        std::cout << "Server says: " << buffer << std::endl;
    }

    close(m_sock_fd);
}

private:
uint32_t m_client_id;
std::string m_server_ip;
int m_sock_fd;
sockaddr_in m_server_addr{};
};


int main() {
    Client Client;

    return Client.run();
}