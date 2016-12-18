#pragma once

#include <vector>

#include "Utils/Point.h"

namespace model {

	class DataInterpreter {
	public:
		DataInterpreter();
		virtual ~DataInterpreter();

		void SetBuffer(utils::Point *buffer, unsigned size);
		utils::Point *Buffer() { return buffer_; };

		void Interpret(char *data, int length);
	private:
		utils::Point *buffer_ = nullptr;
		unsigned buffer_size_ = 0;
		int buffer_idx_ = 0;
	};

} //namespace model
