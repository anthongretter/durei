#include "../include/mockCliente.hpp"

MockCliente::MockCliente(std::string cliente_ip, int cliente_port, std::map<int, std::tuple<std::string, int>> serv, 
                std::tuple<std::string, int> seq) : Cliente(cliente_ip, cliente_port, serv, seq) {}

void MockCliente::criarTransacao(json transacao_json)
{

    int status = 0;
    int id_servidor = escolherServidorAleatorio();
    auto it = servidores.find(id_servidor);
    const auto& info_serv = it->second;
    std::tuple id_info_serv = std::tuple_cat(std::make_tuple(id_servidor), info_serv);
    std::tuple<std::string, int> cliente_info(ip, porta);
    auto nova_transacao = std::make_shared<Transacao>(status, cliente_info, sequenciador, id_info_serv);
    transacoes[status] = nova_transacao;
   
    transacoes[0] = nova_transacao;
    auto it_ = transacoes.find(status);
    Transacao& t = *(it_->second);

    for (const auto& item : transacao_json["read"]) {
        t.read(item);
    }

    for (const auto& [key, value] : transacao_json["write"].items()) {
        t.write(key, value);
    }


}


void MockCliente::enviarTransacao() {

    auto it = transacoes.find(0);
    Transacao& t = *(it->second);

    t.commit();

}