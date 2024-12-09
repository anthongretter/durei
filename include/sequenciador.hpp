#ifndef SEQUENCIADOR_HPP
#define SEQUENCIADOR_HPP

#include <string>
#include <map>
#include <tuple>
#include "meia.hpp"

class Sequenciador : public Meia {
public:
    Sequenciador(std::string ip, int port, std::map<int, std::tuple<std::string, int>> servs);
    ~Sequenciador();
    void executar();
    std::thread inputThread;

private:
    int num_sequencia = 1;
    std::map<int, std::tuple<std::string, int>> servidores;
    void lidarComMensagem(int cliente_fd, const std::string& menssagem, struct sockaddr_in client_addr) override;
    void waitForCloseCommand();
};

#endif // SEQUENCIADOR_HPP