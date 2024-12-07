#include "../include/meia.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdatomic.h>
#include <string>
#include <unistd.h>


Meia::Meia(const std::string &ip, const int &port)
        : _ip(ip), _porta(port), _escutando(false), _parar(true) {

    _socketfd = socket(IPV4, TCP, 0);
//    fcntl(_socketfd, F_SETFL, O_NONBLOCK);
}


Meia::~Meia() {
    pararDeEscutar();
    close(_socketfd);
}


int Meia::mandarPara(const std::string &ip, const int &porta, const std::string &mensagem) {
    int destinatariofd = _obterDestinatario(ip, porta);
    if (destinatariofd == -1) {
        return -1;
    }

    std::string para = ip + ":" + std::to_string(porta);
    std::cout << _ip << ":" << _porta << " -> "<< para << " Mandando: " << mensagem << std::endl;

    {
        LockGuard lock(_mutex);
        return send(destinatariofd, mensagem.c_str(), strlen(mensagem.c_str()), 0);
    }
}


void Meia::comecarEscutar() {
    {
        LockGuard lock(_mutex);
        _escutando = true;
        _parar = false;
        _threadOuvinte = std::thread(&Meia::_escutar, this);
    }
    _cv.notify_one();
}


void Meia::pararDeEscutar() {
    {
        LockGuard lock(_mutex);
        _escutando = false;
        _parar = true;
    }
    _cv.notify_all();

    if (_threadOuvinte.joinable()) {
        _threadOuvinte.join();
    }

    for (std::thread* t : _threadsDeLidacoes) {
        if (t->joinable()) {
            t->join();
        }
    }
}


void Meia::_escutar() {
    while (1) {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [this] { return _escutando || _parar; });
        }

        if (!_escutando && _parar) return;

        char buffer[1024] = {0};    // mudar este jeito
        int bytesLidos;
        {
            LockGuard lock(_mutex);
            bytesLidos = recv(_socketfd, &buffer, sizeof(buffer), 0);
        }

        Reacao reacao;

        if (bytesLidos > 0) reacao = ALGO_A_LER;
        else if (bytesLidos == 0) reacao = CONEXAO_ENCERRADA;
        else if (bytesLidos == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == ENOTCONN)) reacao = NADA_A_LER;
        else {
            fprintf(stderr, "Erro no recv(): %s\n", strerror(errno));
            reacao = ERRO;
        }

        _reagirAoEscutado(reacao, std::string(buffer), 0);

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simular
    }
}



