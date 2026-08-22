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

		bool			invite_only;
		bool			topic_restricted;
		bool			has_key;
		std::string		key;
		bool			has_limit;
		int				user_limit;
    public:
		Channel();
		Channel(const std::string &name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
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
 
		void retirer_client(int fd);
		const std::string &get_topic() const;
		void               set_topic(const std::string &topic);

    bool               is_invite_only() const;
    void               set_invite_only(bool on);

    bool               is_topic_restricted() const;
    void               set_topic_restricted(bool on);

    bool               has_channel_key() const;
    const std::string &get_key() const;
    void               set_key(const std::string &k);
    void               unset_key();

    bool               has_user_limit() const;
    int                get_user_limit() const;
    void               set_user_limit(int l);
    void               unset_user_limit();
	std::string        get_mode_string() const;
};

#endif