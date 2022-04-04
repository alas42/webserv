#include "Response.hpp"

Response::Response(void): _header("s"), _body(""), _raw_response("") {

	this->_binary = false;
	this->_setting_mimes();
}

Response::~Response(void)
{}

Response::Response(const Response & other): _header(other._header), _body(other._body), _raw_response(other._raw_response) {

	this->_binary = other._binary;
	this->_mimes = other._mimes;
}

Response & Response::operator=(const Response & other) {

	if (this != &other) {
		this->_body = other._body;
		this->_header = other._header;
		this->_raw_response = other._raw_response;
		this->_mimes = other._mimes;
		this->_binary = other._binary;
	}
	return (*this);
}

std::string &	Response::getRawResponse(void) {
	return this->_raw_response;
}

void	Response::create_cgi_base(const char *filename) {

	std::ifstream f(filename);
	std::stringstream ss;
	std::string header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
	std::string str(""), body("");
	size_t i = 0;

	f.clear();
	f.seekg(0, std::ios::beg);
	if (f) {
		while (f.good()) {
			getline(f, str);
			if (i == 0) {
				header.append(str);
				header.append("\nContent-Length: ");
				str.clear();
			}
			else if (i > 1) {
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

void	Response::create_continue(void)
{
	std::string		header("HTTP/1.1 100 Continue\r\n\r\n");
	this->_raw_response = header;
}

void	Response::create_get(std::string filename)
{
	if (filename.find(".html") == std::string::npos && filename.find(".txt") == std::string::npos)
	{
		this->binary(filename);
		return ;
	}
	std::ifstream 		f(filename.c_str());
	std::stringstream	ss;
	std::string			header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
	std::string			str, body;

	if (f) {
		header.append("Content-Length: ");
		while (f.good()) {
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

void	Response::create_post(std::string filename) {
	(void)filename;
}

/*
** To do list : Content-type : set to correct one
*/
void	Response::binary(std::string filename) {

	std::size_t		length, found;
	std::string			header, extension;
	std::stringstream	ss;
	std::ifstream		f(filename.c_str(), std::ios::binary);

	header = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-type: *\r\nContent-Length: ";
	found = filename.find_last_of(".");
	extension = filename.substr(found);
	std::map<std::string, std::string>::iterator it = this->_mimes.find(extension);

	if (it != this->_mimes.end())
		header.replace(header.find("*"), 1, it->second);
	else
		header.replace(header.find("*"), 1, "application/octet-stream");
	if (!f)
		return ;

	f.clear();
	f.seekg(0, std::ios::end);
	length = f.tellg();
	f.seekg(0, std::ios::beg);
	ss << length;
	header.append(ss.str());

	std::string content((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
	this->_body = content;
	this->_header = header;
	this->_raw_response = this->_header;
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
	f.close();
}

/*
** Common mime types
** If we don't know it, we should check if it is a binary ?
**		Yes -> use Content-Type: application/octet-stream
**		No  -> use Content-Type: text/plain
*/
void	Response::_setting_mimes(void) {

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

void	Response::error(std::string const error_code)
{
	std::string			error_page("data/error_pages/" + error_code + ".html");
	std::ifstream		f(error_page.c_str());
	std::stringstream	ss;
	std::string			header("HTTP/1.1 "+ error_code +" Bad Request\r\nConnection: keep-alive\r\n");
	std::string			str, body;

	if (f) {
		header.append("Content-Length: ");
		while (f.good()) {
			getline(f, str);
			body.append(str);
			body.append("\r\n");
		}
	}
	else {
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

void	Response::print_directory(std::string root_dir, std::string dir)
{
	DIR *dpdf;
	struct dirent *epdf;
	std::stringstream ss;
	std::string header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
	std::string str, body;
	struct stat buf;
	int res_stat = 0;
	dpdf = opendir(root_dir.c_str());
	if (dpdf != NULL)
	{
		header.append("Content-Length: ");
		while ((epdf = readdir(dpdf)))
		{
			body.append("<a href=\"");
			body.append(dir);
			//body.append("/");
			body.append(epdf->d_name);
			std::string is_dir = epdf->d_name;
			is_dir = dir + is_dir;
			res_stat = stat(is_dir.c_str() ,&buf);
			if (S_ISDIR(buf.st_mode) != 0)
				body.append("/");
			body.append("\">");
			body.append(epdf->d_name);
			body.append("</a>\r\n");
			//printf("Filename: %s",epdf->d_name);
	      // std::cout << epdf->d_name << std::endl;
	  }
	}
	else
		return ;
	closedir(dpdf);
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);

}

void	Response::create_delete(std::string filename)
{
	std::stringstream ss;
	std::string header("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n");
	std::string body;
	header.append("Content-Length: ");
	body.append(filename);
	body.append(" deleted.");
	body.append("\r\n");
	this->_body = body;
	ss << body.size();
	header.append(ss.str());
	this->_header = header;
	this->_raw_response.append(this->_header);
	this->_raw_response.append("\r\n\r\n");
	this->_raw_response.append(this->_body);
}
