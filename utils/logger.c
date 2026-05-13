#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "../core/system_context.h"

// ล้าง Log เก่าเมื่อเริ่มโปรแกรมใหม่
void initLogger() {
    FILE* f = fopen("system_log.txt", "w");
    if (f) {
        fprintf(f, "=== BANGCARE SYSTEM LOG STARTED ===\n");
        fclose(f);
    }
}

// บันทึกเหตุการณ์ลงไฟล์
void logEvent(LogLevel level, const char* module, const char* message) {
    FILE* f = fopen("system_log.txt", "a");
    if (f == NULL) return;

    time_t now = (time_t)gSystem.simulatedTime;
    struct tm* info = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", info);

    const char* level_names[] = {"INFO", "WARN", "ERR ", "SYS "};
    fprintf(f, "[%s] [%s] [%-8s] %s\n", time_str, level_names[level], module, message);
    
    fclose(f);
}