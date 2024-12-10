#ifndef MOCKCLIENTE_HPP
#define MOCKCLIENTE_HPP

#include <string>
#include <map>
#include <tuple>
#include <nlohmann/json.hpp>
#include "cliente.hpp"

using json = nlohmann::json;

class MockCliente : public Cliente {
public:
    MockCliente(std::string cliente_ip, int cliente_port, std::map<int, std::tuple<std::string, int>> serv, 
                std::tuple<std::string, int> seq);

    void criarTransacao(json transacao_json);
    void enviarTransacao();

};

#endif // MOCKCLIENTE_HPP