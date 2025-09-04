#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "df1_serial.h"

int main(int argc, char* argv[])
{
    printf("AB DF1 简单写入示例\n");
    printf("====================\n");

    // 创建DF1串口通信实例
    df1_serial_t* df1_serial = df1_serial_create();
    if (!df1_serial)
    {
        printf("错误: 无法创建DF1串口通信实例\n");
        return -1;
    }

    // 配置串口
    df1_serial_config_t serial_config;
    df1_serial_config_default(&serial_config);

    // 可以根据需要修改串口配置
    if (argc > 1)
    {
        strcpy(serial_config.port_name, argv[1]);
    }
    printf("使用串口: %s\n", serial_config.port_name);

    // 配置DF1协议
    df1_config_t df1_config;
    df1_config_init(&df1_config, 1, 1, 0); // 站号=1, 目标节点=1, 源节点=0

    // 打开串口连接
    printf("\n正在连接PLC...\n");
    if (df1_serial_open(df1_serial, &serial_config, &df1_config) != 0)
    {
        printf("错误: 无法打开串口连接\n");
        df1_serial_destroy(df1_serial);
        return -1;
    }
    printf("连接成功！\n");

    // 写入示例
    printf("\n开始写入数据...\n");

    // 写入整数到N7:10
    int16_t write_value = 1234;
    if (df1_serial_write_int16(df1_serial, "N7:10", write_value) == 0)
    {
        printf("成功写入 N7:10 = %d\n", write_value);

        // 读回验证
        int16_t read_back;
        if (df1_serial_read_int16(df1_serial, "N7:10", &read_back) == 0)
        {
            printf("读回验证 N7:10 = %d\n", read_back);
        }
    }
    else
    {
        printf("错误: 写入N7:10失败\n");
    }

    // 写入浮点数到F8:10
    float write_float = 123.45f;
    if (df1_serial_write_float(df1_serial, "F8:10", write_float) == 0)
    {
        printf("成功写入 F8:10 = %.2f\n", write_float);

        // 读回验证
        float read_back_float;
        if (df1_serial_read_float(df1_serial, "F8:10", &read_back_float) == 0)
        {
            printf("读回验证 F8:10 = %.2f\n", read_back_float);
        }
    }
    else
    {
        printf("错误: 写入F8:10失败\n");
    }

    // 写入原始字节数据
    uint8_t raw_data[] = {0x12, 0x34, 0x56, 0x78};
    if (df1_serial_write(df1_serial, "N7:11", raw_data, sizeof(raw_data)) == 0)
    {
        printf("成功写入原始数据到 N7:11\n");

        // 读回验证
        uint8_t read_back_raw[4];
        size_t actual_size;
        if (df1_serial_read(df1_serial, "N7:11", read_back_raw, sizeof(read_back_raw), &actual_size) == 0)
        {
            printf("读回验证原始数据 (%zu 字节): ", actual_size);
            for (size_t i = 0; i < actual_size; i++)
            {
                printf("%02X ", read_back_raw[i]);
            }
            printf("\n");
        }
    }
    else
    {
        printf("错误: 写入原始数据失败\n");
    }

    // 关闭连接
    printf("\n正在断开连接...\n");
    df1_serial_close(df1_serial);
    df1_serial_destroy(df1_serial);

    printf("示例完成！\n");
    return 0;
}
