#ifndef BED_SYSTEM_H
#define BED_SYSTEM_H
#include <stdbool.h>

// 5.1 Bed Structure (อัปเดตชื่อตัวแปรตามสเปก)
typedef struct BedNode {
    int idBed;              // เปลี่ยนชื่อให้ตรงตามสเปก (จากเดิม bedId)
    char type[5];           // "ER" หรือ "OPD" (จากเดิม bedType)
    bool isOccupied;        // สถานะเตียง (ว่าง/ไม่ว่าง)
    
    // pointer ไปหาคนไข้ (ใช้ void* ไปก่อนระหว่างรอรวมไฟล์ หากรวมแล้วเปลี่ยนเป็น Patient* ได้)
    void* patient;          

    struct BedNode* prev;   // สำหรับ Doubly Linked List
    struct BedNode* next;   
} BedNode;

// โครงสร้างสำหรับคุม List
typedef struct {
    BedNode* head;
    BedNode* tail;
    int totalBeds;
    int occupiedBeds;
} BedList;

// ประกาศฟังก์ชัน
void initBeds();
void displayBedAllocation();
bool allocateBed(int severity); 
bool freeBed(int idBed);        

#endif
