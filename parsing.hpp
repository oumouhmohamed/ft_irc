#ifndef PARSING_HPP
# define PARSING_HPP

# include <string>
# include <vector>

struct Message
{
	std::string               command;
	std::vector<std::string>  params;
};

Message parse_message(const std::string &line);

#endif