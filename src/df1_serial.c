#include "df1_serial.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <errno.h>

void df1_serial_config_default(df1_serial_config_t* config)
{
    if (!config)
        return;

    strcpy(config->port_name, "/dev/ttyUSB0");
    config->baud_rate = 19200;
    config->data_bits = 8;
    config->stop_bits = 1;
    config->parity = 0; // None
    config->timeout_ms = 1000;
}

df1_serial_t* df1_serial_create(void)
{
    df1_serial_t* df1_serial = (df1_serial_t*)malloc(sizeof(df1_serial_t));
    if (!df1_serial)
    {
        return NULL;
    }

    memset(df1_serial, 0, sizeof(df1_serial_t));
    df1_serial->fd = -1;
    df1_serial->is_open = false;

    return df1_serial;
}

void df1_serial_destroy(df1_serial_t* df1_serial)
{
    if (!df1_serial)
        return;

    if (df1_serial->is_open)
    {
        df1_serial_close(df1_serial);
    }

    free(df1_serial);
}

static int configure_serial_port(int fd, const df1_serial_config_t* config)
{
    struct termios options;

    if (tcgetattr(fd, &options) != 0)
    {
        return -1;
    }

    // 设置波特率
    speed_t baud;
    switch (config->baud_rate)
    {
    case 9600:
        baud = B9600;
        break;
    case 19200:
        baud = B19200;
        break;
    case 38400:
        baud = B38400;
        break;
    case 57600:
        baud = B57600;
        break;
    case 115200:
        baud = B115200;
        break;
    default:
        return -1;
    }

    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    // 设置数据位
    options.c_cflag &= ~CSIZE;
    switch (config->data_bits)
    {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        return -1;
    }

    // 设置停止位
    if (config->stop_bits == 1)
    {
        options.c_cflag &= ~CSTOPB;
    }
    else if (config->stop_bits == 2)
    {
        options.c_cflag |= CSTOPB;
    }
    else
    {
        return -1;
    }

    // 设置校验位
    switch (config->parity)
    {
    case 0: // None
        options.c_cflag &= ~PARENB;
        break;
    case 1: // Odd
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
        break;
    case 2: // Even
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        break;
    default:
        return -1;
    }

    // 其他设置
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    // 设置超时
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = config->timeout_ms / 100; // 以0.1秒为单位

    return tcsetattr(fd, TCSANOW, &options);
}

int df1_serial_open(df1_serial_t* df1_serial, const df1_serial_config_t* serial_config, const df1_config_t* df1_config)
{
    if (!df1_serial || !serial_config || !df1_config)
    {
        return -1;
    }

    if (df1_serial->is_open)
    {
        return -1; // 已经打开
    }

    // 打开串口
    df1_serial->fd = open(serial_config->port_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (df1_serial->fd < 0)
    {
        return -1;
    }

    // 配置串口
    if (configure_serial_port(df1_serial->fd, serial_config) != 0)
    {
        close(df1_serial->fd);
        df1_serial->fd = -1;
        return -1;
    }

    // 保存配置
    df1_serial->serial_config = *serial_config;
    df1_serial->df1_config = *df1_config;
    df1_serial->is_open = true;

    return 0;
}

int df1_serial_close(df1_serial_t* df1_serial)
{
    if (!df1_serial || !df1_serial->is_open)
    {
        return -1;
    }

    if (df1_serial->fd >= 0)
    {
        close(df1_serial->fd);
        df1_serial->fd = -1;
    }

    df1_serial->is_open = false;
    return 0;
}

static int send_and_receive(df1_serial_t* df1_serial, const uint8_t* send_data, size_t send_size, uint8_t* recv_data,
                            size_t recv_size, size_t* actual_recv_size)
{
    if (!df1_serial->is_open)
    {
        return -1;
    }

    // 发送数据
    ssize_t written = write(df1_serial->fd, send_data, send_size);
    if (written != (ssize_t)send_size)
    {
        return -1;
    }

    // 等待响应
    fd_set read_fds;
    struct timeval timeout;

    FD_ZERO(&read_fds);
    FD_SET(df1_serial->fd, &read_fds);

    timeout.tv_sec = df1_serial->serial_config.timeout_ms / 1000;
    timeout.tv_usec = (df1_serial->serial_config.timeout_ms % 1000) * 1000;

    int result = select(df1_serial->fd + 1, &read_fds, NULL, NULL, &timeout);
    if (result <= 0)
    {
        return -1; // 超时或错误
    }

    // 接收数据
    ssize_t received = read(df1_serial->fd, recv_data, recv_size);
    if (received < 0)
    {
        return -1;
    }

    *actual_recv_size = (size_t)received;
    return 0;
}

int df1_serial_read(df1_serial_t* df1_serial, const char* address, uint8_t* data, size_t data_size, size_t* actual_size)
{
    if (!df1_serial || !address || !data || !actual_size)
    {
        return -1;
    }

    // 构建读取命令
    uint8_t command[512];
    size_t command_size;

    // 增加事务ID
    df1_serial->df1_config.transaction_id++;

    int result = df1_build_read_command(&df1_serial->df1_config, address, (uint16_t)data_size, command, sizeof(command),
                                        &command_size);
    if (result != 0)
    {
        return -1;
    }

    // 发送命令并接收响应
    uint8_t response[512];
    size_t response_size;

    result = send_and_receive(df1_serial, command, command_size, response, sizeof(response), &response_size);
    if (result != 0)
    {
        return -1;
    }

    // 解析响应
    return df1_parse_response(response, response_size, data, data_size, actual_size);
}

int df1_serial_write(df1_serial_t* df1_serial, const char* address, const uint8_t* data, size_t data_size)
{
    if (!df1_serial || !address || !data)
    {
        return -1;
    }

    // 构建写入命令
    uint8_t command[512];
    size_t command_size;

    // 增加事务ID
    df1_serial->df1_config.transaction_id++;

    int result = df1_build_write_command(&df1_serial->df1_config, address, data, (uint16_t)data_size, command,
                                         sizeof(command), &command_size);
    if (result != 0)
    {
        return -1;
    }

    // 发送命令并接收响应
    uint8_t response[512];
    size_t response_size;

    result = send_and_receive(df1_serial, command, command_size, response, sizeof(response), &response_size);
    if (result != 0)
    {
        return -1;
    }

    // 解析响应（写入命令通常只返回状态）
    uint8_t dummy_data[1];
    size_t dummy_size;
    return df1_parse_response(response, response_size, dummy_data, sizeof(dummy_data), &dummy_size);
}

int df1_serial_read_int16(df1_serial_t* df1_serial, const char* address, int16_t* value)
{
    if (!df1_serial || !address || !value)
    {
        return -1;
    }

    uint8_t data[2];
    size_t actual_size;

    int result = df1_serial_read(df1_serial, address, data, sizeof(data), &actual_size);
    if (result != 0 || actual_size != 2)
    {
        return -1;
    }

    // AB PLC使用小端序
    *value = (int16_t)(data[0] | (data[1] << 8));
    return 0;
}

int df1_serial_write_int16(df1_serial_t* df1_serial, const char* address, int16_t value)
{
    if (!df1_serial || !address)
    {
        return -1;
    }

    uint8_t data[2];
    data[0] = (uint8_t)(value & 0xFF);
    data[1] = (uint8_t)(value >> 8);

    return df1_serial_write(df1_serial, address, data, sizeof(data));
}

int df1_serial_read_float(df1_serial_t* df1_serial, const char* address, float* value)
{
    if (!df1_serial || !address || !value)
    {
        return -1;
    }

    uint8_t data[4];
    size_t actual_size;

    int result = df1_serial_read(df1_serial, address, data, sizeof(data), &actual_size);
    if (result != 0 || actual_size != 4)
    {
        return -1;
    }

    // AB PLC使用小端序
    union {
        float f;
        uint8_t bytes[4];
    } converter;

    converter.bytes[0] = data[0];
    converter.bytes[1] = data[1];
    converter.bytes[2] = data[2];
    converter.bytes[3] = data[3];

    *value = converter.f;
    return 0;
}

int df1_serial_write_float(df1_serial_t* df1_serial, const char* address, float value)
{
    if (!df1_serial || !address)
    {
        return -1;
    }

    union {
        float f;
        uint8_t bytes[4];
    } converter;

    converter.f = value;

    return df1_serial_write(df1_serial, address, converter.bytes, sizeof(converter.bytes));
}
