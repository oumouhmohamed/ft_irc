#include "server.hpp"
#include <cstdlib>

std::string Server::ft_help_mode(std::string modestr, Channel *channel, const Message msg,int fd, std::string channel_name)
{
	size_t arg_index = 2;
	char   sign = '+';
	char   last_sign_written = 0;
	std::string change;
	std::vector<std::string> change_args;

	for (size_t i = 0; i < modestr.size(); i++)
	{
		char c = modestr[i];

		if (c == '+' || c == '-')
		{
			sign = c;
			continue;
		}

		bool applied = false;

		if (c == 'i')
		{
			channel->set_invite_only(sign == '+');
			applied = true;
		}
		else if (c == 't')
		{
			channel->set_topic_restricted(sign == '+');
			applied = true;
		}
		else if (c == 'k')
		{
			if (sign == '+')
			{
				if (arg_index >= msg.params.size())
				{
					send_numeric_reply(fd, 461, clients[fd].get_nick(), "MODE :Not enough parameters");
					return "";
				}
				channel->set_key(msg.params[arg_index]);
				change_args.push_back(msg.params[arg_index]);
				arg_index++;
			}
			else
				channel->unset_key();
			applied = true;
		}
		else if (c == 'o')
		{
			if (arg_index >= msg.params.size())
			{
				send_numeric_reply(fd, 461, clients[fd].get_nick(), "MODE :Not enough parameters");
				return "";
			}
			const std::string &target_nick = msg.params[arg_index];
			int target_fd = find_client_fd_by_nickname(target_nick);
			arg_index++;

			if (target_fd == -1 || !channel->is_member(target_fd))
			{
				send_numeric_reply(fd, 441, clients[fd].get_nick(),
						target_nick + " " + channel_name + " :They aren't on that channel");
				return "";
			}
			if (sign == '+')
				channel->add_operator(target_fd);
			else
				channel->retirer_operator(target_fd);

			change_args.push_back(target_nick);
			applied = true;
		}
		else if (c == 'l')
		{
			if (sign == '+')
			{
				if (arg_index >= msg.params.size())
				{
					send_numeric_reply(fd, 461, clients[fd].get_nick(), "MODE :Not enough parameters");
					return "";
				}

				long limit = std::strtol(msg.params[arg_index].c_str(), NULL, 10);
				arg_index++;
				if (limit <= 0)
					continue;

				channel->set_user_limit(static_cast<int>(limit));
				change_args.push_back(msg.params[arg_index - 1]);
			}
			else
				channel->unset_user_limit();
			applied = true;
		}
		else
		{
			std::string unknown(1, c);
			send_numeric_reply(fd, 472, clients[fd].get_nick(),
					unknown + " :is unknown mode char to me");
			return "";
		}

		if (applied)
		{
			if (sign != last_sign_written)
			{
				change += sign;
				last_sign_written = sign;
			}
			change += c;
		}
	}

	if (change.empty())
		return "";
	std::string mode_line = ":" + client_prefix(fd) + " MODE " + channel_name + " " + change;
	for (size_t i = 0; i < change_args.size(); i++)
		mode_line += " " + change_args[i];
	mode_line += "\r\n";
	return mode_line;
}

void Server::handl_Mode(int fd, const Message &msg)
{
	if (!clients[fd].is_registered())
	{
		send_numeric_reply(fd, 451, "*", ":You have not registered");
		return;
	}

	if (msg.params.empty())
	{
		send_numeric_reply(fd, 461, clients[fd].get_nick(), "MODE :Not enough parameters");
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
		std::string modes = channel->get_mode_string();
		if (modes.empty())
			modes = "+";
		send_numeric_reply(fd, 324, clients[fd].get_nick(), channel_name + " " + modes);
		return;
	}

	if (!channel->is_operator(fd))
	{
		send_numeric_reply(fd, 482, clients[fd].get_nick(),
				channel_name + " :You're not channel operator");
		return;
	}

	const std::string &modestr = msg.params[1];
	std::string mode_line = ft_help_mode(modestr, channel, msg, fd, channel_name);
	if (!mode_line.empty())
		diffusion_msg_to_channel(*channel, -1, mode_line);
}
