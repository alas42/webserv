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

		//GET
		std::string						& getIpAddress(void);
		int								& getPorts(void);
		std::vector<std::string>		& getServerNames(void);
		std::map<int, std::string>		& getErrorPages(void);
		int								& getClientMaxBodySize(void);
		std::string 					& getCgiPass(void);
		std::vector<std::string>		& getAlowMethods(void);
		std::map<std::string, Config>	& getLocation(void);
		std::string						& getRoot(void);
		std::vector<std::string>		& getIndex(void);
		bool							& getAutoIndex(void);
		int	parseServer(std::vector<std::vector<std::string> > confFile, size_t i);

	private:
		std::string						_ipAddress;
		int								_ports;
		std::vector<std::string>		_serverNames;
		std::map<int, std::string>		_errorPages;
		int								_clientMaxBodySize;
		std::string						_cgiPass;
		std::vector<std::string>		_allowMethods;
		std::map<std::string, Config>	_location;
		std::string						_root;
		std::vector<std::string>		_index;
		bool							_autoIndex;

		// Parse file .conf
		int	_parseLocationDeep(std::vector<std::vector<std::string> > confFile, size_t i);

		// SET
		void	_setListen(std::vector<std::string> line);
		void	_setServerName(std::vector<std::string> line);
		void	_setErrorPage(std::vector<std::string> line);
		void	_setClientMaxBodySize(std::vector<std::string> line);
		void	_setCgiPass(std::vector<std::string> line);
		void	_setAllowMethods(std::vector<std::string> line);
		int		_setLocation(std::vector<std::vector<std::string> > confFile, size_t i);
		void	_setRoot(std::vector<std::string> line);
		void	_setIndex(std::vector<std::string> line);
		void	_setAutoIndex(std::vector<std::string> line);

		// Util

		// void							_setEnv(std::vector<std::vector<std::string> > confOut);

};

#endif
