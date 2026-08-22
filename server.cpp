#include "server.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
extern bool sig_stp;

Server::Server() : port(0), key_serv(""), fd_serv(-1), poll_fds(), clients(), channels()
{
}

Server::Server(const Server &other) : port(other.port), key_serv(other.key_serv), fd_serv(other.fd_serv), poll_fds(other.poll_fds), clients(other.clients), channels(other.channels)
{
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		port = other.port;
		key_serv = other.key_serv;
		fd_serv = other.fd_serv;
		poll_fds = other.poll_fds;
		clients = other.clients;
		channels = other.channels;
	}
	return *this;
}

Server::~Server()
{
    for (size_t i = 0; i < poll_fds.size(); i++)
	{
		if (poll_fds[i].fd != fd_serv)
			close(poll_fds[i].fd);
	}
	if (fd_serv != -1)
		close(fd_serv);
}

int Server::get_port() const
{
	return (port);
}

const std::string &Server::get_key_serv() const
{
	return (key_serv);
}

bool Server::pars_port(const std::string &portStr)
{
	if (portStr.empty() || portStr.size() > 5)
	{
		std::cerr << "Erreur: invalid port: doit etre compris entre 1 et 65535" << std::endl;
		return (false);
	}

    for (size_t i = 0; i < portStr.size(); i++)
    {
        if (portStr[i] < '0' || portStr[i] > '9')
        {
            std::cerr << "Erreur: invalid port: doit etre compris entre 1 et 65535" << std::endl;
            return (false);
        }
    }

	long value = std::strtol(portStr.c_str(), NULL, 10);

	if (value < 1 || value > 65535)
	{
		std::cerr << "Erreur: invalid port: doit etre compris entre 1 et 65535" << std::endl;
		return (false);
	}

	port = static_cast<int>(value);
	return (true);
}


bool Server::check_key_serv(const std::string &key_serv)
{
	if (key_serv.empty())
	{
		std::cerr << "Erreur : mot de passe invalide : ne doit pas être vide ni contenir d’espaces" << std::endl;
		return (false);
	}

    for (size_t i = 0; i < key_serv.size(); i++)
    {
        if (key_serv[i] == ' ' || key_serv[i] == '\t' || key_serv[i] == '\n' || key_serv[i] == '\r')
        {
            std::cerr << "Erreur : mot de passe invalide : ne doit pas être vide ni contenir d’espaces" << std::endl;
            return (false);
        }
    }
	return (true);
}

bool Server::init_serv(const std::string &portStr, const std::string &key_serv)
{
	if (!pars_port(portStr))
		return (false);
	if (!check_key_serv(key_serv))
		return (false);

	this->key_serv = key_serv;
	return (true);
}


bool Server::config_socket()
{
	fd_serv = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_serv == -1)
	{
		std::cerr << "Erreur : socket() a echoue" << std::endl;
		return (false);
	}
 
	int op = 1;
	if (setsockopt(fd_serv, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) == -1)
	{
		std::cerr << "Erreur : socket() a echoue" << std::endl;
		close(fd_serv);
		fd_serv = -1;
		return (false);
	}
 
	if (fcntl(fd_serv, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Erreur : fcntl() a echoue" << std::endl;
		close(fd_serv);
		fd_serv = -1;
		return (false);
	}
 
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
 
	if (bind(fd_serv, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
	{
		std::cerr << "Erreur : bind() a echoue (port déjà utilisé)" << std::endl;
		close(fd_serv);
		fd_serv = -1;
		return (false);
	}
 
	if (listen(fd_serv, SOMAXCONN) == -1)
	{
		std::cerr << "Erreur : listen() a echoue" << std::endl;
		close(fd_serv);
		fd_serv = -1;
		return (false);
	}
    return (true);
}

bool Server::start_serv()
{
    if (!config_socket())
        return (false);

    struct pollfd listen_poll;
	listen_poll.fd = fd_serv;
	listen_poll.events = POLLIN;
	listen_poll.revents = 0;
	poll_fds.push_back(listen_poll);

    std::cout << "Le serveur ecoute sur le port " << port << std::endl;
    return (true);
}

bool Server::set_non_block(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Erreur: fcntl() echoue" << std::endl;
		return (false);
	}
	return (true);
}

bool Server::accept_new_client()
{
	int client_fd = accept(fd_serv, NULL, NULL);
	if (client_fd == -1)
	{
		std::cerr << "Erreur: accept() echoue" << std::endl;
		return (false);
	}
 
	if (!set_non_block(client_fd))
	{
		close(client_fd);
		return (false);
	}
 
	struct pollfd client_poll;
	client_poll.fd = client_fd;
	client_poll.events = POLLIN;
	client_poll.revents = 0;
	poll_fds.push_back(client_poll);

    clients[client_fd] = Client(client_fd);
 
	std::cout << "Nouveau client connecte fd :  " << client_fd  << std::endl;
	return (true);
}

void Server::deconnect_client(size_t indice)
{
	int fd = poll_fds[indice].fd;
	supp_from_all_channels(fd);
	std::cout << "Client deconnecte fd : " << fd  << std::endl;
	clients.erase(fd);
	close(fd);
	poll_fds.erase(poll_fds.begin() + indice);
}

bool Server::lire_depuis_client(size_t indice)
{
	int fd = poll_fds[indice].fd;
	char buffer[1024];
	ssize_t bytes = recv(fd, buffer, sizeof(buffer), 0);
 
	// bytes == 0  -> le client a ferme la connexion (EOF)
	// bytes == -1 -> erreur reseau reelle
	if (bytes <= 0)
	{
		deconnect_client(indice);
		return (false);
	}
 
	clients[fd].append_to_buffer(buffer, static_cast<size_t>(bytes));
 
	while (clients[fd].is_line_complete())
	{
		std::string line = clients[fd].extract_line();

		Message msg = parse_message(line);
		process_command(fd, msg);
	}
 
	return (true);
}

bool Server::send_repons_to_client(size_t index)
{
	int fd = poll_fds[index].fd;
	const std::string &out = clients[fd].get_outp_Buffer();
 
	if (out.empty())
	{
		poll_fds[index].events &= ~POLLOUT;
		return (true);
	}
 
	ssize_t sent = send(fd, out.c_str(), out.size(), 0);
	if (sent <= 0)
	{
		deconnect_client(index);
		return (false);
	}
 
	clients[fd].consume_outp(static_cast<size_t>(sent));
 
	if (clients[fd].get_outp_Buffer().empty())
		poll_fds[index].events &= ~POLLOUT;
 
	return (true);
}
 
void Server::run_serv()
{
	while (!sig_stp)
	{
		int ret = poll(&poll_fds[0], poll_fds.size(), -1);
		if (ret == -1)
		{
			if (errno == EINTR)
				break;
			std::cerr << "Erreur: poll() echoue" << std::endl;
			break;
		}
		size_t i = 0;
		while (i < poll_fds.size())
		{
			if (poll_fds[i].fd == fd_serv)
			{
				if (poll_fds[i].revents & POLLIN)
					accept_new_client();
				i++;
				continue;
			}
 
			// Le client s'est deconnecte (ou erreur) : on ferme et on
			// retire son fd du tableau surveille par poll(). Sans ca,
			if (poll_fds[i].revents & (POLLHUP | POLLERR))
			{
				deconnect_client(i);
				continue;
			}
 
			if (poll_fds[i].revents & POLLIN)
			{
				if (!lire_depuis_client(i))
					continue;
			}

			// si fd est pret en ecriture.
			if (poll_fds[i].revents & POLLOUT)
			{
				if (!send_repons_to_client(i))
					continue;
			}
			i++;
		}
	}
}