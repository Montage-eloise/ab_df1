#include "df1_protocol.h"
#include "df1_address.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// CRC16 计算表
static const uint16_t crc16_table[256]
    = {0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1,
       0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40,
       0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1,
       0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
       0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1,
       0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40,
       0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1,
       0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
       0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
       0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
       0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0,
       0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
       0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1,
       0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140,
       0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0,
       0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
       0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0,
       0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
       0x4100, 0x81C1, 0x8081, 0x4040};

// 计算CRC16
static uint16_t calculate_crc16(const uint8_t* data, size_t length)
{
    uint16_t crc = 0x0000;
    for (size_t i = 0; i < length; i++)
    {
        crc = (crc >> 8) ^ crc16_table[(crc ^ data[i]) & 0xFF];
    }
    return crc;
}

// 计算BCC校验
static uint8_t calculate_bcc(uint8_t station, const uint8_t* data, size_t length)
{
    int sum = station;
    for (size_t i = 0; i < length; i++)
    {
        sum += data[i];
    }
    return (uint8_t)(~sum + 1);
}

// 添加长度到缓冲区
static size_t add_length_to_buffer(uint8_t* buffer, uint16_t value)
{
    if (value < 255)
    {
        buffer[0] = (uint8_t)value;
        return 1;
    }
    else
    {
        buffer[0] = 0xFF;
        buffer[1] = (uint8_t)(value & 0xFF);
        buffer[2] = (uint8_t)(value >> 8);
        return 3;
    }
}

void df1_config_init(df1_config_t* config, uint8_t station, uint8_t dst_node, uint8_t src_node)
{
    if (!config)
        return;

    config->station = station;
    config->dst_node = dst_node;
    config->src_node = src_node;
    config->check_type = DF1_CHECK_CRC16;
    config->transaction_id = 0;
}

int df1_build_read_command(const df1_config_t* config, const char* address, uint16_t length, uint8_t* buffer,
                           size_t buffer_size, size_t* actual_size)
{
    if (!config || !address || !buffer || !actual_size)
    {
        return -1;
    }

    // 解析地址
    df1_address_t addr;
    if (df1_address_parse(address, &addr) != 0)
    {
        return -1;
    }

    // 构建命令内容
    uint8_t cmd_buffer[256];
    size_t cmd_pos = 0;

    // 目标节点和源节点
    cmd_buffer[cmd_pos++] = config->dst_node;
    cmd_buffer[cmd_pos++] = config->src_node;

    // 命令头
    cmd_buffer[cmd_pos++] = 0x0F; // Command
    cmd_buffer[cmd_pos++] = 0x00; // Status

    // 事务ID
    cmd_buffer[cmd_pos++] = (uint8_t)(config->transaction_id & 0xFF);
    cmd_buffer[cmd_pos++] = (uint8_t)(config->transaction_id >> 8);

    // 读命令
    cmd_buffer[cmd_pos++] = DF1_CMD_READ;

    // 数据长度
    cmd_buffer[cmd_pos++] = (uint8_t)(length & 0xFF);

    // 文件号
    cmd_pos += add_length_to_buffer(&cmd_buffer[cmd_pos], addr.db_block);

    // 数据类型
    cmd_buffer[cmd_pos++] = (uint8_t)addr.data_code;

    // 起始地址
    cmd_pos += add_length_to_buffer(&cmd_buffer[cmd_pos], addr.address_start);

    // 子元素地址（通常为0）
    cmd_pos += add_length_to_buffer(&cmd_buffer[cmd_pos], 0);

    // 打包命令
    size_t packed_pos = 0;

    // DLE STX
    buffer[packed_pos++] = 0x10;
    buffer[packed_pos++] = 0x01;

    // 站号
    buffer[packed_pos++] = config->station;
    if (config->station == 0x10)
    {
        buffer[packed_pos++] = config->station; // DLE转义
    }

    // DLE STX
    buffer[packed_pos++] = 0x10;
    buffer[packed_pos++] = 0x02;

    // 命令数据（需要DLE转义）
    for (size_t i = 0; i < cmd_pos; i++)
    {
        buffer[packed_pos++] = cmd_buffer[i];
        if (cmd_buffer[i] == 0x10)
        {
            buffer[packed_pos++] = 0x10; // DLE转义
        }
    }

    // DLE ETX
    buffer[packed_pos++] = 0x10;
    buffer[packed_pos++] = 0x03;

    // 计算校验
    if (config->check_type == DF1_CHECK_BCC)
    {
        uint8_t bcc = calculate_bcc(config->station, cmd_buffer, cmd_pos);
        buffer[packed_pos++] = bcc;
    }
    else
    {
        // CRC16校验
        uint8_t crc_data[256];
        size_t crc_pos = 0;
        crc_data[crc_pos++] = config->station;
        crc_data[crc_pos++] = 0x02;
        memcpy(&crc_data[crc_pos], cmd_buffer, cmd_pos);
        crc_pos += cmd_pos;
        crc_data[crc_pos++] = 0x03;

        uint16_t crc = calculate_crc16(crc_data, crc_pos);
        buffer[packed_pos++] = (uint8_t)(crc >> 8);
        buffer[packed_pos++] = (uint8_t)(crc & 0xFF);
    }

    *actual_size = packed_pos;
    return (packed_pos <= buffer_size) ? 0 : -1;
}

int df1_build_write_command(const df1_config_t* config, const char* address, const uint8_t* data, uint16_t data_length,
                            uint8_t* buffer, size_t buffer_size, size_t* actual_size)
{
    if (!config || !address || !data || !buffer || !actual_size)
    {
        return -1;
    }

    // 解析地址
    df1_address_t addr;
    if (df1_address_parse(address, &addr) != 0)
    {
        return -1;
    }

    // 构建命令内容
    uint8_t cmd_buffer[512];
    size_t cmd_pos = 0;

    // 目标节点和源节点
    cmd_buffer[cmd_pos++] = config->dst_node;
    cmd_buffer[cmd_pos++] = config->src_node;

    // 命令头
    cmd_buffer[cmd_pos++] = 0x0F; // Command
    cmd_buffer[cmd_pos++] = 0x00; // Status

    // 事务ID
    cmd_buffer[cmd_pos++] = (uint8_t)(config->transaction_id & 0xFF);
    cmd_buffer[cmd_pos++] = (uint8_t)(config->transaction_id >> 8);

    // 写命令
    cmd_buffer[cmd_pos++] = DF1_CMD_WRITE;

    // 数据长度
    cmd_buffer[cmd_pos++] = (uint8_t)(data_length & 0xFF);

    // 文件号
    cmd_pos += add_length_to_buffer(&cmd_buffer[cmd_pos], addr.db_block);

    // 数据类型
    cmd_buffer[cmd_pos++] = (uint8_t)addr.data_code;

    // 起始地址
    cmd_pos += add_length_to_buffer(&cmd_buffer[cmd_pos], addr.address_start);

    // 子元素地址（通常为0）
    cmd_pos += add_length_to_buffer(&cmd_buffer[cmd_pos], 0);

    // 写入数据
    memcpy(&cmd_buffer[cmd_pos], data, data_length);
    cmd_pos += data_length;

    // 打包命令（类似读命令的打包过程）
    size_t packed_pos = 0;

    // DLE STX
    buffer[packed_pos++] = 0x10;
    buffer[packed_pos++] = 0x01;

    // 站号
    buffer[packed_pos++] = config->station;
    if (config->station == 0x10)
    {
        buffer[packed_pos++] = config->station;
    }

    // DLE STX
    buffer[packed_pos++] = 0x10;
    buffer[packed_pos++] = 0x02;

    // 命令数据（需要DLE转义）
    for (size_t i = 0; i < cmd_pos; i++)
    {
        buffer[packed_pos++] = cmd_buffer[i];
        if (cmd_buffer[i] == 0x10)
        {
            buffer[packed_pos++] = 0x10;
        }
    }

    // DLE ETX
    buffer[packed_pos++] = 0x10;
    buffer[packed_pos++] = 0x03;

    // 计算校验
    if (config->check_type == DF1_CHECK_BCC)
    {
        uint8_t bcc = calculate_bcc(config->station, cmd_buffer, cmd_pos);
        buffer[packed_pos++] = bcc;
    }
    else
    {
        uint8_t crc_data[512];
        size_t crc_pos = 0;
        crc_data[crc_pos++] = config->station;
        crc_data[crc_pos++] = 0x02;
        memcpy(&crc_data[crc_pos], cmd_buffer, cmd_pos);
        crc_pos += cmd_pos;
        crc_data[crc_pos++] = 0x03;

        uint16_t crc = calculate_crc16(crc_data, crc_pos);
        buffer[packed_pos++] = (uint8_t)(crc >> 8);
        buffer[packed_pos++] = (uint8_t)(crc & 0xFF);
    }

    *actual_size = packed_pos;
    return (packed_pos <= buffer_size) ? 0 : -1;
}

int df1_parse_response(const uint8_t* response, size_t response_size, uint8_t* data, size_t data_size,
                       size_t* actual_data_size)
{
    if (!response || !data || !actual_data_size)
    {
        return -1;
    }

    // 查找DLE STX (0x10 0x02)
    int data_start = -1;
    for (size_t i = 0; i < response_size - 1; i++)
    {
        if (response[i] == 0x10 && response[i + 1] == 0x02)
        {
            data_start = i + 2;
            break;
        }
    }

    if (data_start < 0 || data_start >= (int)response_size - 6)
    {
        return -1; // 没有找到有效的数据开始位置
    }

    // 提取数据部分（去除DLE转义）
    uint8_t temp_buffer[512];
    size_t temp_pos = 0;

    for (size_t i = data_start; i < response_size - 1; i++)
    {
        if (response[i] == 0x10)
        {
            if (response[i + 1] == 0x10)
            {
                // DLE转义
                temp_buffer[temp_pos++] = 0x10;
                i++; // 跳过下一个字节
            }
            else if (response[i + 1] == 0x03)
            {
                // DLE ETX，数据结束
                break;
            }
        }
        else
        {
            temp_buffer[temp_pos++] = response[i];
        }
    }

    if (temp_pos < 6)
    {
        return -1; // 数据太短
    }

    // 检查状态码
    if (temp_buffer[3] == 0xF0)
    {
        return -1; // 扩展错误状态
    }

    if (temp_buffer[3] != 0x00)
    {
        return -1; // 错误状态
    }

    // 提取实际数据
    if (temp_pos > 6)
    {
        size_t actual_len = temp_pos - 6;
        if (actual_len > data_size)
        {
            actual_len = data_size;
        }
        memcpy(data, &temp_buffer[6], actual_len);
        *actual_data_size = actual_len;
    }
    else
    {
        *actual_data_size = 0;
    }

    return 0;
}

const char* df1_get_error_description(uint8_t error_code)
{
    uint8_t lower = error_code & 0x0F;
    uint8_t upper = error_code & 0xF0;

    switch (lower)
    {
    case 1:
        return "DST node is out of buffer space";
    case 2:
        return "Cannot guarantee delivery: link layer";
    case 3:
        return "Duplicate token holder detected";
    case 4:
        return "Local port is disconnected";
    case 5:
        return "Application layer timed out waiting for a response";
    case 6:
        return "Duplicate node detected";
    case 7:
        return "Station is offline";
    case 8:
        return "Hardware fault";
    default:
        break;
    }

    switch (upper)
    {
    case 0x10:
        return "Illegal command or format";
    case 0x20:
        return "Host has a problem and will not communicate";
    case 0x30:
        return "Remote node host is missing, disconnected, or shut down";
    case 0x40:
        return "Host could not complete function due to hardware fault";
    case 0x50:
        return "Addressing problem or memory protect rungs";
    case 0x60:
        return "Function not allowed due to command protection selection";
    case 0x70:
        return "Processor is in Program mode";
    case 0x80:
        return "Compatibility mode file missing or communication zone problem";
    case 0x90:
        return "Remote node cannot buffer command";
    case 0xA0:
        return "Wait ACK (1775-KA buffer full)";
    case 0xB0:
        return "Remote node problem due to download";
    case 0xC0:
        return "Wait ACK (1775-KA buffer full)";
    case 0xF0:
        return "Error code in the EXT STS byte";
    default:
        return "Unknown error";
    }
}

const char* df1_get_ext_error_description(uint8_t ext_error_code)
{
    switch (ext_error_code)
    {
    case 1:
        return "A field has an illegal value";
    case 2:
        return "Less levels specified in address than minimum for any address";
    case 3:
        return "More levels specified in address than system supports";
    case 4:
        return "Symbol not found";
    case 5:
        return "Symbol is of improper format";
    case 6:
        return "Address doesn't point to something usable";
    case 7:
        return "File is wrong size";
    case 8:
        return "Cannot complete request, situation has changed since the start of the command";
    case 9:
        return "Data or file is too large";
    case 10:
        return "Transaction size plus word address is too large";
    case 11:
        return "Access denied, improper privilege";
    case 12:
        return "Condition cannot be generated - resource is not available";
    case 13:
        return "Condition already exists - resource is already available";
    case 14:
        return "Command cannot be executed";
    case 15:
        return "Histogram overflow";
    case 16:
        return "No access";
    case 17:
        return "Illegal data type";
    case 18:
        return "Invalid parameter or invalid data";
    case 19:
        return "Address reference exists to deleted area";
    case 20:
        return "Command execution failure for unknown reason";
    case 21:
        return "Data conversion error";
    case 22:
        return "Scanner not able to communicate with 1771 rack adapter";
    case 23:
        return "Type mismatch";
    case 24:
        return "1771 module response was not valid";
    case 25:
        return "Duplicated label";
    case 26:
        return "File is open; another node owns it";
    case 27:
        return "Another node is the program owner";
    case 30:
        return "Data table element protection violation";
    case 31:
        return "Temporary internal problem";
    case 34:
        return "Remote rack fault";
    case 35:
        return "Timeout";
    case 36:
        return "Unknown error";
    default:
        return "Unknown extended error";
    }
}
