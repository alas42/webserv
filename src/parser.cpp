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

void	setEnv(std::vector<std::vector<std::string>> confOut) {

	// std::vector<std::vector<std::string>>::iterator itVector = confOut.begin();

	std::string listEnv[4] = {"listen", "server_name", "root", "location"};
	// {"CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", "PATH_INFO", \
	// 						"QUERY_STRING", "REMOTE_ADDR", "REQUEST_METHODE", "SCRIPT_NAME", \
	// 						"SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", "SERVER_SOFTWQRE", \
	// 						"REQUEST_URI", "REDIRECT_STATUS", "SCRIPT_FILENAME"};


	for(std::vector<std::vector<std::string>>::iterator itVector = confOut.begin(); itVector != confOut.end(); itVector++) {
		for (int i = 0; i != 5 ; i++) {
			if (itVector.base()->begin()->find(listEnv[i]) != std::string::npos)
			{
				std::vector<std::string>::iterator itString = itVector.base()->begin();
				std::cout << (itString.base() + 1)->c_str() << "111111111111111111111" << std::endl;
				// std::cout << itVector.base()->begin()->c_str() << "111111111111111111111" << std::endl;
				switch (i) {
					case 0:
						setenv("SERVER_PORT", std::to_string(atoi(((itVector.base()->begin()) + 1)->c_str())).c_str() , 1);
						// std::cout << "SERVERRRRRRRRR" << std::atoi(((itVector.base()->begin()) + 1)->c_str() ) << std::endl;s
					case 1:
						setenv("SERVER_NAME", ((itVector.base()->begin()) + 1)->c_str(), 1);
					case 2:
						// setenv("SERVER_NAME", ((itVector.base()->begin()) + 1)->c_str(), 1);
					case 3:
						// setenv("SERVER_NAME", ((itVector.base()->begin()) + 1)->c_str(), 1);

				}

			// std::cout << itVector.base()->begin()->c_str() << "222222222222222222222222222222222222222222" << std::endl;

			}

		}
	}
	getenv("SERVER_NAME");
	// for(std::vector<std::string>::iterator itString = itVector.base()->begin(); itString != itVector.base()->end(); itString++) {
	// 	for (int i = 0; i != 5 ; i++) {
	// 		if (itString.base()->compare(listEnv[i]) == 0)
	// 			std::cout << itString.base()->c_str() << std::endl;
	// 			// switch (i) {
	// 			// 	case 0:
	// 			// 		setenv)
	// 			// }
	// 		std::cout << itString.base()->c_str() << std::endl;

	// 	}
	// 	std::cout << ""<< std::endl;

	// }

}

std::vector<std::string> splitStringToVector(std::string s){

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
	std::vector<std::vector<std::string>> confOut;
	std::vector<std::string> tmp;

	if (nginxConf.is_open()) {
		while (getline(nginxConf, line)) {
			tmp = splitStringToVector(line);
			confOut.push_back(tmp);
		}
	}
	else {
		std::cout << "FUCK" << std::endl;
	}

	setEnv(confOut);

}
