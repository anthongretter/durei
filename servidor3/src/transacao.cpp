#include "../include/transacao.hpp"
#include <iostream>
#include <sys/socket.h>
#include <variant>
#include <arpa/inet.h>
#include <nlohmann/json.hpp>

Transacao::Transacao(int t, std::tuple<std::string, int> c, std::tuple<std::string, int> seq,
            std::tuple<int, std::string, int> serv)
    : Meia(std::get<0>(c), std::get<1>(c))
{
    t = t;
    sequenciador = seq;
    servidor = serv;
}

void Transacao::read(std::string item) {
    auto it = ws.find(item);
    if (it != ws.end()) {
        std::cout << "ITEM " << item << " ENCONTRADO EM WS -- VALOR: " << it->second << "\n";
    } else {
        std::cout << "PEDINDO ITEM " << item << " PARA SERVIDOR " << std::get<0>(servidor) << "\n";
        nlohmann::json mensagem = {
            {"tipo", "read"},
            {"item", item}
        };
        std::string json_str = mensagem.dump(4);
        std::string to_ip = std::get<1>(servidor);
        int to_port = std::get<2>(servidor);
        std::string resposta = mandarParaEReceber(to_ip, to_port, json_str);
        if (resposta == "") return;
        nlohmann::json json_resposta = nlohmann::json::parse(resposta);
        int resultado = json_resposta["resultado"];
        if (resultado == 0) {
            std::cout << "ITEM " << item << " NÃO ENCONTRADO NO SERVIDOR\n";
        } else {
            std::string valor = json_resposta["valor"];
            int versao = json_resposta["versao"];
            rs[item] = {valor, versao};
            std::cout << "ITEM " << item << " RECEBIDO E ADICIONADO A RS  -- VALOR: " <<
                        valor << "   -- VERSÃO: " << versao << "\n";
        }
    }
}

void Transacao::write(std::string item, std::string valor) {
    auto it = ws.find(item);
    if (it != ws.end()) {
        it->second = valor;
    } else {
        ws[item] = valor;
    }
    std::cout << "VALOR " << valor << " ESCRITO NA VARIÁVEL " << item << " EM WS\n";
}

void Transacao::commit() {
    nlohmann::json mensagem = {
        {"tipo", "commit"},
        {"from", _ip},
        {"port", _porta},
        {"transacao", t},
        {"s_id", std::get<0>(servidor)}, // manda o id do servidor (podia ser qualquer servidor) para só 1 dos servidores responder o resultado da transacao evitando o envio de mensagens desnecessárias dos outros servidores (evita congestionmento na rede)
        {"rs", {}},
        {"ws", {}}
    };
    for (const auto& item : rs) {
        const std::string& var = item.first;
        const std::tuple<std::string, int>& tupla = item.second;
        mensagem["rs"][var] = {std::get<0>(tupla), std::get<1>(tupla)};
    }
    for (const auto& item : ws) {
        const std::string& var = item.first;
        const std::string& valor = item.second;
        mensagem["ws"][var] = valor;
    }

    std::string json_str = mensagem.dump(4);

    mandarPara(std::get<0>(sequenciador), std::get<1>(sequenciador), json_str);
    escutar();
}

void Transacao::lidarComMensagem(int cliente_fd, const std::string& menssagem) {
    nlohmann::json json_mensagem = nlohmann::json::parse(menssagem);
    if (json_mensagem["tipo"] == "abortado") {
        std::cout << "TRANSAÇÃO ABORTADA POIS CONTÉM INFORMAÇÕES OBSOLETAS\n";
        pararDeEscutar();
    } else if (json_mensagem["tipo"] == "comitado") {
        std::cout << "TRANSAÇÃO EFETIVADA COM SUCESSO!\n";
        pararDeEscutar();
    }
}

void Transacao::printarInfo() {
    std::cout << "Conteúdo de rs:" << std::endl;
    // Iterando sobre o mapa rs
    for (const auto& entry : rs) {
        const std::string& item = entry.first;
        const auto& tupla = entry.second;
        const std::string& valor = std::get<0>(tupla);
        const int& versao = std::get<1>(tupla);

        std::cout << "Item: " << item << ", Valor: " << valor << ", Versão: " << versao << "\n";
    }

    std::cout << "Conteúdo de ws:" << std::endl;
    // Iterando sobre o mapa ws
    for (const auto& entry : ws) {
        const std::string& item = entry.first;
        const std::string& valor = entry.second;

        std::cout << "Item: " << item << ", Valor: " << valor << "\n";
    }
}

