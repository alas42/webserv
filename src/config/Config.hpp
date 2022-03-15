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
		void	parse(const char *conf_file);

		std::vector<int>	&	getPorts(void);
		std::string			&	getIpAddress(void);

	private:
		std::vector<int>	_ports;
		std::string			_ip_address;

		void						_parceToEnv(const char *conf);
		std::vector<std::string>	_splitStringToVector(std::string s);
		void						_setEnv(std::vector<std::vector<std::string> > confOut);

};

#endif
