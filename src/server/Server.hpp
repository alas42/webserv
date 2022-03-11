#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include "../config/Config.hpp"

class Server
{
	public:
		Server(void);
		~Server(void);
		Server(const Server & other);
		Server & operator=(const Server & other);
		void	config(char * conf_file);
		int		setup(void);
		void	run(void);
		void	clean(void);

	private:
		Config 	_config;
		int		_timeout;
};

#endif
