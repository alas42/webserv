#include "Response.hpp"

Response::Response(void)
{}

Response::~Response(void)
{}

Response::Response(const Response & other)
{
	*this = other;
}

Response & Response::operator=(const Response & other)
{
	if (this != &other)
	{

	}
	return (*this);
}
