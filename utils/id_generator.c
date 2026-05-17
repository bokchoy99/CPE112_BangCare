#include <stdio.h>
#include "id_generator.h"
#include "../core/config.h"

/* Formats dest as "BC-XXX" where XXX is zero-padded to 3 digits. */
void generatePatientID(char* dest, int counter) {
    sprintf(dest, "%s%03d", ID_PREFIX, counter);
}