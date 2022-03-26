#include "Response.hpp"

Response::Response(void): _header("s"), _body(""), _raw_response("")
{
	this->_binary = false;
}

Response::~Response(void)
{}

Response::Response(const Response & other): _header(other._body), _body(other._body), _raw_response(other._raw_response)
{
	this->_binary = other._binary;
}

Response & Response::operator=(const Response & other)
{
	if (this != &other)
	{
		this->_body = other._body;
		this->_header = other._header;
		this->_raw_response = other._raw_response;
		this->_mimes = other._mimes;
		this->_binary = other._binary;
	}
	return (*this);
}

void			Response::setHeader(std::string new_header) {this->_header = new_header;}
void			Response::setBody(std::string new_body) {this->_body= new_body;}
void			Response::setRawResponse(std::string new_raw_response) {this->_raw_response = new_raw_response;}
std::string &	Response::getHeader(void){ return this->_header; }
std::string &	Response::getBody(void) {return this->_body; }
std::string &	Response::getRawResponse(void){ return this->_raw_response; }

void	Response::create_cgi_base(void)
{
	const char * filename = "data/execve.log"; //return of cgi processing for now
	std::ifstream f(filename);
	std::stringstream ss;
	std::string header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
	std::string str(""), body("");
	size_t i = 0;

	if (f)
	{
		while (f.good())
		{
			getline(f, str);
			if (i == 0) // first-line = content type line (for php-cgi)
			{
				header.append(str);
				header.append("\nContent-Length: ");
				str.clear();
			}
			else if (i > 1)
			{
				body.append(str);
				body.append("\r\n");
			}
			i++;
		}
	}
	f.close();
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

void	Response::create_get(std::string filename)
{
	/*
	** Checks for the file (existing etc)
	** First check mime_type of request
	** Binary of text ?
	*/
	std::cout << "Response.create_get(" << filename << ")" << std::endl;
	if (filename.find(".html") == std::string::npos && filename.find(".txt") == std::string::npos)
	{
		this->binary(filename);
		return ;
	}
	std::ifstream 		f(filename.c_str());
	std::stringstream	ss;
	std::string			header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
	std::string			str, body;

	if (f)
	{
		header.append("Content-Length: ");
		while (f.good())
		{
			getline(f, str);
			body.append(str);
			body.append("\r\n");
		}
	}
	else
		return ;

	f.close();
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

void	Response::create_post(std::string filename)
{
	(void)filename;
}

void	Response::binary(std::string filename)
{
	std::size_t length;
	std::string header;
	std::stringstream ss;
	std::ifstream f(filename.c_str(), std::ios::binary);
	if (!f)
		return ;
	header = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-type: image/png\r\nContent-Length: ";
	f.seekg(0, std::ios::end);
	length = f.tellg();
	f.seekg(0, std::ios::beg);
	f.close();
	ss << length;
	header.append(ss.str());

	std::string content((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
	this->_body = content;
	this->_header = header;
	this->_raw_response = this->_header;
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

/*
** Common mime types
** If we don't know it, we should check if it is a binary ?
**		Yes -> use Content-Type: application/octet-stream
**		No  -> use Content-Type: text/plain
*/
void	Response::setting_mimes(void)
{
	this->_mimes[".avi"] = 	"video/x-msvideo";
	this->_mimes[".bmp"] = 	"image/bmp";
	this->_mimes[".csv"] = 	"text/csv";
	this->_mimes[".epub"] = "application/epub+zib";
	this->_mimes[".gif"] = 	"image/gif";
	this->_mimes[".html"] = "text/html";
	this->_mimes[".htm"] = 	"text/html";
	this->_mimes[".php"] = 	"text/plain";
	this->_mimes[".jpg"] = 	"image/jpg";
	this->_mimes[".jpeg"] = "image/jpeg";
	this->_mimes[".json"] = "application/json";
	this->_mimes[".mpeg"] = "video/mpeg";
	this->_mimes[".png"] = 	"image/png";
	this->_mimes[".pdf"] = 	"application/pdf";
	this->_mimes[".svg"] = 	"image/svg+xml";
	this->_mimes[".xml"] = 	"application/xml";
	this->_mimes[".zip"] = 	"application/zip";
	this->_mimes[".*"] = 	"application/octet-stream";
}

void	Response::create_bad_request(void)
{
	std::ifstream f("data/error_pages/400.html");
	std::stringstream ss;
	std::string header("HTTP/1.1 400 Bad Request\r\nConnection: keep-alive\r\n");
	std::string str, body;
	if (f)
	{
		header.append("Content-Length: ");
		while (f.good())
		{
			getline(f, str);
			body.append(str);
			body.append("\r\n");
		}
	}
	else
	{
		return ;
	}
	f.close();
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

void	Response::create_Forbidden(void)
{
	//creer un fd
	//ajouter a la liste des polls dans Server.cpp
	//une fois qu'on passe (boucle suivante) dedans
	//lire le fichier et continuer la preparation de la reponse
	std::ifstream f("data/error_pages/403.html");
	std::stringstream ss;
	std::string header("HTTP/1.1 403 Forbidden\r\nConnection: keep-alive\r\n");
	std::string str, body;
	if (f)
	{
		header.append("Content-Length: ");
		while (f.good())
		{
			getline(f, str);
			body.append(str);
			body.append("\r\n");
		}
	}
	else
	{
		return ;
	}
	f.close();
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

void	Response::create_not_found(void)
{
	std::ifstream f("data/error_pages/404.html");
	std::stringstream ss;
	std::string header("HTTP/1.1 404 Not Found\r\nConnection: keep-alive\r\n");
	std::string str, body;
	if (f)
	{
		header.append("Content-Length: ");
		while (f.good())
		{
			getline(f, str);
			body.append(str);
			body.append("\r\n");
		}
	}
	else // didn't find
	{
		return ;
	}
	f.close();
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

void	Response::create_internal_error(void)
{
	std::ifstream f("data/error_pages/500.html");
	std::stringstream ss;
	std::string header("HTTP/1.1 500 Internal Server Error\r\nConnection: keep-alive\r\n");
	std::string str, body;
	if (f)
	{
		header.append("Content-Length: ");
		while (f.good())
		{
			getline(f, str);
			body.append(str);
			body.append("\r\n");
		}
	}
	else // didn't find
	{
		return ;
	}
	f.close();
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}
