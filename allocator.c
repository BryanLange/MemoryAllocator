#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------------- 
   About: This program manages a contiguous region of memory.
   	  Memory size specified at execution.
   Commands: RQ P1 200600 B (Request ProcessName Size Approach)
   	     RL P0 (Release ProcessName)
   	     C (Compaction)
   	     STAT (Status Report)
   	     QUIT
   Compile: $ gcc allocator.c -o allocator
   Execute: $ ./allocator 1048576
----------------------------------------------------------------------------*/

#define READLENGTH 20


struct Block {
	int id;
	int start;
	int size;
	struct Block *next;
};


// creates and returns a new memory block with given parameters
struct Block * newBlock(int pProcess, int pStart, int pSize) 
{
	struct Block *temp = (struct Block *)malloc(sizeof(struct Block));
	
	temp->id = pProcess;
	temp->start = pStart;
	temp->size = pSize;
	temp->next = NULL;
	
	return temp;
} // end newBlock()



// initialize memory list
// returns dummy node pointing to free block with size of limit
struct Block * initialize(int limit) 
{
	struct Block *dummy = newBlock(-2, -2, limit);
	struct Block *temp = newBlock(-1, 0, limit);

	dummy->next = temp;

	return dummy;
} // end initialize()



// helper function, joins all adjacent blocks of free memory
void mergeFree(struct Block *nav) 
{
	while(nav->next != NULL) {
		if((nav->id == -1) && (nav->next->id == -1)) {
			nav->size += nav->next->size;
			nav->next = nav->next->next;
		}
		else {
			nav = nav->next;
		}
	}
} // end mergeFree()



// compact unused holes of memory into one block
// processes moved to low end of memory, free moved to high end
void compaction(struct Block* memory) 
{
	struct Block *nav = memory;
	int tid, tsize;
	
	while(nav->next != NULL) {
		if(nav->id == -1) {
			tid = nav->next->id;
			tsize = nav->next->size;

			nav->next->id = nav->id;
			nav->next->size = nav->size;

			nav->id = tid;
			nav->size = tsize;

			nav->next->start = nav->start + nav->size;
		}
		nav = nav->next;
	}

	mergeFree(memory);
} // end compaction()



// release a specified block of memory, if exists
void release(struct Block *memory, char *line) 
{
	if(line[0] == 'P') {
		struct Block *nav = memory;
		int found = 0;
		int process = line[1] - '0'; // convert char to int

		while(nav->next != NULL) {
			if(nav->next->id == process) {			
				found = 1;
				nav->next->id = -1;
				mergeFree(nav);
				break;
			}
			nav = nav->next;
		}

		if(found == 0) {
			printf("Process P%d not found.\n", process);
		}
	}
	else {
		printf("Invalid process name: %s\n", line);
	} 
} // end release()



// allocate smallest hole in memory to specified process
// search entire memory first
// display error message if insufficient space, or specified process
//	 is already in memory
void bestFit(struct Block *memory, int process, int reqSize) 
{
	struct Block *nav = memory;
	struct Block *best = NULL;

	while(nav != NULL) {
		if(nav->id == process) {
			printf("Process P%d already exists.\n", process);
			return;
		}
		else if(best == NULL) {
			if((nav->id == -1) && (nav->size >= reqSize)) {
				best = nav;
			} 
		} 
		else {
			if((nav->id == -1) && (nav->size >= reqSize) &&
			   (nav->size < best->size)) {
				best = nav;
			}
		}
		nav = nav->next;
	}

	if(best == NULL) {
		printf("Insufficent memory, request rejected.\n");
	} 
	else {
		if(reqSize == best->size) {
			best->id = process;
		} 
		else {
			struct Block* new = newBlock(-1, best->start+reqSize,
											 best->size-reqSize);
			best->id = process;
			best->size = reqSize;

			new->next = best->next;
			best->next = new;
		}
	}
} // end bestFit()



// parse memory request for which approach to use
// only 'best fit' supported now
void parseRequest(struct Block *memory, char *line) 
{
	char *process, *approach=NULL, *eptr;
	int reqSize;

	process = strtok(line, " ");
	reqSize = (int)strtol(strtok(0, " "), &eptr, 10);
	approach = strtok(0, "\n");

	if(process[0] != 'P') {
		printf("Invalid process name: %s\n", line);
	} else if(approach==NULL) {
		printf("No approach specified. (B/F/W)\n");
	} else if(reqSize <= 0) {
		printf("Zero memory requested.\n");
	} else if(strcmp(approach, "B") == 0) {
		// (char - '0') converts char to int
		bestFit(memory, process[1] - '0', reqSize);  
	} else if(strcmp(approach, "F") == 0) {
		printf("First fit not supported.\n");
	} else if(strcmp(approach, "W") == 0) {
		printf("Worst fit not supported.\n");
	} else {
		printf("Invalid approach specified.\n");
	}
} // end parseRequest()


 
// display memory information
// report blocks of free and allocated memory, low to high index
void statusReport(struct Block *memory) 
{
	struct Block *nav = memory->next;
	int limit = memory->size;	

	while(nav != NULL) {
		int end = nav->start + nav->size-1;

		printf("\nAddresses [%d : ", nav->start);
		
		if(end == limit-1) {
			printf("END] ");
		} else {
			printf("%d] ", end);
		}

		if(nav->id == -1) {
			printf("Free\n");
		} else {
			//printf("Process P%d\n", nav->id);
			printf("Process P%d\n", nav->id);
		}

		nav = nav->next;
	}
	printf("\n");
} // end statusReport()



// frees malloc memory
void flush(struct Block* memory) 
{
	struct Block* temp;
	while(memory != NULL) {
		temp = memory;
		memory = memory->next;
		free(temp);
	}
} // end flush()




int main(int argc, char* argv[]) 
{
	// check for valid command line arguments	
	if(argc != 2) {
		printf("Invalid number of arguments.\n");
		return 1;
	}


	char line[READLENGTH];
	char *command, *eptr;
	int limit = (int)strtol(argv[1], &eptr, 10);
	int run=1;

	
	// initialize memory list
	struct Block *memory = initialize(limit);


	while(run == 1) {
		printf("allocator>");
		fgets(line, READLENGTH, stdin); // console input


		// for loop used to parse which command was entered
		for(int i=0; i<READLENGTH; i++) {	

			
			// request or release entered
			if(line[i] == ' ') { 
				
				command = strtok(line, " ");
				
				if(strcmp(command, "RQ") == 0) {
					parseRequest(memory, strtok(0, "\n"));
				}

				else if(strcmp(command, "RL") == 0) {
					release(memory, strtok(0, "\n"));
				}

				else {
					printf("Invalid command or case.\n");
				}
				break;
			}


			// single command entered
			if(line[i] == '\n') { 
				
				command = strtok(line, "\n");
				
				if(strcmp(command, "QUIT") == 0) {
					run = 0;
				}

				else if(strcmp(command, "C") == 0) {
					compaction(memory);
				} 

				else if(strcmp(command, "STAT") == 0) {
					statusReport(memory);
				} 

				else {
					printf("Invalid command or case.\n");
				}
				break;
			} 
		}
	} // end menu loop


	// free allocated memory
	flush(memory);

	return 0;
} // end main()
