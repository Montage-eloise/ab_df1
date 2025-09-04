cmake_minimum_required(VERSION 3.10)
project(ab_df1_lib VERSION 1.0.0 LANGUAGES C)

# 设置C标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")

# 包含目录
include_directories(include)

# 库源文件
set(LIB_SOURCES
    src/df1_address.c
    src/df1_protocol.c
    src/df1_serial.c
)

# 创建静态库
add_library(ab_df1_static STATIC ${LIB_SOURCES})
target_include_directories(ab_df1_static PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(ab_df1_static PROPERTIES OUTPUT_NAME ab_df1)

# 创建动态库
add_library(ab_df1_shared SHARED ${LIB_SOURCES})
target_include_directories(ab_df1_shared PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(ab_df1_shared PROPERTIES 
    OUTPUT_NAME ab_df1
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# 别名目标
add_library(ab_df1::static ALIAS ab_df1_static)
add_library(ab_df1::shared ALIAS ab_df1_shared)

# 示例程序
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_executable(simple_read examples/simple_read.c)
    target_link_libraries(simple_read ab_df1_static)
    
    add_executable(simple_write examples/simple_write.c)
    target_link_libraries(simple_write ab_df1_static)
    
    add_executable(address_parser_demo examples/address_parser_demo.c)
    target_link_libraries(address_parser_demo ab_df1_static)
endif()

# 测试程序
option(BUILD_TESTS "Build test programs" ON)
if(BUILD_TESTS)
    enable_testing()
    
    add_executable(test_address tests/test_address.c)
    target_link_libraries(test_address ab_df1_static)
    add_test(NAME AddressTest COMMAND test_address)
    
    add_executable(test_protocol tests/test_protocol.c)
    target_link_libraries(test_protocol ab_df1_static)
    add_test(NAME ProtocolTest COMMAND test_protocol)
endif()

# 安装设置
include(GNUInstallDirs)

# 安装库文件
install(TARGETS ab_df1_static ab_df1_shared
    EXPORT ab_df1_targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装头文件
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# 安装示例程序（可选）
if(BUILD_EXAMPLES)
    install(TARGETS simple_read simple_write address_parser_demo
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()

# 创建和安装配置文件
include(CMakePackageConfigHelpers)

# 生成配置文件
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ab_df1_libConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ab_df1_libConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ab_df1_lib
)

# 生成版本文件
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ab_df1_libConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 安装配置文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ab_df1_libConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ab_df1_libConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ab_df1_lib
)

# 安装导出目标
install(EXPORT ab_df1_targets
    FILE ab_df1_libTargets.cmake
    NAMESPACE ab_df1::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ab_df1_lib
)

# 打包设置
set(CPACK_PACKAGE_NAME "ab_df1_lib")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Allen-Bradley DF1 Protocol Communication Library")
set(CPACK_PACKAGE_VENDOR "AB DF1 Library Project")
set(CPACK_PACKAGE_CONTACT "developer@example.com")

# 设置包类型
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM")
endif()

include(CPack)

# 显示配置信息
message(STATUS "")
message(STATUS "AB DF1 Library Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "")
