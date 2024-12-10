#include <nlohmann/json.hpp>
#include "../include/sequenciador.hpp"

Sequenciador::Sequenciador(std::string ip, int port, std::map<int, std::tuple<std::string, int>> servs) 
    : Meia(ip, port)
{
    servidores = servs;
    inputThread = std::thread(&Sequenciador::esperarComandoClose, this);
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

// Thread para esperar um "close" de input do terminal
// Serve para fechar as portas corretamente e finalizar
// o programa sem precisar interrompe-lo
void Sequenciador::esperarComandoClose() {
    std::string command;
    while (running) {
        std::cin >> command;
        if (command == "close") {
            pararDeEscutar();
            break;
        }
    }
}

// Adiciona o atributo ordem mensagem json
// vinda do socket escutando e repassa a mensagem
// aos servidores
void Sequenciador::lidarComMensagem(int cliente_fd, const std::string& menssagem) {
    std::cout << "RECEBE MENSAGEM DE UM CLIENTE\n";
    nlohmann::json json_m = nlohmann::json::parse(menssagem);
    json_m["ordem"] = num_sequencia;
    std::string json_requisicao = json_m.dump(4);
    std::cout << "ADICIONADO NUMERO DE SEQUÊNCIA " << num_sequencia << " A MENSAGEM\n";
    std::cout << "ENVIANDO A MENSAGEM AOS SERVIDORES\n";
    for (const auto& serv : servidores) {
        const std::tuple<std::string, int>& tupla = serv.second;
        std::string ip_serv = std::get<0>(tupla);
        int porta_serv = std::get<1>(tupla);
        mandarPara(ip_serv, porta_serv, json_requisicao);
        std::cout << "MENSAGEM ENVIADA A SERVIDOR " << serv.first << "\n";
    }
    num_sequencia++;
}