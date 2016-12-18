#include "Model/DataInterpreter.h"
#include "Log.h"

#define IS_DATA_VALID(x) (x != '\0')
#define IS_DATA_FRAME_1(x) ((x & (1UL << 6)) == 0)
#define IS_DATA_FRAME_2(x) ((x & (1UL << 6)))

//#define ADC_MAX_VALUE 4096.0 //ADC with 12 bit resolution
#define ADC_MAX_VALUE 256.0 //ADC with 8 bit resolution

namespace model {

DataInterpreter::DataInterpreter()
{

}

DataInterpreter::~DataInterpreter()
{

}

void DataInterpreter::SetBuffer(utils::Point *buffer, unsigned size)
{
	buffer_ = buffer;
	buffer_size_ = size;
}

void DataInterpreter::Interpret(char *data, int length)
{
	unsigned dataIdx = 0;

	while (dataIdx < length) {
		buffer_[buffer_idx_].y = (float)data[dataIdx++]/ADC_MAX_VALUE;
		buffer_[buffer_idx_].x = (float)(buffer_idx_ - 500)/(buffer_size_*0.5);

		++buffer_idx_;

		buffer_idx_ %= buffer_size_;

	}
}

} //namespace model
