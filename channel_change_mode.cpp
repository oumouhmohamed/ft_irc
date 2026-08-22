#include "channel.hpp"

bool Channel::is_invite_only() const
{
	return (invite_only);
}

void Channel::set_invite_only(bool on)
{
	invite_only = on;
}

bool Channel::is_topic_restricted() const
{
	return (topic_restricted);
}

void Channel::set_topic_restricted(bool on)
{
	topic_restricted = on;
}

bool Channel::has_channel_key() const
{
	return (has_key);
}

const std::string &Channel::get_key() const
{
	return (key);
}

void Channel::set_key(const std::string &k)
{
	key = k;
	has_key = true;
}

void Channel::unset_key()
{
	key = "";
	has_key = false;
}

bool Channel::has_user_limit() const
{
	return (has_limit);
}

int Channel::get_user_limit() const
{
	return (user_limit);
}

void Channel::set_user_limit(int l)
{
	user_limit = l;
	has_limit = true;
}

void Channel::unset_user_limit()
{
	user_limit = 0;
	has_limit = false;
}

std::string Channel::get_mode_string() const
{
	std::string modes;

	if (invite_only)
		modes += "i";
	if (topic_restricted)
		modes += "t";
	if (has_key)
		modes += "k";
	if (has_limit)
		modes += "l";
	if (modes.empty())
		return ("");
	return ("+" + modes);
}