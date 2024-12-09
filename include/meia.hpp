#ifndef MEIA_HPP
#define MEIA_HPP

#include <string>
#include <thread>
#include <sys/socket.h>
#include <type_traits>
#include <variant>
#include <arpa/inet.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <atomic>

#define IPV4 AF_INET
#define IPV6 AF_INET6
#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM


/**
 * Uma Meia representa uma interface para sockets,
 * podendo criar um socket temporario para receber mensagens,
 * ou registrar outras Meias para mandar mensagens.
 */
class Meia {
public:
    Meia(const std::string &ip, const int &porta);
    ~Meia();

    virtual void lidarComMensagem(int cliente_fd, const std::string& menssagem, struct sockaddr_in client_addr) = 0;

    void mandarPara(const std::string &ip, const int &porta, const std::string &mensagem);
    std::string mandarParaEReceber(const std::string &ip, const int &porta, const std::string &mensagem);
    void escutar();
    void pararDeEscutar();

protected:
    std::atomic<bool> running;
    std::mutex _lock;
    const int _porta;
    const std::string _ip;

private:
    void conectarCom(int &sockfd, const std::string &ip, const int &porta);
};


#endif //MEIA_HPP