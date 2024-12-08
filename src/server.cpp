#include "../include/server.hpp"

using json = nlohmann::json;

Server::Server(std::string ip, const int porta): Meia(ip, porta) {

    comecarEscutar();

}

void Server::lidarComMensagem(const char mensagem[]) {
   
    // Parsear a string para um objeto JSON
    json parsed_json;
    try {
        parsed_json = json::parse(mensagem);
    } catch (const json::parse_error& e) {
        std::cerr << "Erro ao parsear JSON: " << e.what() << std::endl;
        return;
    }

    std::string port_str = parsed_json["MENSAGEM"]["port"];
    int port = std::stoi(port_str);

    registrarOutraMeia(0, "127.0.0.1", port);
    conectarComOutrasMeias();

    bool abortar = conferirTransacao(parsed_json["MENSAGEM"]);

    if (abortar) {
        mandarPara(0, "Abortar");
    } else {
        mandarPara(0, "Sucesso");
    }

}

bool Server::conferirTransacao(json message_json) {

    std::string filename = "dataBaseServers/dataSet.json";
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para leitura!" << std::endl;
        return true;
    }

    json dataSet;
    file >> dataSet;
    file.close();

    bool abortar = false;
    if (message_json.contains("read")) {
        auto read = message_json["read"];

        // Percorrendo todos os campos dentro do objeto "read"
        for (auto& [key, value] : read.items()) {
            
            // Verificando se o valor é uma lista (array)
            if (value.is_array()) {
                for (size_t i = 0; i < value.size(); ++i) {
                    if ((dataSet.contains(key)) && (value[i] != dataSet[key][i])) {
                        abortar = true;
                    }
                }
            } else {
                abortar = true;
            }
        }
    }

    if (abortar) return true;

    return false;
}