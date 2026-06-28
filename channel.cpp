#include "channel.hpp"

Channel::Channel() : name("")
{
}

Channel::Channel(const std::string &name) : name(name)
{
}

Channel::~Channel()
{
}

const std::string &Channel::get_name() const
{
	return (name);
}

void Channel::add_member(int fd)
{
	members.push_back(fd);
}


void Channel::retirer_member(int fd)
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i] == fd)
		{
			members.erase(members.begin() + i);
			return;
		}
	}
}

bool Channel::is_member(int fd) const
{
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i] == fd)
			return (true);
	}
	return (false);
}

bool Channel::is_empty() const
{
	return (members.empty());
}

const std::vector<int> &Channel::get_members() const
{
	return (members);
}

void Channel::add_operator(int fd)
{
	operators.push_back(fd);
}
 
void Channel::retirer_operator(int fd)
{
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (operators[i] == fd)
		{
			operators.erase(operators.begin() + i);
			return;
		}
	}
}
 
bool Channel::is_operator(int fd) const
{
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (operators[i] == fd)
			return (true);
	}
	return (false);
}
 
void Channel::invit_member(int fd)
{
	if (!is_invited(fd))
		invited.push_back(fd);
}
 
bool Channel::is_invited(int fd) const
{
	for (size_t i = 0; i < invited.size(); i++)
	{
		if (invited[i] == fd)
			return (true);
	}
	return (false);
}
 
void Channel::retirer_invite(int fd)
{
	for (size_t i = 0; i < invited.size(); i++)
	{
		if (invited[i] == fd)
		{
			invited.erase(invited.begin() + i);
			return;
		}
	}
}
 
const std::string &Channel::get_topic() const
{
	return (topic);
}
 
void Channel::set_topic(const std::string &t)
{
	topic = t;
}

// channel.cpp
void Channel::retirer_client(int fd)
{
	retirer_member(fd);
	retirer_operator(fd);
	retirer_invite(fd);
}