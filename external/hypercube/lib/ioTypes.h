#pragma once
#include <string>
namespace SEP {
enum dataType {
  DATA_BYTE,
  DATA_INT,
  DATA_FLOAT,
  DATA_COMPLEX,
  DATA_DOUBLE,
  DATA_UNKNOWN
};

size_t getDataTypeSize(dataType typ);
dataType toElementType(const std::string &name);
std::string getTypeString(dataType typ);

}  // namespace SEP
