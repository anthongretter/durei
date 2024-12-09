#include <iostream>
#include "../include/servidor.hpp"
#include "../include/leitor_config.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <id_servidor>\n";
        return 1;
    }

    int id_servidor;
    try {
        id_servidor = std::stoi(argv[1]);
    } catch (...) {
        std::cerr << "ID do servidor inválido.\n";
        return 1;
    }

    std::string db_caminho = "dataBaseServers/dataSet.json";
    std::string arq = "servidores_config.txt";
    try {
        LeitorConfig leitor(arq);
        auto it = leitor.getServidores().find(id_servidor);
        if (it != leitor.getServidores().end()) {
            const auto& info_serv = it->second;
            Servidor servidor(id_servidor, std::get<0>(info_serv), std::get<1>(info_serv), db_caminho);
            servidor.executar();
        } else {
            std::cerr << "ID do servidor não está no arquivo " << arq << "\n";
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}