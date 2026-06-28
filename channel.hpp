#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class Channel
{
	private:
		std::string       name;
		std::vector<int>  members;
        std::vector<int>  operators;
		std::vector<int>  invited;
		std::string       topic;

    
    public:
		Channel();
		Channel(const std::string &name);
		~Channel();

		const std::string &get_name() const;
        const std::vector<int> &get_members() const;

		void add_member(int fd);
		void retirer_member(int fd);
		bool is_member(int fd) const;
		bool is_empty() const;

        void add_operator(int fd);
		void retirer_operator(int fd);
		bool is_operator(int fd) const;
 
		void invit_member(int fd);
		bool is_invited(int fd) const;
		void retirer_invite(int fd);
 
		// channel.hpp  (dans la partie public)
		void retirer_client(int fd);
		const std::string &get_topic() const;
		void               set_topic(const std::string &topic);

};

#endif