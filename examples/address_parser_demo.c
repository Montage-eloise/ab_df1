#include <stdio.h>
#include <string.h>
#include "df1_address.h"

void test_address_parsing(const char* address_str)
{
    printf("测试地址: %s\n", address_str);

    df1_address_t addr;
    if (df1_address_parse(address_str, &addr) == 0)
    {
        printf("  解析成功:\n");
        printf("    数据类型代码: 0x%02X\n", addr.data_code);
        printf("    文件号: %u\n", addr.db_block);
        printf("    起始地址: %u\n", addr.address_start);

        // 转换回字符串验证
        char buffer[64];
        if (df1_address_to_string(&addr, buffer, sizeof(buffer)) == 0)
        {
            printf("    转换回字符串: %s\n", buffer);
        }
        else
        {
            printf("    转换回字符串失败\n");
        }
    }
    else
    {
        printf("  解析失败\n");
    }
    printf("\n");
}

int main()
{
    printf("AB DF1 地址解析演示\n");
    printf("====================\n\n");

    printf("支持的地址类型和示例:\n");
    printf("A - ASCII 文件\n");
    printf("B - Binary 文件\n");
    printf("N - Integer 文件\n");
    printf("F - Float 文件\n");
    printf("ST - String 文件\n");
    printf("S - Status 文件\n");
    printf("C - Counter 文件\n");
    printf("I - Input 文件\n");
    printf("O - Output 文件\n");
    printf("R - Control 文件\n");
    printf("T - Timer 文件\n");
    printf("L - Long Integer 文件\n");
    printf("\n");

    // 测试各种地址格式
    test_address_parsing("N7:0");
    test_address_parsing("N7:1");
    test_address_parsing("F8:0");
    test_address_parsing("B3:0");
    test_address_parsing("T4:0");
    test_address_parsing("C5:0");
    test_address_parsing("I:0");
    test_address_parsing("O:0");
    test_address_parsing("S2:0");
    test_address_parsing("ST1:0");
    test_address_parsing("A9:0");
    test_address_parsing("R6:0");
    test_address_parsing("L9:0");

    // 测试错误格式
    printf("测试错误格式:\n");
    test_address_parsing("N7");   // 缺少冒号
    test_address_parsing("X7:0"); // 不支持的类型
    test_address_parsing("");     // 空字符串
    test_address_parsing("N:0");  // 缺少文件号

    return 0;
}
