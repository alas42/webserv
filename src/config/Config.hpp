#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>

class Config
{
	public:
		Config(void);
		~Config(void);
		Config(const Config & other);
		Config & operator=(const Config & other);
        void     parse(const char *conf_file);
        
	private:

};

#endif