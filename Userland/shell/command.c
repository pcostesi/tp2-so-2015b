#include <stdlib.h>
#include <command.h>
#include <stdint.h>
#include <string.h>
#include <shell.h>
#include <stdio.h>
#include <libc.h>

struct ps_list{
	char *name;
	int pid;
	char status;  /*check*/
};

/*****Commands functions*****/

int help(char *argv[], int argc) 
{
	int cmd_index;
	cmd_entry* table = get_command_table();
	if (argc == 1) {
		cmd_index = get_cmd_index(argv[0]);
		if (cmd_index != -1) {
			printf("%s", table[cmd_index].help);
		} else {
			help_error_print(table);
		}
	} else if (argc == 0) {
		help_error_print(table);
	}

	return 0;
}

int echo(char** args, int argc)
{
	int i;
	for(i = 0; i < argc; i++)
	{
		printf("%s ", args[i]);
	}
	putc('\n');
	return 0;
}
int clear(char** args, int argc)
{
    ioctl(STDOUT, IOCTL_CLR,    (void *) 0);
    ioctl(STDOUT, IOCTL_MOVE,   (void *) 0);
	return 0;
}
int commands(char** args, int argc)
{
	printf("Available commands are: \n");
	print_commands();
	return 0;
}

int date(char** args, int argc)
{
	struct rtc_time time_struct;
	gettime(&time_struct);
	printf("%d/%d/%d\n",
		time_struct.day,
		time_struct.mon,
		time_struct.year
	);
	return 0;
}
int time(char** args, int argc)
{
	struct rtc_time time_struct;
	gettime(&time_struct);
	printf("%d:%d:%d\n",
		time_struct.hour,
		time_struct.min,
		time_struct.sec
	);
	return 0;
}
int set_date(char** args, int argc)
{
	struct rtc_time time_struct;
	int days, months, years;

	gettime(&time_struct);
	if(argc <1){
		fprintf(STDERR, INVALID_DATE);
		return 0;
	}
	if(!parse_date(args[0], &days, &months, &years)){
		fprintf(STDERR, INVALID_DATE);
		return 0;

	}
	time_struct.year = years;
	time_struct.mon = months;
	time_struct.day = days;

		return 0;settime(&time_struct);

}
int set_time(char** args, int argc)
{
	struct rtc_time time_struct;
	int seconds, minutes, hours;

	gettime(&time_struct);
	if(argc < 1){
		fprintf(STDERR, INVALID_TIME);
		return 0;
	}	
	if(!parse_time(args[0], &seconds, &minutes, &hours)){
		fprintf(STDERR, INVALID_TIME);
		return 0;
	}


	time_struct.sec = seconds;
	time_struct.min = minutes;
	time_struct.hour = hours;

	settime(&time_struct);
	return 0;

}
 int halt_system(char** args, int argc)
{
	halt();
	return 0;
}
int print_ascii_table(char** args, int argc)
{
	int i;

	for (i = 0; i < 128; i++){
		printf("%x: %c", i, i < 32 ? ' ' : i);

		if (i % 8) {
			printf(" \t| ");
		} else {
    		putc('\n');
    	}
	}

    i = putc('\n');
    printf("last call to putc returned %d\n", i);
	return 0;
}

int setcolor(char** args, int argc)
{
	int fore, back;
	if (argc != 2) {
		fore = atoi(args[0]);
		
		if (fore == 42) {
			printf("pid is %d\n", getpid());
			kill(getpid(), 9);
		}

		printf("usage: setcolor foreground background\n");
		printf("\nColors are:\n");

		printf("\tdarker       | lighter\n");
		printf("\tblack:     0 | light gray:   8\n");
		printf("\tblue:      1 | celurean:     9\n");
		printf("\tgreen:     2 | light green: 10\n");
		printf("\tcyan:      3 | light cyan:  11\n");
		printf("\tred:       4 | light red:   12\n");
		printf("\tmagenta 1: 5 | magenta 2:   13\n");
		printf("\tbrown:     6 | yellow:      14\n");
		printf("\tgray 1:    7 | white:       15\n");
		printf("\nnote: you might not choose two of the same.\n");
		return 0;
	}

	fore = atoi(args[0]);
	back = atoi(args[1]);

	if (fore == back) {
		fprintf(STDERR, "Aaaand you chose two of the same...\n Nope.\n");
		return 0;
	}

	if (fore >= 16 || back >= 16 || fore < 0 || back < 0) {
		fprintf(STDERR, "Invalid parameter: %d, %d\n", fore, back);
		return 0;
	}

	ioctl(STDOUT, IOCTL_SET_COLOR, IOCTL_COLOR(fore, back));
	clear(args, argc);
	return 0;
}

int kill_cmd(char** args, int argc)
{
	if (argc != 1){
		printf(KILL_ERROR);
		return 0;
	}

	int pid = atoi(args[0]);
	if(pid == -1){
		printf(KILL_ERROR);
		return 0;
	}
	if(kill(pid, 9)){
		printf("Killed pid: %d", pid);
	} else {
		printf("No process with pid: %d found", pid);	
	}
	return 0;
}

int ps_cmd(char** args, int argc)
{
	int count, indx;
	static struct ps_list processes[19]; //19 ain't magic! maximum lines in terminal
	printf("Name\tPID\tStatus\n");
	//count = syso get processes()	
	count = 0;	
	for (indx = 0; indx < count; indx++){
		printf("%s\t%d\t%c\n", processes[indx].name, processes[indx].pid, processes[indx].status);
	}
	return 0;
}


int consumer_cmd(char** args, int argc)
{
	int aux = s_to_i(args[0]);
	if(argc != 2 || aux < 1 || strlen(args[2]) == 0){
		printf("Invalid arguments, use help for more details \n");
		return 0;
	}
	return -1; // excec(&producer);
}


int producer_cmd(char** args, int argc)
{
	int fd = s_to_i(args[0]);
	int size = s_to_i(args[1]);
	if(argc != 2 || fd < 1 || size < 1){
		printf("Invalid arguments, use help for more details \n");
		return 0;
	}
	return -1; // excec(&consumer);	
}


int exec_string_malloc(char** args, int argc)
{
    
    while (argc)
    {
        void * s = malloc(strlen(*args)+1);
        if (s){
            strcpy(s, *args++);
            printf("Malloc string \"%s\" at address %d\n", s, s); 
        }else{
            printf("Not enough heap for \"%s\"", *args);
        }
        argc--;
    }
    return 0;
}

int exec_malloc(char** args, int argc)
{

	void * s = malloc(s_to_i(*args));
    if (s){
        printf("Malloc %d bytes at address %d\n", s_to_i(*args), s);
    }else{
        printf("Not enough heap for %d bytes\n", s_to_i(*args));
    }
    return 0;
}

int exec_print_heap(char** args, int argc)
{

    block* cur_block = get_base_block();
    
    if (!cur_block){
        printf("Heap is empty bro\n");
        return 1;
    }

    while (cur_block){
        printf("Block address: %d\nData address: %d\nBlock size: %d\nPrev block: %d\nNext block: %d\nFree: %d\nString: %s\n\n", cur_block, cur_block + 1, cur_block->size, cur_block->prev, cur_block->next, cur_block->free, cur_block + 1);
        cur_block = cur_block->next;
    }

    return 0;
}

int exec_free(char** args, int argc)
{

    while (argc)
    {
        uint64_t dir = s_to_i(*args++);
        free((void*)dir);
        printf("Freeing address %d\n", (void*)dir);
        argc--;
    }

    return 0;
}


int close_pipe(char** args, int argc)
{
	if(argc != 1){
		printf("Invalid arguments, please refer to help");
		return 0;
	}
	cpipe(atoi(args[0]));
	return 0;
}



int get_pipes(char** args, int argc)
{
	int pipes[80];
	int aux = 0;
	gpipes((int **) &pipes);
	printf("Los pipes activos son: \n" );
	while(pipes[aux] != NULL){
		printf("%d, \n", pipes[aux]);
		aux++;
	}
	return 0;
}



/*****Auxiliary functions for commands*****/






//Receives input string, parses it for a date, returns 1 if valid, 0 if not
int parse_date(char* date_string, int* days, int*months, int*years)
{
	int len =0;
	len = strlen(date_string);
	if(len !=8)
	{
		return 0;
	}
	if(date_string[2]!= '/' || date_string[5] != '/')
	{
		return 0;	
	}
	//days
	if( !is_num(date_string[0]) || !is_num(date_string[1]))
	{
		return 0;
	}
	//months
	if( !is_num(date_string[3]) || !is_num(date_string[4]) )
	{
		return 0;
	}
	//years
	if( !is_num(date_string[6]) || !is_num(date_string[7]) )
	{	
		return 0;
	}
	*days = ((int)date_string[0]-'0' )*10+(int)(date_string[1]-'0'); 
	*months = ((int)date_string[3]-'0' )*10+(int)(date_string[4]-'0');
	*years = ((int)date_string[6]-'0' )*10+(int)(date_string[7]-'0'); 	
	return valid_date(*days, *months, *years);
	return 0;

}

int parse_time(char* time_string, int* seconds, int*minutes, int*hours)
{	
	int len =0;
	len = strlen(time_string);
	if(len !=8)
	{
		return 0;
	}
	if(time_string[2]!= ':' || time_string[5] != ':')
	{
		return 0;	
	}
	//seconds
	if( !is_num(time_string[0]) || !is_num(time_string[1]))
	{
		return 0;
	}
	//minutes
	if( !is_num(time_string[3]) || !is_num(time_string[4]))
	{
		return 0;
	}
	//hours
	if( !is_num(time_string[6]) || !is_num(time_string[7]))
	{
		return 0;
	}
	*seconds = (time_string[0]-'0')*10 + (time_string[1]-'0');
	*minutes = (time_string[3]-'0')*10 + (time_string[4]-'0');
	*hours = (time_string[6]-'0')*10 + (time_string[7]-'0');
	return valid_time(*seconds, *minutes, *hours);
	return 0;
}


int is_num(char c)
{
	if( (c >= '0') && (c<+ '9'))
	{
		return 1;
	}
	return 0;
	return 0;
}

int valid_time(int sec, int min, int hrs)
{

	if( sec >60 || sec < 0	||
		min >60 || min < 0	||
		hrs >24 || sec < 0)
	{
		return 0;
	}
	return 1;
	return 0;

}

int valid_date(int day, int month, int year)
{
	unsigned short monthlen[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (month < 1 || day < 1 || month > 12){
		return 0;
	}
	if (is_leap_year(year) && month == 2) {
		monthlen[1]++;
	}
	if (day>monthlen[month-1] || day < 1) {
		return 0;
	}
	return 1;
	return 0;
}

int is_leap_year(int year)
{
	return ((!(year%4) && (year%100)) || !(year%400));
}	

int getchar_cmd(char *argv[], int argc) 
{
	printf("Please type in a character\n");
	char c = getc();
	printf("You pressed: %c\n", c);
	return 0;
}
int printf_cmd(char *argv[], int argc) 
{
	printf("Testing printf...\n\n");
	printf("Printing an integer: %d\n", 99);
	printf("Printing a string: %s\n", "This is a real cool string!");
	printf("Printing in uppercase hexadecimal notation: %x\n", 0x55fa);
	printf("Printing a single char: %c\n", 'z');
	return 0;
}
int scanf_cmd(char *argv[], int argc) 
{
	int n;
	char vec[SCANF_MAX_STR_BUFFER];

	printf("Welcome to scanf user test:\n ");
	printf("And so, the trial begins...\n");
	printf("Please type in a number: \n");
	scanf("%d", &n);
	printf("You typed in: %d\n\n", n);
	reset_vect(vec);
	
	printf("Trial number 2...type in a short text: ");
	scanf("%s", vec);
	printf("You typed in: %s\n\n", vec);
	reset_vect(vec);

	printf("Trial number 3...Please type in a single character: ");
	scanf("%c", vec);
	printf("\nYou typed: %c\n\n", vec[0]);
	reset_vect(vec);

	printf("And so, the final trial begins:\n");
	printf("Are you ready? Type in Y or N\n");
	scanf("%s", vec);
	reset_vect(vec);
	printf(" Actually your response was irrelevant, proceeding with last trial...\n ");
	printf("Type in string format, your credit card number followed by verification code\n" );
	scanf("%s", vec);
	printf("\nYour fake card data was: %s\n\n", vec);
	reset_vect(vec);
	return 0;
}
int reset_vect(char vec[])
{
	int i;
	for(i=0; i<50; i++ )
	{
		vec[i] = 0;
	}
	return 0;
}


int help_error_print()
{
	printf("\nInvoke help as follows: \"help \"command_name\"\".\nTo see list of available commands type \"commands\"");	
	return 0;
}

void producer(int fd, char* msg) 
{
	opipe(fd);
	wpipe(fd, (void *)msg, strlen(msg));
	return;
	cpipe(fd);
	kill(getpid(), 9);
}


void consumer(int fd, int size ) 
{
	char read[200];
	opipe(fd);
	int read_count = rpipe(fd, (void *)read, size);
	while( read_count != -1) {
		if(read_count != 0){
			rpipe(fd, read, size);
			printf("Managed to read: %d characters, which were: \"%c\"\n", read_count, read);
		}
	}
	cpipe(fd);
	kill(getpid(), 9);
}