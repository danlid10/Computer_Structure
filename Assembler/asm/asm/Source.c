#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 500


  /////////////////////////////////////////////////////////////////////////
 /////////////////////// Label functions & structs ///////////////////////
/////////////////////////////////////////////////////////////////////////

//The lable struct will help us save nformation about the lables, we will collect it on the firs iteration. 
typedef struct Label
{
	char name[50];				    // The label's name
	int location;					//  line's location
	struct Label* next;		     	// a pointer to the next label
} label;

// this function creates a label from the given name and location
label* create_a_new_label(char name[50], int location)
{
	// memory allocation and make sure allocation succeed  
	label* new_label = (label*)malloc(sizeof(label));
	if (new_label != NULL) {				// data insertion to label
		strcpy(new_label->name, name);
		new_label->location = location;
		new_label->next = NULL;
	}
	return new_label;
}

// adds a label to the front of an existing label list. we will use it at the first iteration
label* add(label* head, char name[50], int location) {
	// build a new label and make sure it worked.
	label* new_label = create_a_new_label(name, location);     if (new_label == NULL)
		return NULL;
	new_label->next = head;
	return new_label;
}
// find_name will scan the label list and look for "name". this function will return the "name's" location.
// find_name will be used in the second iteration
int find_name(label* head, char name[50]) {
	char current[50];					// current label's name
	strcpy(current, head->name);
	while (strcmp(current, name) != 0) {
		head = head->next;
		if (head == NULL) { 		   // in case not found the lable's name return -1
			return -1;
		}
		strcpy(current, head->name);   // update name to current name

	}
	return head->location;			   // return the current's location

}

// destroys the list and frees the memory
void destroy_list_free_mem(label* head)
{
	label* temp;				//A pointer to a label we are going to destroy after updating head
	while (head != NULL)	    // Destroy all of the list
	{
		temp = head;		    // temp <-- current node, head <-- head->next
		head = head->next;
		free(temp);		    	// we destroy temp and free the memory
	}
}

  /////////////////////////////////////////////////////////////////////////
 /////////////////// Memory Line functions & structs /////////////////////
/////////////////////////////////////////////////////////////////////////

// MemoryLine will save memory lines
typedef struct MemoryLine {
	char opcode[6];				// command oppcode
	char rd[6];					// the registers
	char rs[6];
	char rt[6];
	char rm[6];
	char imm1[50];				// immediate values
	char imm2[50];
	int pos;					// position of character in line
	struct MemoryLine* next;  	// memory will be a linked list 
}MemoryLine;

// create_new_line will create new memory line
MemoryLine* create_new_line(char opcode[6], char rd[6], char rs[6], char rt[6], char rm[6], char imm1[50], char imm2[50], int pos) {
	MemoryLine* new_line = (MemoryLine*)malloc(sizeof(MemoryLine));  	// allocate memory for the label and create a pointer to it
	if (new_line != NULL) {   // Make sure allocation successful
		strcpy(new_line->opcode, opcode);
		strcpy(new_line->rd, rd);
		strcpy(new_line->rs, rs);
		strcpy(new_line->rt, rt);
		strcpy(new_line->rm, rm);
		strcpy(new_line->imm1, imm1);
		strcpy(new_line->imm2, imm2);
		new_line->pos = pos;
		new_line->next = NULL;
	}
	return new_line;
}

// add_line will add a new line to memory structure. this line will be added to the end to let the writing run it like an array.
MemoryLine* add_line(MemoryLine* head, char opcode[6], char rd[6], char rs[6], char rt[6], char rm[6], char imm1[50], char imm2[50], int pos) {
	MemoryLine* tail;							// the last line as for now
	MemoryLine* new_line = create_new_line(opcode, rd, rs, rt, rm, imm1, imm2, pos);  // create a line
	if (new_line == NULL)						// if the new line is null, return null
		return NULL;
	if (head == NULL)							//  return the current head if no head sent
		return new_line;
	tail = head;		     					// get the "tail" to the end of the list
	while (tail->next != NULL)
		tail = tail->next;
	tail->next = new_line;						// add the new line
	return head;								// return updated memory
}

// destroy_MemoryLine will destroy the list's memory and free the memory the assembler used
void destroy_MemoryLine(MemoryLine* head) {
	MemoryLine* temp;			//a pointer to a line we are going to destroy after updating the head
	while (head != NULL)		// untill the end
	{
		temp = head;			// temp gets the current node while head - head ->next
		head = head->next;
		free(temp);  			// we destroy temp and free the memory
	}
}

// getAtPos will get the memory line at position.
MemoryLine* getAtPos(MemoryLine* head, int pos) {
	while (head != NULL && head->pos != pos)   	// go until you find position
		head = head->next;     return head;
}


  /////////////////////////////////////////////////////////////////////////
 ///////////////////// Memory functions & structs ////////////////////////
/////////////////////////////////////////////////////////////////////////

// it will be used in the second iteration, so it will be able to return two values.
typedef struct Memory
{
	MemoryLine* head;			// head of memory line list
	int last;					// position of last
	int last2;
}Memory;

//create_mem will create memory structures
Memory* create_mem(MemoryLine* head, int pos1, int pos2)
{
	Memory* mem = (Memory*)malloc(sizeof(Memory));
	mem->head = head;
	mem->last = pos1;
	mem->last2 = pos2;
	return(mem);						 //Return the number of lines
}

// destroy_mem will destroys the memory struct after use
void destroy_mem(Memory* mem) {
	destroy_MemoryLine(mem->head);
	free(mem);
}

  /////////////////////////////////////////////////////////////////////////
 ///////////////////////////// First iteration////////////////////////////
/////////////////////////////////////////////////////////////////////////

//at thefirst iteration we will create a labels list
label* Make_Labels_List(FILE *asembl) {
	// rowIndex - where the PC will go after reading the label
	// k - the char index , j - index in the label name string we are building, option - determining if it's a label only line or a label + command line
	// counter will be the line number in the new hexadecimal code. it will go up when a line that gets translated is found
	int rowIndex = 0, k, j, option, counter = 0, i = 0;
	// line the current line being read, tav1 is the first char after the whit spaces and the tabs and i used to check for remarks, 
	// tav - current char when reading label name
	// label_line - contains the name of the label once iteration is complete
	char line[MAX_LINE], tav1, tav, lable_line[50], dots[50];
	strcpy(dots, ":");
	label* head = NULL;				             //  label list's head
	while (!feof(asembl)) {
		fgets(line, MAX_LINE, asembl);			 // read a command from the assembler file
		option = 0;							     // reset option
		k = 0;
		while ((line[k] == ' ') || (line[k] == '\t'))  k++;
		if (line[k] == '\n') continue;
		if (strcmp(line, "\n") == 0) continue;  //If line is blank, continue
		tav1 = line[k];
		if (tav1 == '#') continue;			    //If line is Remark, continue
		if (strstr(line, dots) != NULL)			//If dots are found, this is a label
		{
			if (strstr(line, "#") != NULL)      // however, ":" can be in a remark. so check for that as well, if so go to another line
				if ((strstr(line, dots)) > (strstr(line, "#"))) continue;
		}
		if (strstr(line, ".word") != NULL) continue;  //If line is .word, continue
		if (strstr(line, dots) != NULL)				  //If dots are found, this is a label
		{
			if (strstr(line, "#") != NULL) // however, ":" can be in a remark. so check for that as well, if so go to another line
				if ((strstr(line, dots)) > (strstr(line, "#"))) continue;
			j = 0;  
			//Read the label name, first reset indexes
			do {
				tav = line[k];				 // get current char
				// two dots is where it ends so skip
				if (tav != ':') {
					if (tav != '\t' && tav != ' ')   // don't read tabs and spaces
					{
						lable_line[j] = tav;		 // grab the read char and put it in name string
						j++;                         // increment name string index
					}
					k++;						     // increment reading index
				}
			} while (tav != ':');
			lable_line[j] = '\0';              // label name is null terminated
			k++;							   // Check if the line is lable line only by seeing if there are only spaces and tabs till the end
			while ((line[k] == ' ') || (line[k] == '\t'))  k++;
			option = ((line[k] == '\n') || (line[k] == '#'));  // option is 1 on label only line, otherwise 0
			head = add(head, lable_line, counter);   // finally we add the label to label list
			if (option == 1) counter = counter - 1;  // Only label line - add label and decrement counter
		}
		 // Check if the current line is space line using k - most commands in fib.asm and our files start with a tab or a space
		k = 0;
		if ((line[k] == '\t') || (line[k] == ' '))  k++;
		if (line[k] == '\n')  continue;
		counter++;  // increment counter
	}
	return head;   // return list
}

  /////////////////////////////////////////////////////////////////////////
 ////////////// functions for the second iterations //////////////////////
/////////////////////////////////////////////////////////////////////////

// specialworld - uses .word commands to save in memory
// head - head of memory line list
// line - current line
// pos1 - last line's index
// k - index of character being read
// returns updated memory line list
MemoryLine *specialword(MemoryLine* head, char line[MAX_LINE], int *pos1, int *k) {
	char wordP[15], wordN[15];  // wordP - address, wordN - data
	int j = 0;					// index for string we copy to
	*k = 0;						// reset k index
	while (line[*k] != ' ') *k = *k + 1;            // go past all the spaces
	*k = *k + 1;
	j = 0;						//Copy Address. first reset j then copy char char until the next space
	while (line[*k] != ' ') {
		wordP[j] = line[*k];
		j = j + 1; *k = *k + 1;
	}
	wordP[j] = '\0'; *k = *k + 1;   // terminate string with null and increment end to next char
	j = 0;						    //Copy Data. using the same way.
	while (line[*k] != ' ') {
		if (line[*k] == '\n') break;// but detect an end of line string because after the data there can be a line end
		wordN[j] = line[*k];
		j = j + 1; *k = *k + 1;
	}
	wordN[j] = '\0';
	int pos = 0;					// pos - current line address, can be hexadecimal or decimal
	if (wordP[0] == '0') {			//  change Address int. the if block considers an hexadecimal input
		if (wordP[1] == 'x' || wordP[1] == 'X') pos = strtol(wordP, NULL, 16);
	}
	else pos = atoi(wordP);			// else blocks considers a decimal input
	// now. we will save the command in the memory list. NONO will be used as an indicator when writing to turn the command into a .word
	char nono[5] = "NONO"; // a string used to copy nono to required places. fifth char is null
	strcpy(nono, "NONO"); strcpy(nono, "NONO"); strcpy(nono, "NONO"); strcpy(nono, "NONO");
	head = add_line(head, nono, nono, nono, nono, nono, nono, wordN, pos);  // save line to line list. wordN - the immediate value, is used as the data
	if (pos > *pos1) *pos1 = pos;  // update the location of the end
	return head;
}
// reads opcode.
// line - current line
// option - a string of the opcode
// k - index in line being read
void readorder(char line[MAX_LINE], char *option, int *k) {
	char tav;	// char at index k
	int j = 0; // current index index 
	do { //continue untill dollar of first register
		tav = line[*k]; // read current char
		if (tav == '\0') break;
		if (tav != '$') // if it's a regular char
		{
			if (tav != '\t') 
				if (tav != ' ')
				{
					option[j] = tav;
					j = j + 1;
				}
			*k = *k + 1;
		}
	} while (tav != '$');
	option[j] = '\0'; // EOL terminates the opcode
}
//readdollar will find the next dollar sign
// line - current line
// k - current index
void readdollar(char line[MAX_LINE], int *k) {
	while (line[*k] != '$' && line[*k] != '\0') *k = *k + 1;
}
// reads register value
// line - current line
// rdst - reg name
void readrdst(char line[MAX_LINE], char *rdst, int *k) {
	int j = 0; // Read rd
	while (line[*k] != ',' && line[*k] != '\0')
	{
		if (line[*k] != ' ' && line[*k] != '\t') { // read if not a whitespace
			rdst[j] = line[*k];
			j = j + 1;
		}
		*k = *k + 1;
	}
	rdst[j] = '\0';// null terminate
//	return rdst; // return the string
}

// readimmd will read imm2 and imm1
// line - current assembly line
// imm - the pointer of the string we copy to
// k - current line index
void readimmd(char line[MAX_LINE], char *imm, int *k) {
	// go to next immediate
	while ((line[*k] == ' ') || (line[*k] == '\t') || (line[*k] == ',')) {
		*k = *k + 1;
	}
	int j = 0; // index of char being copied in immediate string
	while (line[*k] != ' ')
	{
		if (line[*k] != ' ' && line[*k] != '\t' &&line[*k] != '\0') {
			if ((line[*k] == '\t') || (line[*k] == '#') || (line[*k] == '\n'))	break;
			imm[j] = line[*k];
			j = j + 1;
		}
		*k = *k + 1;
	}
	imm[j] = '\0';
	//	return imm;
}

// readLine reads line of memory and adds to memory line list "head
// line - the current line
// pos1 - number of last line of memin
// i - current instruction line index. might be more than pos1. is processed as we go so that's why a pointer
// head - the head of the memory line list we add to
// k - current index of char being read
MemoryLine *readLine(char *line, int *pos1, int *i, MemoryLine *head, int *k) {
	char option[6], rd[6], rs[6], rt[6], rm[6], imm1[50], imm2[50]; // the line's properties
	readorder(line, option, k);			      // read the opcode                     
	readrdst(line, rd, k);                    // Read rd
	readdollar(line, k);                      // wait for dollar sign
	readrdst(line, rs, k);                    // Read rs
	readdollar(line, k);                      // wait for dollar sign
	readrdst(line, rt, k);                    // Read rt
	readdollar(line, k);                      // wait for dollar sign
	readrdst(line, rm, k);                    // Read rm				
	readimmd(line, imm1, k);                  //handle imm1
	readimmd(line, imm2, k);                  //handle imm2
	head = add_line(head, option, rd, rs, rt, rm, imm1, imm2, *i);	*i = *i + 1;	// save line to line list
	if (*i > *pos1) *pos1 = *i;  //Update last line position
	return head;
}

  /////////////////////////////////////////////////////////////////////////
 ///////////////////////////// Second iteration///////////////////////////
/////////////////////////////////////////////////////////////////////////

// SecondRun will read each line of the memory and copies it to a memory list
// FILE - INPUT assembly file
Memory* SecondRun(FILE* file, int inicator) {
	// k - current index of the char being read, i - current position in the file
	// pos1 - last line position, pos - the address in .word commands, as an int
	int k = 0, i = 0, pos2 = 0, pos_tmp = 0, pos1 = 0;
	// line - current line. tav1 - save the first character after white spaces
	
	char line[MAX_LINE], tav1, *dots = ":";
	MemoryLine* head = NULL;  // the Memory list's head. it will contain info about each memory line in the end
	while ((fgets(line, MAX_LINE, file)) != NULL) { // read the file line by line
		k = 0;
		while ((line[k] == ' ') || (line[k] == '\t'))  k++;
		if (line[k] == '\n') continue;
		if (strcmp(line, "\n") == 0) continue;  //empty line
		tav1 = line[k];
		if (tav1 == '#') continue;  // Remark --> continue
		while ((line[k] == ' ') || (line[k] == '\t'))  k++;
		if (line[k] == '\n') continue;
		if (strcmp(line, "\n") == 0) continue; 
		tav1 = line[k];
		if (tav1 == '#') continue;
		if (strcmp(line, "\n") == 0) continue;  
		char wo[6] = ".word"; // a string for  the following comparison
		int isword = 0; // booleand for .world detection

		if (strstr(line, wo) != NULL) { // It means .word in the line
			pos_tmp = pos1;
			head = specialword(head, line, &pos1, &k);
			pos2 = pos1;
			pos1 = pos_tmp;      
			isword = 1;
		}
		else if (strstr(line, dots) != NULL) {     //in case of regular order and label
			if (strstr(line, "#") != NULL) {       //now we check if the dots is remark and not a label
				if ((strstr(line, dots)) > (strstr(line, "#"))) {        //label line- check if the line include only label or order
					goto mark; // start reading
				}
			} // the following code section will work if the there is mark # and apears aftre dots or if there isn't mark #
			k = 0;
			while (line[k] != ':') k++;
			k++;
			if (line[k] == '\n') continue;
			else
				while ((line[k] == ' ') || (line[k] == '\t')) k++;
			if (line[k] == '\n') continue;
			if (line[k] == '#')	continue;
		}


		else// Order line only
		{
			k = 0;
			while ((line[k] == ' ') || (line[k] == '\t')) k++;  // roll to end of spaces
			if (line[k] == '#')	continue;
			if (line[k] == '\n') continue;
		}
		if (!isword && inicator != 2 ) { // copy line in all the other cases (not .world)
		mark:
			k = k;
			head = readLine(line, &pos1, &i, head, &k);

		}
	}
	if( inicator == 2) head = readLine(line, &pos1, &i, head, &k);
	return create_mem(head, pos1, pos2);  // create memory structure and return it to main function
}


  /////////////////////////////////////////////////////////////////////////
 // helpful functions for printing to a file & print to a file function //
/////////////////////////////////////////////////////////////////////////

//print_rd_rs_rt_rm - prints rd,rs,rt into the memin file
// rdst - register name to print
// memin - current file's pointer
// num - will be flipped to zero if it's a ".world"
void print_rd_rs_rt_rm(char *rdst, FILE *memin, int *num)
{ // basically a big if block that checks the name of the register and converts it to a number
	if (strcmp(rdst, "$zero") == 0)
		fprintf(memin, "0");
	else if (strcmp(rdst, "$imm1") == 0)
		fprintf(memin, "1");
	else if (strcmp(rdst, "$imm2") == 0)
		fprintf(memin, "2");
	else if (strcmp(rdst, "$v0") == 0)
		fprintf(memin, "3");
	else if (strcmp(rdst, "$a0") == 0)
		fprintf(memin, "4");
	else if (strcmp(rdst, "$a1") == 0)
		fprintf(memin, "5");
	else if (strcmp(rdst, "$a2") == 0)
		fprintf(memin, "6");
	else if (strcmp(rdst, "$t0") == 0)
		fprintf(memin, "7");
	else if (strcmp(rdst, "$t1") == 0)
		fprintf(memin, "8");
	else if (strcmp(rdst, "$t2") == 0)
		fprintf(memin, "9");
	else if (strcmp(rdst, "$s0") == 0)
		fprintf(memin, "A");
	else if (strcmp(rdst, "$s1") == 0)
		fprintf(memin, "B");
	else if (strcmp(rdst, "$s2") == 0)
		fprintf(memin, "C");
	else if (strcmp(rdst, "$gp") == 0)
		fprintf(memin, "D");
	else if (strcmp(rdst, "$sp") == 0)
		fprintf(memin, "E");
	else if (strcmp(rdst, "$ra") == 0)
		fprintf(memin, "F");
	else if (strcmp(rdst, "NONO") == 0)
		*num = 0;
	else
		fprintf(memin, "0");
}

//print_op_code will print the opcode to the file
// opc - opcode string
// memin - pointer to output file 
int print_op_code(char *opc, FILE *memin)
{
	if (strcmp(opc, "add") == 0) {
		fprintf(memin, "00"); return 1;
	}
	else if (strcmp(opc, "sub") == 0) {
		fprintf(memin, "01"); return 1;
	}
	else if (strcmp(opc, "mac") == 0) {
		fprintf(memin, "02"); return 1;
	}
	else if (strcmp(opc, "and") == 0) {
		fprintf(memin, "03"); return 1;
	}
	else if (strcmp(opc, "or") == 0) {
		fprintf(memin, "04"); return 1;
	}
	else if (strcmp(opc, "xor") == 0) {
		fprintf(memin, "05"); return 1;
	}
	else if (strcmp(opc, "sll") == 0) {
		fprintf(memin, "06"); return 1;
	}
	else if (strcmp(opc, "sra") == 0) {
		fprintf(memin, "07"); return 1;
	}
	else if (strcmp(opc, "srl") == 0) {
		fprintf(memin, "08"); return 1;
	}
	else if (strcmp(opc, "beq") == 0) {
		fprintf(memin, "09"); return 1;
	}
	else if (strcmp(opc, "bne") == 0) {
		fprintf(memin, "0A"); return 1;
	}
	else if (strcmp(opc, "blt") == 0) {
		fprintf(memin, "0B"); return 1;
	}
	else if (strcmp(opc, "bgt") == 0) {
		fprintf(memin, "0C"); return 1;
	}
	else if (strcmp(opc, "ble") == 0) {
		fprintf(memin, "0D"); return 1;
	}
	else if (strcmp(opc, "bge") == 0) {
		fprintf(memin, "0E"); return 1;
	}
	else if (strcmp(opc, "jal") == 0) {
		fprintf(memin, "0F"); return 1;
	}
	else if (strcmp(opc, "lw") == 0) {
		fprintf(memin, "10"); return 1;
	}
	else if (strcmp(opc, "sw") == 0) {
		fprintf(memin, "11"); return 1;
	}
	else if (strcmp(opc, "reti") == 0) {
		fprintf(memin, "12"); return 1;
	}
	else if (strcmp(opc, "in") == 0) {
		fprintf(memin, "13"); return 1;
	}
	else if (strcmp(opc, "out") == 0) {
		fprintf(memin, "14"); return 1;
	}
	else if (strcmp(opc, "halt") == 0) {
		fprintf(memin, "15"); return 1;
	}
	else // on .word
		return 0;
}


//gets memory head and output file indicator. prints the memory into file
void Print_Data_To_File(Memory* mem, FILE *memin, int write_file_num)
{
	// i - memory index, num - word for .word
	int end_of_loop = mem->last - 1;
	if (write_file_num == 2) {
		end_of_loop = mem->last2;   
	}
	int i = 0, num = 0; int flag = 0, ind = 0;
	while (mem->head != NULL && i <= end_of_loop) {
		MemoryLine *currentLine = getAtPos(mem->head, i); // go to current line's data once
		printf("line: %s \n", currentLine);

		// Printing Opcode. if data for the ith row does not exist print a zero
		if (write_file_num == 2) {
			if (!flag && currentLine != NULL) {// if there is no opcode. this block of code is used to get the word for the .word command
				ind = 1;
				if ((strcmp(currentLine->opcode, "NONO") == 0)) {
					if ((currentLine->imm2[0] == '0') && ((currentLine->imm2[1] == 'x') || (currentLine->imm2[1] == 'X')))
						num = strtol(currentLine->imm2, NULL, 16);
					else  //Imiddiate is decimal
						num = atoi(currentLine->imm2);
					fprintf(memin, "%08X\n", num);  //Print immidiate in hex
					i++;
					continue;
				}
			}
			if (ind == 1) {
				fprintf(memin, "00000000");
			}
			if (i != end_of_loop) fprintf(memin, "\n");  //Print \n except the last line
			i++;
			continue;
		}
		if (currentLine == NULL) fprintf(memin, "00");// if there is no opcode --> print 2 zeros                                
		else flag = print_op_code(currentLine->opcode, memin); // print the opcode
		if (!flag && currentLine != NULL) {// get the word for the .word command in case there is no opcode
			if ((strcmp(currentLine->opcode, "NONO") == 0)) {
				if ((currentLine->imm2[0] == '0') && ((currentLine->imm2[1] == 'x') || (currentLine->imm2[1] == 'X'))) 
					num = strtol(currentLine->imm2, NULL, 16);
				else  //Imiddiate is decimal
					num = atoi(currentLine->imm2);
				fprintf(memin, "%08X ffff", num);  //Print  in hex
			}
		}
		else if (!flag && (i != mem->last - 1)) // if there is nothing print a zero
			fprintf(memin, "00");
		if (currentLine == NULL) fprintf(memin, "0"); // Printing Rd
		else print_rd_rs_rt_rm(currentLine->rd, memin, &num);
		if (currentLine == NULL) fprintf(memin, "0"); 		// Printing Rs
		else print_rd_rs_rt_rm(currentLine->rs, memin, &num);
		if (currentLine == NULL) fprintf(memin, "0"); 		// Printing Rt
		else print_rd_rs_rt_rm(currentLine->rt, memin, &num);
		if (currentLine == NULL) fprintf(memin, "0"); 		// Printing Rm
		else print_rd_rs_rt_rm(currentLine->rm, memin, &num);
		// a check wheter to print the immediate and skip .word lines        
		if (currentLine == NULL) fprintf(memin, "%03X", 0 & 0xfff); // if the line is null print zero to immediate
		else if (strcmp(currentLine->opcode, "NONO") != 0)  // print if satisfied
		{
			if ((currentLine->imm1[0] == '0') && ((currentLine->imm1[1] == 'x') || (currentLine->imm1[1] == 'X'))) num = strtol(currentLine->imm1, NULL, 16);  //Check if immidiate in hex
			else num = atoi(currentLine->imm1);
			fprintf(memin, "%03X", num & 0xfff);  //Print immidiate in hex.
		}
		if (currentLine == NULL) fprintf(memin, "%03x", 0 & 0xfff); // print zero to immediate if this is a null line
		else if (strcmp(currentLine->opcode, "NONO") != 0)  //  print if satisfied
		{
			if ((currentLine->imm2[0] == '0') && ((currentLine->imm2[1] == 'x') || (currentLine->imm2[1] == 'X'))) num = strtol(currentLine->imm2, NULL, 16);  //Check if immidiate in hex
			else num = atoi(currentLine->imm2);
			fprintf(memin, "%03X", num & 0xfff);  //Print immidiate in hex
		}
		if (i != end_of_loop) fprintf(memin, "\n");  //Print '\n' except the last line
		i++;  // next line
	}
}

// Change_Lab will switch function that runs between the second run and the write. changes label names in the memory structure to thier locations taken
// from the label structure
// this function also changes immediate to zero if the register name $zero was recorded in the immediate field
void Change_Lab(MemoryLine* head, label* lb)
{
	char temp[50];	// current memory line
	MemoryLine *current = head;
	while (current != NULL) {		// find if there is a label on the immediate 
		int loc = find_name(lb, current->imm2);
		if (loc != -1) { // if true
			_itoa(loc, temp, 10); //  int --> string , puts in temp
			strcpy(current->imm2, temp); // Copy label location number to immidiate
		}
		if (strcmp(current->imm2, "$zero") == 0) // In case immidiate is &zero
		{
			strcpy(current->imm2, "0"); // Changes immidiate to "0"
		}
		current = current->next;
	}
}

  /////////////////////////////////////////////////////////////////////////
 //////////////////////////////// Main ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	// open the input file. doing so in the main function will allow us to have infinite length file names
	FILE *asembl = fopen(argv[1], "r"); 	
	if (asembl == NULL) { // in case of null file --> exit(1)
		exit(1);
	}
	label* labels = Make_Labels_List(asembl); // First iteration
	fclose(asembl); // close file
	asembl = fopen(argv[1], "r"); 	// open again for the second
	if (asembl == NULL) {  // in case of null file --> exit(1)
		exit(1);
	}
	Memory *memory = SecondRun(asembl, 1);	// Second iteration

	fclose(asembl); // close file
	asembl = fopen(argv[1], "r"); 	// open again for the second
	if (asembl == NULL) {  // in case of null file --> exit(1)
		exit(1);
	}
	Memory *memory2 = SecondRun(asembl, 2);
	fclose(asembl); // close file
	Change_Lab(memory->head, labels); // Change labels from words to numbers

	// Write Data to file imemin
	FILE* memin = fopen(argv[2], "w"); 
	if (memin == NULL)
		exit(1);
	Print_Data_To_File(memory, memin, 1);
	fclose(memin);
	// End of imemin writing

	// Write Data to file dmemin
	FILE* dmemin = fopen(argv[3], "w");
	if (dmemin == NULL)
		exit(1);
	Print_Data_To_File(memory2, dmemin, 2);
	fclose(dmemin);

	// free the memory taken by the label list and memory structure
	destroy_list_free_mem(labels);
	destroy_mem(memory);
	destroy_mem(memory2);
}