#ifndef AB_DF1_ADDRESS_H_
#define AB_DF1_ADDRESS_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AB DF1 地址类型枚举
 */
typedef enum {
    DF1_ADDR_A = 0x8E,   // A file (ASCII)
    DF1_ADDR_B = 0x85,   // B file (Binary)
    DF1_ADDR_N = 0x89,   // N file (Integer)
    DF1_ADDR_F = 0x8A,   // F file (Float)
    DF1_ADDR_ST = 0x8D,  // ST file (String)
    DF1_ADDR_S = 0x84,   // S file (Status)
    DF1_ADDR_C = 0x87,   // C file (Counter)
    DF1_ADDR_I = 0x83,   // I file (Input)
    DF1_ADDR_O = 0x82,   // O file (Output)
    DF1_ADDR_R = 0x88,   // R file (Control)
    DF1_ADDR_T = 0x86,   // T file (Timer)
    DF1_ADDR_L = 0x91    // L file (Long Integer)
} df1_addr_type_t;

/**
 * @brief DF1 地址结构体
 */
typedef struct {
    df1_addr_type_t data_code;    // 数据类型代码
    uint16_t db_block;            // 文件号
    uint16_t address_start;       // 起始地址
    uint16_t length;              // 数据长度
} df1_address_t;

/**
 * @brief 解析DF1地址字符串
 * 
 * @param address_str 地址字符串，如 "N7:1"
 * @param addr 输出的地址结构体
 * @return 0 成功，-1 失败
 */
int df1_address_parse(const char* address_str, df1_address_t* addr);

/**
 * @brief 将地址结构体转换为字符串
 * 
 * @param addr 地址结构体
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @return 0 成功，-1 失败
 */
int df1_address_to_string(const df1_address_t* addr, char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // AB_DF1_ADDRESS_H_
