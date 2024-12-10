#ifndef LEITOR_CONFIG_HPP
#define LEITOR_CONFIG_HPP

#include <string>
#include <map>

// Faz a leitura do arquivo de configuração (servidores_config.txt)
class LeitorConfig {
public:
    LeitorConfig() = default;
    LeitorConfig(std::string arq);
    const std::map<int, std::tuple<std::string, int>>& getServidores();
    const std::tuple<std::string, int>& getSequenciador();


private:
    std::map<int, std::tuple<std::string, int>> servidores;
    std::tuple<std::string, int> sequenciador;
    std::string arquivo;
};

#endif // TRANSACAO_HPP
