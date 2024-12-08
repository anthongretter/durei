#include "../include/sequencer.hpp"

using json = nlohmann::json;

Sequenciador::Sequenciador(std::string ip, const int porta, std::string _servidores_arquivo): Meia(ip, porta) {

    lerServidores(_servidores_arquivo);
    comecarEscutar();

}

int Sequenciador::lerServidores(std::string _servidores_arquivo) {
    // Abrindo o arquivo de entrada
    std::ifstream arquivo(_servidores_arquivo);
    
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return 1;
    }

    std::string linha;
    // Lê o arquivo linha por linha
    while (std::getline(arquivo, linha)) {
        std::string endereco;
        int porta;

        size_t pos_colon = linha.find(":");
        size_t pos_comma = linha.find(",");

        if (pos_colon != std::string::npos && pos_comma != std::string::npos) {
            endereco = linha.substr(pos_colon + 2, pos_comma - pos_colon - 2); 
            porta = std::stoi(linha.substr(pos_comma + 2));

            servidores.emplace_back(endereco, porta);
        }
    }

    // Fechar o arquivo
    arquivo.close();

    int indice_servidor = 0;
    for (const auto& servidor : servidores) {
        registrarOutraMeia(indice_servidor, std::get<0>(servidor), std::get<1>(servidor));
        indice_servidor++;
    }

    return 0;
}


void Sequenciador::lidarComMensagem(const char mensagem[]) {

    std::string tuple_as_string = "{\"ID\": " + std::to_string(indice_mensagem) + ","
                                + "\n\"MENSAGEM\": " + mensagem + "}";
    
    conectarComOutrasMeias();
    int indice_servidor = 0;
    for (const auto& servidor : servidores) {
        std::cout << indice_servidor << std::endl;
        enviarMensagens(indice_servidor, tuple_as_string);
        indice_servidor++;
    }

    indice_mensagem += 1;
}

int Sequenciador::enviarMensagens(int indice_servidor, std::string mensagem) {
    mandarPara(indice_servidor, mensagem);
    return 0;
}
