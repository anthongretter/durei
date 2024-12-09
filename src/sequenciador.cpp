#include <nlohmann/json.hpp>
#include "../include/sequenciador.hpp"

Sequenciador::Sequenciador(std::string ip, int port, std::map<int, std::tuple<std::string, int>> servs) 
    : Meia(ip, port)
{
    servidores = servs;
    inputThread = std::thread(&Sequenciador::waitForCloseCommand, this);
    std::cout << "Sequenciador pronto!" << std::endl;
}

Sequenciador::~Sequenciador() {
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

void Sequenciador::executar() {
    escutar();
}

void Sequenciador::waitForCloseCommand() {
    std::string command;
    while (running) {
        std::cin >> command;
        if (command == "close") {
            pararDeEscutar();
            break;
        }
    }
}

void Sequenciador::lidarComMensagem(int cliente_fd, const std::string& menssagem) {
    nlohmann::json json_m = nlohmann::json::parse(menssagem);
    json_m["ordem"] = num_sequencia;
    std::string json_requisicao = json_m.dump(4);

    for (const auto& serv : servidores) {
        const std::tuple<std::string, int>& tupla = serv.second;
        std::string ip_serv = std::get<0>(tupla);
        int porta_serv = std::get<1>(tupla);
        mandarPara(ip_serv, porta_serv, json_requisicao);
    }
    num_sequencia++;
}