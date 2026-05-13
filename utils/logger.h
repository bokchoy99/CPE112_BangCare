#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

/**
 * Log Levels สำหรับแบ่งประเภทเหตุการณ์
 */
typedef enum {
    LOG_INFO,    // ข้อมูลทั่วไป (เช่น การรับคนไข้สำเร็จ)
    LOG_WARNING, // ข้อควรระวัง (เช่น เตียงเต็ม หรือหา ID ไม่เจอ)
    LOG_ERROR,   // ข้อผิดพลาด (เช่น การจอง Memory ล้มเหลว)
    LOG_SYSTEM   // การเปลี่ยนแปลงสถานะระบบ (เช่น การ Tick เวลา)
} LogLevel;

/**
 * เริ่มต้นระบบ Logger และสร้าง/ล้างไฟล์ system_log.txt
 */
void initLogger();

/**
 * บันทึกเหตุการณ์ลงในไฟล์ system_log.txt พร้อม Timestamp และชื่อโมดูล
 * @param level ระดับความสำคัญของ Log
 * @param module ชื่อโมดูลที่เรียกใช้ (เช่น "HASHTAB", "TRIAGE", "BED_MGR")
 * @param message ข้อความที่ต้องการบันทึก
 */
void logEvent(LogLevel level, const char* module, const char* message);

#endif