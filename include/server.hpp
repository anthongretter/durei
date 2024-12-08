#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "meia.hpp"

using json = nlohmann::json;

class Server : public Meia {
public:
    Server(std::string ip, const int porta);

private:
    std::string ip;
    std::vector<std::tuple<std::string, int>> servidores; // Mapa de servidores (ID -> (IP, Porta))

    int proxima_mensagem = 0;
    void lidarComMensagem(const char mensagem[]) override;
    bool conferirTransacao(json message_json);
    void registrarTransacao() {};
};

#endif // SERVER_HPP