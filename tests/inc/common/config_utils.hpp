#pragma once

#include <cstdlib>

/**
 * 如果 expr 为假，则中止程序执行。
 */
#define ABORT_UNLESS(expr)             \
    do {                               \
        if (!(expr)) { std::abort(); } \
    } while (0)
