saarwe
204175137 Saar Weitzman

This program simulates access to the physical memory while using paging mechanism

how to install the program:
open linux terminal, navigate to the folder containing ex5
using the "cd" command (confirm it by using ls command)
incase you want to use the makefile, type make and the program will
automaticily be compiled, if you don't, type gcc -Wall mem_sim.c main.c -o ex5


to activate program:
open linux terminal, navigate to ex5 executeable file
location using "cd" command (confirm it using ls command) and type: ./ex5


there are 2 structs in the program - sim_database, page_descriptor, array of which frame saves which page and frame counter

------------------program functions -------------------------------
* incase of memory allocation failure, program will exit with
message "Memory allocation failed"


struct sim_database* init_system(char exe_file_name[], char swap_file_name[] , int text_size, int data_bss_size, int heap_stack_size);
input: the names of execute and swap files, the number of text addreses, data_bss and heap_stack in the simulator proogram
output: returns a pointer to sim_database 


char load(struct sim_database* mem_sim , int address);
input: sim_database and logical address
output: loads the needed char from the physical memory and return it


void store(struct sim_database* mem_sim , int address, char value);
input: sim_database, logical address, value (char) to store in main memory
output: stores the value (char) in the physical memory


void print_memory(struct sim_database* mem_sim);
input: sim_database
output: prints the main memory


void print_swap (struct sim_database* mem_sim);
input: sim_database
output: prints the swap file


void print_page_table(struct sim_database* mem_sim);
input: sim_database
output: prints the page_descriptor table


void clear_system(struct sim_database* mem_sim);
input: sim_database
output: closes the fds and frees the pointer


int placeAvailable(int index_to_add);
input: integer between 0 and (MEMORY_SIZE / PAGE_SIZE)- 1
output: return NO_PLACE_AVAILABLE (-2) if there is no available place in the frame_available array


void getFromExetuable(struct sim_database *mem_sim, int page,char temp[]);
input: sim_database, page number, the array to put the block from the exec_file
output: return the whole page from execute file


char* read_from_swap(struct sim_database *mem_sim, int page);
input: sim_database, page number
output: return the whole page from swap file


void move_to_swap(struct sim_database *mem_sim, int page);
input: sim_database, page number
output: moving the wanted page the swap file


char return_char(struct sim_database *mem_sim, int offset, int page);
input: sim_database, offset, page
output: returns the value (char) in main_memory[page*PAGE_SIZE + offset]


void replace_char(struct sim_database *mem_sim, int offset, int page, char value);
input: sim_database, offset, page, value
output: replace the value (char) in main_memory[page*PAGE_SIZE + offset]


void insert_to_main_memory(struct sim_database *mem_sim, char *copy, int page);
input: sim_database, copy, page
output: inserts the copy to the page that been given


void clear_page_v_f(struct sim_database *mem_sim);
input: sim_database
output: clear the valid to 0 and frame to -1 when the page goes out from
the main memroy


int check_address(struct sim_database *mem_sim ,int address);
input: sim_database, address
output: checks if the address is valid or iinvalid in the addresses zone.


void error(char* msg);
input: char[] - error messege
output: prints in stderr format the error


int main() command:
input: none
output: This program simulates access to the physical memory while using paging mechanism
that has been fully tested with valgrind to test for memory
leak, no leak was found.
