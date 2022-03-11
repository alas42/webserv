#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include <vector>
# include "./../../inc/webserv.hpp"

class Config
{
	public:
		Config(void);
		~Config(void);
		Config(Config const & other);
		Config & operator=(Config const & other);
        void     parse(const char *conf_file);
        
		std::vector<int>  &	getServerFds(void);

	private:
		std::vector<int>	_server_fds;
	
};

#endif