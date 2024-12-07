#include "../include/meia.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdatomic.h>
#include <unistd.h>


Meia::Meia(const std::string &ip, const int &port)
        : _ip(ip), _porta(port), _escutando(false), _parar(true) {}


Meia::~Meia() {
    pararDeEscutar();

    for (SocketEmbrulho meia : _outrasMeias) {
        close(meia.socketfd);
    }
}


int Meia::mandarPara(const int &numeroMeia, const std::string &mensagem) {
    std::cout << _ip << ":" << _porta << " -> Mandando: " << mensagem << std::endl;
    return send(_outrasMeias.at(numeroMeia).socketfd, mensagem.c_str(), strlen(mensagem.c_str()), 0);
}


void Meia::registrarOutraMeia(const int numeroMeia, const std::string &ip, const int &porta) {
    SocketEmbrulho outraMeia = _novoSocket(ip, porta);
    _outrasMeias.insert(_outrasMeias.begin() + numeroMeia, outraMeia);
}


void Meia::conectarComOutrasMeias() {
    for (SocketEmbrulho meia : _outrasMeias) {
        connect(meia.socketfd, (struct sockaddr*) &meia.addr, sizeof(meia.addr));
    }
}


void Meia::comecarEscutar() {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _escutando = true;
        _parar = false;

        SocketEmbrulho embrulho = _novoSocket(_ip, _porta);
        _socketfdOuvinte = embrulho.socketfd;

        bind(_socketfdOuvinte, (struct sockaddr*) &embrulho.addr, sizeof(embrulho.addr));
        listen(_socketfdOuvinte, MAX_CLIENTS);
        _threadOuvinte = std::thread(&Meia::_escutar, this);
    }
    _cv.notify_one();
}


void Meia::pararDeEscutar() {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _escutando = false;
        _parar = true;
    }
    _cv.notify_all();

    if (_threadOuvinte.joinable()) {
        _threadOuvinte.join();
        close(_socketfdOuvinte);
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
        int meiaCliente = accept(_socketfdOuvinte, nullptr, nullptr);

        if (recv(meiaCliente, buffer, sizeof(buffer), 0) != -1) {
            std::thread * lidador = new std::thread(&Meia::lidarComMensagem, this, buffer);
            _threadsDeLidacoes.push_back(lidador);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simular
    }
}
