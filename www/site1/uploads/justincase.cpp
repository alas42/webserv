/*
bool	Response::nextBit(void)
{
	if (_is_binary)
	{
		std::ifstream	f(this->_filename.c_str(), std::ios::binary);
		size_t pos = _length_response - getRemainingLength();
		f.seekg(pos, std::ios::beg);
	}
	else
	{
		std::ifstream	f(this->_filename.c_str());
		size_t pos = _length_response - getRemainingLength();
		f.seekg(pos, std::ios::beg);
	}
	2. read from there till buffer size
	3. store it
	4. increment how many bytes were read
	5. close filestream
	return false;
}

void	Response::setLengthResponse(std::streampos len_of_file)
{
	size_t len_file = static_cast<size_t>(len_of_file);
	this->_length_response = len_file + this->_header.size();
}
*/
