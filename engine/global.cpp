#include <iostream>
#include "global.h"

using namespace std;
Global &Global::instance() {
    static Global slavelog;
    return slavelog;
}

Global::Global()
{
    start = boost::posix_time::second_clock::local_time();

}

void Global::setColor(int color)
{
    file.open(color == WHITE ? "white.log" : "black.log", fstream::out);
}

void Global::log(const string &str)
{
    boost::posix_time::time_duration diff = boost::posix_time::second_clock::local_time() - start;
    long long total_ms = diff.total_milliseconds();
    int total_sec = (total_ms / 1000);
    int total_min = (total_ms / (1000 * 60));

    int milliseconds = total_ms % 1000;
    int seconds = total_sec % 60;
    int minutes = total_min % 60;

    if (file.is_open()) {
        file << "[" << setfill('0') << setw(2)
             << minutes << ":"
             << seconds << "."
             << setw(3)
             << milliseconds
             << "] " << str << endl;
    } else {
        cout << str << endl;
    }
}
