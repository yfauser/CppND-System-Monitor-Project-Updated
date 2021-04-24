#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);  // TODO: See src/format.cpp
std::string TwoDigits(std::string const &timestr);
};                                    // namespace Format

#endif