#include "../include/chat.h"

// 全局变量定义
Client clients[MAX_CLIENTS];
int client_count = 0;
HANDLE client_mutex;

int main() {
    // 设置控制台代码页为 UTF-8
    SetConsoleOutputCP(65001);
    
    printf("=== Socket通信服务器启动 ===\n");
    printf("版本: 1.0\n");
    printf("支持最多 %d 个客户端同时在线\n\n", MAX_CLIENTS);
    
    // 初始化Winsock
    init_winsock();
    printf("✓ Winsock初始化成功\n");
    
    // 创建互斥锁保护全局数据
    client_mutex = CreateMutex(NULL, FALSE, NULL);
    if (client_mutex == NULL) {
        printf("创建互斥锁失败\n");
        cleanup_winsock();
        return 1;
    }
    printf("✓ 互斥锁创建成功\n");
    
    // 创建服务器Socket
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("创建Socket失败: %d\n", WSAGetLastError());
        CloseHandle(client_mutex);
        cleanup_winsock();
        return 1;
    }
    printf("✓ 服务器Socket创建成功\n");
    
    // 配置服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(CHAT_PORT);
    
    // 绑定Socket到地址
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("绑定Socket失败: %d\n", WSAGetLastError());
        printf("提示: 端口 %d 可能被其他程序占用\n", CHAT_PORT);
        closesocket(server_socket);
        CloseHandle(client_mutex);
        cleanup_winsock();
        return 1;
    }
    printf("✓ Socket绑定到端口 %d 成功\n", CHAT_PORT);
    
    // 开始监听连接
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("监听失败: %d\n", WSAGetLastError());
        closesocket(server_socket);
        CloseHandle(client_mutex);
        cleanup_winsock();
        return 1;
    }
    printf("✓ 开始监听客户端连接...\n");
    
    // 启动Web服务器线程
    HANDLE web_thread = CreateThread(NULL, 0, web_server_thread, NULL, 0, NULL);
    if (web_thread == NULL) {
        printf("创建Web服务器线程失败\n");
    } else {
        printf("✓ Web服务器线程启动成功\n");
    }
    
    printf("\n=== 服务器就绪 ===\n");
    printf("聊天服务器: 端口 %d\n", CHAT_PORT);
    printf("Web监控界面: http://localhost:%d\n", WEB_PORT);
    printf("等待客户端连接...\n\n");
    
    // 主循环 - 接受客户端连接
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket == INVALID_SOCKET) {
            printf("接受连接失败: %d\n", WSAGetLastError());
            continue;
        }
        
        char* client_ip = inet_ntoa(client_addr.sin_addr);
        printf("新客户端连接来自: %s\n", client_ip);
        
        // 为每个客户端创建独立的处理线程
        HANDLE client_thread = CreateThread(NULL, 0, handle_client, (LPVOID)client_socket, 0, NULL);
        if (client_thread == NULL) {
            printf("创建客户端处理线程失败\n");
            closesocket(client_socket);
        } else {
            CloseHandle(client_thread);  // 分离线程，让其独立运行
        }
    }
    
    // 清理资源
    closesocket(server_socket);
    if (web_thread != NULL) {
        CloseHandle(web_thread);
    }
    CloseHandle(client_mutex);
    cleanup_winsock();
    return 0;
}