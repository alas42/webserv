#include "Cgi.hpp"

Cgi::Cgi(void)
{}

Cgi::~Cgi(void)
{}

Cgi::Cgi(std::string path, bool post, std::string infile, std::map<std::string, std::string> env_vars):
	_post(post), _path_to_cgi(path), _infile(infile), _env_vars(env_vars)
{}

Cgi::Cgi(Cgi const & other):
	_post(other._post), _path_to_cgi(other._path_to_cgi), _infile(other._infile),
	_env_vars(other._env_vars)
{}

Cgi & Cgi::operator=(Cgi const & other)
{
	if (this != &other)
	{
		_post = other._post;
		_path_to_cgi = other._path_to_cgi;
		_infile = other._infile;
		_env_vars = other._env_vars;
	}
	return *this;
}

char	**Cgi::create_env_tab(void)
{
	char		*tmp = NULL;
	char 		**env_tab = NULL;
	size_t		length = 0;
	size_t 		i = 0;

	env_tab = (char **)malloc(sizeof(char *) * (this->_env_vars.size() + 1));
	std::map<std::string, std::string>::iterator it = this->_env_vars.begin();
	for(;it != this->_env_vars.end(); it++) {
		tmp = strdup(it->second.c_str());
		if (tmp == NULL)
			length = strlen(it->first.c_str()) + 1;
		else
			length = strlen(it->first.c_str()) + 2 + strlen(it->second.c_str());
		env_tab[i] = (char *)malloc(sizeof(char) * (length));
		env_tab[i] = strcpy(env_tab[i], it->first.c_str());
		if (tmp) {
			env_tab[i] = strcat(env_tab[i], "=\0");
			env_tab[i] = strcat(env_tab[i], tmp);
		}
		env_tab[i][length - 1] = '\0';
		if (tmp)
			free(tmp);
		i++;
	}
	env_tab[this->_env_vars.size()] = 0;
	return env_tab;
}

void	Cgi::execute(void)
{

	char 		**tab = (char **)malloc(sizeof(char *) * 3);
	char		**env_tab = NULL;
	pid_t		c_pid;
	FILE 		*fi, *fo;
	int			status = 0, fdi, fdo;

	env_tab = create_env_tab();
	std::cout << _path_to_cgi << std::endl;
	tab[0] = strdup(this->_path_to_cgi.c_str());
	tab[1] = strdup(this->_env_vars["SCRIPT_FILENAME"].c_str());
	tab[2] = 0;

	c_pid = fork();
	if (c_pid == 0) {
		fo = fopen(std::string("cgi_" + this->_infile).c_str(), "a");
		fdo = fileno(fo);
		if (dup2(fdo, STDOUT_FILENO) == -1)
			perror("dup2");
		if (this->_post) {
			fi = fopen(this->_infile.c_str(), "r");
			fdi = fileno(fi);
			if (dup2(fdi, STDIN_FILENO) == -1)
				perror("dup2");
		}
		execve(this->_path_to_cgi.c_str(), tab, env_tab);
		exit(EXIT_SUCCESS);
	}
	else if (c_pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else {
		waitpid(c_pid, &status, 0);
	}
	for(size_t i = 0; tab[i]; i++)
		free(tab[i]);
	free(tab);
	for(size_t i = 0; env_tab[i]; i++)
		free(env_tab[i]);
	free(env_tab);
}
