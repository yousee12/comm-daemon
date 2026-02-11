#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "Server.hpp"
#define PORT 8080
int main() {
    Server server(PORT);
    server.run();

    return 0;
}