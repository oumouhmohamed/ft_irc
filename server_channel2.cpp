#include "server.hpp"

/*
 KICK <channel> <pseudo> [raison] : seul un operateur du canal
 peut ejecter un membre. Le message KICK est diffuse a TOUT le
 monde, y compris la victime (-1 = personne n'est exclu)
*/
void Server::handl_kick(int fd, const Message &msg)
{
	if (!clients[fd].is_registered())
	{
		send_numeric_reply(fd, 451, "*", ":You have not registered");
		return;
	}

	if (msg.params.size() < 2)
	{
		send_numeric_reply(fd, 461, clients[fd].get_nick(), "KICK :Not enough parameters");
		return;
	}

	const std::string &channel_name = msg.params[0];
	const std::string &target_nick = msg.params[1];
	std::string reason = (msg.params.size() >= 3) ? msg.params[2] : target_nick;

	Channel *channel = find_channel(channel_name);

	if (!channel)
	{
		send_numeric_reply(fd, 403, clients[fd].get_nick(), channel_name + " :No such channel");
		return;
	}

	if (!channel->is_member(fd))
	{
		send_numeric_reply(fd, 442, clients[fd].get_nick(),
				channel_name + " :You're not on that channel");
		return;
	}

	if (!channel->is_operator(fd))
	{
		send_numeric_reply(fd, 482, clients[fd].get_nick(),
				channel_name + " :You're not channel operator");
		return;
	}

	int targetFd = find_client_fd_by_nickname(target_nick);

	if (targetFd == -1 || !channel->is_member(targetFd))
	{
		send_numeric_reply(fd, 441, clients[fd].get_nick(),
				target_nick + " " + channel_name + " :They aren't on that channel");
		return;
	}

	std::string kickLine = ":" + client_prefix(fd) + " KICK " + channel_name
			+ " " + target_nick + " :" + reason + "\r\n";
	diffusion_msg_to_channel(*channel, -1, kickLine);

	channel->retirer_client(targetFd);

	if (channel->is_empty())
		channels.erase(channel_name);
}

/*
** INVITE <pseudo> <channel> : seul un operateur peut inviter.
** Pour l'instant, ca se contente de prevenir le pseudo vise et de
** memoriser l'invitation dans le canal (utile a l'etape 9, quand
** le mode +i empechera de JOIN sans invitation prealable).
*/
void Server::handl_invit(int fd, const Message &msg)
{
	if (!clients[fd].is_registered())
	{
		send_numeric_reply(fd, 451, "*", ":You have not registered");
		return;
	}

	if (msg.params.size() < 2)
	{
		send_numeric_reply(fd, 461, clients[fd].get_nick(), "INVITE :Not enough parameters");
		return;
	}

	const std::string &target_nick = msg.params[0];
	const std::string &channel_name = msg.params[1];

	Channel *channel = find_channel(channel_name);

	if (!channel)
	{
		send_numeric_reply(fd, 403, clients[fd].get_nick(), channel_name + " :No such channel");
		return;
	}

	if (!channel->is_member(fd))
	{
		send_numeric_reply(fd, 442, clients[fd].get_nick(),
				channel_name + " :You're not on that channel");
		return;
	}

	if (!channel->is_operator(fd))
	{
		send_numeric_reply(fd, 482, clients[fd].get_nick(),
				channel_name + " :You're not channel operator");
		return;
	}

	int targetFd = find_client_fd_by_nickname(target_nick);

	if (targetFd == -1)
	{
		send_numeric_reply(fd, 401, clients[fd].get_nick(), target_nick + " :No such nick/channel");
		return;
	}

	if (channel->is_member(targetFd))
	{
		send_numeric_reply(fd, 443, clients[fd].get_nick(),
				target_nick + " " + channel_name + " :is already on channel");
		return;
	}

	channel->invit_member(targetFd);

	std::string inviteLine = ":" + client_prefix(fd) + " INVITE " + target_nick + " " + channel_name + "\r\n";
	queue_response(targetFd, inviteLine);

	send_numeric_reply(fd, 341, clients[fd].get_nick(), target_nick + " " + channel_name);
}

/*
** TOPIC <channel> [nouveau sujet] : sans argument, affiche le sujet
** actuel. Avec un argument, le modifie et previent tout le canal.
** Pas de restriction aux operateurs pour l'instant : en IRC, c'est
** seulement le mode +t (etape 9) qui impose cette restriction —
** par defaut, n'importe quel membre peut changer le sujet.
*/
void Server::handl_topic(int fd, const Message &msg)
{
	if (!clients[fd].is_registered())
	{
		send_numeric_reply(fd, 451, "*", ":You have not registered");
		return;
	}

	if (msg.params.empty())
	{
		send_numeric_reply(fd, 461, clients[fd].get_nick(), "TOPIC :Not enough parameters");
		return;
	}

	const std::string &channel_name = msg.params[0];
	Channel *channel = find_channel(channel_name);

	if (!channel)
	{
		send_numeric_reply(fd, 403, clients[fd].get_nick(), channel_name + " :No such channel");
		return;
	}

	if (!channel->is_member(fd))
	{
		send_numeric_reply(fd, 442, clients[fd].get_nick(),
				channel_name + " :You're not on that channel");
		return;
	}
	if (msg.params.size() < 2)
	{
		if (channel->get_topic().empty())
			send_numeric_reply(fd, 331, clients[fd].get_nick(), channel_name + " :No topic is set");
		else
			send_numeric_reply(fd, 332, clients[fd].get_nick(), channel_name + " :" + channel->get_topic());
		return;
	}
	if (channel->is_topic_restricted() && !channel->is_operator(fd))
	{
		send_numeric_reply(fd, 482, clients[fd].get_nick(),
				channel_name + " :You're not channel operator");
		return;
	}	
	channel->set_topic(msg.params[1]);

	std::string topic_line = ":" + client_prefix(fd) + " TOPIC " + channel_name + " :" + msg.params[1] + "\r\n";
	diffusion_msg_to_channel(*channel, -1, topic_line);
}

