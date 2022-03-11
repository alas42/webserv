#include "Request.hpp"

Request::Request(void)
{}

Request::~Request(void)
{}

Request::Request(const Request & other)
{
    *this = other;
}

Request & Request::operator=(const Request & other)
{
    if (this != &other)
    {

    }
    return (*this);
}
