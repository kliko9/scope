#pragma once

#include <vector>

namespace model {

	class DataBuffer {
	public:
		DataBuffer();
		virtual ~DataBuffer();

		void DataWrite(unsigned short data);
		unsigned short DataRead();
		const std::vector<unsigned short> &GetBuffer() {return buffer_;};

	private:
		std::vector<unsigned short> buffer_;

		unsigned long read_pointer_ = 0;
		unsigned long write_pointer_ = 0;
	};

} //namespace model
