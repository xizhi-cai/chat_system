#ifndef CHAT_H
#define CHAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <time.h>
#include <stdarg.h>

// MSVC特定的pragma，MinGW会忽略警告
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

#define MAX_CLIENTS 50
#define BUFFER_SIZE 1024
#define NAME_SIZE 32
#define CHAT_PORT 8888
#define WEB_PORT 8080

// 消息类型枚举
typedef enum {
    MSG_LOGIN,      // 用户登录
    MSG_CHAT,       // 聊天消息
    MSG_LOGOUT,     // 用户登出
    MSG_USER_LIST   // 用户列表更新
} MessageType;

// 消息结构体
typedef struct {
    MessageType type;
    char sender[NAME_SIZE];
    char content[BUFFER_SIZE];
    char timestamp[32];
} Message;

// 客户端信息结构体
typedef struct {
    SOCKET socket;
    char name[NAME_SIZE];
    char ip[16];
    int active;
} Client;

// 全局变量声明
extern Client clients[MAX_CLIENTS];
extern int client_count;
extern HANDLE client_mutex;

// 函数声明
void init_winsock();
void cleanup_winsock();
DWORD WINAPI handle_client(LPVOID client_socket);
DWORD WINAPI web_server_thread(LPVOID param);
DWORD WINAPI receive_messages(LPVOID client_socket);
void broadcast_message(Message* msg, SOCKET sender);
void send_user_list();
char* get_current_time();
void log_message(const char* format, ...);

#endif