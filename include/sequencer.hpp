#ifndef SEQUENCIADOR_HPP
#define SEQUENCIADOR_HPP

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "meia.hpp"

class Sequenciador : public Meia {
public:
    Sequenciador(std::string ip, const int porta, std::string _servidores_arquivo);
    int enviarMensagens(int indice_servidor, std::string mensagem);

private:
    std::string ip;
    std::vector<std::tuple<std::string, int>> servidores; // Mapa de servidores (ID -> (IP, Porta))

    int indice_mensagem = 0;
    int lerServidores(std::string _servidores);
    void lidarComMensagem(const char mensagem[]) override;
};

#endif // SEQUENCIADOR_HPP