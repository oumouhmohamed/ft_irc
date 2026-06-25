#include "parsing.hpp"

Message parse_message(const std::string &line)
{
	Message msg;
	size_t  pos = 0;

	while (pos < line.size() && line[pos] == ' ')
		pos++;

	size_t start = pos;
	while (pos < line.size() && line[pos] != ' ')
		pos++;
	msg.command = line.substr(start, pos - start);

	while (pos < line.size())
	{
		while (pos < line.size() && line[pos] == ' ')
			pos++;

		if (pos >= line.size())
			break;

		if (line[pos] == ':')
		{
			msg.params.push_back(line.substr(pos + 1));
			break;
		}

		start = pos;
		while (pos < line.size() && line[pos] != ' ')
			pos++;
		msg.params.push_back(line.substr(start, pos - start));
	}
	return (msg);
}