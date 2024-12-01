#include "transacao.hpp"
#include <iostream>
#include <sys/socket.h>
#include <variant>
#include <arpa/inet.h>

Transacao::Transacao(int c_id, int t, int s_id, std::map<int, std::tuple<std::string, int>> servidores) {
    c_id = c_id;
    t = t;
    s_id = s_id;
    servidores = servidores;
}

void Transacao::read(std::string item) {
    auto it = ws.find(item);
    if (it != ws.end()) {
        // TODO
        // como ficam as versões??

    } else {
        const auto& tupla = servidores[s_id];
        const std::string& ip = std::get<0>(tupla);
        const int& porta = std::get<1>(tupla);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(ip.c_str());
        address.sin_port = htons(porta);
        int len = sizeof(address);
        // int result = connect(sockfd, (struct sockaddr *)&address, len);
        // PEDIR item pro servidor
        // printar mensagem de recebimento
    }
}

void Transacao::write(std::string item, std::string valor) {
    auto it = ws.find(item);
    if (it != ws.end()) {
        it->second = valor;
    } else {
        ws[item] = valor;
    }
    std::cout << "VALOR " << valor << " ESCRITO EM " << item << "\n";
}

void Transacao::commit() {
    for (const auto& entry : servidores) {
        auto& tupla = entry.second;
        std::string ip = std::get<0>(tupla);
        int porta = std::get<1>(tupla);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(ip.c_str());
        address.sin_port = htons(porta);
        int len = sizeof(address);
        // int result = connect(sockfd, (struct sockaddr *)&address, len);
        // MANDAR RS E WS
        // aumenta as versões??
    }
}

void Transacao::printarInfo() {
    std::cout << "Conteúdo de rs:" << std::endl;
    // Iterando sobre o mapa rs
    for (const auto& entry : rs) {
        const std::string& item = entry.first;
        const auto& tupla = entry.second;
        const std::string& valor = std::get<0>(tupla);
        const int& versao = std::get<1>(tupla);

        std::cout << "Item: " << item << ", Valor: " << valor << ", Versão: " << versao << "\n";
    }

    std::cout << "Conteúdo de ws:" << std::endl;
    // Iterando sobre o mapa ws
    for (const auto& entry : ws) {
        const std::string& item = entry.first;
        const std::string& valor = entry.second;

        std::cout << "Item: " << item << ", Valor: " << valor << "\n";
    }
}

