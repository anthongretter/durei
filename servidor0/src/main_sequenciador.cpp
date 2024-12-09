#include <iostream>
#include "../include/sequenciador.hpp"
#include "../include/leitor_config.hpp"

int main(int argc, char* argv[]) {
    if (argc != 1) {
        std::cerr << "Uso: " << argv[0] << " <id_servidor>\n";
        return 1;
    }

    std::string arq = "servidores_config.txt";
    LeitorConfig leitor(arq);
    const auto& seq_info = leitor.getSequenciador();
    Sequenciador sequenciador(std::get<0>(seq_info), std::get<1>(seq_info), leitor.getServidores());
    sequenciador.executar();
    return 0;
}