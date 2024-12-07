#include <iostream>
#include "../include/cliente.hpp"
#include "../include/meia.hpp"


int main(int argc, char* argv[]) {
//    if (argc != 2) {
//        std::cerr << "Uso: " << argv[0] << " <id_cliente>\n";
//        return 1;
//    }
//
//    int cliente_id;
//    try {
//        cliente_id = std::stoi(argv[1]);  // Converte o argumento para inteiro
//    } catch (...) {
//        std::cerr << "ID do cliente inválido.\n";
//        return 1;
//    }
//
//    Cliente cliente(cliente_id);
//
//    // Lê o arquivo de configuração dos servidores
//    if (cliente.lerConfiguracaoServidores("src/servidores_config.txt")) return 1;
//
//    // Executa o cliente
//    cliente.executar();

    // Exemplo Meia:
    MeiaBobaServidor sv("127.0.0.1", 8000);

    MeiaBobaCliente c("127.0.0.1", 8001);

    std::this_thread::sleep_for(std::chrono::seconds(1)); // Simular
    c.mandarPara("127.0.0.1", 8000, "Olá MeiaBobaServidor sou " + c.cabecalho());
    c.comecarEscutar();

    std::this_thread::sleep_for(std::chrono::seconds(3)); // Simular


    sv.pararDeEscutar();

    return 0;
}