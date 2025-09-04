#include "df1_address.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int df1_address_parse(const char* address_str, df1_address_t* addr)
{
    if (!address_str || !addr)
    {
        return -1;
    }

    // 查找冒号分隔符
    const char* colon = strchr(address_str, ':');
    if (!colon)
    {
        return -1; // 地址格式错误，必须包含':'
    }

    // 解析地址类型和文件号
    size_t prefix_len = colon - address_str;
    char prefix[16] = {0};
    if (prefix_len >= sizeof(prefix))
    {
        return -1; // 前缀太长
    }

    strncpy(prefix, address_str, prefix_len);

    // 解析地址类型
    char addr_type = toupper(prefix[0]);
    switch (addr_type)
    {
    case 'A':
        addr->data_code = DF1_ADDR_A;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    case 'B':
        addr->data_code = DF1_ADDR_B;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    case 'N':
        addr->data_code = DF1_ADDR_N;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    case 'F':
        addr->data_code = DF1_ADDR_F;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    case 'S':
        if (prefix_len > 1 && toupper(prefix[1]) == 'T')
        {
            // ST 类型
            addr->data_code = DF1_ADDR_ST;
            addr->db_block = (prefix_len == 2) ? 1 : (uint16_t)atoi(prefix + 2);
        }
        else
        {
            // S 类型
            addr->data_code = DF1_ADDR_S;
            addr->db_block = (prefix_len == 1) ? 2 : (uint16_t)atoi(prefix + 1);
        }
        break;
    case 'C':
        addr->data_code = DF1_ADDR_C;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    case 'I':
        addr->data_code = DF1_ADDR_I;
        addr->db_block = (prefix_len == 1) ? 1 : (uint16_t)atoi(prefix + 1);
        break;
    case 'O':
        addr->data_code = DF1_ADDR_O;
        addr->db_block = (prefix_len == 1) ? 0 : (uint16_t)atoi(prefix + 1);
        break;
    case 'R':
        addr->data_code = DF1_ADDR_R;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    case 'T':
        addr->data_code = DF1_ADDR_T;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    case 'L':
        addr->data_code = DF1_ADDR_L;
        addr->db_block = (uint16_t)atoi(prefix + 1);
        break;
    default:
        return -1; // 不支持的地址类型
    }

    // 解析起始地址
    addr->address_start = (uint16_t)atoi(colon + 1);
    addr->length = 0; // 默认长度为0，由调用者设置

    return 0;
}

int df1_address_to_string(const df1_address_t* addr, char* buffer, size_t buffer_size)
{
    if (!addr || !buffer || buffer_size == 0)
    {
        return -1;
    }

    const char* type_str = "";
    switch (addr->data_code)
    {
    case DF1_ADDR_A:
        type_str = "A";
        break;
    case DF1_ADDR_B:
        type_str = "B";
        break;
    case DF1_ADDR_N:
        type_str = "N";
        break;
    case DF1_ADDR_F:
        type_str = "F";
        break;
    case DF1_ADDR_ST:
        type_str = "ST";
        break;
    case DF1_ADDR_S:
        type_str = "S";
        break;
    case DF1_ADDR_C:
        type_str = "C";
        break;
    case DF1_ADDR_I:
        type_str = "I";
        break;
    case DF1_ADDR_O:
        type_str = "O";
        break;
    case DF1_ADDR_R:
        type_str = "R";
        break;
    case DF1_ADDR_T:
        type_str = "T";
        break;
    case DF1_ADDR_L:
        type_str = "L";
        break;
    default:
        return -1;
    }

    int result = snprintf(buffer, buffer_size, "%s%u:%u", type_str, addr->db_block, addr->address_start);

    return (result >= 0 && result < (int)buffer_size) ? 0 : -1;
}
