#include "format.h"

#include <string>
#include <chrono>
#include <sstream>

using std::string;
using std::chrono::duration_cast;
using std::chrono::hours;
using std::chrono::minutes;

// Function to format the time to two digits, e.g. 3 sec/min/hours
// will be returned as 03 seconds
string Format::TwoDigits(string const &timestr) {
    if (timestr.length() == 1) {
        return "0" + timestr;
    }
    else return timestr;
}

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// Inspired by https://stackoverflow.com/questions/42866524/convert-chronoduration-to-string-or-c-string
string Format::ElapsedTime(long seconds) {
    std::chrono::seconds sec(seconds);
    std::ostringstream secstream, minstream, hourstream;
    hourstream << duration_cast<hours>(sec).count();
    minstream << duration_cast<minutes>(sec).count() % 60;
    secstream << sec.count() % 60;
    string secs = secstream.str();
    string mins = minstream.str();
    string hours = hourstream.str();
    return TwoDigits(hours) + ":" + TwoDigits(mins) + ":" + TwoDigits(secs);
}