#include <shell.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <libc.h>
#include <command.h>


char shell_buffer[SHELL_BUFFER_SIZE];
int curr_pos =0;

cmd_entry cmd_table[23];
int cmd_count = (sizeof(cmd_table) / sizeof(cmd_entry));

void init_shell()
{

	clean_buffer();
	initialize_cmd_table(); 
	prnt_welcome_msg();
	print_shell_text();


}

void print_shell_text()
{
	printf(SHELL_TEXT);
}

void print_shell_error()
{
	fprintf(STDERR, "We are sorry, \"%s\" command does not exist. \nFor full list of commands please type: \"commands\"\n", shell_buffer);
}

int update_shell()
{
	char key;
	key = getc();

	if(key == 0) {
		return 0;
	}

	if(key == '\n') {
		putc('\n');
		shell_buffer[curr_pos] = 0;
		if (excecute_command(shell_buffer) == -1) {
			return 0;
		}
		print_shell_text();
		clean_buffer();

	} else if (key == '\b') {
		if(curr_pos > 0){
			putc('\b');
			putc(' ');
			putc('\b');
			curr_pos--;
			shell_buffer[curr_pos] = '\0';
		} else {
			beep();
		}

	} else if(curr_pos >= SHELL_BUFFER_SIZE-2) {
		//sound beep
		beep();
		return 0;

	} else {
		putc(key);
		shell_buffer[curr_pos] = key;
		curr_pos++;
	}
	return 1;
}

int excecute_command(char* buffer)
{
	int argc,
		cmd_len;
	char * args[64];

	int cmd_no = parse_command(buffer);
	if( cmd_no == -1)
	{
		print_shell_error();
		return 0;
	}

	cmd_len = strlen(cmd_table[cmd_no].name);
	argc = get_arguments(buffer + cmd_len, args);
	return cmd_table[cmd_no].func(args, argc);
}

int parse_command(char* buffer)
{
	int i = 0,
		cmd_no = -1;

	if(buffer[0] == '\0') {
		return -1;
	}

	while (cmd_no == -1 && i < cmd_count) {
		if (substr(cmd_table[i].name, buffer)) {
			cmd_no = i;
		}
		i++;
	}

	i = strlen(cmd_table[cmd_no].name);
	char next = buffer[i];

	if(next == ' ' || next == '\0'){
		return cmd_no;
	}

	return -1;
}



unsigned int get_arguments(char* buffer, char ** args)
{
	int arg = 0,	// current argument
		i = 0;		// current buffer index

	char * ptr = NULL;

	while (buffer[i] != '\0' && buffer[i] == ' ') {
		i++;
	}

	while (buffer[i] != '\0' && arg < 64) {
		// guard condition
		if (buffer[i] == ' ') {
			ptr = NULL;
			buffer[i] = '\0';
			i++;
			continue;
		}

		if (ptr == NULL) {
			ptr = buffer + i;
			args[arg] = ptr;
			arg++;
		}

		i++;
	}
	return arg;
}


void clean_buffer()
 {
	int i = 0;
	while(i < SHELL_BUFFER_SIZE && i < curr_pos)
	{
		shell_buffer[i] = '\0';
		i++;
	}
	curr_pos = 0;
}


void prnt_welcome_msg()
 {
	printf(WELCOME_TEXT);
}

//Returns the index of the command in the command table, if such command does not exist, returns -1
int get_cmd_index(char * cmd_name) 
{
	int i;
	for( i=0; i < cmd_count; i++) {
		if (strcmp(cmd_name, cmd_table[i].name) == 0) {
			return i;
		}
	}
	return -1;
}


cmd_entry* get_command_table()
{
	return cmd_table;
}


//Prints list of available commands
void print_commands()
{
	int i;
	for( i=0; i < cmd_count; i++)
	{
		printf("\t%s\n", cmd_table[i].name);
	}

}

int get_cmd_count()
{
	return cmd_count;
}

void initialize_cmd_table()
{
	cmd_table[0].name = "echo";
	cmd_table[1].name = "clear";
	cmd_table[2].name = "date";
	cmd_table[3].name = "time";
	cmd_table[4].name = "setdate";
	cmd_table[5].name = "settime";
	cmd_table[6].name = "getchar";
	cmd_table[7].name = "printf";
	cmd_table[8].name = "scanf";
	cmd_table[9].name = "help";
	cmd_table[10].name = "halt";
	cmd_table[11].name = "commands";
	cmd_table[12].name = "printascii";
	cmd_table[13].name = "setcolor";
	cmd_table[14].name = "kill";
	cmd_table[15].name = "producer";
	cmd_table[16].name = "smalloc";
	cmd_table[17].name = "malloc";
	cmd_table[18].name = "free";
	cmd_table[19].name = "pheap";
	cmd_table[20].name = "consumer";
	cmd_table[21].name = "pipeclose";
	cmd_table[22].name = "getpipes";

	cmd_table[0].func = &echo;
	cmd_table[1].func = &clear;
	cmd_table[2].func = &date;
	cmd_table[3].func = &time;
	cmd_table[4].func = &set_date;
	cmd_table[5].func = &set_time;
	cmd_table[6].func = &getchar_cmd;
	cmd_table[7].func = &printf_cmd;
	cmd_table[8].func = &scanf_cmd;
	cmd_table[9].func = &help;
	cmd_table[10].func = &halt_system;
	cmd_table[11].func = &commands;
	cmd_table[12].func = &print_ascii_table;
	cmd_table[13].func = &setcolor;
	cmd_table[14].func = &kill_cmd;
	cmd_table[15].func = &producer_cmd;
	cmd_table[16].func = &exec_string_malloc;
	cmd_table[17].func = &exec_malloc;
	cmd_table[18].func = &exec_free;
	cmd_table[19].func = &exec_print_heap;
	cmd_table[20].func = &consumer_cmd;
	cmd_table[21].func = &close_pipe;
	cmd_table[22].func = &get_pipes;

	cmd_table[0].help = "Echo repeats the input string following echo statement \n example: \"echo Hello I am using echo\".\n";
	cmd_table[1].help = "Clears the screen, uses no arguments, therefore will ignore any ones received.\n";
	cmd_table[2].help = "Prints current system date on screen with format: \"dd/mm/yy\".\n";
	cmd_table[3].help = "Prints current system time on screenwith format: \"hs:mm:ss\".\n";
	cmd_table[4].help = "Sets system date, format must be dd/mm/yyyy.\nExample: \"setdate 05/05/2015\".\n";
	cmd_table[5].help = "Sets system time, format must be ss:mm:hh.\n Example: \" settime 10:59:22\".\n";
	cmd_table[6].help = "Test command for directive board to test getchar() functionality.\n";
	cmd_table[7].help = "Test command for supreme leaders to test printf() functionality .\n";
	cmd_table[8].help = "Test command for the High Command to test scanf() functionality.\n";
	cmd_table[9].help = "Displays information about following command, syntaxt: \"help \"command_name\"\".\n";
	cmd_table[10].help = "Halts the system.\n";
	cmd_table[11].help = "Displays list of available commands.\n";
	cmd_table[12].help = "Prints entire list of ascii characters in order.\n";
	cmd_table[13].help = "Sets the console color (duh).\n";
	cmd_table[14].help = "Terminates process with selected id, format is: \"kill id\".\n";
	cmd_table[15].help = "Initializes a producer task, input is: \"producer fd message\". \n";
	cmd_table[16].help = "Test function to instance memory for a given string and returns the virtual address. \n";
	cmd_table[17].help = "Receives the amount of bytes to be insanced, and return the virtual address.\n";
	cmd_table[18].help = "Receives the virtual addresses to be freed.\n";
	cmd_table[19].help = "Prints current heap.\n";
	cmd_table[20].help = "Initializes a consumer task, input is: \"producer fd numberToRead\". \n";
	cmd_table[21].help = "Closes the Pipe, format is: \"pipeclose fd\".\n";
	cmd_table[22].help = "Returns the current open pipes. Receives no argumentes, ignores any given.\n";
}	
