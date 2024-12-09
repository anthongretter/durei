#include <nlohmann/json.hpp>
#include "../include/servidor.hpp"
#include <fstream>

Servidor::Servidor(int serv_id, std::string serv_ip, int serv_port, std::string db) 
    : Meia(serv_ip, serv_port)
{
    id = serv_id;
    db_caminho = db;
    inputThread = std::thread(&Servidor::waitForCloseCommand, this);
    std::cout << "Servidor " << serv_id << " pronto! (" << serv_ip << ":" << serv_port << ")" << std::endl;
}

Servidor::~Servidor() {
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

void Servidor::read(std::string item, int &sockfd) {
    std::ifstream file(db_caminho);

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para leitura!" << std::endl;
        return;
    }

    nlohmann::json dataSet;
    file >> dataSet;
    file.close();

    auto it = dataSet.find(item);
    if (it != dataSet.end()) {
        std::vector<nlohmann::json> vetor = it->get<std::vector<nlohmann::json>>();
        std::string valor = vetor[0].get<std::string>();
        int versao = vetor[1].get<int>();
        nlohmann::json resposta = {
            {"resultado", 1},   
            {"valor", valor},
            {"versao", versao}
        };
        std::string json_str = resposta.dump(4);
        send(sockfd, json_str.c_str(), json_str.size(), 0);
        std::cout << "ITEM " << item << " ENVIADO COM VALOR " << valor << " E VERSÃO " << versao <<
                    " PARA CLIENTE\n";
    } else {
        nlohmann::json resposta = {
            {"resultado", 0},   
        };
        std::string json_str = resposta.dump(4);
        send(sockfd, json_str.c_str(), json_str.size(), 0);
        std::cout << "ITEM " << item << " PEDIDO PELO CLIENTE NÃO ENCONTRADO\n";
    }
}

void Servidor::tentarComitar(nlohmann::json json_pedido) {
    std::ifstream file(db_caminho);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para leitura!" << std::endl;
        return;
    }
    nlohmann::json dataSet;
    file >> dataSet;
    file.close();

    bool abort = false;
    // Verificar versões
    if (json_pedido.contains("rs") && json_pedido["rs"].is_object()) {
        for (const auto& [item, rs_tupla] : json_pedido["rs"].items()) {
            auto it = dataSet.find(item);
            std::vector<nlohmann::json> vetor = it->get<std::vector<nlohmann::json>>();
            if (rs_tupla[1].get<int>() < vetor[1].get<int>()) {
                abort = true;
            }
        }
    }

    if (abort) {
        nlohmann::json resposta = {
            {"tipo", "abortado"}
        };
        std::string json_resposta = resposta.dump(4);
        if (id == json_pedido["s_id"]) {
            mandarPara(json_pedido["from"], json_pedido["port"], json_resposta); }
        std::cout << "TRANSAÇÃO " << json_pedido["transacao"] << " DO CLIENTE COM IP " << json_pedido["from"] << " E PORTA " << json_pedido["port"] << " ABORTADA\n";
        
    } else {
        if (json_pedido.contains("ws") && json_pedido["ws"].is_object()) {
            for (const auto& [item, valor] : json_pedido["ws"].items()) {
                auto it = dataSet.find(item);
                if (it != dataSet.end()) {
                    std::vector<nlohmann::json> vetor = it->get<std::vector<nlohmann::json>>();
                    int versao = vetor[1].get<int>() + 1;
                    it.value() = {valor, versao};
                } else {
                    dataSet[item] = {valor, 0};
                }
            }
            std::ofstream file2(db_caminho);

            if (!file2.is_open()) {
                std::cerr << "Erro ao abrir o arquivo para escrita!" << std::endl;
                return;
            }

            // Converte o objeto JSON de volta para uma string e grava no arquivo
            file2 << dataSet.dump(4); // O "4" indica o número de espaços de indentação para formatar o JSON

            file2.close();
        }
        nlohmann::json resposta = {
            {"tipo", "comitado"}
        };
        std::string json_resposta = resposta.dump(4);
        if (id == json_pedido["s_id"]) {
            mandarPara(json_pedido["from"], json_pedido["port"], json_resposta); }
        std::cout << "TRANSAÇÃO " << json_pedido["transacao"] << " DO CLIENTE COM IP " << json_pedido["from"] << " E PORTA " << json_pedido["port"] << " CONFIRMADA\n";
        printarInfo();
    }
}

void Servidor::printarInfo() {
    std::ifstream file(db_caminho);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para leitura!" << std::endl;
        return;
    }
    std::cout << "comeco print\n";
    nlohmann::json dataSet;
    file >> dataSet;
    file.close();

    std::cout << "CONTEÚDO ATUAL DO BANCO DE DADOS DO SERVIDOR " << id << std::endl;

    for (const auto& [item, value] : dataSet.items()) {
        std::vector<nlohmann::json> vetor = value.get<std::vector<nlohmann::json>>();
        std::string valor = vetor[0].get<std::string>();
        int versao = vetor[1].get<int>();

        std::cout << "Item: " << item << ", Valor: " << valor << ", Versão: " << versao << "\n";
    }
}

void Servidor::waitForCloseCommand() {
    std::string command;
    while (running) {
        std::cin >> command;
        if (command == "close") {
            pararDeEscutar();
            break;
        }
    }
}

void Servidor::executar() {
    escutar();    
}

void Servidor::lidarComMensagem(int cliente_fd, const std::string& menssagem) {
    nlohmann::json json_m = nlohmann::json::parse(menssagem);
    if (json_m["tipo"] == "read") {
        read(json_m["item"], cliente_fd);
    } else if (json_m["tipo"] == "commit") {
        pendentes.push_back(json_m);
        while (true) {
            bool continua = false;
            for (auto it = pendentes.begin(); it != pendentes.end(); ) {
                if ((*it)["ordem"] == proxima_mensagem) {
                    tentarComitar(*it);
                    it = pendentes.erase(it); // Remove e avança o iterador
                    proxima_mensagem++;
                    continua = true;
                } else {
                    ++it; // Avança o iterador apenas se não remover
                }
            }
            if (!continua) {
                break;
            }
            
        }
    }
    std::cout << "lidou\n";
}