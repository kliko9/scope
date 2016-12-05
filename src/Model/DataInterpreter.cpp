#include "Model/DataInterpreter.h"

#define IS_DATA_VALID(x) (x != '\0')
#define IS_DATA_FRAME_1(x) ((x & (1UL << 6)) == 0)
#define IS_DATA_FRAME_2(x) ((x & (1UL << 6)))

#define ADC_MAX_VALUE 4096.0 //ADC with 12 bit resolution

namespace model {

DataInterpreter::DataInterpreter()
{

}

DataInterpreter::~DataInterpreter()
{

}

std::vector<double> &DataInterpreter::Interpret(const char *data)
{
	int value = 0;
	buffer_.clear();

	while (IS_DATA_VALID(*data) && IS_DATA_VALID(*(data + 1))) {

		if (IS_DATA_FRAME_1(*data))
			value = ((*data & 0x3F) << 6);
		else {
			++data;
			continue;
		}

		if (IS_DATA_FRAME_2(*(data + 1)))
			value |= (*(data + 1) & 0x3F);
		else {
			++data;
			continue;
		}

		buffer_.push_back(value/ADC_MAX_VALUE);
		++data;
		value = 0;
	}

	return buffer_;
}

} //namespace model
