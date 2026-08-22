#include "server.hpp"
#include <iostream>
#include <csignal>
#include <unistd.h>
bool sig_stp = 0;

void handle_signal(int sig)
{
	(void)sig;
	sig_stp = 1;
}
int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, handle_signal);
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	Server server;

	if (!server.init_serv(argv[1], argv[2]))
		return (1);

	std::cout << "[OK] Arguments valides." << std::endl;
	std::cout << "     port     = " << server.get_port() << std::endl;
	std::cout << "     password = " << server.get_key_serv() << std::endl;

    if (!server.start_serv())
	    return (1);

    server.run_serv();
    
    /*
        g++ -Wall -Wextra -Werror -std=c++98 main.cpp Server.cpp -o ircserv
        ./ircserv 6667 monmotdepasse
        nc -zv 127.0.0.1 6667
    */

	return (0);
}