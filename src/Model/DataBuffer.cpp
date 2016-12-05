#include "Model/DataBuffer.h"

#define BUFFER_SIZE 1000000

namespace model {

DataBuffer::DataBuffer()
{
	if (buffer_.max_size() < BUFFER_SIZE)
		buffer_ = std::vector<unsigned short>(buffer_.max_size(), 0);
	else
		buffer_ = std::vector<unsigned short>(BUFFER_SIZE, 0);
}

DataBuffer::~DataBuffer()
{

}

void DataBuffer::DataWrite(unsigned short data)
{
	if (write_pointer_ > buffer_.size() - 1)
		write_pointer_ = 0;

	buffer_[write_pointer_] = data;
}

unsigned short DataBuffer::DataRead()
{
	if (read_pointer_ > buffer_.size() - 1)
		read_pointer_ = 0;

	return buffer_[read_pointer_++];
}

} //namespace model

