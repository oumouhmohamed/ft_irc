#include "server.hpp"

std::string Server::client_prefix(int fd) const
{
	std::map<int, Client>::const_iterator it = clients.find(fd);

	if (it == clients.end())
		return ("unknown");
	return (it->second.get_nick() + "!" + it->second.get_user() + "@localhost");
}


Channel *Server::find_channel(const std::string &name)
{
	std::map<std::string, Channel>::iterator it = channels.find(name);

	if (it == channels.end())
		return (NULL);
	return (&it->second);
}


int Server::find_client_fd_by_nickname(const std::string &nick) const
{
	for (std::map<int, Client>::const_iterator it = clients.begin();
			it != clients.end(); ++it)
	{
		if (it->second.get_nick() == nick)
			return (it->first);
	}
	return (-1);
}

void Server::diffusion_msg_to_channel(const Channel &channel, int exclude_fd,
		const std::string &line)
{
	const std::vector<int> &members = channel.get_members();

	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i] != exclude_fd)
			queue_response(members[i], line);
	}
}

void Server::handl_join(int fd, const Message &msg)
{
	if (!clients[fd].is_registered())
	{
		send_numeric_reply(fd, 451, "*", ":You have not registered");
		return;
	}

	if (msg.params.empty())
	{
		send_numeric_reply(fd, 461, clients[fd].get_nick(), "JOIN :Not enough parameters");
		return;
	}

	const std::string &channel_name = msg.params[0];

	if (channel_name.empty() || channel_name[0] != '#')
	{
		send_numeric_reply(fd, 403, clients[fd].get_nick(), channel_name + " :No such channel");
		return;
	}

	if (channels.find(channel_name) == channels.end())
		channels[channel_name] = Channel(channel_name);

	Channel &channel = channels[channel_name];

	if (channel.is_member(fd))
		return;

	channel.add_member(fd);
	if (channel.get_members().size() == 1)   // premier membre = créateur
    	channel.add_operator(fd);

	std::string join_line = ":" + client_prefix(fd) + " JOIN " + channel_name + "\r\n";
	diffusion_msg_to_channel(channel, -1, join_line);

	const std::vector<int> &members = channel.get_members();
	std::string names;

	for (size_t i = 0; i < members.size(); i++)
	{
		if (i > 0)
			names += " ";
		names += clients[members[i]].get_nick();
	}

	send_numeric_reply(fd, 353, clients[fd].get_nick(), "= " + channel_name + " :" + names);
	send_numeric_reply(fd, 366, clients[fd].get_nick(), channel_name + " :End of /NAMES list");
}


void Server::handl_part(int fd, const Message &msg)
{
	if (!clients[fd].is_registered())
	{
		send_numeric_reply(fd, 451, "*", ":You have not registered");
		return;
	}

	if (msg.params.empty())
	{
		send_numeric_reply(fd, 461, clients[fd].get_nick(), "PART :Not enough parameters");
		return;
	}

	const std::string &channel_name = msg.params[0];
	Channel *channel = find_channel(channel_name);

	if (!channel || !channel->is_member(fd))
	{
		send_numeric_reply(fd, 442, clients[fd].get_nick(),
				channel_name + " :You're not on that channel");
		return;
	}

	std::string partLine = ":" + client_prefix(fd) + " PART " + channel_name + "\r\n";
	diffusion_msg_to_channel(*channel, -1, partLine);

	channel->retirer_client(fd);

	if (channel->is_empty())
		channels.erase(channel_name);
}

void Server::handl_privmsg(int fd, const Message &msg)
{
	if (!clients[fd].is_registered())
	{
		send_numeric_reply(fd, 451, "*", ":You have not registered");
		return;
	}

	if (msg.params.empty())
	{
		send_numeric_reply(fd, 411, clients[fd].get_nick(), ":No recipient given (PRIVMSG)");
		return;
	}

	if (msg.params.size() < 2)
	{
		send_numeric_reply(fd, 412, clients[fd].get_nick(), ":No text to send");
		return;
	}

	const std::string &target = msg.params[0];
	const std::string &text = msg.params[1];
	std::string line = ":" + client_prefix(fd) + " PRIVMSG " + target + " :" + text + "\r\n";

	if (!target.empty() && target[0] == '#')
	{
		Channel *channel = find_channel(target);

		if (!channel || !channel->is_member(fd))
		{
			send_numeric_reply(fd, 403, clients[fd].get_nick(), target + " :No such channel");
			return;
		}

		diffusion_msg_to_channel(*channel, fd, line);
	}
	else
	{
		int target_fd = find_client_fd_by_nickname(target);

		if (target_fd == -1)
		{
			send_numeric_reply(fd, 401, clients[fd].get_nick(), target + " :No such nick/channel");
			return;
		}

		queue_response(target_fd, line);
	}
}


void Server::supp_from_all_channels(int fd)
{
	std::map<std::string, Channel>::iterator it = channels.begin();

	while (it != channels.end())
	{
		if (it->second.is_member(fd))
		{
			std::string partLine = ":" + client_prefix(fd) + " PART " + it->first + "\r\n";
			diffusion_msg_to_channel(it->second, fd, partLine);
			it->second.retirer_client(fd);
		}

		if (it->second.is_empty())
		{
			std::map<std::string, Channel>::iterator toErase = it;
			++it;
			channels.erase(toErase);
		}
		else
			++it;
	}
}