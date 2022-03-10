#include <iostream>
#include <string>
#include <stdlib.h>

int main(int ac, char **av, char **env)
{
	/*std::cout << getenv("DOCUMENT_NAME") << std::endl;*/
	printf("%s\n", "testing printf");
	std::cout << "testing stdcout" << std::endl;
	std::cout << getenv("DOCUMENT_ROOT") << std::endl;
/*	std::cout << getenv("DOCUMENT_URL") << std::endl;
	std::cout << getenv("DOCUMENT_URI") << std::endl;
	std::cout << getenv("CONTENT_TYPE") << std::endl;
	std::cout << getenv("CONTENT_LENGTH") << std::endl;*/
	std::cout << getenv("GATEWAY_INTERFACE") << std::endl;
	/*std::cout << getenv("PATH_INFO") << std::endl;*/
	std::cout << getenv("QUERY_STRING") << std::endl;
	/*std::cout << getenv("REMOTE_ADDR") << std::endl;*/
	/*std::cout << getenv("SCRIPT_NAME") << std::endl;
	std::cout << getenv("SCRIPT_FILENAME") << std::endl;*/
	std::cout << getenv("SERVER_NAME") << std::endl;
	std::cout << getenv("SERVER_PORT") << std::endl;
	std::cout << getenv("SERVER_PROTOCOL") << std::endl;
	std::cout << getenv("SERVER_SOFTWARE") << std::endl;
	std::cout << getenv("REQUEST_METHOD") << std::endl;
	std::cout << getenv("REDIRECT_STATUS") << std::endl;
	(void)ac;
	(void)av;
	(void)env;
	return (0);
}