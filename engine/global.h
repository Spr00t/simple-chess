#pragma once
#include <fstream>
#include <string>

// Constants to compare with the macros
#define WHITE 0x00
#define BLACK 0x10
#define TOGGLE_COLOR(x) (0x10 ^ x)
#define NOT !
class Global
{
public:
    void setSlaveMode(bool);
    bool isSlaveMode() const;

    void setColor(int color);
    void log(const std::string & str);
    static Global& instance();
    int color = -1;
    std::fstream file;
    bool slave_mode = false;

public:
};
