#pragma once

#include <vector>

namespace model {

	class DataInterpreter {
	public:
		DataInterpreter();
		virtual ~DataInterpreter();

		std::vector<double> &Interpret(const char *data);
	private:
		std::vector<double> buffer_;

	};

} //namespace model
