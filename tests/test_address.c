#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "df1_address.h"

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

// 测试地址解析功能
int test_address_parsing() {
    printf("测试地址解析功能...\n");
    
    df1_address_t addr;
    
    // 测试N7:0地址
    TEST_ASSERT(df1_address_parse("N7:0", &addr) == 0, "N7:0解析失败");
    TEST_ASSERT(addr.data_code == DF1_ADDR_N, "N7:0数据类型错误");
    TEST_ASSERT(addr.db_block == 7, "N7:0文件号错误");
    TEST_ASSERT(addr.address_start == 0, "N7:0起始地址错误");
    
    // 测试F8:123地址
    TEST_ASSERT(df1_address_parse("F8:123", &addr) == 0, "F8:123解析失败");
    TEST_ASSERT(addr.data_code == DF1_ADDR_F, "F8:123数据类型错误");
    TEST_ASSERT(addr.db_block == 8, "F8:123文件号错误");
    TEST_ASSERT(addr.address_start == 123, "F8:123起始地址错误");
    
    // 测试ST类型
    TEST_ASSERT(df1_address_parse("ST1:5", &addr) == 0, "ST1:5解析失败");
    TEST_ASSERT(addr.data_code == DF1_ADDR_ST, "ST1:5数据类型错误");
    TEST_ASSERT(addr.db_block == 1, "ST1:5文件号错误");
    TEST_ASSERT(addr.address_start == 5, "ST1:5起始地址错误");
    
    // 测试I类型（默认文件号）
    TEST_ASSERT(df1_address_parse("I:0", &addr) == 0, "I:0解析失败");
    TEST_ASSERT(addr.data_code == DF1_ADDR_I, "I:0数据类型错误");
    TEST_ASSERT(addr.db_block == 1, "I:0默认文件号错误");
    
    // 测试O类型（默认文件号）
    TEST_ASSERT(df1_address_parse("O:0", &addr) == 0, "O:0解析失败");
    TEST_ASSERT(addr.data_code == DF1_ADDR_O, "O:0数据类型错误");
    TEST_ASSERT(addr.db_block == 0, "O:0默认文件号错误");
    
    TEST_PASS("地址解析功能");
}

// 测试错误输入
int test_invalid_addresses() {
    printf("测试无效地址...\n");
    
    df1_address_t addr;
    
    // 测试无效格式
    TEST_ASSERT(df1_address_parse("N7", &addr) != 0, "缺少冒号的地址应该解析失败");
    TEST_ASSERT(df1_address_parse("X7:0", &addr) != 0, "无效地址类型应该解析失败");
    TEST_ASSERT(df1_address_parse("", &addr) != 0, "空字符串应该解析失败");
    TEST_ASSERT(df1_address_parse(NULL, &addr) != 0, "NULL指针应该解析失败");
    TEST_ASSERT(df1_address_parse("N7:0", NULL) != 0, "NULL输出指针应该解析失败");
    
    TEST_PASS("无效地址处理");
}

// 测试地址转字符串功能
int test_address_to_string() {
    printf("测试地址转字符串功能...\n");
    
    df1_address_t addr;
    char buffer[64];
    
    // 测试N7:0
    addr.data_code = DF1_ADDR_N;
    addr.db_block = 7;
    addr.address_start = 0;
    
    TEST_ASSERT(df1_address_to_string(&addr, buffer, sizeof(buffer)) == 0, "N7:0转字符串失败");
    TEST_ASSERT(strcmp(buffer, "N7:0") == 0, "N7:0转字符串结果错误");
    
    // 测试F8:123
    addr.data_code = DF1_ADDR_F;
    addr.db_block = 8;
    addr.address_start = 123;
    
    TEST_ASSERT(df1_address_to_string(&addr, buffer, sizeof(buffer)) == 0, "F8:123转字符串失败");
    TEST_ASSERT(strcmp(buffer, "F8:123") == 0, "F8:123转字符串结果错误");
    
    // 测试ST类型
    addr.data_code = DF1_ADDR_ST;
    addr.db_block = 1;
    addr.address_start = 5;
    
    TEST_ASSERT(df1_address_to_string(&addr, buffer, sizeof(buffer)) == 0, "ST1:5转字符串失败");
    TEST_ASSERT(strcmp(buffer, "ST1:5") == 0, "ST1:5转字符串结果错误");
    
    // 测试缓冲区太小
    TEST_ASSERT(df1_address_to_string(&addr, buffer, 5) != 0, "缓冲区太小应该失败");
    
    // 测试NULL指针
    TEST_ASSERT(df1_address_to_string(NULL, buffer, sizeof(buffer)) != 0, "NULL地址指针应该失败");
    TEST_ASSERT(df1_address_to_string(&addr, NULL, sizeof(buffer)) != 0, "NULL缓冲区指针应该失败");
    
    TEST_PASS("地址转字符串功能");
}

// 测试往返转换
int test_roundtrip_conversion() {
    printf("测试往返转换...\n");
    
    // 测试结构：{输入地址, 期望输出地址}
    struct {
        const char* input;
        const char* expected;
    } test_cases[] = {
        {"N7:0", "N7:0"},
        {"N7:123", "N7:123"},
        {"F8:0", "F8:0"},
        {"F8:456", "F8:456"},
        {"B3:0", "B3:0"},
        {"B3:789", "B3:789"},
        {"T4:0", "T4:0"},
        {"C5:0", "C5:0"},
        {"I:0", "I1:0"},      // I:0 会被转换为 I1:0（默认文件号1）
        {"I1:0", "I1:0"},
        {"O:0", "O0:0"},      // O:0 会被转换为 O0:0（默认文件号0）
        {"O1:0", "O1:0"},
        {"S:0", "S2:0"},      // S:0 会被转换为 S2:0（默认文件号2）
        {"S2:0", "S2:0"},
        {"ST:0", "ST1:0"},    // ST:0 会被转换为 ST1:0（默认文件号1）
        {"ST1:0", "ST1:0"},
        {"ST10:123", "ST10:123"},
        {"A9:0", "A9:0"},
        {"R6:0", "R6:0"},
        {"L9:0", "L9:0"}
    };
    
    size_t num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (size_t i = 0; i < num_tests; i++) {
        df1_address_t addr;
        char buffer[64];
        
        // 解析地址
        TEST_ASSERT(df1_address_parse(test_cases[i].input, &addr) == 0, 
                   "往返转换解析失败");
        
        // 转换回字符串
        TEST_ASSERT(df1_address_to_string(&addr, buffer, sizeof(buffer)) == 0,
                   "往返转换字符串化失败");
        
        // 验证结果
        if (strcmp(test_cases[i].expected, buffer) != 0) {
            printf("测试失败: 输入='%s', 期望='%s', 实际='%s'\n", 
                   test_cases[i].input, test_cases[i].expected, buffer);
            return 0;
        }
    }
    
    TEST_PASS("往返转换");
}

int main() {
    printf("AB DF1 地址解析单元测试\n");
    printf("========================\n\n");
    
    int passed = 0;
    int total = 0;
    
    total++; passed += test_address_parsing();
    total++; passed += test_invalid_addresses();
    total++; passed += test_address_to_string();
    total++; passed += test_roundtrip_conversion();
    
    printf("\n测试结果: %d/%d 通过\n", passed, total);
    
    if (passed == total) {
        printf("所有测试通过！\n");
        return 0;
    } else {
        printf("有测试失败！\n");
        return 1;
    }
}
