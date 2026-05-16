ecx_add_test_executable(
    NAME    test_Configuration_no_user_config
    SOURCES test_no_user_config.cpp
    DEFINES  "ECX_USE_USER_CONFIG=0" # 强制不使用用户配置，测试默认配置是否生效
    PREFIX  "Configuration::NoUserConfig::"
)
    
ecx_add_test_executable(
    NAME    test_Configuration_default_config_path
    SOURCES test_default_config_path.cpp
    PREFIX  "Configuration::DefaultConfigPath::"
)

target_include_directories(test_Configuration_default_config_path
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"  # 模块本地 config（优先于共享）
)
