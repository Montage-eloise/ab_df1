#ifndef AB_DF1_SERIAL_H_
#define AB_DF1_SERIAL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "df1_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 串口配置结构体
 */
typedef struct {
    char port_name[64];        // 串口名称，如 "/dev/ttyUSB0"
    int baud_rate;             // 波特率
    int data_bits;             // 数据位
    int stop_bits;             // 停止位
    int parity;                // 校验位: 0=None, 1=Odd, 2=Even
    int timeout_ms;            // 超时时间（毫秒）
} df1_serial_config_t;

/**
 * @brief DF1串口通信结构体
 */
typedef struct {
    int fd;                    // 文件描述符
    df1_serial_config_t serial_config; // 串口配置
    df1_config_t df1_config;   // DF1协议配置
    bool is_open;              // 连接状态
} df1_serial_t;

/**
 * @brief 初始化串口配置为默认值
 * 
 * @param config 串口配置结构体指针
 */
void df1_serial_config_default(df1_serial_config_t* config);

/**
 * @brief 创建DF1串口通信实例
 * 
 * @return DF1串口通信实例指针，失败返回NULL
 */
df1_serial_t* df1_serial_create(void);

/**
 * @brief 销毁DF1串口通信实例
 * 
 * @param df1_serial DF1串口通信实例指针
 */
void df1_serial_destroy(df1_serial_t* df1_serial);

/**
 * @brief 打开串口连接
 * 
 * @param df1_serial DF1串口通信实例
 * @param serial_config 串口配置
 * @param df1_config DF1协议配置
 * @return 0 成功，-1 失败
 */
int df1_serial_open(df1_serial_t* df1_serial, const df1_serial_config_t* serial_config,
                   const df1_config_t* df1_config);

/**
 * @brief 关闭串口连接
 * 
 * @param df1_serial DF1串口通信实例
 * @return 0 成功，-1 失败
 */
int df1_serial_close(df1_serial_t* df1_serial);

/**
 * @brief 读取PLC数据
 * 
 * @param df1_serial DF1串口通信实例
 * @param address 地址字符串
 * @param data 输出数据缓冲区
 * @param data_size 数据缓冲区大小
 * @param actual_size 实际读取的数据大小
 * @return 0 成功，-1 失败
 */
int df1_serial_read(df1_serial_t* df1_serial, const char* address,
                   uint8_t* data, size_t data_size, size_t* actual_size);

/**
 * @brief 写入PLC数据
 * 
 * @param df1_serial DF1串口通信实例
 * @param address 地址字符串
 * @param data 写入数据
 * @param data_size 数据大小
 * @return 0 成功，-1 失败
 */
int df1_serial_write(df1_serial_t* df1_serial, const char* address,
                    const uint8_t* data, size_t data_size);

/**
 * @brief 读取16位整数
 * 
 * @param df1_serial DF1串口通信实例
 * @param address 地址字符串
 * @param value 输出值
 * @return 0 成功，-1 失败
 */
int df1_serial_read_int16(df1_serial_t* df1_serial, const char* address, int16_t* value);

/**
 * @brief 写入16位整数
 * 
 * @param df1_serial DF1串口通信实例
 * @param address 地址字符串
 * @param value 写入值
 * @return 0 成功，-1 失败
 */
int df1_serial_write_int16(df1_serial_t* df1_serial, const char* address, int16_t value);

/**
 * @brief 读取32位浮点数
 * 
 * @param df1_serial DF1串口通信实例
 * @param address 地址字符串
 * @param value 输出值
 * @return 0 成功，-1 失败
 */
int df1_serial_read_float(df1_serial_t* df1_serial, const char* address, float* value);

/**
 * @brief 写入32位浮点数
 * 
 * @param df1_serial DF1串口通信实例
 * @param address 地址字符串
 * @param value 写入值
 * @return 0 成功，-1 失败
 */
int df1_serial_write_float(df1_serial_t* df1_serial, const char* address, float value);

#ifdef __cplusplus
}
#endif

#endif // AB_DF1_SERIAL_H_
