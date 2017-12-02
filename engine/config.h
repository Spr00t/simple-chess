#include "global.h"
enum class Mode {
    NORMAL,
    MASTER,
    SLAVE,
};

struct Config
{
    static Config from_args(int argc, char *argv[]);

    Mode mode = Mode::NORMAL;
    int color = WHITE;
};
