#ifndef CONFIG_H
#define CONFIG_H

/* -- Simulation Timing ---------------------------------------- */
#define TICK_UNIT_MINUTES       2   /* Minutes per simulation tick.              */
#define RECOVERY_TICK_MINUTES   10  /* Minutes allocated for post-care recovery. */
#define AGING_THRESHOLD_TICKS   5   /* Ticks before severity is escalated.       */

/* -- Bed Capacity --------------------------------------------- */
#define MAX_ER_BEDS   5
#define MAX_OPD_BEDS  10

/* -- Treatment Duration (ticks) ------------------------------- */
#define TREAT_TIME_S5   15  /* 30 min */
#define TREAT_TIME_S4   10  /* 20 min */
#define TREAT_TIME_S3   7   /* 14 min */
#define TREAT_TIME_S2   5   /* 10 min */
#define TREAT_TIME_S1   5   /* 10 min */

/* -- File paths ----------------------------------------------- */
#define LOG_FILE        "data_output/system_log.txt"
#define HISTORY_FILE    "data_output/patient_history.csv"
#define HASH_TABLE_SIZE 101
#define ID_PREFIX       "BC-"
#endif