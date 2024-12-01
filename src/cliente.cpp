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

Cliente::Cliente(int cliente_id) {
    id = cliente_id;
}

int Cliente::lerConfiguracaoServidores(const std::string& arquivo) {
    std::ifstream arquivoEntrada(arquivo);
    if (!arquivoEntrada.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << arquivo << "\n";
        return 1;
    }

    std::string linha;
    while (std::getline(arquivoEntrada, linha)) {
        std::istringstream linhaStream(linha);
        std::string id_str, restante;

        // Lê o ID antes de ":"
        if (!std::getline(linhaStream, id_str, ':')) {
            std::cerr << "Formato inválido na linha: " << linha << " do arquivo " << arquivo << "\n";
            return 1;
        }
        id_str.erase(0, id_str.find_first_not_of(" \t"));
        id_str.erase(id_str.find_last_not_of(" \t") + 1);
        if (id_str.empty() || !std::all_of(id_str.begin(), id_str.end(), ::isdigit)) {
            std::cerr << "ID inválido na linha: " << linha << " do arquivo " << arquivo << "\n";
            return 1;
        }

        // Converte o ID para inteiro
        int s_id;
        try {
            s_id = std::stoi(id_str);
        } catch (...) {
            std::cerr << "ID inválido na linha: " << linha << " do arquivo " << arquivo << "\n";
            return 1;
        }

        // Lê o restante da linha após ":"
        if (!std::getline(linhaStream, restante)) {
            std::cerr << "Formato inválido na linha: " << linha << " do arquivo " << arquivo << "\n";
            return 1;
        }

        // Processa o restante para obter IP e Porta
        std::istringstream restanteStream(restante);
        std::string ip, porta_str;
        int porta;

        if (std::getline(restanteStream, ip, ',') && std::getline(restanteStream, porta_str)) {
            // Remove espaços em branco do IP
            ip.erase(0, ip.find_first_not_of(" \t"));
            ip.erase(ip.find_last_not_of(" \t") + 1);
            for (char c : ip) {
                if (!isdigit(c) && c != '.') {
                    std::cerr << "Ip inválido na linha: " << linha << " do arquivo " << arquivo << "\n";
                    return 1;
                }
            }
            porta_str.erase(0, porta_str.find_first_not_of(" \t"));
            porta_str.erase(porta_str.find_last_not_of(" \t") + 1);
            if (porta_str.empty() || !std::all_of(porta_str.begin(), porta_str.end(), ::isdigit)) {
                std::cerr << "Porta inválida na linha: " << linha << " do arquivo " << arquivo << "\n";
                return 1;
            }
            try {
                porta = std::stoi(porta_str);
            } catch (...) {
                std::cerr << "Porta inválida na linha: " << linha << " do arquivo " << arquivo << "\n";
                return 1;
            }
            // Salva no mapa de servidores
            servidores[s_id] = {ip, porta};
        } else {
            std::cerr << "Erro ao processar IP e Porta na linha: " << linha << " do arquivo " << arquivo << "\n";
            return 1;
        }
    }
    return 0;
}

void Cliente::printarTransacoes() {
    for (const auto& entry : transacoes) {
        int idTransacao = entry.first;           // ID da transação
        std::cout << "transaction_" << idTransacao << "\n";
    }
}

int Cliente::escolherServidor() {
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

void Cliente::executar() {
    std::cout << "Cliente " << id << " iniciado. Digite 'close' para encerrar.\n";
    std::string comando;

    int status = 0;

    while (true) {
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
            } else if (comando == "new") {  // Cria nova transação
                int s_id = escolherServidor();
                status = transacoes.size() + 1;
                Transacao nova_transacao(status, id, s_id, servidores);
                transacoes[status] = nova_transacao;
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
        } else {
            std::cout << ">" << "transation_" << status << "> ";
            std::getline(std::cin, comando);
            // Tirando espaços do começo e fim
            comando.erase(0, comando.find_first_not_of(" \t"));
            comando.erase(comando.find_last_not_of(" \t") + 1);

            auto it = transacoes.find(status);
            Transacao& t = it->second;

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
            } else if (comando == "abort") {
                transacoes.erase(it);
            } else {
                std::cout << "Comando desconhecido.\n";
            }

        }

    }
}
