#include "simulator.h"

int main(int argc, char *argv[])
{
	int reg[REGS] = { 0 }, ioreg[IOREGS] = { 0 };

	FILE *fp_imemin=NULL, *fp_dmemin=NULL, *fp_diskin = NULL, *fp_irq2in = NULL, *fp_dmemout = NULL, *fp_regout=NULL, *fp_trace = NULL,
		*fp_hwregtrace = NULL, *fp_cycles=NULL, *fp_leds = NULL, *fp_display7seg = NULL, *fp_diskout = NULL,
		*fp_monitor_txt = NULL,	*fp_monitor_yuv = NULL;

	int data_memory[MAX_LINES] = { 0 };
	instruction instruction_memory[MAX_LINES];
	int disk_memory[SECTORS][SECTOR_SIZE] = { 0 };
	int irq2_cycles[MAX_LINES] = { 0 };

	int next_irq2_idx = 0, disk_count = 0, pc = 0, irq = 0, busy_with_interruption = 0;
	int *in_irq_ptr = &busy_with_interruption;
	instruction cur_inst;
	int frame_buffer[MONITOR_PIXELS] = { 0 };

	if (argc != 15)	// check command line arguments
	{
		printf("Error: Incorrect command line arguments number!\n");
		return 1;
	}

	// opening files needed, checking if they have been opening properly 
	fp_trace = fopen(argv[TRACE], "w");
	if (fp_trace == NULL)
	{
		printf("Error: The file trace.txt couldn't open properly");
		return 1;
	}

	fp_hwregtrace = fopen(argv[HWREGTRACE], "w");
	if (fp_hwregtrace == NULL)
	{
		printf("Error: The file hwregtrace.txt couldn't open properly");
		return 1;
	}

	fp_leds = fopen(argv[LEDS], "w");
	if (fp_leds == NULL)
	{
		printf("Error: The file leds.txt couldn't open properly");
		return 1;
	}

	fp_display7seg = fopen(argv[DISPLAY7SEG], "w");
	if (fp_display7seg == NULL)
	{
		printf("Error: The file display7seg.txt couldn't open properly");
		return 1;
	}

	// filling the data from the different files into the correct array.

	// read dmemin
	fp_dmemin = fopen(argv[DMEMIN], "r");
	if (fp_dmemin == NULL)
	{
		printf("Error: The file dmemin.txt couldn't open properly");
		return 1;
	}
	read_dmemin(fp_dmemin, data_memory); 
	fclose(fp_dmemin);

	// read imemin
	fp_imemin = fopen(argv[IMEMIN], "r");
	if (fp_imemin == NULL)
	{
		printf("Error: The file imemin.txt couldn't open properly");
		return 1;
	}
	read_imemin(fp_imemin, instruction_memory); 
	fclose(fp_imemin);

	// read irq2in
	fp_irq2in = fopen(argv[IRQ2IN], "r");
	if (fp_irq2in == NULL)
	{
		printf("Error: The file imemin.txt couldn't open properly");
		return 1;
	}
	read_irq2in(fp_irq2in, irq2_cycles);
	fclose(fp_irq2in);

	// read diskin
	fp_diskin = fopen(argv[DISKIN], "r");
	if (fp_diskin == NULL)
	{
		printf("Error: The file diskin.txt couldn't open properly");
		return 1;
	}
	read_diskin(fp_diskin, disk_memory);  
	fclose(fp_diskin);
	
	// loading the first instruction
	cur_inst = instruction_memory[0];

	do
	{
		// update irq2status register
		next_irq2_idx = update_irq2status(ioreg, irq2_cycles, next_irq2_idx);

		// handle hard disk
		disk_count = handle_hard_disk(ioreg, data_memory, disk_memory, disk_count);

		// handle timer
		handle_timer(ioreg);
		
		// execute the next instruction from the assembly 
		pc = decode_inst(pc, reg, ioreg, data_memory, cur_inst, in_irq_ptr, fp_trace, fp_hwregtrace, fp_leds, fp_display7seg, frame_buffer);

		// dealing with interruption
		irq = (ioreg[0] & ioreg[3]) | (ioreg[1] & ioreg[4]) | (ioreg[2] & ioreg[5]);
		if (irq == 1 && busy_with_interruption == 0) // interrupt has been made and the cpu is not busy with another interruption
		{
			ioreg[7] = pc;  // irq_return = pc
			pc = ioreg[6];  // new_pc = irqhandler
			busy_with_interruption = 1;  
		}
	
		// increase cycle
		ioreg[8]++;
		printf("%d\n", ioreg[4]);
		cur_inst = instruction_memory[pc];
	
	} while (pc != -1);


	// write cycles
	fp_cycles = fopen(argv[CYCLES], "w");
	if (fp_cycles == NULL)
	{
		printf("Error: The file cycles.txt couldn't open properly");
		return 1;
	}
	fprintf(fp_cycles, "%u\n", ioreg[8]);
	fclose(fp_cycles);

	// write regout
	fp_regout = fopen(argv[REGOUT], "w");
	if (fp_regout == NULL)
	{
		printf("Error: The file regout.txt couldn't open properly");
		return 1;
	}
	write_regout(fp_regout, reg);
	fclose(fp_regout);

	// write dmemout
	fp_dmemout = fopen(argv[DMEMOUT], "w");
	if (fp_dmemout == NULL)
	{
		printf("Error: The file dmemout.txt couldn't open properly");
		return 1;
	}
	write_dmemout(fp_dmemout, data_memory);
	fclose(fp_dmemout);

	// write monitor
	fp_monitor_txt = fopen(argv[MONITOR], "w");
	if (fp_monitor_txt == NULL)
	{
		printf("Error: The file monitor.txt couldn't open properly");
		return 1;
	}
	fp_monitor_yuv = fopen(argv[MONITOR_YUV], "wb");
	if (fp_monitor_yuv == NULL)
	{
		printf("Error: The file monitor_yuv couldn't open properly");
		return 1;
	}
	write_monitor(fp_monitor_txt, fp_monitor_yuv, frame_buffer);
	fclose(fp_monitor_txt);
	fclose(fp_monitor_yuv);

	// write diskout
	fp_diskout = fopen(argv[DISKOUT], "w");
	if (fp_diskout == NULL)
	{
		printf("Error: The file diskout.txt couldn't open properly");
		return 1;
	}
	write_diskout(fp_diskout, disk_memory);
	fclose(fp_diskout);

	// closing all opened files
	fclose(fp_trace);
	fclose(fp_hwregtrace);
	fclose(fp_leds);
	fclose(fp_display7seg);
	
	return 0;
}

// slice hexadecimal stirng (str[start:start+len]), return as int 
int slice_atoi_hex(char str[], int start, int len)
{
	char tmp[INST_SIZE + 3];
	strncpy(tmp, str + start, len);
	tmp[len] = '\0';
	return strtoul(tmp, NULL, 16);
}

// sign extend n-bits to 32-bits 
int sign_extantion(int num, int bits)
{
	return (num << (INT_SIZE - bits)) >> (INT_SIZE - bits);
}

//	read "irq2in" input file to irq2cycles array. 
void read_irq2in(FILE* fp_irq2in, int irq2_cycles[])
{
	int i;
	char line[DATA_SIZE + 3];
	for (i = 0; fgets(line, DATA_SIZE + 3, fp_irq2in); i++)
	{
		line[strcspn(line, "\r\n")] = '\0';	// remove \n and \r
		irq2_cycles[i] = atoi(line);
	}
}

// update irq2status register, update next irq2 cycle
int update_irq2status(int ioreg[], int irq2_cycles[], int next_irq2_idx)
{
	if (irq2_cycles[next_irq2_idx] == ioreg[8])
	{
		ioreg[5] = 1;
		next_irq2_idx++;	
	}
	return next_irq2_idx;
}

//	read "dmemin" input file to the memory array. 
void read_dmemin(FILE* fp_dmemin, int mem[])
{
	int i;
	char line[DATA_SIZE + 3];
	for (i = 0; fgets(line, DATA_SIZE + 3, fp_dmemin); i++)
	{
		line[strcspn(line, "\r\n")] = '\0';	// remove \n and \r
		mem[i] = strtoul(line, NULL, 16);
	}
}

//creating a new instruction as written in the imemin file, only now it appears as an integer (hexadecimal) and not as a stirng.
void create_instruction(char line[], instruction *inst)
{
	inst->opcode = slice_atoi_hex(line, 0, 2);
	inst->rd = slice_atoi_hex(line, 2, 1);
	inst->rs = slice_atoi_hex(line, 3, 1);
	inst->rt = slice_atoi_hex(line, 4, 1);
	inst->rm = slice_atoi_hex(line, 5, 1);
	inst->immediate1 = sign_extantion(slice_atoi_hex(line, 6, 3), 12);
	inst->immediate2 = sign_extantion(slice_atoi_hex(line, 9, 3), 12);
}

// read imemin
void read_imemin(FILE *fp_imemin, instruction *instruction_memory)
{
	char new_inst[INST_SIZE + 3];
	while (fgets(new_inst, INST_SIZE + 3, fp_imemin))
	{
		new_inst[strcspn(new_inst, "\r\n")] = '\0';	// remove \n and \r
		create_instruction(new_inst, instruction_memory);
		instruction_memory++;
	}
}

// read diskin
void read_diskin(FILE* fp_diskin, int *disk_memory)
{
	char line[DATA_SIZE + 3];
	while (fgets(line, DATA_SIZE + 3, fp_diskin))
	{
		line[strcspn(line, "\r\n")] = '\0';	// remove \n and \r
		*disk_memory = strtoul(line, NULL, 16);
		disk_memory++;
	}
}

//	write line to "trace" output file 
void write_trace(FILE* fp_trace, int pc, instruction inst, int reg[])
{
	int i = 0, imm1, imm2;
	imm1 = inst.immediate1 & 0xFFF;
	imm2 = inst.immediate2 & 0xFFF;
	fprintf(fp_trace, "%03X ", pc);
	fprintf(fp_trace, "%02X%01X%01X%01X%01X%03X%03X", inst.opcode, inst.rd, inst.rs, inst.rt, inst.rm, imm1, imm2);
	for (i = 0; i < REGS; i++)
		fprintf(fp_trace, " %08x", reg[i]);
	fprintf(fp_trace, "\n");

}

//	gets IO register number, return its name
const char* ioreg_name(int ioreg_num)
{
	switch (ioreg_num)
	{
	case 0:	return "irq0enable";
	case 1:	return "irq1enable";
	case 2: return "irq2enable";
	case 3: return "irq0status";
	case 4: return "irq1status";
	case 5: return "irq2status";
	case 6: return "irqhandler";
	case 7: return "irqreturn";
	case 8: return "clks";
	case 9: return "leds";
	case 10: return "display7seg";
	case 11: return "timerenable";
	case 12: return "timercurrent";
	case 13: return "timermax";
	case 14: return "diskcmd";
	case 15: return "disksector";
	case 16: return "diskbuffer";
	case 17: return "diskstatus";
	case 18: return "reserved";
	case 19: return "reserved";
	case 20: return "monitoraddr";
	case 21: return "monitordata";
	case 22: return "monitorcmd";
	default: return "error";
	}
}

// decode and execute instruction, return new PC
int decode_inst(int pc, int reg[], int ioreg[], int data_mem[], instruction inst, int* in_irq_ptr, FILE* fp_trace, FILE* fp_hwregtrace, FILE* fp_leds, FILE* fp_display7seg, int frame_buffer[])
{
	int shamt, mask, mem_idx, leds_before = 0, seven_segment_before = 0;
	reg[1] = inst.immediate1;
	reg[2] = inst.immediate2;

	// print the instruction to "trace" output file before furfilling it
	write_trace(fp_trace, pc, inst, reg);

	switch (inst.opcode)
	{
	case 0:		// add
		reg[inst.rd] = reg[inst.rs] + reg[inst.rt] + reg[inst.rm];
		pc++;
		break;
	case 1:		// sub
		reg[inst.rd] = reg[inst.rs] - reg[inst.rt] - reg[inst.rm];
		pc++;
		break;
	case 2:		// mac
		reg[inst.rd] = reg[inst.rs] * reg[inst.rt] + reg[inst.rm];
		pc++;
		break;
	case 3:		// and
		reg[inst.rd] = reg[inst.rs] & reg[inst.rt] & reg[inst.rm];
		pc++;
		break;
	case 4:		// or
		reg[inst.rd] = reg[inst.rs] | reg[inst.rt] | reg[inst.rm];
		pc++;
		break;
	case 5:		// xor
		reg[inst.rd] = reg[inst.rs] ^ reg[inst.rt] ^ reg[inst.rm];
		pc++;
		break;
	case 6:		// sll
		shamt = (reg[inst.rt] >= 0) ? reg[inst.rt] : (reg[inst.rt] & 0x1F);
		reg[inst.rd] = reg[inst.rs] << shamt;
		pc++;
		break;
	case 7:		// sra 
		shamt = (reg[inst.rt] >= 0) ? reg[inst.rt] : (reg[inst.rt] & 0x1F);
		reg[inst.rd] = reg[inst.rs] >> shamt;
		pc++;
		break;
	case 8:		// srl 
		shamt = (reg[inst.rt] >= 0) ? reg[inst.rt] : (reg[inst.rt] & 0x1F);
		mask = (1 << (INT_SIZE - shamt)) - 1;
		reg[inst.rd] = (reg[inst.rs] >> shamt) & mask;
		pc++;
		break;
	case 9:		// beq
		if (reg[inst.rs] == reg[inst.rt])
			pc = reg[inst.rm] & 0xFFF;
		else
			pc++;
		break;
	case 10:	// bne
		if (reg[inst.rs] != reg[inst.rt])
			pc = reg[inst.rm] & 0xFFF;
		else
			pc++;
		break;
	case 11:	// blt
		if (reg[inst.rs] < reg[inst.rt])
			pc = reg[inst.rm] & 0xFFF;
		else
			pc++;
		break;
	case 12:	// bgt
		if (reg[inst.rs] > reg[inst.rt])
			pc = reg[inst.rm] & 0xFFF;
		else
			pc++;
		break;
	case 13:	// ble
		if (reg[inst.rs] <= reg[inst.rt])
			pc = reg[inst.rm] & 0xFFF;
		else
			pc++;
		break;
	case 14:	// bge
		if (reg[inst.rs] >= reg[inst.rt])
			pc = reg[inst.rm] & 0xFFF;
		else
			pc++;
		break;
	case 15:	// jal
		reg[inst.rd] = pc + 1;
		pc = reg[inst.rm] & 0xFFF;
		break;
	case 16:	// lw
		mem_idx = (reg[inst.rs] + reg[inst.rt]) % MAX_LINES;
		reg[inst.rd] = data_mem[mem_idx] + reg[inst.rm];
		pc++;
		break;
	case 17:	// sw
		mem_idx = (reg[inst.rs] + reg[inst.rt]) % MAX_LINES;
		data_mem[mem_idx] = reg[inst.rm] + reg[inst.rd];
		pc++;
		break;
	case 18:	// reti
		pc = ioreg[7];
		*in_irq_ptr = 0;	// busy_with_interruption = 0
		break;
	case 19:	// in
		if (reg[inst.rs] + reg[inst.rt] == 22) // in monitorcmd (register 22) return 0
			reg[inst.rd] = 0;
		else 
			reg[inst.rd] = ioreg[reg[inst.rs] + reg[inst.rt]];
		pc++;
		fprintf(fp_hwregtrace, "%u READ %s %08x\n", ioreg[8], ioreg_name(reg[inst.rs] + reg[inst.rt]), reg[inst.rd]);
		break;
	case 20:	// out
		//sample the leds and 7-segment diaplay before changing it
		leds_before = ioreg[9];
		seven_segment_before = ioreg[10];

		//implementing out command
		ioreg[reg[inst.rs] + reg[inst.rt]] = reg[inst.rm];
		pc++;
		fprintf(fp_hwregtrace, "%u WRITE %s %08x\n", ioreg[8], ioreg_name(reg[inst.rs] + reg[inst.rt]), reg[inst.rm]);

		// dealing with monitor 
		if (ioreg[22] == 1) //when the monitorcmd = 1 (write pixel to monitor), there is an update of the pixel which his content is in monitordata using out command
		{
			frame_buffer[ioreg[20]] = ioreg[21] % 256;
			ioreg[22] = 0; // after writing to monitor_buffer, monitorcmd = 0
		}
		// dealing with leds
		if (ioreg[9] != leds_before) // a change in leds has been made - write "leds" output file
			fprintf(fp_leds, "%u %08x\n",ioreg[8], ioreg[9]);
		//dealing with 7-segment display
		if (ioreg[10] != seven_segment_before) // a change in 7-segment display has been made - write "display7seg" output file
			fprintf(fp_display7seg, "%u %08x\n", ioreg[8], ioreg[10]);

		break;
	case 21:	// halt
		pc = -1;
		break;
	default:
		pc = -1;
	}

	reg[0] = 0;
	reg[1] = inst.immediate1;
	reg[2] = inst.immediate2;

	return pc;
}

//	write line to "regout" output file 
void write_regout(FILE* fp_regout, int reg[])
{
	int i;
	for (i = 3; i < REGS; i++)
		fprintf(fp_regout, "%08X\n", reg[i]);
}

// finding the last line which is not equal to 0 in memory
int last_data_mem(int mem[])
{
	int i, final = 0;
	for (i = 0; i < MAX_LINES; i++)
		if (mem[i] != 0)
			final = i;
	return final;
}

// finding the last line which is not equal to 0 in disk
int last_sector_disk(int disk_mem[][SECTOR_SIZE])
{
	int i, j, final = 0;
	for (i = 0; i < SECTORS; i++)
		for (j = 0; j < SECTOR_SIZE; j++)
			if (disk_mem[i][j] != 0)	// sector i not all zeros
			{
				final = i;
				break;	
			}
			
	return final;
}

//	write memroy array to "dmemout" output file 
void write_dmemout(FILE* fp_dmemout, int data_memory[])
{
	int i, final_data;
	final_data = last_data_mem(data_memory);
	for (i = 0; i <= final_data; i++)
		fprintf(fp_dmemout, "%08X\n", data_memory[i]);

}

//write to monitor
void write_monitor(FILE *fp_monitor_txt, FILE *fp_monitor_yuv, int frame_buffer[])
{
	int i;
	for (i = 0; i < MONITOR_PIXELS; i++)
	{
		fprintf(fp_monitor_txt, "%02X\n", frame_buffer[i]);
		fprintf(fp_monitor_yuv, "%c", frame_buffer[i]);
	}
}


void write_diskout(FILE* fp_diskout, int disk_memory[][SECTOR_SIZE])
{
	int i, j;
	int final_sector = last_sector_disk(disk_memory);

	for (i = 0; i <= final_sector; i++)
		for (j = 0; j < SECTOR_SIZE; j++)
			fprintf(fp_diskout, "%08X\n", disk_memory[i][j]);
}

// handling with the timer
void handle_timer(int ioreg[])
{
	if (ioreg[11] == 1) // checking if timer is enabled
	{
		if (ioreg[12] < ioreg[13]) // as long as the timercuurent < timermax we will increase it
			ioreg[12]++; // increasing the timer counter
		else
		{
			ioreg[12] = 0; // timercurrent = 0
			ioreg[3] = 1; // interruption 0 is turning on
		}
	}
}

int handle_hard_disk(int ioreg[], int data_memory[], int disk_memory[][SECTOR_SIZE], int disk_count)
{
	int  mem_idx, disk_sec_idx, disk_word_idx;
	if (ioreg[14] != 0)  // there is a command to the hard disk
	{
		ioreg[17] = 1;	// diskstatus = 1 - disk busy
		if (disk_count % 8 == 0 )	// read/wrtire every 8 cycles - total cycles (1024) / sector size (128) = 8 cycles
		{
			mem_idx = (ioreg[16] + (disk_count / 8)) % MAX_LINES;
			disk_sec_idx = ioreg[15] % SECTORS;
			disk_word_idx = (disk_count / 8) % SECTOR_SIZE;

			if (ioreg[14] == 1)  // diskcmd = 1 - read sector
				data_memory[mem_idx] = disk_memory[disk_sec_idx][disk_word_idx];
			else				// diskcmd = 2 - write sector
				disk_memory[disk_sec_idx][disk_word_idx] = data_memory[mem_idx];
		}

		if (disk_count < DISK_CYCLES - 1) 
			disk_count++;
		else
		{
			disk_count = 0;
			ioreg[17] = 0;	// diskstatus = 0 - disk available
			ioreg[14] = 0;	// diskcmd = 0 - no command
			ioreg[4] = 1;	// irq1status = 1
		}
	}
	return disk_count;
}