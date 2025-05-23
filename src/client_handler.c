#include "../include/chat.h"

DWORD WINAPI handle_client(LPVOID client_socket) {
    SOCKET sock = (SOCKET)client_socket;
    Message msg;
    int client_index = -1;
    char client_name[NAME_SIZE] = {0};
    
    printf("客户端处理线程启动，Socket: %llu\n", (unsigned long long)sock);
    
    // 接收客户端登录信息
    int bytes_received = recv(sock, (char*)&msg, sizeof(Message), 0);
    if (bytes_received <= 0) {
        printf("接收登录信息失败，连接断开\n");
        closesocket(sock);
        return 0;
    }
    
    // 处理登录消息
    if (msg.type == MSG_LOGIN) {
        WaitForSingleObject(client_mutex, INFINITE);
        
        // 检查是否还有空位
        if (client_count >= MAX_CLIENTS) {
            printf("服务器已满，拒绝新连接: %s\n", msg.sender);
            ReleaseMutex(client_mutex);
            
            // 发送服务器已满消息
            Message full_msg;
            full_msg.type = MSG_CHAT;
            strncpy(full_msg.sender, "系统", NAME_SIZE - 1);
            strncpy(full_msg.content, "服务器已满，请稍后再试", BUFFER_SIZE - 1);
            send(sock, (char*)&full_msg, sizeof(Message), 0);
            
            closesocket(sock);
            return 0;
        }
        
        // 添加新客户端到列表
        client_index = client_count;
        clients[client_index].socket = sock;
        strncpy(clients[client_index].name, msg.sender, NAME_SIZE - 1);
        clients[client_index].name[NAME_SIZE - 1] = '\0';
        clients[client_index].active = 1;
        strncpy(client_name, msg.sender, NAME_SIZE - 1);
        client_count++;
        
        printf("✓ 用户 [%s] 已登录 (总用户数: %d/%d)\n", msg.sender, client_count, MAX_CLIENTS);
        
        // 广播用户登录消息
        Message login_msg;
        login_msg.type = MSG_CHAT;
        strncpy(login_msg.sender, "系统", NAME_SIZE - 1);
        login_msg.sender[NAME_SIZE - 1] = '\0';
        snprintf(login_msg.content, BUFFER_SIZE, "🎉 %s 加入了聊天室", msg.sender);
        strncpy(login_msg.timestamp, get_current_time(), 31);
        login_msg.timestamp[31] = '\0';
        
        ReleaseMutex(client_mutex);
        
        broadcast_message(&login_msg, INVALID_SOCKET);
        send_user_list();
    } else {
        printf("收到非登录消息，断开连接\n");
        closesocket(sock);
        return 0;
    }
    
    // 主消息处理循环
    while (1) {
        bytes_received = recv(sock, (char*)&msg, sizeof(Message), 0);
        if (bytes_received <= 0) {
            printf("客户端 [%s] 连接断开\n", client_name);
            break;
        }
        
        // 添加时间戳
        strncpy(msg.timestamp, get_current_time(), 31);
        msg.timestamp[31] = '\0';
        
        switch (msg.type) {
            case MSG_CHAT:
                printf("[%s] %s: %s\n", msg.timestamp, msg.sender, msg.content);
                broadcast_message(&msg, sock);
                break;
                
            case MSG_LOGOUT:
                printf("用户 [%s] 请求登出\n", msg.sender);
                goto cleanup;
                
            default:
                printf("收到未知消息类型: %d\n", msg.type);
                break;
        }
    }
    
cleanup:
    // 清理客户端资源
    WaitForSingleObject(client_mutex, INFINITE);
    
    if (client_index >= 0 && client_index < client_count) {
        printf("清理用户 [%s] 的资源\n", clients[client_index].name);
        
        // 广播用户离线消息
        Message logout_msg;
        logout_msg.type = MSG_CHAT;
        strncpy(logout_msg.sender, "系统", NAME_SIZE - 1);
        logout_msg.sender[NAME_SIZE - 1] = '\0';
        snprintf(logout_msg.content, BUFFER_SIZE, "👋 %s 离开了聊天室", clients[client_index].name);
        strncpy(logout_msg.timestamp, get_current_time(), 31);
        logout_msg.timestamp[31] = '\0';
        
        // 从客户端列表中移除
        for (int i = client_index; i < client_count - 1; i++) {
            clients[i] = clients[i + 1];
        }
        client_count--;
        
        printf("✓ 用户已移除，当前在线用户数: %d\n", client_count);
        
        ReleaseMutex(client_mutex);
        
        broadcast_message(&logout_msg, INVALID_SOCKET);
        send_user_list();
    } else {
        ReleaseMutex(client_mutex);
    }
    
    closesocket(sock);
    printf("客户端处理线程结束\n");
    return 0;
}

void broadcast_message(Message* msg, SOCKET sender) {
    WaitForSingleObject(client_mutex, INFINITE);
    
    int sent_count = 0;
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket != sender && clients[i].active) {
            if (send(clients[i].socket, (char*)msg, sizeof(Message), 0) == SOCKET_ERROR) {
                printf("发送消息到客户端 [%s] 失败: %d\n", clients[i].name, WSAGetLastError());
                clients[i].active = 0;  // 标记为不活跃
            } else {
                sent_count++;
            }
        }
    }
    
    ReleaseMutex(client_mutex);
    
    if (msg->type == MSG_CHAT && strcmp(msg->sender, "系统") != 0) {
        printf("消息已广播给 %d 个用户\n", sent_count);
    }
}

void send_user_list() {
    Message user_list_msg;
    user_list_msg.type = MSG_USER_LIST;
    strncpy(user_list_msg.sender, "系统", NAME_SIZE - 1);
    user_list_msg.sender[NAME_SIZE - 1] = '\0';
    
    char user_list[BUFFER_SIZE] = "";
    WaitForSingleObject(client_mutex, INFINITE);
    
    for (int i = 0; i < client_count; i++) {
        if (i > 0) {
            strncat(user_list, ",", BUFFER_SIZE - strlen(user_list) - 1);
        }
        strncat(user_list, clients[i].name, BUFFER_SIZE - strlen(user_list) - 1);
    }
    
    ReleaseMutex(client_mutex);
    
    strncpy(user_list_msg.content, user_list, BUFFER_SIZE - 1);
    user_list_msg.content[BUFFER_SIZE - 1] = '\0';
    strncpy(user_list_msg.timestamp, get_current_time(), 31);
    user_list_msg.timestamp[31] = '\0';
    
    broadcast_message(&user_list_msg, INVALID_SOCKET);
}