#ifndef MEIA_HPP
#define MEIA_HPP

#include <cstddef>
#include <cstring>
#include <stdatomic.h>
#include <string>
#include <sys/types.h>
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
#include <set>
#include <list>

#define IPV4 AF_INET
#define IPV6 AF_INET6
#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM
#define MAX_CLIENTS 10


/**
 * Uma Meia representa uma interface para sockets
 */
class Meia {
public:
    typedef std::lock_guard<std::mutex> LockGuard;

    enum Reacao : int {
        CONEXAO_ENCERRADA,
        NADA_A_LER,
        ALGO_A_LER,
        ERRO
    };

    Meia(const std::string &ip, const int &porta);
    ~Meia();

    virtual void lidarComMensagem(const std::string &mensagem) = 0;

    const std::string ip() { return _ip; }
    const int porta() { return _porta; }

    int mandarPara(const std::string &ip, const int &porta, const std::string &mensagem);
    void comecarEscutar();
    void pararDeEscutar();

    // operator std::string() const { return _ip + ":" + std::to_string(_porta); }

    std::string cabecalho() { return "<" + _ip + ":" + std::to_string(_porta) + "> "; }

protected:
    virtual void _reagirAoEscutado(Reacao &reacao, const std::string &mensagem, const int &quem) = 0;
    virtual int _obterDestinatario(const std::string &ip, const int &porta) = 0;

    void _escutar();

    const int _porta;
    const std::string _ip;

    int _socketfd;
    bool _escutando;
    bool _parar;

    std::vector<std::thread*> _threadsDeLidacoes;
    std::thread _threadOuvinte;
    std::condition_variable _cv;
    std::mutex _mutex;
};


class ClienteMeia : public Meia {
public:
    ClienteMeia(const std::string &ip, const int &porta) : Meia(ip, porta) {};

protected:
    int _obterDestinatario(const std::string &ip, const int &porta) override {
        std::string destinatario = ip + ":" + std::to_string(porta);

        {
            LockGuard lock(_mutex);
            if (destinatario != _conectadoA) {

                sockaddr_in para;
                para.sin_family = IPV4;
                para.sin_addr.s_addr = inet_addr(ip.c_str());
                para.sin_port = htons(porta);

                int status = connect(_socketfd, (struct sockaddr*) &para, sizeof(para));

                if (status == -1) {
                    std::cout << cabecalho() << "ERRO: Não foi possível conectar a " << ip << ":" << porta << std::endl;
                    fprintf(stderr, "Erro no recv(): %s\n", strerror(errno));
                    return -1;
                }
                _conectadoA = destinatario;
            }
        }

        // clientes não recebem FD do servidor, usa-se o próprio,
        // já que só podem estabelecer uma conexão por vez.
        return _socketfd;
    }

    void _reagirAoEscutado(Reacao &reacao, const std::string &mensagem, const int &quem) override {
        switch (reacao) {
        case ALGO_A_LER:
            _threadsDeLidacoes.push_back(
                new std::thread(&Meia::lidarComMensagem, this, mensagem));
            break;
        case CONEXAO_ENCERRADA:
            // TODO retirar ip e porta da mensagem e tirar de _conexoesAtivas
            break;
        case NADA_A_LER:
            break;
        case ERRO:
            std::cout << cabecalho() << "Erro ao receber algo!" << std::endl;
            break;
        };
    }

    std::string _conectadoA;
};


class ServidorMeia : public Meia {
public:
    struct aMandar {
        std::string mensagem;
        int quem;
        aMandar(const std::string &mensagem, int &quem): mensagem(mensagem), quem(quem) {}
    };

    ServidorMeia(const std::string &ip, const int &porta) : Meia(ip, porta) {
        fcntl(_socketfd, F_SETFL, O_NONBLOCK);

        sockaddr_in addr;
        addr.sin_family = IPV4;
        addr.sin_addr.s_addr = inet_addr(_ip.c_str());
        addr.sin_port = htons(_porta);

        bind(_socketfd, (struct sockaddr*) &addr, sizeof(addr));
        comecarEscutar();
    }

    ~ServidorMeia() {
        for (auto clientefd : _conexoesAtivas) {
            close(clientefd);
        }
        Meia::~Meia();
    }

    void comecarEscutar() {
        listen(_socketfd, MAX_CLIENTS);
        Meia::comecarEscutar();
    }

    // void lidarComMensagemServidor(aMandar &a) {
    //     {
    //         LockGuard lock(_mutex);
    //     }
    // }

protected:
    int _obterDestinatario(const std::string &ip, const int &porta) override {
        // std::string chave = ip + ":" + std::to_string(porta);
        // {
        //     LockGuard lock(_mutex);
        //     if (_conexoesAtivas.find(chave) != _conexoesAtivas.end()) {
        //         return _conexoesAtivas[chave];
        //     }
        // }
        // std::cout << "Erro no chave: " << chave << std::endl;
        return -1;
    }

    void _escutar() {
        while (1) {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _cv.wait(lock, [this] { return _escutando || _parar; });
            }

            if (!_escutando && _parar) return;

            {
                LockGuard lock(_mutex);
                for (auto clientefd : _conexoesAtivas) {
                    char buffer[1024] = {0};    // mudar este jeito

                    int bytesLidos = recv(clientefd, &buffer, sizeof(buffer), 0);
                    Reacao reacao;

                    if (bytesLidos > 0) reacao = ALGO_A_LER;
                    else if (bytesLidos == 0) reacao = CONEXAO_ENCERRADA;
                    else if (bytesLidos == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOTCONN)) reacao = NADA_A_LER;
                    else {
                        fprintf(stderr, "Erro no recv(): %s\n", strerror(errno));
                        reacao = ERRO;
                    }

                    _reagirAoEscutado(reacao, std::string(buffer), clientefd);
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(1)); // Simular
        }
    }


    void _reagirAoEscutado(Reacao &reacao, const std::string &mensagem, const int &quem) override {
        int clientefd;

        switch (reacao) {
            case ALGO_A_LER:
                std::cout << cabecalho() << "Remandando: " << mensagem << std::endl;
                // _threadsDeLidacoes.push_back(
                //     new std::thread(&ServidorMeia::lidarComMensagemServidor, this, mensagem, quem));
                send(quem, mensagem.c_str(), strlen(mensagem.c_str()), 0);
                break;
            case NADA_A_LER:
                // procurar novas conexoes
                // FIXME retirar ip e porta da mensagem adicionar em _conexoesAtivas
                clientefd = accept(_socketfd, NULL, NULL);

                if (clientefd > 0) {
                    // std::string cara = "127.0.0.1:8001";
                    // std::cout << cabecalho() << "Conectado com " << cara << std::endl;
                    _conexoesAtivas.push_back(clientefd);
                }

                break;
            case CONEXAO_ENCERRADA:
                // TODO retirar ip e porta da mensagem e tirar de _conexoesAtivas
                break;
            case ERRO:
                std::cout << cabecalho() << "Erro ao receber algo!" << std::endl;
                break;
        };
    }

    std::list<int> _conexoesAtivas;
};


/**
 * Exemplso de implementacões das classes Meia
 */
class MeiaBobaCliente : public ClienteMeia {
public:
    MeiaBobaCliente(const std::string &ip, const int &porta) : ClienteMeia(ip, porta) {};

    void lidarComMensagem(const std::string &mensagem) override {
        std::cout << _ip << ":" << _porta << " -> Recebeu: " << mensagem << std::endl;
    }
};

class MeiaBobaServidor : public ServidorMeia {
public:
    MeiaBobaServidor(const std::string &ip, const int &porta) : ServidorMeia(ip, porta) {};

    void lidarComMensagem(const std::string &mensagem) override {
        unsigned primeiro = mensagem.find("<");
        unsigned ultimo = mensagem.find(">");
        std::string quem = mensagem.substr(primeiro, ultimo - primeiro);

        unsigned sep = quem.find(":");
        std::string ip = quem.substr(0, sep);
        int porta = std::stoi(quem.substr(sep, quem.size()));

        mandarPara(ip, porta, "Olá <" + quem + ">, você é um bobão!");
    }
};


#endif //MEIA_HPP
