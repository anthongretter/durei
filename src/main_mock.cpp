#include <iostream>
#include "../include/mockCliente.hpp"
#include "../include/leitor_config.hpp"

using json = nlohmann::json;

void teste0() {
    std::cout << "Clientes em sequência" << std::endl;

    LeitorConfig leitor("servidores_config.txt");
    const auto& it = leitor.getServidores().find(0);

    MockCliente cliente_1("127.0.0.1", 1600, leitor.getServidores(), leitor.getSequenciador());    
    json transacao_json_1 = {
        {"read", {
            {"x", "y"}
        }},
        {"write", {
            {"x", "AAA"},
            {"y", "AAA"}
        }}
    };

    // Cliente 1 cria transação
    // lendo e modificando os valores de x e y
    cliente_1.criarTransacao(transacao_json_1);
    // Cliente 1 envia transação
    cliente_1.enviarTransacao();

    MockCliente cliente_2("127.0.0.1", 1601, leitor.getServidores(), leitor.getSequenciador());
    json transacao_json_2 = {
        {"read", {
            {"x", "y"}
        }},
        {"write", {
            {"x", "BBB"},
            {"y", "BBB"}
        }}
    };

    // Cliente 2 cria transação 
    // lendo e modificando os valores de x e y
    cliente_2.criarTransacao(transacao_json_2);
    // Cliente 2 envia a transação antes do cliente 1
    cliente_2.enviarTransacao();

}

void teste1() {
    std::cout << "Clientes concorrentes abortando transação" << std::endl;

    LeitorConfig leitor("servidores_config.txt");
    const auto& it = leitor.getServidores().find(0);

    MockCliente cliente_1("127.0.0.1", 1600, leitor.getServidores(), leitor.getSequenciador());    
    json transacao_json_1 = {
        {"read", {
            {"x", "y"}
        }},
        {"write", {
            {"x", "AAA"},
            {"y", "AAA"}
        }}
    };

    // Cliente 1 cria transação
    // lendo e modificando os valores de x e y
    cliente_1.criarTransacao(transacao_json_1);
    // Cliente 1 envia transação
    cliente_1.enviarTransacao();

    // Cliente 1 cria transação 
    // lendo e modificando os valores de x e y
    cliente_1.criarTransacao(transacao_json_1);

    MockCliente cliente_2("127.0.0.1", 1601, leitor.getServidores(), leitor.getSequenciador());
    json transacao_json_2 = {
        {"read", {
            {"x", "y"}
        }},
        {"write", {
            {"x", "BBB"},
            {"y", "BBB"}
        }}
    };

    // Cliente 2 cria transação 
    // lendo e modificando os valores de x e y
    cliente_2.criarTransacao(transacao_json_2);
    // Cliente 2 envia a transação antes do cliente 1
    cliente_2.enviarTransacao();

    // Cliente 1 envia transação que será abortada
    cliente_1.enviarTransacao();
}

void teste2() {
    std::cout << "Função 2 executada!" << std::endl;
}

int main(int argc, char* argv[]) {

    // Converter o argumento para inteiro
    int entrada = std::atoi(argv[1]);

    // Switch para decidir qual função chamar
    switch (entrada) {
        case 0:
            teste0();
            break;
        case 1:
            teste1();
            break;
        case 2:
            teste2();
            break;
        default:
            std::cout << "Entrada inválida!" << std::endl;
            break;
    }

    return 0;
}