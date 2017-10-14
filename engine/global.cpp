#include <iostream>
#include "global.h"

using namespace std;
Global &Global::instance() {
    static Global slavelog;
    return slavelog;
}

bool Global::isSlaveMode() const
{
    return slave_mode;
}

void Global::setSlaveMode(bool set)
{
    slave_mode = set;
}

void Global::setColor(int color)
{
    file.open(color == WHITE ? "white.log" : "black.log", fstream::out);
}

void Global::log(const string &str)
{
    if (file.is_open()) {
        file << str << endl;
    } else {
        cout << str << endl;
    }
}
