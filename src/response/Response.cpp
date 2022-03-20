#include "Response.hpp"

Response::Response(void): _header("s"),_body(""), _raw_response("")
{}

Response::~Response(void)
{}

Response::Response(const Response & other): _header(other._body), _body(other._body), _raw_response(other._raw_response)
{}

Response & Response::operator=(const Response & other)
{
	if (this != &other)
	{
		this->_body = other._body;
		this->_header = other._header;
		this->_raw_response = other._raw_response;
	}
	return (*this);
}

void	Response::setHeader(std::string new_header) {this->_header = new_header;}
void	Response::setBody(std::string new_body) {this->_body= new_body;}
void	Response::setRawResponse(std::string new_raw_response) {this->_raw_response = new_raw_response;}
std::string & Response::getHeader(void){ return this->_header; }
std::string & Response::getBody(void) {return this->_body; }
std::string & Response::getRawResponse(void){ return this->_raw_response; }

void	Response::create_cgi_base(void)
{
	const char * filename = "data/execve.log"; //return of cgi processing for now
	std::ifstream f(filename);
	std::stringstream ss;
	std::string header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
	std::string str, body;
	size_t i = 0;

	if (f)
	{
		while (f.good())
		{
			if (i == 0) // first-line = content type line (for php-cgi)
			{
				getline(f, str);
				header.append(str);
				header.append("\r\nContent-Length: ");
				str.clear();
			}
			else
			{
				getline(f, str);
				body.append(str);
				body.append("\r\n");
			}
			i++;
		}
	}
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

// https://stackoverflow.com/questions/43670762/c-sending-png-file-via-http
void	Response::create_get(std::string filename)
{
	if (filename.find(".png") != std::string::npos) //exception for image (i guess all binary?)
	{
		this->image(filename);
		return ;
	}
	std::ifstream f(filename.c_str());
	std::stringstream ss;
	std::string header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
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
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}

void	Response::image(std::string filename)
{
	std::ifstream f(filename.c_str(), std::ios::binary);
	if (!f)
		return ;
	std::stringstream ss;
	std::string header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-type: image/png\r\nContent-Length: ");
	f.seekg(0, std::ios::end);
	std::size_t length = f.tellg();
	f.seekg(0, std::ios::beg);
	header.append(std::to_string(length));

	std::string content( (std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
	this->_body = content;
	this->_header = header;
	this->_raw_response = this->_header;
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}
