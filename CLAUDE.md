# CLAUDE.md

面向 arm-none-eabi-gcc 嵌入式工程的 C++20 **header-only** 公共库（namespace `ecx::`）。

## 项目结构与约定

- `src/algo/` `src/proto/` `src/rtos/` —— 库本体；**只放头文件**，新增模块直接进对应子目录即可，无需改 CMake。
- `src/rtos/` 依赖 FreeRTOS，**当前不参与构建/测试**；clangd 已抑制其 `pp_file_not_found`。
- `tests/` —— host 端 GoogleTest 用例。每个模块用自己的 CMakeLists.txt 显式声明 (源, config) 矩阵；详见 [tests/CMakeLists.txt](tests/CMakeLists.txt) 顶部注释。要点：
  - 模块测试源：`tests/unit/<module>/test_*.cpp`，变体专属测试与默认测试同层放置，是否编进某 exec 由模块 CMakeLists.txt 显式决定。
  - 模块 CMakeLists.txt：用 `ecx_add_test_executable(NAME ... SOURCES ... [CONFIG ecx_config_<name>.hpp] [PREFIX ...])` 声明 exec。不传 CONFIG 走 `ecx_config_default.hpp`；传则通过 `ECX_INC_USER_CONFIG` 注入指定 config 头（见 [src/_inc/configuration.hpp](src/_inc/configuration.hpp)）。
  - 跨模块共享 config：`tests/configs/ecx_config_<name>.hpp`（扁平，不嵌子目录；模块也可在自己的目录下放同名头来覆盖共享）。
  - 跨模块公用测试工具：`tests/support/`（headers-only，自动暴露为 `ecx_test_support`）。
  - 模块本地 helper headers：直接放模块目录，模块 CMakeLists 决定哪些源进 exec。
  - 集成测试：`tests/integration/CMakeLists.txt`（按需创建，顶层自动 add_subdirectory）。
  - 新增模块只需 `mkdir tests/unit/<M>` + 写 `tests/unit/<M>/CMakeLists.txt`，顶层 foreach 自动接住。
  - ctest 命名：在 CMakeLists 里用 `PREFIX "<module>::<variant?>::"` 显式给出，例如 `containers::usage_assert::SpscZeroCopyQueueTest.X`。
- `cmake/` —— arm-none-eabi 工具链文件与目标平台设定。
- 全项目语言风格：**注释一律用中文**；标识符遵循 [.clang-tidy](.clang-tidy)（类 CamelCase、函数 lower_case、私有成员 `_` 后缀、`constexpr` UPPER_CASE）。
- 代码风格由 [.clang-format](.clang-format) 强制（Google 风、C++20、100 列）。
- `.clang-tidy` 设了 `WarningsAsErrors: "*"`，提交前确保无 lint 失败。

## CMake 工程的定位

CMake 这里**不为分发服务**——分发由 [.github/workflows/release.yml](.github/workflows/release.yml) 把 `src/* + LICENSE` 平铺到 `release` 分支完成。**CMake 的全部目标就是跑 ctest + 验证目标工具链能编**。

三个 preset：

| Preset | 用途 |
|---|---|
| `test-dev` | 日常迭代，`-O0 -g`，跑测试。**clangd 也读这个 build 的 compile_commands**。 |
| `test-opt` | `-O3 -DNDEBUG` 下回归一遍，揪优化暴露的问题。 |
| `build-armgcc` | arm-none-eabi-gcc + Cortex-M4F 编 algo+proto，仅出静态库不跑测试。 |

## 常用命令

```bash
cmake --preset test-dev && cmake --build --preset test-dev && ctest --preset test-dev
cmake --preset test-opt && cmake --build --preset test-opt && ctest --preset test-opt
cmake --preset build-armgcc && cmake --build --preset build-armgcc
```

## 边界与禁区

- **不要**把构建/测试基建塞进 `src/`——会被 release 分发污染。`tests/`、`cmake/`、`CMakeLists.txt`、`CMakePresets.json`、`.clangd` 必须留在根目录或非 `src/` 子目录。
- **不要**手工 push `release` 分支，它由 workflow 全量重写。
- **不要**修改 [.github/workflows/release.yml](.github/workflows/release.yml) 的 sparse-checkout 规则除非确认要变发布契约。
- HAL/RTOS 三方依赖目前不在仓库内提供，也未约定 imported target；不要为了让 `src/rtos/` 编过而引入伪 stub。等真要做 mock 时再统一设计。
- 不要在源码里写英文注释（除非引用外部规范名词）。

## 关键文件

- [CMakeLists.txt](CMakeLists.txt) —— 根工程，定义 INTERFACE 库 `ecx::ecx` 与 `ECX_BUILD_TESTS` / `ECX_BUILD_ARM_CHECK` 选项（后者会动态扫描头文件并生成检查用的 cpp）。
- [CMakePresets.json](CMakePresets.json) —— 三个 preset 的全部配置。
- [tests/CMakeLists.txt](tests/CMakeLists.txt) —— GoogleTest v1.17.0 via FetchContent；按目录自动发现模块与变体并生成多个 exec。
- [cmake/arm-none-eabi.cmake](cmake/arm-none-eabi.cmake) —— 交叉工具链定义。
- [.clangd](.clangd) —— 指向 `build/test-dev`，并对 `src/rtos/.*` 抑制头缺失诊断。
- [README.md](README.md) —— 面向人的环境/开发/发布说明。
