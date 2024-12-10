#ifndef TRANSACAO_HPP
#define TRANSACAO_HPP

#include <string>
#include <map>
#include "meia.hpp"

// Transação herda de Meia para utilizar a interface de sockets
// Cada socket pode ter várias transações

class Transacao : public Meia {
public:
    Transacao() = default;
    Transacao(int t, std::tuple<std::string, int> c, std::tuple<std::string, int> seq, std::tuple<int, std::string, int> serv);
    void read(std::string item);
    void write(std::string item, std::string valor);
    void commit();
    void printarInfo();

private:
    int t;
    std::tuple<int, std::string, int> servidor;
    std::tuple<std::string, int> sequenciador;
    std::map<std::string, std::tuple<std::string, int>> rs; 
    std::map<std::string, std::string> ws; 

    void lidarComMensagem(int cliente_fd, const std::string& menssagem) override;

};

#endif // TRANSACAO_HPP
