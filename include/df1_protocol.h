#ifndef AB_DF1_PROTOCOL_H_
#define AB_DF1_PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "df1_address.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DF1协议校验类型
 */
typedef enum {
    DF1_CHECK_BCC = 0,    // BCC校验
    DF1_CHECK_CRC16 = 1   // CRC16校验
} df1_check_type_t;

/**
 * @brief DF1协议配置结构体
 */
typedef struct {
    uint8_t station;           // 站号
    uint8_t dst_node;          // 目标节点号
    uint8_t src_node;          // 源节点号
    df1_check_type_t check_type; // 校验类型
    uint16_t transaction_id;   // 事务ID计数器
} df1_config_t;

/**
 * @brief DF1命令类型
 */
typedef enum {
    DF1_CMD_READ = 0xA2,       // 读命令
    DF1_CMD_WRITE = 0xAA,      // 写命令
    DF1_CMD_MASK_WRITE = 0xAB  // 掩码写命令
} df1_command_t;

/**
 * @brief 初始化DF1配置
 * 
 * @param config 配置结构体指针
 * @param station 站号
 * @param dst_node 目标节点号
 * @param src_node 源节点号
 */
void df1_config_init(df1_config_t* config, uint8_t station, uint8_t dst_node, uint8_t src_node);

/**
 * @brief 构建DF1读取命令
 * 
 * @param config DF1配置
 * @param address 地址字符串
 * @param length 读取长度
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @param actual_size 实际生成的命令大小
 * @return 0 成功，-1 失败
 */
int df1_build_read_command(const df1_config_t* config, const char* address, 
                          uint16_t length, uint8_t* buffer, size_t buffer_size, 
                          size_t* actual_size);

/**
 * @brief 构建DF1写入命令
 * 
 * @param config DF1配置
 * @param address 地址字符串
 * @param data 写入数据
 * @param data_length 数据长度
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @param actual_size 实际生成的命令大小
 * @return 0 成功，-1 失败
 */
int df1_build_write_command(const df1_config_t* config, const char* address,
                           const uint8_t* data, uint16_t data_length,
                           uint8_t* buffer, size_t buffer_size, 
                           size_t* actual_size);

/**
 * @brief 解析DF1响应数据
 * 
 * @param response 响应数据
 * @param response_size 响应数据大小
 * @param data 输出数据缓冲区
 * @param data_size 数据缓冲区大小
 * @param actual_data_size 实际数据大小
 * @return 0 成功，-1 失败
 */
int df1_parse_response(const uint8_t* response, size_t response_size,
                      uint8_t* data, size_t data_size, size_t* actual_data_size);

/**
 * @brief 获取DF1错误描述
 * 
 * @param error_code 错误代码
 * @return 错误描述字符串
 */
const char* df1_get_error_description(uint8_t error_code);

/**
 * @brief 获取DF1扩展错误描述
 * 
 * @param ext_error_code 扩展错误代码
 * @return 扩展错误描述字符串
 */
const char* df1_get_ext_error_description(uint8_t ext_error_code);

#ifdef __cplusplus
}
#endif

#endif // AB_DF1_PROTOCOL_H_
