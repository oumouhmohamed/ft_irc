#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>

class Client
{
	private:
		int          fd_client;
		std::string  buffer;
		std::string  outp_Buffer;
 
		bool         pass;
		std::string  nick;
		std::string  user;
		bool         registered;

    public:
		Client();
		Client(int fd);
		Client(const Client &other);
		Client &operator=(const Client &other);
		~Client();

		int  get_fd() const;

		void        append_to_buffer(const char *data, size_t len);
		bool        is_line_complete() const;
		std::string extract_line();

		void               queue_output(const std::string &data);
		const std::string &get_outp_Buffer() const;
		void               consume_outp(size_t n);
 
		bool                pass_ok() const;
		void                set_pass_ok(bool ok);

		const std::string  &get_nick() const;
		void                set_nick(const std::string &nick);

		const std::string  &get_user() const;
		void                set_user(const std::string &user);

		bool                is_registered() const;
		void                set_registered(bool reg);


};

#endif