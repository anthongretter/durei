#include "../include/transacao.hpp"
#include <iostream>
#include <sys/socket.h>
#include <variant>
#include <arpa/inet.h>
#include <nlohmann/json.hpp>

Transacao::Transacao(int t, std::tuple<std::string, int> c, std::tuple<std::string, int> seq, std::tuple<int, std::string, int> serv) {
    t = t;
    cliente = c;
    sequenciador = seq;
    servidor = serv;
}

void Transacao::read(std::string item) {
    auto it = ws.find(item);
    if (it != ws.end()) {
        std::cout << "VARIÁVEL " << item << " ENCONTRADA EM WS -- VALOR: " << it->second << "\n";
    } else {
        std::cout << "PEDINDO VARIÁVEL " << item << " PARA SERVIDOR " << std::get<0>(servidor) << "\n";
        std::string mensagem = "read(" + item + ")";
        std::string to_ip = std::get<1>(servidor);
        int to_port = std::get<2>(servidor);
        // TODO:
        // mandar por TCP mensagem para to_ip to_port
        // receber o valor e printar 
        // std::cout << "VARIÁVEL " << item << " RECEBIDA E ADICIONADA A RS  -- VALOR: " << valor << "   -- VERSÃO: " << versao << "\n";
    }
}

void Transacao::write(std::string item, std::string valor) {
    auto it = ws.find(item);
    if (it != ws.end()) {
        it->second = valor;
    } else {
        ws[item] = valor;
    }
    std::cout << "VALOR " << valor << " ESCRITO NA VARIÁVEL " << item << " EM WS\n";
}

void Transacao::commit() {
    nlohmann::json commit = {
        {"from", std::get<0>(cliente)},
        {"port", std::get<1>(cliente)},
        {"rs", {}},
        {"ws", {}}
    };
    for (const auto& item : rs) {
        const std::string& var = item.first;
        const std::tuple<std::string, int>& tupla = item.second;
        commit["rs"][var] = {std::get<0>(tupla), std::get<1>(tupla)};
    }
    for (const auto& item : ws) {
        const std::string& var = item.first;
        const std::string& valor = item.second;
        commit["ws"][var] = valor;
    }

    std::string json_str = commit.dump(4);
    std::cout << json_str << std::endl;

    // TODO:
    // mandar para sequenciador  -- ip: std::get<0>(sequenciador)   porta: std::get<1>(sequenciador)
    // receber resposta do servidor  -- tem que informar o servidor dessa transação no json????
    // apaga a transacao e, dependendo da resposta, escreve mensagem de erro ou de confirmação da transacao
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

