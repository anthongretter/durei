#include "../include/cliente.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <tuple>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <algorithm>
#include <cctype>

Cliente::Cliente(std::string cliente_ip, int cliente_port, std::map<int, std::tuple<std::string, int>> serv, 
                std::tuple<std::string, int> seq) {
    porta = cliente_port;
    ip = cliente_ip;
    servidores = serv;
    sequenciador = seq;
}

// retorno do comando ls para ver as transacoes ainda nao comitadas
void Cliente::printarTransacoes() {
    for (const auto& entry : transacoes) {
        int idTransacao = entry.first;           // ID da transação
        std::cout << "transaction_" << idTransacao << "\n";
    }
}

// escolhe um servidor aleatório para fazer a requisição de read(<variavel>)
int Cliente::escolherServidorAleatorio() {
    std::vector<int> chaves;
    
    // Preenche o vetor de chaves com as chaves do mapa
    for (const auto& pair : servidores) {
        chaves.push_back(pair.first);
    }
    
    // Inicializa o gerador de números aleatórios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chaves.size() - 1);
    
    // Seleciona uma chave aleatória do vetor
    int chave_aleatoria = chaves[dis(gen)];
    return chave_aleatoria;
}

// executa o cliente, esperando ele digitar algo no terminal
void Cliente::executar() {
    std::cout << "Cliente iniciado. Digite 'close' para encerrar.\n";
    std::string comando;

    int status = 0;
    int new_t_id = 1;

    while (true) {
        // usuário nao está "dentro" de nenhuma transação
        if (status == 0) {
            std::cout << "> ";
            std::getline(std::cin, comando);
            // Tirando espaços do começo e fim
            comando.erase(0, comando.find_first_not_of(" \t"));
            comando.erase(comando.find_last_not_of(" \t") + 1);

            if (comando == "close") {
                break;
            } else if (comando == "ls") {
                printarTransacoes();
            } else if (comando.rfind("new", 0) == 0) {  // Cria nova transação
                std::istringstream stream(comando);
                std::string new_str, servidor;
                stream >> new_str >> servidor;
                new_str.erase(0, new_str.find_first_not_of(" \t")); // Remove espaços no início
                new_str.erase(new_str.find_last_not_of(" \t") + 1); // Remove espaços no final
                if (new_str != "new") {
                    std::cout << "Comando inválido\n";
                } else {

                    // Verificar se foi dado um id de servidor, caso contrário escolhe um aleatorio
                    if (!servidor.empty()) {
                        try {
                            int id_servidor = std::stoi(servidor);
                            auto it = servidores.find(id_servidor);
                            if (it != servidores.end()) {
                                const auto& info_serv = it->second;
                                std::tuple id_info_serv = std::tuple_cat(std::make_tuple(id_servidor), info_serv);
                                std::tuple<std::string, int> cliente_info(ip, porta);
                                status = new_t_id;
                                auto nova_transacao = std::make_shared<Transacao>(status, cliente_info, sequenciador, id_info_serv);
                                new_t_id++;
                                transacoes[status] = nova_transacao;
                            } else {
                                std::cout << "ID " << servidor << " do servidor inválido\n";
                            }
                        } catch (...) {
                            std::cout << "ID " << servidor << " do servidor inválido\n";
                        }
                    } else {
                        int s_id = escolherServidorAleatorio();
                        auto it = servidores.find(s_id);
                        const auto& info_serv = it->second;
                        std::tuple id_info_serv = std::tuple_cat(std::make_tuple(s_id), info_serv);
                        std::tuple<std::string, int> cliente_info(ip, porta);
                        status = new_t_id;
                        auto nova_transacao = std::make_shared<Transacao>(status, cliente_info, sequenciador, id_info_serv);
                        transacoes[status] = nova_transacao;
                        new_t_id++;
                    }
                }

            } else if (comando.rfind("transaction_", 0) == 0) {
                std::string numero_str = comando.substr(12);
                try {
                    int transacao_id = std::stoi(numero_str);
                    if (transacoes.find(transacao_id) != transacoes.end()) {
                        status = transacao_id;  // Muda o status para a transação encontrada
                    } else {
                        std::cout << "Transação com ID " << transacao_id << " não existe.\n";
                    }
                } catch (...) {
                    std::cout << "Comando inválido. Não foi possível converter o número da transação.\n";
                }
            } else if (comando.rfind("write(", 0) == 0 || comando.rfind("read(", 0) == 0) {
                std::cout << "Entre em uma transação para usar o comando " << comando << "\n";
            } else {
                std::cout << "Comando desconhecido.\n";
            }
        } else {   // usuário está "dentro" da transação de id <status>
            std::cout << ">" << "transation_" << status << "> ";
            std::getline(std::cin, comando);
            // Tirando espaços do começo e fim
            comando.erase(0, comando.find_first_not_of(" \t"));
            comando.erase(comando.find_last_not_of(" \t") + 1);

            auto it = transacoes.find(status);
            Transacao& t = *(it->second);

            if (comando == "close") {
                break;
            } else if (comando == "ls") {
                // mostrar ws e rs
                t.printarInfo();
            } else if (comando == "exit") {
                status = 0;
            } else if (comando.rfind("write(", 0) == 0 && comando.back() == ')') {
                size_t inicio = comando.find('(');
                size_t fim = comando.size() - 1;
                if (inicio != fim + 1) {
                    std::string parametros = comando.substr(inicio + 1, fim - inicio - 1);
                    size_t pos_virgula = parametros.find(',');

                    std::string item, valor;

                    if (pos_virgula != std::string::npos) {
                        // Se a vírgula existe, dividir em variável e valor
                        item = parametros.substr(0, pos_virgula);
                        item.erase(0, item.find_first_not_of(" \t")); // Remove espaços no início
                        item.erase(item.find_last_not_of(" \t") + 1); // Remove espaços no final
                        valor = parametros.substr(pos_virgula + 1);
                        valor.erase(0, valor.find_first_not_of(" \t"));
                        valor.erase(valor.find_last_not_of(" \t") + 1);
                        t.write(item, valor);
                    } else {
                        std::cout << "Comando write deve ser da forma: write(item, valor) \n";
                    }

                } else {
                    std::cout << "Comando write deve ser da forma: write(item, valor)\n";
                }
            } else if (comando.rfind("read(", 0) == 0 && comando.back() == ')') {
                size_t inicio = comando.find('(');
                size_t fim = comando.size() - 1;
                if (inicio != fim + 1) {
                    std::string item = comando.substr(inicio + 1, fim - inicio - 1);
                    t.read(item);
                } else {
                    std::cout << "Comando read deve ser da forma: read(item)\n";
                }
            } else if (comando == "commit") {
                t.commit();
                transacoes.erase(it);
                status = 0;
            } else if (comando == "abort") {
                transacoes.erase(it);
                status = 0;
            } else {
                std::cout << "Comando desconhecido.\n";
            }

        }

    }
}
