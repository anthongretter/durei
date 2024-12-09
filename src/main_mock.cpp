#include <iostream>
#include "../include/mockCliente.hpp"
#include "../include/leitor_config.hpp"

using json = nlohmann::json;

int main() {

    LeitorConfig leitor("servidores_config.txt");
    const auto& it = leitor.getServidores().find(0);
    MockCliente cliente("127.0.0.1", 1600, leitor.getServidores(), leitor.getSequenciador());
        
    json transacao_json = {
        {"read", {
            {"x", "y"}
        }},
        {"write", {
            {"f", "ABC"},
            {"g", "TGF"}
        }}
    };

    cliente.criarTransacao(transacao_json);
    cliente.enviarTransacao();

    return 0;
}