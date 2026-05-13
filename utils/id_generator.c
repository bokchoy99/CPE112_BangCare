#include <stdio.h>
#include "id_generator.h"
#include "../core/config.h" // สำหรับ ID_PREFIX

void generatePatientID(char* dest, int counter) {
    sprintf(dest, "%s%03d", ID_PREFIX, counter);
}