#include "webserv.hpp"

bool	isServer(std::vector<std::string> confOut) {

	bool ret = false;

	for(std::vector<std::string>::iterator it = confOut.begin(); it != confOut.end(); it++) {
		if (it.base()->find("listen") != std::string::npos) {
			setenv("SERVER_PORT", it.base()->erase(it.base()->find("listen"), 7).c_str(), 1);
		}
	}
	std::cout << getenv("SERVER_PORT") << std::endl << std::endl;
	return ret;
}

void	parceToEnv(char *conf) {

	std::ifstream nginxConf(conf);
	std::string line;
	std::vector<std::string> confOut;

	if (nginxConf.is_open()) {
		while (getline(nginxConf, line)) {
			confOut.push_back(line);
		}
	}
	if (confOut.begin()->compare("server {") != 0 || (confOut.end() - 1)->compare("}") != 0)
		printf("egssthssebes\n");

	isServer(confOut);

}
