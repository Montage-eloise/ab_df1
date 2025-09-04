# AB DF1 通信库

一个用于Allen-Bradley PLC DF1协议通信的C/C++开源库。支持通过串口与Micro-Logix1000、SLC500、SLC 5/03、SLC 5/04等PLC进行通信。

## 特性

- ✅ 支持DF1协议的读写操作
- ✅ 支持多种数据类型（N、F、B、T、C、I、O、S、ST、A、R、L）
- ✅ 支持BCC和CRC16两种校验方式
- ✅ 纯C实现，无第三方依赖
- ✅ 跨平台支持（Linux、Windows、macOS）
- ✅ 完整的示例和测试代码
- ✅ 符合开源库规范

## 支持的PLC型号

- Micro-Logix 1000
- SLC 500
- SLC 5/03
- SLC 5/04
- PLC-5

## 快速开始

### 编译和安装

#### 使用CMake（推荐）

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译
make

# 运行测试
make test

# 安装（可选）
sudo make install
```

#### 使用Makefile

```bash
# 编译所有目标
make

# 运行测试
make test

# 安装（可选）
sudo make install
```

### 基本使用示例

```c
#include "ab_df1/df1_serial.h"

int main() {
    // 创建DF1串口通信实例
    df1_serial_t* df1_serial = df1_serial_create();
    
    // 配置串口
    df1_serial_config_t serial_config;
    df1_serial_config_default(&serial_config);
    strcpy(serial_config.port_name, "/dev/ttyUSB0");
    
    // 配置DF1协议
    df1_config_t df1_config;
    df1_config_init(&df1_config, 1, 1, 0); // 站号=1, 目标节点=1, 源节点=0
    
    // 打开连接
    if (df1_serial_open(df1_serial, &serial_config, &df1_config) == 0) {
        // 读取N7:0寄存器
        int16_t value;
        if (df1_serial_read_int16(df1_serial, "N7:0", &value) == 0) {
            printf("N7:0 = %d\\n", value);
        }
        
        // 写入N7:1寄存器
        if (df1_serial_write_int16(df1_serial, "N7:1", 1234) == 0) {
            printf("写入成功\\n");
        }
    }
    
    // 关闭连接
    df1_serial_close(df1_serial);
    df1_serial_destroy(df1_serial);
    
    return 0;
}
```

## API 参考

### 地址格式

库支持以下地址格式：

| 类型 | 描述             | 示例          |
| ---- | ---------------- | ------------- |
| N    | Integer文件      | N7:0, N7:1    |
| F    | Float文件        | F8:0, F8:1    |
| B    | Binary文件       | B3:0, B3:1    |
| T    | Timer文件        | T4:0, T4:1    |
| C    | Counter文件      | C5:0, C5:1    |
| I    | Input文件        | I:0, I1:0     |
| O    | Output文件       | O:0, O1:0     |
| S    | Status文件       | S2:0, S2:1    |
| ST   | String文件       | ST1:0, ST10:0 |
| A    | ASCII文件        | A9:0, A9:1    |
| R    | Control文件      | R6:0, R6:1    |
| L    | Long Integer文件 | L9:0, L9:1    |

### 主要函数

#### 串口通信

```c
// 创建和销毁
df1_serial_t* df1_serial_create(void);
void df1_serial_destroy(df1_serial_t* df1_serial);

// 连接管理
int df1_serial_open(df1_serial_t* df1_serial, 
                   const df1_serial_config_t* serial_config,
                   const df1_config_t* df1_config);
int df1_serial_close(df1_serial_t* df1_serial);

// 数据读写
int df1_serial_read(df1_serial_t* df1_serial, const char* address,
                   uint8_t* data, size_t data_size, size_t* actual_size);
int df1_serial_write(df1_serial_t* df1_serial, const char* address,
                    const uint8_t* data, size_t data_size);

// 类型化读写
int df1_serial_read_int16(df1_serial_t* df1_serial, const char* address, int16_t* value);
int df1_serial_write_int16(df1_serial_t* df1_serial, const char* address, int16_t value);
int df1_serial_read_float(df1_serial_t* df1_serial, const char* address, float* value);
int df1_serial_write_float(df1_serial_t* df1_serial, const char* address, float value);
```

#### 地址解析

```c
int df1_address_parse(const char* address_str, df1_address_t* addr);
int df1_address_to_string(const df1_address_t* addr, char* buffer, size_t buffer_size);
```

#### 协议命令构建

```c
int df1_build_read_command(const df1_config_t* config, const char* address, 
                          uint16_t length, uint8_t* buffer, size_t buffer_size, 
                          size_t* actual_size);
int df1_build_write_command(const df1_config_t* config, const char* address,
                           const uint8_t* data, uint16_t data_length,
                           uint8_t* buffer, size_t buffer_size, 
                           size_t* actual_size);
```

## 示例程序

### 简单读取示例

```bash
./build/simple_read [串口设备]
```

### 简单写入示例

```bash
./build/simple_write [串口设备]
```

### 地址解析演示

```bash
./build/address_parser_demo
```

## 测试

项目包含完整的单元测试：

```bash
# 使用CMake
cd build && make test

# 使用Makefile
make test

# 手动运行
./build/test_address
./build/test_protocol
```

## 配置选项

### 串口配置

```c
typedef struct {
    char port_name[64];        // 串口名称，如 "/dev/ttyUSB0"
    int baud_rate;             // 波特率（9600, 19200, 38400, 57600, 115200）
    int data_bits;             // 数据位（7, 8）
    int stop_bits;             // 停止位（1, 2）
    int parity;                // 校验位: 0=None, 1=Odd, 2=Even
    int timeout_ms;            // 超时时间（毫秒）
} df1_serial_config_t;
```

### DF1协议配置

```c
typedef struct {
    uint8_t station;           // 站号
    uint8_t dst_node;          // 目标节点号
    uint8_t src_node;          // 源节点号
    df1_check_type_t check_type; // 校验类型（BCC或CRC16）
    uint16_t transaction_id;   // 事务ID计数器
} df1_config_t;
```

## 错误处理

库中的所有函数都返回状态码：
- `0`: 成功
- `-1`: 失败

可以使用以下函数获取错误描述：

```c
const char* df1_get_error_description(uint8_t error_code);
const char* df1_get_ext_error_description(uint8_t ext_error_code);
```

## 限制和注意事项

1. **数据长度限制**：
   - SLC 5/01或SLC 5/02：最多82字节
   - SLC 5/03或SLC 5/04：最多225-236字节

2. **通信机制**：半双工通信，每次只能进行一个操作

3. **平台兼容性**：
   - Linux：完全支持
   - Windows：需要适配串口API
   - macOS：基本支持

4. **线程安全**：当前版本不是线程安全的，多线程使用需要外部同步

## 贡献

欢迎贡献代码！请遵循以下步骤：

1. Fork本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开Pull Request

## 编码规范

- 遵循Google C++风格指南
- 使用C99标准
- 函数和变量使用小写字母和下划线
- 类型名使用小写字母和下划线，以`_t`结尾

## 许可证

本项目采用MIT许可证 - 详见 [LICENSE](LICENSE) 文件
