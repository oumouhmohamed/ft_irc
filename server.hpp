#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
#include<iostream>
# include <vector>
# include <poll.h>
# include <map>
# include "client.hpp"
# include "parsing.hpp"
# include "channel.hpp"

class Server
{

	private:
		int             port;
		std::string     key_serv;
        int             fd_serv;
        std::vector<struct pollfd>  poll_fds;
        std::map<int, Client> clients;
        std::map<std::string, Channel>  channels;
		

    public:
        Server();
        Server(const Server &other);
        Server &operator=(const Server &other);
        ~Server();

        bool init_serv(const std::string &portStr, const std::string &key_serv);

        int                 get_port() const;
        const std::string  &get_key_serv() const;
        bool pars_port(const std::string &portStr);
		bool check_key_serv(const std::string &key_serv);

        
        bool config_socket();
        bool start_serv();

        void run_serv();
        bool set_non_block(int fd);
		bool accept_new_client();

        bool lire_depuis_client(size_t indice);
        bool send_repons_to_client(size_t index);
		void deconnect_client(size_t indice);

		void process_command(int fd, const Message &msg);
		void handl_pass(int fd, const Message &msg);
		void handl_nick(int fd, const Message &msg);
		void handl_user(int fd, const Message &msg);
		void try_register(int fd);
		bool is_nick_taken(const std::string &nick, int exclude_fd) const;
 
		void        queue_response(int fd, const std::string &line);
		void        send_numeric_reply(int fd, int code, const std::string &target,
						const std::string &text);
		std::string format_code(int code) const;

        void     handl_join(int fd, const Message &msg);
		void     handl_part(int fd, const Message &msg);
		void     handl_privmsg(int fd, const Message &msg);
		
		Channel *find_channel(const std::string &name);
		int      find_client_fd_by_nickname(const std::string &nick) const;
		void     diffusion_msg_to_channel(const Channel &channel, int exclude_fd,
						const std::string &line);
		void     supp_from_all_channels(int fd);
		std::string client_prefix(int fd) const;


        void handl_kick(int fd, const Message &msg);
		void handl_invit(int fd, const Message &msg);
		void handl_topic(int fd, const Message &msg);
		void handl_Mode(int fd, const Message &msg);
		std::string ft_help_mode(std::string modestr, Channel *channel, const Message msg,int fd, std::string channel_name);
	};

#endif
