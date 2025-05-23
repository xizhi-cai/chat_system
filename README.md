# Socket 聊天系统

一个基于 Socket 的多用户聊天系统，支持实时消息传递和用户状态管理。

## 功能特点

- 支持多用户同时在线聊天
- 实时消息广播
- 用户在线状态显示
- Web 监控界面
- 支持中文消息

## 系统要求

- Windows 操作系统
- MinGW-w64 编译器
- Make 工具

## 编译和运行

1. 编译项目：
```bash
mingw32-make
```

2. 运行服务器：
```bash
mingw32-make run-server
```

3. 运行客户端：
```bash
mingw32-make run-client
```

## 使用说明

1. 启动服务器后，可以通过 `http://localhost:8080` 访问 Web 监控界面
2. 客户端连接默认使用 `127.0.0.1:8888`
3. 在客户端输入昵称后即可开始聊天
4. 输入 `quit` 或 `exit` 退出程序
5. 输入 `help` 查看帮助信息

## 项目结构

```
chat_system/
├── include/         # 头文件
├── src/            # 源代码
├── bin/            # 编译输出目录
├── obj/            # 目标文件目录
├── web/            # Web 监控界面
├── Makefile        # 构建脚本
└── README.md       # 项目说明
```

## 许可证

MIT License 