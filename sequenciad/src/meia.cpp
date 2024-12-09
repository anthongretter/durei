#include "../include/meia.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <fcntl.h>
#include <atomic>
#include <unistd.h>


Meia::Meia(const std::string &ip, const int &port)
        : _ip(ip), _porta(port), running(true) {}


Meia::~Meia() {
    pararDeEscutar();
}


void Meia::mandarPara(const std::string &ip, const int &porta, const std::string &mensagem) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar socket");
        return;
    }
    std::cout << ip << " " << porta << "\n";
    conectarCom(sockfd, ip, porta);

    if (send(sockfd, mensagem.c_str(), mensagem.size(), 0) < 0) {
        perror("Erro ao enviar mensagem");
    }

    close(sockfd);
}


std::string Meia::mandarParaEReceber(const std::string &ip, const int &porta, const std::string &mensagem) {
    char buffer[1024] = {0};
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar socket");
        return "";
    }

    conectarCom(sockfd, ip, porta);

    if (send(sockfd, mensagem.c_str(), mensagem.size(), 0) < 0) {
        perror("Erro ao enviar mensagem");
        close(sockfd);
        return "";
    }

    int bytes_recebidos = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_recebidos < 0) {
        perror("Erro ao receber mensagem");
        close(sockfd);
        return "";
    }

    std::string resposta_recebida(buffer, bytes_recebidos);
    close(sockfd);

    return resposta_recebida;
}

void Meia::conectarCom(int &sockfd, const std::string &ip, const int &porta) {
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip.c_str());
    address.sin_port = htons(porta);
    int len = sizeof(address);
    if (connect(sockfd, (struct sockaddr *)&address, len) < 0) {
        perror(("Erro ao conectar com " + ip + ":" + std::to_string(porta)).c_str());
        close(sockfd);
        throw std::runtime_error("Erro ao conectar");
    }
}

void Meia::escutar() {
    struct sockaddr_in address;
    int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(_ip.c_str());
    address.sin_port = htons(_porta);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            continue;
        }
        char buffer[1024] = {0};
        int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

        if (bytes_read > 0) {
            std::string message(buffer, bytes_read);
            lidarComMensagem(client_fd, message);
        }

        close(client_fd);
    }
    close(server_fd);
}

void Meia::pararDeEscutar() {
    std::lock_guard<std::mutex> guard(_lock);
    running = false;
}
