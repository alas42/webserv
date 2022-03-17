#ifndef CONFIG_HPP
# define CONFIG_HPP

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
		std::string						_ip_address;
		std::vector<int>				_ports;
		std::vector<std::string>		_serverNames;
		std::map<int, std::string>		_errorPages;
		int								_clientMaxBodySize;
		std::vector<std::string>		_allowMethods;
		std::map<std::string, Config>	_location;
		std::string						_root;
		std::vector<std::string>		_index;
		bool							_autoIndex;


		std::vector<std::vector<std::string> >	_getConfOfFile(const char *conf);
		int	_parseServer(std::vector<std::vector<std::string> > confFile, size_t i);

		// SET

		void	_setListen(std::vector<std::string> line);
		void	_setServerName(std::vector<std::string> line);
		void	_setErrorPage(std::vector<std::string> line);
		void	_setClientMaxBodySize(std::vector<std::string> line);
		void	_setAllowMethods(std::vector<std::string> line);
		void	_setLocation(std::vector<std::string> line);
		void	_setRoot(std::vector<std::string> line);
		void	_setIndex(std::vector<std::string> line);
		void	_setAutoIndex(std::vector<std::string> line);


		std::vector<std::string>		_split(std::string s, std::string charset);
		void							_setEnv(std::vector<std::vector<std::string> > confOut);

};

#endif
