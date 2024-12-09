#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include "meia.hpp"
#include <nlohmann/json.hpp>

class Servidor : public Meia {
public:
    Servidor(int serv_id, std::string serv_ip, int serv_port, std::string db);
    ~Servidor();
    void executar();
    std::thread inputThread;

private:
    int id;
    int proxima_mensagem = 1;
    std::vector<nlohmann::json> pendentes;
    std::string db_caminho;
    std::map<std::string, std::tuple<std::string, int>> db;  // mapa de item para tupla (valor, versão)

    void lidarComMensagem(int cliente_fd, const std::string& menssagem) override;
    void read(std::string item, int &sockfd);
    void tentarComitar(nlohmann::json json_pedido);
    void printarInfo();
    void waitForCloseCommand();

};

#endif // SERVIDOR_HPP
