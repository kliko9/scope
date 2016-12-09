#pragma once

#include <vector>

namespace model {

	class DataInterpreter {
	public:
		DataInterpreter();
		virtual ~DataInterpreter();

		std::vector<float> &Interpret(const char *data);
	private:
		std::vector<float> buffer_;

	};

} //namespace model
