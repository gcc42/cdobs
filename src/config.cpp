#include "config.h"
#include <iostream>
#include <fstream>
#include <string>

const std::string kDefaultDbFile = "cdobs.sqlite";
const std::string kDefaultDbPath = "./";

const int kMaxTimeLength = 100;

const int kMaxObjectSize = 5000000;
const int kSegmentSize = 2000000; // 2mb segments

const std::string kDevNullFileName = "/dev/null";

#ifdef IS_DEBUG
std::ostream &dout = std::cout;
#else
std::ostream &dout = *(new std::ofstream(kDevNullFileName.c_str()));
#endif 