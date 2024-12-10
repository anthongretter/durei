#include "../include/leitor_config.hpp"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Le o arquivo e salva as informacoes nos atributor servidores e sequenciador
LeitorConfig::LeitorConfig(std::string arq) {
    arquivo = arq;

    std::ifstream arquivoEntrada(arquivo);
    if (!arquivoEntrada.is_open()) {
        throw std::runtime_error("Erro ao abrir o arquivo: " + arquivo + "\n");
    }

    std::string primeira_linha;
    std::getline(arquivoEntrada, primeira_linha);
    std::istringstream primeira_stream(primeira_linha);
    std::string ip_seq, porta_seq_str;
    int porta_seq;

    if (std::getline(primeira_stream, ip_seq, ',') && std::getline(primeira_stream, porta_seq_str)) {
        // Remove espaços em branco do IP
        ip_seq.erase(0, ip_seq.find_first_not_of(" \t"));
        ip_seq.erase(ip_seq.find_last_not_of(" \t") + 1);
        for (char c : ip_seq) {
            if (!isdigit(c) && c != '.') {
                throw std::runtime_error("Ip inválido na linha: " + primeira_linha + " do arquivo " + arquivo + "\n");
            }
        }
        porta_seq_str.erase(0, porta_seq_str.find_first_not_of(" \t"));
        porta_seq_str.erase(porta_seq_str.find_last_not_of(" \t") + 1);
        if (porta_seq_str.empty() || !std::all_of(porta_seq_str.begin(), porta_seq_str.end(), ::isdigit)) {
            throw std::runtime_error("Porta inválida na linha: " + primeira_linha + " do arquivo " + arquivo + "\n");
        }
        try {
            porta_seq = std::stoi(porta_seq_str);
        } catch (...) {
            throw std::runtime_error("Porta inválida na linha: " + primeira_linha + " do arquivo " + arquivo + "\n");
        }
        // Salva no mapa de servidores
        sequenciador = {ip_seq, porta_seq};
    } else {
        throw std::runtime_error("Erro ao processar IP e Porta na linha: " + primeira_linha + " do arquivo " + arquivo + "\n");
    }

    std::string linha;
    while (std::getline(arquivoEntrada, linha)) {
        std::istringstream linhaStream(linha);
        std::string id_str, restante;

        // Lê o ID antes de ":"
        if (!std::getline(linhaStream, id_str, ':')) {
            throw std::runtime_error("Formato inválido na linha: " + linha + " do arquivo " + arquivo + "\n");
        }
        id_str.erase(0, id_str.find_first_not_of(" \t"));
        id_str.erase(id_str.find_last_not_of(" \t") + 1);
        if (id_str.empty() || !std::all_of(id_str.begin(), id_str.end(), ::isdigit)) {
            throw std::runtime_error("ID inválido na linha: " + linha + " do arquivo " + arquivo + "\n");
        }

        // Converte o ID para inteiro
        int s_id;
        try {
            s_id = std::stoi(id_str);
        } catch (...) {
            throw std::runtime_error("ID inválido na linha: " + linha + " do arquivo " + arquivo + "\n");
        }

        // Lê o restante da linha após ":"
        if (!std::getline(linhaStream, restante)) {
            throw std::runtime_error("Formato inválido na linha: " + linha + " do arquivo " + arquivo + "\n");
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
                    throw std::runtime_error("IP inválido na linha: " + linha + " do arquivo " + arquivo + "\n");
                }
            }
            porta_str.erase(0, porta_str.find_first_not_of(" \t"));
            porta_str.erase(porta_str.find_last_not_of(" \t") + 1);
            if (porta_str.empty() || !std::all_of(porta_str.begin(), porta_str.end(), ::isdigit)) {
                throw std::runtime_error("Porta inválida na linha: " + linha + " do arquivo " + arquivo + "\n");
            }
            try {
                porta = std::stoi(porta_str);
            } catch (...) {
                throw std::runtime_error("Porta inválida na linha: " + linha + " do arquivo " + arquivo + "\n");
            }
            // Salva no mapa de servidores
            servidores[s_id] = {ip, porta};
        } else {
            throw std::runtime_error("Erro ao processar IP e Porta na linha: " + linha + " do arquivo " + arquivo + "\n");
        }
    }
}


const std::map<int, std::tuple<std::string, int>>& LeitorConfig::getServidores() {
    return servidores;
}

const std::tuple<std::string, int>& LeitorConfig::getSequenciador() {
    return sequenciador;
}