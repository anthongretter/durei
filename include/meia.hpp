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

#define IPV4 AF_INET
#define IPV6 AF_INET6
#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM
#define MAX_CLIENTS 10


/**
 * Uma Meia representa uma interface para sockets,
 * podendo criar um socket temporario para receber mensagens,
 * ou registrar outras Meias para mandar mensagens.
 */
class Meia {

    struct SocketEmbrulho {
        int socketfd;
        sockaddr_in addr;
        SocketEmbrulho(int &fd, sockaddr_in &addr): socketfd(fd), addr(addr) {};
    };

public:
    Meia(const std::string &ip, const int &porta);
    ~Meia();

    virtual void lidarComMensagem(const char mensagem[]) = 0;

    int mandarPara(const int &numeroMeia, const std::string &mensagem);
    void registrarOutraMeia(const int numeroMeia, const std::string &ip, const int &porta);
    void conectarComOutrasMeias();

    void comecarEscutar();
    void pararDeEscutar();

protected:
    void _escutar();

    SocketEmbrulho _novoSocket(const std::string &ip, const int &porta) {
        int sfd = socket(IPV4, TCP, 0);
        fcntl(sfd, F_SETFL, O_NONBLOCK);

        sockaddr_in addr;
        addr.sin_family = IPV4;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = porta;

        return SocketEmbrulho(sfd, addr);
    };

    const int _porta;
    const std::string _ip;
    int _socketfdOuvinte;
    bool _escutando;
    bool _parar;

    std::vector<SocketEmbrulho> _outrasMeias;
    std::vector<std::thread*> _threadsDeLidacoes;
    std::thread _threadOuvinte;
    std::condition_variable _cv;
    std::mutex _mutex;
};


/**
 * Exemplo de implementacão da classe Meia
 */
class MeiaBoba : public Meia {
public:
    MeiaBoba(const std::string &ip, const int &porta) : Meia(ip, porta) {};

    void lidarComMensagem(const char mensagem[]) override {
        std::cout << _ip << ":" << _porta << " -> Recebeu: " << mensagem << std::endl;
    }
};


#endif //MEIA_HPP
