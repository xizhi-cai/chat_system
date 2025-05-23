#include "../include/chat.h"

DWORD WINAPI web_server_thread(LPVOID param) {
    printf("启动Web服务器线程...\n");
    
    SOCKET web_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_socket == INVALID_SOCKET) {
        printf("创建Web服务器Socket失败: %d\n", WSAGetLastError());
        return 1;
    }
    
    struct sockaddr_in web_addr;
    memset(&web_addr, 0, sizeof(web_addr));
    web_addr.sin_family = AF_INET;
    web_addr.sin_addr.s_addr = INADDR_ANY;
    web_addr.sin_port = htons(WEB_PORT);
    
    if (bind(web_socket, (struct sockaddr*)&web_addr, sizeof(web_addr)) == SOCKET_ERROR) {
        printf("Web服务器绑定失败: %d\n", WSAGetLastError());
        printf("端口 %d 可能被占用\n", WEB_PORT);
        closesocket(web_socket);
        return 1;
    }
    
    if (listen(web_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Web服务器监听失败: %d\n", WSAGetLastError());
        closesocket(web_socket);
        return 1;
    }
    
    printf("✓ Web服务器启动成功，端口: %d\n", WEB_PORT);
    
    while (1) {
        SOCKET client_socket = accept(web_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            continue;
        }
        
        char buffer[4096];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            
            // 简单解析HTTP请求
            char* method = strtok(buffer, " ");
            char* path = strtok(NULL, " ");
            
            if (method && path) {
                printf("Web请求: %s %s\n", method, path);
            }
        }
        
        // 构造HTTP响应
        char response_template[] = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Connection: close\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html lang='zh-CN'>"
            "<head>"
            "<meta charset='UTF-8'>"
            "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
            "<title>Socket通信服务器状态</title>"
            "<style>"
            "body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }"
            ".container { max-width: 600px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
            ".header { text-align: center; color: #2c3e50; border-bottom: 2px solid #3498db; padding-bottom: 10px; }"
            ".status { margin: 20px 0; }"
            ".status-item { display: flex; justify-content: space-between; padding: 10px; margin: 5px 0; background: #ecf0f1; border-radius: 5px; }"
            ".status-value { font-weight: bold; color: #27ae60; }"
            ".users { margin-top: 20px; }"
            ".user-list { background: #f8f9fa; padding: 15px; border-radius: 5px; border-left: 4px solid #3498db; }"
            "</style>"
            "</head>"
            "<body>"
            "<div class='container'>"
            "<div class='header'>"
            "<h1>🚀 Socket通信服务器</h1>"
            "<p>实时状态监控</p>"
            "</div>"
            "<div class='status'>"
            "<div class='status-item'>"
            "<span>服务器状态:</span>"
            "<span class='status-value'>🟢 运行中</span>"
            "</div>"
            "<div class='status-item'>"
            "<span>聊天端口:</span>"
            "<span class='status-value'>%d</span>"
            "</div>"
            "<div class='status-item'>"
            "<span>Web端口:</span>"
            "<span class='status-value'>%d</span>"
            "</div>"
            "<div class='status-item'>"
            "<span>在线用户数:</span>"
            "<span class='status-value'>%d / %d</span>"
            "</div>"
            "<div class='status-item'>"
            "<span>启动时间:</span>"
            "<span class='status-value'>%s</span>"
            "</div>"
            "</div>"
            "<div class='users'>"
            "<h3>📋 使用说明</h3>"
            "<div class='user-list'>"
            "<p>1. 编译并运行客户端程序连接到服务器</p>"
            "<p>2. 聊天端口: %d</p>"
            "<p>3. 支持最多 %d 个用户同时在线</p>"
            "<p>4. 刷新页面查看最新状态</p>"
            "</div>"
            "</div>"
            "</div>"
            "<script>"
            "setTimeout(function(){ location.reload(); }, 30000);"  // 30秒自动刷新
            "</script>"
            "</body>"
            "</html>";
        
        char final_response[8192];
        WaitForSingleObject(client_mutex, INFINITE);
        int current_users = client_count;
        ReleaseMutex(client_mutex);
        
        snprintf(final_response, sizeof(final_response), response_template,
                CHAT_PORT, WEB_PORT, current_users, MAX_CLIENTS, 
                get_current_time(), CHAT_PORT, MAX_CLIENTS);
        
        send(client_socket, final_response, strlen(final_response), 0);
        closesocket(client_socket);
    }
    
    closesocket(web_socket);
    return 0;
}