# 更新日志

本文档记录了AB DF1库的所有重要更改。

格式基于[Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
并且本项目遵循[语义化版本](https://semver.org/lang/zh-CN/)。

## [未发布]

### 计划添加
- Windows平台串口支持
- 更多数据类型支持
- 异步通信接口
- 连接池管理

## [1.0.0] - 2024-01-XX

### 新增
- 完整的DF1协议实现
- 支持串口通信
- 地址解析功能
- 读写操作支持
- 多种数据类型支持（N、F、B、T、C、I、O、S、ST、A、R、L）
- BCC和CRC16校验支持
- CMake构建系统
- Makefile构建支持
- 完整的示例程序
- 单元测试套件
- 详细的文档

### 支持的平台
- Linux (完全支持)
- macOS (基本支持)

### 支持的PLC型号
- Micro-Logix 1000
- SLC 500
- SLC 5/03
- SLC 5/04
- PLC-5

### 已知限制
- 不支持Windows平台
- 非线程安全
- 半双工通信限制
