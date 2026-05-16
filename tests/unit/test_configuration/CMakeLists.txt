ecx_add_test_executable(
    NAME    test_Configuration_no_user_config
    SOURCES test_no_user_config.cpp
    DEFINES  "ECX_USE_USER_CONFIG=0" # 强制不使用用户配置，测试默认配置是否生效
    PREFIX  "Configuration::NoUserConfig::"
)

