#include "client.hpp"

Client::Client() :
	fd_client(-1), buffer(""), outp_Buffer(""), pass(false),
	nick(""), user(""), registered(false)
{
}

Client::Client(int fd) :
	fd_client(fd), buffer(""), outp_Buffer(""), pass(false),
	nick(""), user(""), registered(false)
{
}

Client::~Client()
{
}

int Client::get_fd() const
{
	return (fd_client);
}

void Client::append_to_buffer(const char *data, size_t len)
{
	buffer.append(data, len);
}

bool Client::is_line_complete() const
{
	for (size_t i = 0; i < buffer.size(); i++)
	{
		if (buffer[i] == '\n')
			return (true);
	}
	return (false);
}

std::string Client::extract_line()
{
	size_t pos = 0;

	while (pos < buffer.size() && buffer[pos] != '\n')
		pos++;

	std::string line = buffer.substr(0, pos);

	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);

	buffer.erase(0, pos + 1);
	return (line);
}

void Client::queue_output(const std::string &data)
{
	outp_Buffer += data;
}
 
const std::string &Client::get_outp_Buffer() const
{
	return (outp_Buffer);
}
 

void Client::consume_outp(size_t n)
{
	outp_Buffer.erase(0, n);
}
 
bool Client::pass_ok() const
{
	return (pass);
}
 
void Client::set_pass_ok(bool ok)
{
	pass = ok;
}
 
const std::string &Client::get_nick() const
{
	return (nick);
}
 
void Client::set_nick(const std::string &n)
{
	nick = n;
}
 
const std::string &Client::get_user() const
{
	return (user);
}
 
void Client::set_user(const std::string &u)
{
	user = u;
}
 
bool Client::is_registered() const
{
	return (registered);
}
 
void Client::set_registered(bool reg)
{
	registered = reg;
}