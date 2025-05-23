# 编译器和编译选项
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LDFLAGS = -lws2_32

# 目录定义
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# 创建目录的命令（Windows兼容）
MKDIR = @if not exist $(@D) mkdir $(@D)

# 服务器相关文件
SERVER_SOURCES = $(SRCDIR)/server.c $(SRCDIR)/client_handler.c $(SRCDIR)/utils.c $(SRCDIR)/web_server.c
SERVER_OBJECTS = $(SERVER_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
SERVER_TARGET = $(BINDIR)/chat_server.exe

# 客户端相关文件
CLIENT_SOURCES = $(SRCDIR)/test_client.c $(SRCDIR)/utils.c
CLIENT_OBJECTS = $(CLIENT_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
CLIENT_TARGET = $(BINDIR)/test_client.exe

# 默认目标：编译所有程序
all: $(SERVER_TARGET) $(CLIENT_TARGET)
	@echo.
	@echo =======================================
	@echo 编译完成！
	@echo 服务器程序: $(SERVER_TARGET)
	@echo 客户端程序: $(CLIENT_TARGET)
	@echo =======================================

# 编译服务器
$(SERVER_TARGET): $(SERVER_OBJECTS) | $(BINDIR)
	@echo 正在链接服务器程序...
	$(CC) $(SERVER_OBJECTS) -o $@ $(LDFLAGS)
	@echo ✓ 服务器编译成功: $@

# 编译客户端
$(CLIENT_TARGET): $(CLIENT_OBJECTS) | $(BINDIR)
	@echo 正在链接客户端程序...
	$(CC) $(CLIENT_OBJECTS) -o $@ $(LDFLAGS)
	@echo ✓ 客户端编译成功: $@

# 编译目标文件
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo 正在编译: $<
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# 创建目录
$(OBJDIR):
	$(MKDIR)

$(BINDIR):
	$(MKDIR)

# 清理编译生成的文件
clean:
	@echo 正在清理编译文件...
	@if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
	@if exist $(BINDIR) rmdir /s /q $(BINDIR)
	@echo ✓ 清理完成

# 运行服务器
run-server: $(SERVER_TARGET)
	@echo 启动聊天服务器...
	@echo =======================================
	$(SERVER_TARGET)

# 运行客户端
run-client: $(CLIENT_TARGET)
	@echo 启动聊天客户端...
	@echo =======================================
	$(CLIENT_TARGET)

# 默认运行服务器
run: run-server

# 显示帮助信息
help:
	@echo Socket通信软件编译帮助
	@echo =======================================
	@echo 可用命令:
	@echo   all         - 编译所有程序 (默认)
	@echo   clean       - 清理编译文件
	@echo   run-server  - 编译并运行服务器
	@echo   run-client  - 编译并运行客户端
	@echo   run         - 编译并运行服务器 (默认)
	@echo   help        - 显示此帮助信息
	@echo =======================================
	@echo 使用示例:
	@echo   mingw32-make              # 编译所有程序
	@echo   mingw32-make run-server   # 运行服务器
	@echo   mingw32-make run-client   # 运行客户端
	@echo   mingw32-make clean        # 清理文件

# 声明伪目标
.PHONY: all clean run run-server run-client help

# 删除隐式规则
.SUFFIXES: