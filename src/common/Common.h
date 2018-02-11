/*
    Copyright (c) Aleksey Fedotov
    MIT license
*/

#pragma once

#include <memory>
#include <iostream>

#define DIE(msg) do { std::cerr << msg << std::endl; exit(1); } while (0)
