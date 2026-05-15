# CLAUDE.md

面向 arm-none-eabi-gcc 嵌入式工程的 C++20 **header-only** 公共库（namespace `ecx::`）。

## 项目结构与约定

- `src/algo/` `src/proto/` `src/rtos/` —— 库本体；**只放头文件**，新增模块直接进对应子目录即可，无需改 CMake。
- `src/rtos/` 依赖 FreeRTOS，**当前不参与构建/测试**；clangd 已抑制其 `pp_file_not_found`。
- `tests/` —— host 端 GoogleTest 用例，仅 `algo` + `proto`。新增用例需把 `.cpp` 文件名追加到 [tests/CMakeLists.txt](tests/CMakeLists.txt) 的 `ecx_tests` 源列表。
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
- [tests/CMakeLists.txt](tests/CMakeLists.txt) —— GoogleTest v1.15.2 via FetchContent。
- [cmake/arm-none-eabi.cmake](cmake/arm-none-eabi.cmake) —— 交叉工具链定义。
- [.clangd](.clangd) —— 指向 `build/test-dev`，并对 `src/rtos/.*` 抑制头缺失诊断。
- [README.md](README.md) —— 面向人的环境/开发/发布说明。
