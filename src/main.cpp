#include <iostream>
#include "../include/cliente.hpp"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <id_cliente>\n";
        return 1;
    }

    int cliente_id;
    try {
        cliente_id = std::stoi(argv[1]);  // Converte o argumento para inteiro
    } catch (...) {
        std::cerr << "ID do cliente inválido.\n";
        return 1;
    }

    Cliente cliente(cliente_id);

    // Lê o arquivo de configuração dos servidores
    if (cliente.lerConfiguracaoServidores("src/servidores_config.txt")) return 1;

    // Executa o cliente
    cliente.executar();

    return 0;
}