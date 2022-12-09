#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#define MAX_LINES 4096
#define INST_SIZE 12
#define DATA_SIZE 8
#define INT_SIZE 32
#define REGS 16
#define IOREGS 23
#define SECTORS 128
#define SECTOR_SIZE	128		
#define MAX_INPUT_LINE 500
#define MONITOR_PIXELS (256*256)
#define DISK_CYCLES 1024
// Command line arguments
#define IMEMIN 1
#define DMEMIN 2
#define DISKIN 3
#define IRQ2IN 4
#define DMEMOUT 5
#define REGOUT 6
#define TRACE 7
#define HWREGTRACE 8
#define CYCLES 9
#define LEDS 10
#define DISPLAY7SEG 11
#define DISKOUT 12
#define MONITOR 13
#define MONITOR_YUV 14

// structs
typedef struct _instruction
{
	int opcode, rd, rs, rt, rm, immediate1, immediate2;
}	instruction;

// functions
int slice_atoi_hex(char str[], int start, int len);
int sign_extantion(int num, int bits);
int update_irq2status(int ioreg[], int irq2_cycles[], int next_irq2_idx);
void read_dmemin(FILE* fp_dmemin, int mem[]);
void create_instruction(char str[], instruction *str_as_ins);
void read_imemin(FILE *fp_imemin, instruction *instruction_memory);
void read_diskin(FILE* fp_diskin, int *disk_mat);
void write_trace( FILE* fp_trace, int pc, instruction inst, int reg[]);
const char* ioreg_name(int ioreg_num);
int decode_inst( int pc, int reg[], int ioreg[], int data_mem[], instruction inst, int* in_irq_ptr, FILE* fp_trace, FILE* fp_hwregtrace, FILE* fp_leds, FILE* fp_display7seg, int frame_buffer[]);
void write_regout(FILE* fp_regout, int reg[]);
void write_dmemout(FILE* fp_dmemout, int mem[]);
void write_monitor(FILE *fp_monitor_txt, FILE *fp_monitor_yuv, int frame_buffer[]);
int last_data_mem(int mem[]);
int last_sector_disk(int disk_mem[][SECTOR_SIZE]);
void write_diskout(FILE* fp_diskout, int disk_memory[][SECTOR_SIZE]);
void read_irq2in(FILE* fp_irq2in, int irq2cycles[]);
void handle_timer(int ioreg[]);
int handle_hard_disk(int ioreg[], int data_memory[], int disk_memory[][SECTOR_SIZE], int disk_count);