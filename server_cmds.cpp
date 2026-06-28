#include "server.hpp"

void Server::queue_response(int fd, const std::string &line)
{
	clients[fd].queue_output(line);

	for (size_t i = 0; i < poll_fds.size(); i++)
	{
		if (poll_fds[i].fd == fd)
		{
			poll_fds[i].events |= POLLOUT;
			break;
		}
	}
}

std::string Server::format_code(int code) const
{
	std::string result;

	result += '0' + (code / 100) % 10;
	result += '0' + (code / 10) % 10;
	result += '0' + code % 10;
	return (result);
}

/*
    Construit et envoie une reponse numerique IRC standard, au format :
    ":<nom-serveur> <code> <cible> <texte>\r\n"
*/
void Server::send_numeric_reply(int fd, int code, const std::string &target,
		const std::string &text)
{
	std::string line = ":ircserv " + format_code(code) + " " + target + " " + text + "\r\n";
	queue_response(fd, line);
}

void Server::process_command(int fd, const Message &msg)
{
	if (msg.command.empty())
        return;
	if (msg.command == "PASS")
		handl_pass(fd, msg);
	else if (msg.command == "NICK")
		handl_nick(fd, msg);
	else if (msg.command == "USER")
		handl_user(fd, msg);
	else if (msg.command == "JOIN")
		handl_join(fd, msg);
	else if (msg.command == "PART")
		handl_part(fd, msg);
	else if (msg.command == "PRIVMSG")
		handl_privmsg(fd, msg);
	else if (msg.command == "KICK")
		handl_kick(fd, msg);
	else if (msg.command == "INVITE")
		handl_invit(fd, msg);
	else if (msg.command == "TOPIC")
		handl_topic(fd, msg);
	else if (!clients[fd].is_registered())
		send_numeric_reply(fd, 451, "*", ":You have not registered");
	else
	{
		// Commande pas encore geree MODE 
		std::string nick = clients[fd].get_nick();
		send_numeric_reply(fd, 421, nick, msg.command + " :Unknown command");
	}

}

void Server::try_register(int fd)
{
	if (clients[fd].is_registered())
		return;

	if (!clients[fd].pass_ok())
		return;
	if (clients[fd].get_nick().empty())
		return;
	if (clients[fd].get_user().empty())
		return;

	clients[fd].set_registered(true);

	std::string welcome = ":Welcome to the ft_irc network, "
			+ clients[fd].get_nick() + "!"
			+ clients[fd].get_user() + "@localhost";
	send_numeric_reply(fd, 1, clients[fd].get_nick(), welcome);
}

void Server::handl_pass(int fd, const Message &msg)
{
	if (clients[fd].is_registered())
	{
		send_numeric_reply(fd, 462, "*", ":You may not reregister");
		return;
	}

	if (msg.params.empty())
	{
		send_numeric_reply(fd, 461, "*", "PASS :Not enough parameters");
		return;
	}

	if (msg.params[0] == key_serv)
	{
		clients[fd].set_pass_ok(true);
		try_register(fd);
	}
	else
		send_numeric_reply(fd, 464, "*", ":Password incorrect");
}

bool Server::is_nick_taken(const std::string &nick, int exclude_fd) const
{
	for (std::map<int, Client>::const_iterator it = clients.begin();
			it != clients.end(); ++it)
	{
		if (it->first != exclude_fd && it->second.get_nick() == nick)
			return (true);
	}
	return (false);
}

void Server::handl_nick(int fd, const Message &msg)
{
	if (msg.params.empty())
	{
		send_numeric_reply(fd, 431, "*", ":No nickname given");
		return;
	}

	const std::string &nick = msg.params[0];

	if (is_nick_taken(nick, fd))
	{
		send_numeric_reply(fd, 433, "*", nick + " :Nickname is already in use");
		return;
	}

	clients[fd].set_nick(nick);
	try_register(fd);
}

void Server::handl_user(int fd, const Message &msg)
{
	if (clients[fd].is_registered())
	{
		send_numeric_reply(fd, 462, "*", ":You may not reregister");
		return;
	}

	if (msg.params.size() < 4)
	{
		send_numeric_reply(fd, 461, "*", "USER :Not enough parameters");
		return;
	}

	clients[fd].set_user(msg.params[0]);
	try_register(fd);
}




