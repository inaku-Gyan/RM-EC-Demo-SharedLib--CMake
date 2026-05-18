# RM-EC-Demo-SharedLib

面向 `arm-none-eabi-gcc` 工具链嵌入式工程的公共 C++20 header-only 库（namespace `ecx::`）。

下游消费方式：通过 `release` 分支取走平铺后的头文件（详见 [发布](#发布) 章节）。本仓库根目录的 CMake 工程**不会随 `release` 分发**，只服务于库本身的开发与测试。

## 环境准备

| 工具                       | 用途                                     | 版本要求                             |
| -------------------------- | ---------------------------------------- | ------------------------------------ |
| CMake                      | 构建系统驱动                             | ≥ 3.25                               |
| Ninja                      | 构建后端（preset 已锁定）                | 任意近代版本                         |
| g++ / clang++              | host 端编译，跑 ctest                    | 支持 C++20（g++ ≥ 11、clang++ ≥ 14） |
| `arm-none-eabi-gcc` 工具链 | 交叉编译验证（`build-armgcc` preset 用） | ≥ 11（C++20 支持）                   |
| VS Code + clangd 插件      | 语言服务（项目已禁用 MS IntelliSense）   | clangd ≥ 17                          |

首次使用 VS Code 打开本仓库：

##### 1. VS Code 配置

安装 `.vscode/extensions.json` 推荐的插件。

VS Code 工作区配置：
```bash
cp .vscode/settings.example.json .vscode/settings.json
```
之后，你可以根据个人需要调整 `settings.json`。

##### 2. Clangd 语言服务器索引准备

Clangd 需要 `compile_commands.json`（编译数据库）来确定编译指令，以提供正确的语言服务功能（如跳转定义、自动补全、错误检查等）。本仓库使用借助 CMake Presets 生成 `compile_commands.json`。

```bash
# 生成本项目业务代码部分使用的 compile_commands.json
cmake --preset build-armgcc

# 生成测试代码使用的 compile_commands.json
cmake --preset test-dev
```

> Clangd 自动根据 `.clangd` 文件中配置的路径寻找 `compile_commands.json`。

## 日常开发

本仓库提供三个 preset：

| Preset         | 用途                                                                                                                                                                                                 | 编译配置                                                                 | 跑测试？        |
| -------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------ | --------------- |
| `test-dev`     | **日常迭代**：写代码、调测试，反馈最快                                                                                                                                                               | `-O0 -g`（Debug）                                                        | ✅               |
| `test-opt`     | **优化烟雾测试**：验证 `-O3 -DNDEBUG` 不会因 UB / 误依赖 assert 而破坏行为                                                                                                                           | `-O3 -DNDEBUG`（Release）                                                | ✅               |
| `build-armgcc` | **目标工具链验证**：用 arm-none-eabi-gcc + Cortex-M4F 编译 algo+proto，会在编译前动态生成包含 `src/` 头文件的 cpp 文件，产出 `libecx_arm_check.a`，确认 headers 在真实嵌入式工具链上能解析与代码生成 | M4F 硬浮点、`--specs=nano.specs -fno-exceptions -fno-rtti`、`MinSizeRel` | ❌（只验证编译） |

### 常用命令

```bash
# 配置/更换 preset（生成 build/<preset>/，包括 compile_commands.json）
cmake --preset test-dev

# 编译（首次会经 FetchContent 拉 GoogleTest）
cmake --build --preset test-dev

# 跑测试（CTest 自动发现 gtest 用例；失败时输出详情）
ctest --preset test-dev
```

`test-opt` 命令同理替换 preset 名即可。更换 preset 后，以上三步必须按顺序执行。

```bash
# 验证 arm-none-eabi-gcc 工具链编译
cmake --preset build-armgcc && cmake --build --preset build-armgcc
```

### 加测试用例

测试源码位于 [tests/](tests/)。新增用例：
1. 在 [tests/](tests/) 下的子目录中新建 `test_xxx.cpp`，沿用 GoogleTest 的 `TEST(Suite, Name) { ... }` 写法。
2. 把文件名追加到子目录 `CMakeLists.txt` 中。
3. 重新 build + ctest 即可。

### 添加新模块到库

新增 header 直接放入 `src/<分类>/` 即可——它们会自动被 `ecx` INTERFACE 库 include。无需改 [CMakeLists.txt](CMakeLists.txt)。

### 切换/添加交叉工具链

当前 [cmake/arm-none-eabi.cmake](cmake/arm-none-eabi.cmake) 锁定 Cortex-M4 + 硬浮点（STM32F4 代表）。如需新增 STM32F7（Cortex-M7）或其他验证档：
1. 复制一份 toolchain 文件，调整 `-mcpu` / `-mfpu` / `-mfloat-abi`；
2. 在 [CMakePresets.json](CMakePresets.json) 中新增一个继承 `armgcc-base` 但覆盖 `toolchainFile` 的 configurePreset 即可。

## 发布

### 触发与产出

发布由 **手动触发 workflow_dispatch** 执行（见 [release.yml](.github/workflows/release.yml)）。工作流做这几件事：

1. 要求在 `main` 分支触发，并输入必填参数 `<release_tag>`。
2. **校验 `<release_tag>` 格式**：`^v(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)(-(alpha|beta)\.[1-9][0-9]*)?$`，不符合直接报错退出。
3. **校验 Git tag 不重复**：`<release_tag>` 与 `development/<release_tag>` 均必须是新 tag。
4. 在源仓库以**稀疏检出**只取 `src/` + `LICENSE`，过滤掉 `tests/`、`test/`、`test_*` 等文件。
5. 切到 `release` 分支，**清空**该分支内容（保留 `.git/`），将 `src/*` **平铺**到分支根，并附上 `LICENSE`。
6. 以 `github-actions[bot]` 身份提交，commit message 标注「源 commit SHA」，并推送：
   - `release` 分支更新；
   - `<release_tag>`（打在 `release` 分支新 commit 上）；
   - `development/<release_tag>`（打在 `main` 分支对应源 commit 上）。

最终结果：`release` 分支根目录直接是本项目用于分发的业务代码。下游可以把 `release` 分支作为 git submodule 引入，或拉取后直接 `add_subdirectory` / 加入 include path。

### 发布步骤（维护者）

1. 在 `main` 分支确认要发布的 commit；本地跑过 `test-dev`、`test-opt`、`build-armgcc` 三档全部 PASS。
2. 在 GitHub 仓库的 **Actions → Release** 中点击 **Run workflow**，并确认运行分支为 `main`。
3. 输入 `<release_tag>`（如 `v1.2.3`、`v1.2.3-beta.1`、`v1.2.3-alpha.1`），再执行。
4. 检查 workflow 运行状态、`release` 分支，以及两个新 tag（`<release_tag>` 与 `development/<release_tag>`）均创建成功。

### 注意事项

- **不要往 `release` 分支手工 push**。该分支由 workflow 全量重写，手工提交会被下一次发布覆盖。
- **不要发布带工具链/测试基建的内容到 `release`**。`tests/`、`cmake/`、`CMakeLists.txt`、`CMakePresets.json`、`.clangd` 等都在 sparse-checkout 之外，已天然排除。
- **并发**：workflow 用 `concurrency: release` 串行化，多次连续发布不会互相覆盖中间态。
