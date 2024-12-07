#ifndef LEITOR_CONFIG_HPP
#define LEITOR_CONFIG_HPP

#include <string>
#include <map>

class LeitorConfig {
public:
    LeitorConfig() = default;
    LeitorConfig(std::string arq);
    std::map<int, std::tuple<std::string, int>> getServidores();
    std::tuple<std::string, int> getSequenciador();


private:
    std::map<int, std::tuple<std::string, int>> servidores;
    std::tuple<std::string, int> sequenciador;
    std::string arquivo;
};

#endif // TRANSACAO_HPP
