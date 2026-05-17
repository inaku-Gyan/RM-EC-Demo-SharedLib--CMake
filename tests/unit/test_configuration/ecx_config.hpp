/**
 * @file ecx_config.hpp
 * @brief ECX 用户默认配置文件。本文件同时有两个作用
 *    1. 作为测试的一部分，用于测试 ECX 的自动发现默认名称的用户配置文件的机制。
 *    2. 配合 arm-none-eabi-gcc 的构建，为开发 ECX 源代码提供
 *       编译数据库（compile_commands.json），以供语言服务器使用。
 *       所以该文件中的配置选项应当配置地尽可能**适用于 ECX 的开发**。
 */

#pragma once

#define ECX_USE_DEV_DEBUG 0
#define ECX_USE_USAGE_CHECK 1

#define ECX_USE_DCACHE 1
#define ECX_DCACHE_LINE_SIZE 32

// 下面这个定义只是用于测试。不会有实际作用
#define SELF_DEFINE_MACRO 1
