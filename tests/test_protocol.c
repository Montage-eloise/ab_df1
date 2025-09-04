#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "df1_protocol.h"

// 简单的测试框架宏
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s\n", message); \
            return 0; \
        } \
    } while(0)

#define TEST_PASS(message) \
    do { \
        printf("PASS: %s\n", message); \
        return 1; \
    } while(0)

// 测试DF1配置初始化
int test_config_init() {
    printf("测试DF1配置初始化...\n");
    
    df1_config_t config;
    df1_config_init(&config, 1, 2, 0);
    
    TEST_ASSERT(config.station == 1, "站号设置错误");
    TEST_ASSERT(config.dst_node == 2, "目标节点设置错误");
    TEST_ASSERT(config.src_node == 0, "源节点设置错误");
    TEST_ASSERT(config.check_type == DF1_CHECK_CRC16, "默认校验类型错误");
    TEST_ASSERT(config.transaction_id == 0, "初始事务ID错误");
    
    TEST_PASS("DF1配置初始化");
}

// 测试读命令构建
int test_build_read_command() {
    printf("测试读命令构建...\n");
    
    df1_config_t config;
    df1_config_init(&config, 1, 2, 0);
    
    uint8_t buffer[256];
    size_t actual_size;
    
    // 构建读取N7:0的命令
    int result = df1_build_read_command(&config, "N7:0", 2, buffer, sizeof(buffer), &actual_size);
    
    TEST_ASSERT(result == 0, "读命令构建失败");
    TEST_ASSERT(actual_size > 10, "读命令长度太短");
    
    // 检查命令头部分
    TEST_ASSERT(buffer[0] == 0x10, "DLE STX第一字节错误");
    TEST_ASSERT(buffer[1] == 0x01, "DLE STX第二字节错误");
    TEST_ASSERT(buffer[2] == 1, "站号错误");
    
    TEST_PASS("读命令构建");
}

// 测试写命令构建
int test_build_write_command() {
    printf("测试写命令构建...\n");
    
    df1_config_t config;
    df1_config_init(&config, 1, 2, 0);
    
    uint8_t buffer[256];
    uint8_t write_data[] = {0x12, 0x34};
    size_t actual_size;
    
    // 构建写入N7:0的命令
    int result = df1_build_write_command(&config, "N7:0", write_data, sizeof(write_data), 
                                        buffer, sizeof(buffer), &actual_size);
    
    TEST_ASSERT(result == 0, "写命令构建失败");
    TEST_ASSERT(actual_size > 12, "写命令长度太短");
    
    // 检查命令头部分
    TEST_ASSERT(buffer[0] == 0x10, "DLE STX第一字节错误");
    TEST_ASSERT(buffer[1] == 0x01, "DLE STX第二字节错误");
    TEST_ASSERT(buffer[2] == 1, "站号错误");
    
    TEST_PASS("写命令构建");
}

// 测试无效参数处理
int test_invalid_parameters() {
    printf("测试无效参数处理...\n");
    
    df1_config_t config;
    df1_config_init(&config, 1, 2, 0);
    
    uint8_t buffer[256];
    size_t actual_size;
    uint8_t write_data[] = {0x12, 0x34};
    
    // 测试NULL参数
    TEST_ASSERT(df1_build_read_command(NULL, "N7:0", 2, buffer, sizeof(buffer), &actual_size) != 0,
               "NULL配置应该失败");
    TEST_ASSERT(df1_build_read_command(&config, NULL, 2, buffer, sizeof(buffer), &actual_size) != 0,
               "NULL地址应该失败");
    TEST_ASSERT(df1_build_read_command(&config, "N7:0", 2, NULL, sizeof(buffer), &actual_size) != 0,
               "NULL缓冲区应该失败");
    TEST_ASSERT(df1_build_read_command(&config, "N7:0", 2, buffer, sizeof(buffer), NULL) != 0,
               "NULL大小指针应该失败");
    
    // 测试无效地址
    TEST_ASSERT(df1_build_read_command(&config, "X7:0", 2, buffer, sizeof(buffer), &actual_size) != 0,
               "无效地址应该失败");
    TEST_ASSERT(df1_build_read_command(&config, "N7", 2, buffer, sizeof(buffer), &actual_size) != 0,
               "格式错误地址应该失败");
    
    // 测试写命令的无效参数
    TEST_ASSERT(df1_build_write_command(NULL, "N7:0", write_data, sizeof(write_data), 
                                       buffer, sizeof(buffer), &actual_size) != 0,
               "写命令NULL配置应该失败");
    TEST_ASSERT(df1_build_write_command(&config, "N7:0", NULL, sizeof(write_data), 
                                       buffer, sizeof(buffer), &actual_size) != 0,
               "写命令NULL数据应该失败");
    
    TEST_PASS("无效参数处理");
}

// 测试响应解析
int test_response_parsing() {
    printf("测试响应解析...\n");
    
    // 模拟一个成功的响应（简化版本）
    uint8_t response[] = {
        0x10, 0x01, 0x01,           // DLE SOH Station
        0x10, 0x02,                 // DLE STX
        0x02, 0x00,                 // DST, SRC
        0x0F, 0x00,                 // CMD, STS (成功)
        0x01, 0x00,                 // TNS
        0x12, 0x34,                 // 数据
        0x10, 0x03,                 // DLE ETX
        0x00, 0x00                  // 校验
    };
    
    uint8_t data[10];
    size_t actual_size;
    
    int result = df1_parse_response(response, sizeof(response), data, sizeof(data), &actual_size);
    
    TEST_ASSERT(result == 0, "响应解析失败");
    TEST_ASSERT(actual_size == 2, "解析的数据长度错误");
    TEST_ASSERT(data[0] == 0x12, "解析的数据内容错误");
    TEST_ASSERT(data[1] == 0x34, "解析的数据内容错误");
    
    // 测试错误响应
    uint8_t error_response[] = {
        0x10, 0x01, 0x01,           // DLE SOH Station
        0x10, 0x02,                 // DLE STX
        0x02, 0x00,                 // DST, SRC
        0x0F, 0x10,                 // CMD, STS (错误)
        0x01, 0x00,                 // TNS
        0x10, 0x03,                 // DLE ETX
        0x00, 0x00                  // 校验
    };
    
    result = df1_parse_response(error_response, sizeof(error_response), data, sizeof(data), &actual_size);
    TEST_ASSERT(result != 0, "错误响应应该解析失败");
    
    TEST_PASS("响应解析");
}

// 测试错误描述
int test_error_descriptions() {
    printf("测试错误描述...\n");
    
    const char* desc = df1_get_error_description(0x10);
    TEST_ASSERT(desc != NULL, "错误描述不应为NULL");
    TEST_ASSERT(strlen(desc) > 0, "错误描述不应为空");
    
    desc = df1_get_ext_error_description(1);
    TEST_ASSERT(desc != NULL, "扩展错误描述不应为NULL");
    TEST_ASSERT(strlen(desc) > 0, "扩展错误描述不应为空");
    
    TEST_PASS("错误描述");
}

int main() {
    printf("AB DF1 协议单元测试\n");
    printf("===================\n\n");
    
    int passed = 0;
    int total = 0;
    
    total++; passed += test_config_init();
    total++; passed += test_build_read_command();
    total++; passed += test_build_write_command();
    total++; passed += test_invalid_parameters();
    total++; passed += test_response_parsing();
    total++; passed += test_error_descriptions();
    
    printf("\n测试结果: %d/%d 通过\n", passed, total);
    
    if (passed == total) {
        printf("所有测试通过！\n");
        return 0;
    } else {
        printf("有测试失败！\n");
        return 1;
    }
}
