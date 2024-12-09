#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include "transacao.hpp"

class Cliente {
public:
    Cliente(std::string cliente_ip, int cliente_port,
            std::map<int,std::tuple<std::string, int>> serv, std::tuple<std::string, int> seq);
    void executar();

private:
    std::string ip;
    int porta;
    std::map<int, std::tuple<std::string, int>> servidores; // Mapa de servidores (ID -> (IP, Porta))
    std::tuple<std::string, int> sequenciador;
    std::map<int, std::shared_ptr<Transacao>> transacoes;

    int escolherServidorAleatorio();
    void printarTransacoes();
};

#endif // CLIENTE_HPP