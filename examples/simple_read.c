#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "df1_serial.h"

int main(int argc, char* argv[])
{
    printf("AB DF1 简单读取示例\n");
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
    printf("波特率: %d\n", serial_config.baud_rate);

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

    // 读取示例
    printf("\n开始读取数据...\n");

    // 读取N7:0寄存器的值
    int16_t n7_0_value;
    if (df1_serial_read_int16(df1_serial, "N7:0", &n7_0_value) == 0)
    {
        printf("N7:0 = %d\n", n7_0_value);
    }
    else
    {
        printf("错误: 读取N7:0失败\n");
    }

    // 读取N7:1寄存器的值
    int16_t n7_1_value;
    if (df1_serial_read_int16(df1_serial, "N7:1", &n7_1_value) == 0)
    {
        printf("N7:1 = %d\n", n7_1_value);
    }
    else
    {
        printf("错误: 读取N7:1失败\n");
    }

    // 读取F8:0浮点数
    float f8_0_value;
    if (df1_serial_read_float(df1_serial, "F8:0", &f8_0_value) == 0)
    {
        printf("F8:0 = %.2f\n", f8_0_value);
    }
    else
    {
        printf("错误: 读取F8:0失败\n");
    }

    // 读取原始字节数据
    uint8_t raw_data[10];
    size_t actual_size;
    if (df1_serial_read(df1_serial, "N7:2", raw_data, sizeof(raw_data), &actual_size) == 0)
    {
        printf("N7:2 原始数据 (%zu 字节): ", actual_size);
        for (size_t i = 0; i < actual_size; i++)
        {
            printf("%02X ", raw_data[i]);
        }
        printf("\n");
    }
    else
    {
        printf("错误: 读取N7:2原始数据失败\n");
    }

    // 关闭连接
    printf("\n正在断开连接...\n");
    df1_serial_close(df1_serial);
    df1_serial_destroy(df1_serial);

    printf("示例完成！\n");
    return 0;
}
