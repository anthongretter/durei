#include <iostream>
#include "../include/cliente.hpp"
#include "../include/leitor_config.hpp"

// main do cliente
// deve ser indicado o ip e porta do cliente

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <ip_cliente> <porta_cliente>\n";
        return 1;
    }

    std::string cliente_ip = argv[1];
    cliente_ip.erase(0, cliente_ip.find_first_not_of(" \t"));
    cliente_ip.erase(cliente_ip.find_last_not_of(" \t") + 1);
    for (char c : cliente_ip) {
        if (!isdigit(c) && c != '.') {
            std::cerr << "IP do cliente inválido\n";
            return 1;
        }
    }
    int cliente_port;
    try {
        cliente_port = std::stoi(argv[2]);  // Converte o argumento para inteiro
    } catch (...) {
        std::cerr << "Porta do cliente inválida.\n";
        return 1;
    }

    try {
        LeitorConfig leitor("servidores_config.txt");
        const auto& it = leitor.getServidores().find(0);
        Cliente cliente(cliente_ip, cliente_port, leitor.getServidores(), leitor.getSequenciador());
        cliente.executar();
    } catch (const std::runtime_error& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}