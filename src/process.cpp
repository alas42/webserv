#include "webserv.hpp"

void	ft_execve(std::string path, char **tab, char **env, int status) {

	status = execve(path.c_str(), tab, env);
	// if (status < 0)
	// {
	// 	free something
	// }
}

void	ft_fork(std::string path, char **tab, char **env) {
	pid_t	c_pid;
	int		status = 0;

	int log = open("data/execve.log", O_WRONLY|O_CREAT, 0666);

	if ((c_pid = fork()) == 0) {
		std::cout << "printed from child process " << getpid() << std::endl;
		close(1);
		dup(log);
		ft_execve(path, tab, env, status);
		exit(EXIT_SUCCESS);
	} else if (c_pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	} else{
		std::cout << "printed from parent process " << getpid() << std::endl;

		waitpid(c_pid, &status, 0);
		close(log);
	}
}
