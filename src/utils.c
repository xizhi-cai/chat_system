#include "../include/chat.h"

void init_winsock() {
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        printf("WSAStartup失败，错误代码: %d\n", result);
        printf("请检查Windows Socket库是否正确安装\n");
        exit(1);
    }
}

void cleanup_winsock() {
    WSACleanup();
    printf("Winsock已清理\n");
}

char* get_current_time() {
    static char time_str[32];
    time_t now = time(NULL);
    struct tm local_time;
    
    // MinGW和MSVC的localtime_s参数顺序相同
    if (localtime_s(&local_time, &now) == 0) {
        strftime(time_str, sizeof(time_str), "%H:%M:%S", &local_time);
    } else {
        // 如果localtime_s失败，使用localtime作为备选
        struct tm* tm_ptr = localtime(&now);
        if (tm_ptr) {
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_ptr);
        } else {
            strcpy(time_str, "00:00:00");
        }
    }
    
    return time_str;
}

void log_message(const char* format, ...) {
    char time_str[64];
    time_t now = time(NULL);
    struct tm local_time;
    
    if (localtime_s(&local_time, &now) == 0) {
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &local_time);
    } else {
        strcpy(time_str, "1970-01-01 00:00:00");
    }
    
    printf("[%s] ", time_str);
    
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    printf("\n");
    fflush(stdout);  // 确保立即输出
}