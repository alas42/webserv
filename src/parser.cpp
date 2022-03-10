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

std::vector<std::string> SplitString(std::string s){

	std::vector<std::string> v;

	std::string temp = "";
	for(size_t i=0;i<s.length();++i){

		if(s[i] == ' '){
			v.push_back(temp);
			temp = "";
		}
		else {
			temp.push_back(s[i]);
		}
	}
	v.push_back(temp);
	return (v);
}

void	parceToEnv(char *conf) {

	std::ifstream nginxConf(conf);
	std::string line = "test";
	std::vector<std::vector<std::string> > confOut;
	std::vector<std::string> tmp;

	if (nginxConf.is_open()) {
		while (getline(nginxConf, line)) {
			tmp = SplitString(line);
			confOut.push_back(tmp);
		}
	}
	else {
		std::cout << "FUCK" << std::endl;
	}
}
