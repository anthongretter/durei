#ifndef TRANSACAO_HPP
#define TRANSACAO_HPP

#include <string>
#include <map>

class Transacao {
public:
    Transacao() = default;
    Transacao(int c_id, int t, int s_id, std::map<int, std::tuple<std::string, int>> servidores);
    void read(std::string item);
    void write(std::string item, std::string valor);
    void commit();
    void printarInfo();

private:
    int c_id, t, s_id;
    std::map<int, std::tuple<std::string, int>> servidores;
    std::map<std::string, std::tuple<std::string, int>> rs; 
    std::map<std::string, std::string> ws; 

};

#endif // TRANSACAO_HPP
