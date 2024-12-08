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
    mandarPara(0, "OK");
}


int Server::enviarMensagens() {

    return 0;
}
