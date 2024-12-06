#ifndef MEIA_HPP
#define MEIA_HPP

#include <string>
#include <thread>
#include <sys/socket.h>
#include <variant>
#include <arpa/inet.h>

class Meia {
public:
    Meia(const std::string &ip, const int &port);
    ~Meia();

    int send(Meia &to, std::string &content);

protected:
    int _listen();

    const int _port;
    const std::string _ip;
    std::thread _listen_thread;
    socket _socket;
};

#endif //MEIA_HPP
