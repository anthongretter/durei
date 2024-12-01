#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include "transacao.hpp"

class Cliente {
public:
    Cliente(int cliente_id);
    int lerConfiguracaoServidores(const std::string& arquivo);
    void executar();

private:
    int id;
    std::map<int, std::tuple<std::string, int>> servidores; // Mapa de servidores (ID -> (IP, Porta))
    std::map<int, Transacao> transacoes;

    int escolherServidor();
    void printarTransacoes();
};

#endif // CLIENTE_HPP
