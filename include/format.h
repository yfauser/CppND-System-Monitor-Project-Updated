#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);
std::string TwoDigits(std::string const &timestr);
};  // namespace Format

#endif