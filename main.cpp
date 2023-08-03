/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */
#include "src/App.h"
#include "entry/entry.h"
#include <iostream>


int _main_(int, char**) { return 0;}

int main(int _argc, const char* const* _argv)
{
    Engine::App application(1024, 768, "useless");

    application.init();
    application.run();
    application.cleanup();
    return 0;
}
