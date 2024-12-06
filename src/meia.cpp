#include "../include/meia.hpp"

Meia::Meia(const std::string &ip, const int &port): _ip(ip), _port(port) {
    _listen_thread = std::thread(&Meia::_listen, this);
}