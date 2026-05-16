include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0
    GIT_SHALLOW    TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# ---------------------------------------------------------------------------
# 目录约定（与 src/<module>/ 镜像）
#   tests/
#   ├── support/                # 跨模块公用测试工具 (headers-only) → ecx_test_support
#   ├── configs/<V>/ecx_config.hpp
#   │                           # 跨模块共享的 ecx_config.hpp 变体
#   ├── unit/<M>/test_*.cpp     # 模块 M 在默认 config 下的单元测试
#   ├── unit/<M>/<V>/test_*.cpp # 模块 M 在变体 V 下的额外测试
#   ├── unit/<M>/<V>/ecx_config.hpp
#   │                           # （可选）模块本地 config，优先于 configs/<V>/
#   └── integration/test_*.cpp  # 跨模块集成测试
#
# 模块根下保留名 `support` 不视为变体，留给模块本地 helper headers。
# ---------------------------------------------------------------------------

# 跨模块公用测试工具：INTERFACE 库，被所有测试可执行文件 link
add_library(ecx_test_support INTERFACE)
target_include_directories(ecx_test_support INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/support"
)
target_link_libraries(ecx_test_support INTERFACE
    GTest::gtest
    GTest::gmock
)

# 创建一个单元测试可执行文件。
#   ecx_add_unit_test_executable(<module> [<variant>])
# 行为：
#   - 不提供 variant：编 tests/unit/<module>/test_*.cpp，不覆盖 ecx_config.hpp。
#     exec 名 ecx_tests_<module>，ctest 前缀 "<module>::"。
#   - 提供 variant：编 tests/unit/<module>/test_*.cpp + tests/unit/<module>/<variant>/test_*.cpp。
#     ecx_config.hpp 解析：模块本地 (tests/unit/<module>/<variant>/) → 共享 (tests/configs/<variant>/) → FATAL_ERROR。
#     exec 名 ecx_tests_<module>_<variant>，ctest 前缀 "<module>::<variant>::"。
#   - 若没有任何测试源，跳过生成 exec。
function(ecx_add_unit_test_executable module)
    set(variant "${ARGN}")

    set(module_dir "${CMAKE_CURRENT_SOURCE_DIR}/unit/${module}")
    file(GLOB base_sources CONFIGURE_DEPENDS "${module_dir}/test_*.cpp")

    set(variant_sources "")
    set(config_dir "")

    if(variant STREQUAL "")
        set(exec_name "ecx_tests_${module}")
        set(test_prefix "${module}::")
    else()
        set(exec_name "ecx_tests_${module}_${variant}")
        set(test_prefix "${module}::${variant}::")

        file(GLOB variant_sources CONFIGURE_DEPENDS
            "${module_dir}/${variant}/test_*.cpp"
        )

        # config 优先级：模块本地 > 共享
        if(EXISTS "${module_dir}/${variant}/ecx_config.hpp")
            set(config_dir "${module_dir}/${variant}")
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/configs/${variant}/ecx_config.hpp")
            set(config_dir "${CMAKE_CURRENT_SOURCE_DIR}/configs/${variant}")
        else()
            message(FATAL_ERROR
                "变体 '${variant}' (模块 '${module}') 找不到 ecx_config.hpp：\n"
                "  既不存在 ${module_dir}/${variant}/ecx_config.hpp\n"
                "  也不存在 ${CMAKE_CURRENT_SOURCE_DIR}/configs/${variant}/ecx_config.hpp"
            )
        endif()
    endif()

    if(NOT base_sources AND NOT variant_sources)
        return()
    endif()

    add_executable(${exec_name} ${base_sources} ${variant_sources})
    target_link_libraries(${exec_name} PRIVATE
        ecx::ecx
        ecx_test_support
        GTest::gtest_main
    )
    # 模块目录加 PRIVATE include，便于变体子目录里的测试 #include 模块本地 helper headers
    target_include_directories(${exec_name} PRIVATE "${module_dir}")
    if(config_dir)
        # config 目录加 PRIVATE include，让 configuration.hpp 的 __has_include(<ecx_config.hpp>)
        # 命中该变体的覆盖文件
        target_include_directories(${exec_name} PRIVATE "${config_dir}")
    endif()
    gtest_discover_tests(${exec_name} TEST_PREFIX "${test_prefix}")
endfunction()

# ---------------------------------------------------------------------------
# 自动发现：遍历 tests/unit/<module>/，对每个模块生成默认 exec，
# 再对模块下每个非保留子目录生成变体 exec。
# ---------------------------------------------------------------------------
set(_unit_root "${CMAKE_CURRENT_SOURCE_DIR}/unit")
file(GLOB _module_paths LIST_DIRECTORIES true CONFIGURE_DEPENDS "${_unit_root}/*")
foreach(_module_path IN LISTS _module_paths)
    if(NOT IS_DIRECTORY "${_module_path}")
        continue()
    endif()
    get_filename_component(_module "${_module_path}" NAME)

    ecx_add_unit_test_executable(${_module})

    file(GLOB _variant_paths LIST_DIRECTORIES true CONFIGURE_DEPENDS "${_module_path}/*")
    foreach(_variant_path IN LISTS _variant_paths)
        if(NOT IS_DIRECTORY "${_variant_path}")
            continue()
        endif()
        get_filename_component(_variant "${_variant_path}" NAME)
        # 模块本地 helper 目录保留名
        if(_variant STREQUAL "support")
            continue()
        endif()
        ecx_add_unit_test_executable(${_module} ${_variant})
    endforeach()
endforeach()

# ---------------------------------------------------------------------------
# 集成测试（可选）：tests/integration/ 下若有 test_*.cpp 就生成一个 exec。
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _integration_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/integration/test_*.cpp"
)
if(_integration_sources)
    add_executable(ecx_tests_integration ${_integration_sources})
    target_link_libraries(ecx_tests_integration PRIVATE
        ecx::ecx
        ecx_test_support
        GTest::gtest_main
    )
    gtest_discover_tests(ecx_tests_integration TEST_PREFIX "integration::")
endif()
