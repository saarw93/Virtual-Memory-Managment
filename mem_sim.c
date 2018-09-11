/*
saarwe
Author: Saar Weitzman
ID: 204175137
The program simulates access to the physical memory while using paging mechanism*/


#include "mem_sim.h"

int frame_available[MEMORY_SIZE / PAGE_SIZE];  //frame array to save which page is in which frame
int frame_index_counter = 0;  //counter to do FIFO in the main memory


/*prints the error received*/
void error(char *msg)
{
    perror(msg);
}

/*initialize the main memory, the swap file, the page table, and open the exec_file and the swap_file*/
struct sim_database* init_system(char exe_file_name[], char swap_file_name[] , int text_size, int data_bss_size, int heap_stack_size )
{
    if (exe_file_name == NULL)
    {
        error("exec_file doesn't exist");
        exit(0);
    }

    struct sim_database *mem = (struct sim_database*)malloc(sizeof(struct sim_database));
    mem->program_fd = open(exe_file_name, O_RDONLY);
    if( mem->program_fd == -1) {
        error("Can't open executable file");
        clear_system(mem);
        exit(0);
    }
    mem->swapfile_fd  = open(swap_file_name, O_RDWR | O_CREAT, 0777);
    if( mem->swapfile_fd == -1)
    {
        error("Can't open swap file");
        clear_system(mem);
    }

    mem->text_size = text_size;  //the code
    mem->data_bss_size = data_bss_size;  //the global variables
    mem->heap_stack_size = heap_stack_size;  //the heap and stack variables

    int i = 0;
    for(; i< text_size/PAGE_SIZE ; i++)
    {
        mem->page_table[i].V = 0;
        mem->page_table[i].D = 0;
        mem->page_table[i].P = 0;
        mem->page_table[i].frame = -1;
    }


    for(; i < NUM_OF_PAGES ; i++)
    {
        mem->page_table[i].V = 0;
        mem->page_table[i].P = 1;
        mem->page_table[i].D = 0;
        mem->page_table[i].frame = -1;
    }

    for (int j = 0; j < MEMORY_SIZE; j++)
        mem->main_memory[j] = '0';


    for (int i = 0; i < MEMORY_SIZE / PAGE_SIZE; i++)
        frame_available[i] = -1;

    char charNULL={' '};

    for(int i = 0; i < (text_size + data_bss_size + heap_stack_size) ; i++)  //initialize the swap_file with spaces
    {
        lseek(mem->swapfile_fd, i, SEEK_SET);
        write(mem->swapfile_fd, &charNULL, sizeof(char));
    }

    return mem;
}


/*loads the page where the needed address is found to the main memory and then return the char in that address*/
char load(struct sim_database *mem_sim, int address)
{
    if (check_address(mem_sim, address) == 0)
        return '\0';

    int offset = address%PAGE_SIZE;
    int page = address / PAGE_SIZE;

    if(mem_sim->page_table[page].V == 1)
    {
       int location = mem_sim->page_table[page].frame * PAGE_SIZE + offset;
       return mem_sim->main_memory[location];   //return the char was asked from the physical memory
    }

    /*from here V = 0*/
    else if(mem_sim->page_table[page].P == 0)
    {
        char *file_input;
        char temp[FRAME_SIZE];
        getFromExetuable(mem_sim, page,temp);
        file_input = temp;

        if(placeAvailable(frame_index_counter) != NO_PLACE_AVAILABLE) //the start index of the frame available in main_mem
            insert_to_main_memory(mem_sim, file_input, page);

        else    //the physical memory is full, need to check if need to move the block to swap or override it
        {
            int index = frame_available[frame_index_counter];  //index = the old page
            if(mem_sim->page_table[index].D == 0)
            {
                clear_page_v_f(mem_sim);
                insert_to_main_memory(mem_sim, file_input, page);
            }
            else // D = 1
            {
                move_to_swap(mem_sim, index);
                clear_page_v_f(mem_sim);
                insert_to_main_memory(mem_sim, file_input, page);
            }
        }
        return return_char(mem_sim, offset, page);

    }
    else  // P == 1
    {
       if (mem_sim->page_table[page].D == 0)
       {
           char *block_ptr;
           if (address > mem_sim->text_size + mem_sim->data_bss_size )  //it's in the heap_stack place, create new frame
           {
               char block[FRAME_SIZE] = {'\0'};
               block_ptr = block;
           }
           else  //it's in the data_bss place, need to read from the exec_file
           {
               char temp[FRAME_SIZE];
                getFromExetuable(mem_sim, page,temp);
               block_ptr = temp;
           }

           if (placeAvailable(frame_index_counter) != NO_PLACE_AVAILABLE) //the start index of the available frame in the main_mem
               insert_to_main_memory(mem_sim, block_ptr, page);

           else //the physical memory is full, need to check if move to swap or override
           {
               int index = frame_available[frame_index_counter]; //the old page
               if (mem_sim->page_table[index].D == 1)
                   move_to_swap(mem_sim, index);

               clear_page_v_f(mem_sim);
               insert_to_main_memory(mem_sim, block_ptr, page);
           }
           return return_char(mem_sim, offset, page);

       }
        else     // D == 1, need to read new page from swap
       {
           int index = frame_available[frame_index_counter]; //the old page
           if (mem_sim->page_table[index].D == 1)
               move_to_swap(mem_sim, index);
           char *dataFromSwap = read_from_swap(mem_sim, page);
           clear_page_v_f(mem_sim);
           insert_to_main_memory(mem_sim, dataFromSwap, page);
           return return_char(mem_sim, offset, page);
       }
    }
}


/*load the page where the needed address is found to the main memory and then store the char in that address*/
void store(struct sim_database *mem_sim, int address, char value)
{
    if (check_address(mem_sim, address) == 0)
        return;

    int offset = address%PAGE_SIZE;
    int page = address / PAGE_SIZE;


    if (address < mem_sim->text_size)
    {
        error("Can't change text");
        return;
    }

    if(mem_sim->page_table[page].V == 1)
    {
        int location = (mem_sim->page_table[page].frame * PAGE_SIZE) + offset;
        mem_sim->main_memory[location] = value;   //put the char "value" in the physical memory
        mem_sim->page_table[page].D = 1;
    }

    /*from here  V == 0*/
    else if(mem_sim->page_table[page].P == 0)
    {
        char temp[FRAME_SIZE];
         getFromExetuable(mem_sim, page,temp);
        char *file_input = temp;

        if(placeAvailable(frame_index_counter) != NO_PLACE_AVAILABLE) //the start index of the frame available in main_mem
        {
            insert_to_main_memory(mem_sim, file_input, page);
            replace_char(mem_sim, offset, page, value);
        }

        else    //the physical memory is full, need to check if need to move the block to swap or override it
        {
            int index = frame_available[frame_index_counter];  //index = the page
            if(mem_sim->page_table[index].D == 0)
            {
                clear_page_v_f(mem_sim);
                insert_to_main_memory(mem_sim, file_input, page);
                replace_char(mem_sim, offset, page, value);
            }
            else // D = 1
            {
                move_to_swap(mem_sim, index);
                clear_page_v_f(mem_sim);
                insert_to_main_memory(mem_sim, file_input, page);
                replace_char(mem_sim, offset, page, value);
            }
        }
    }
    else  // P = 1
    {
        if (mem_sim->page_table[page].D == 0)
        {
            char *block_ptr;
            if (address > mem_sim->text_size + mem_sim->data_bss_size )  //it's in the heap_stack place, create new frame
            {
                char block[FRAME_SIZE] = {'\0'};
                block_ptr = block;
            }
            else  //it's in the data_bss place, need to read from the exec_file
            {
                char temp[FRAME_SIZE];
                getFromExetuable(mem_sim, page,temp);
                block_ptr = temp;
            }
            if (placeAvailable(frame_index_counter) != NO_PLACE_AVAILABLE) //the start index of the frame available in main_mem
            {
                insert_to_main_memory(mem_sim, block_ptr, page);
                replace_char(mem_sim, offset, page, value);
            }
            else  //the physical memory is full, need to check if move to swap or override
            {
                int index = frame_available[frame_index_counter];  //index = the old page

                /*need to check if need to move the frame to the swap*/
                if (mem_sim->page_table[index].D == 1)  //need to move to swap
                    move_to_swap(mem_sim, index);
                clear_page_v_f(mem_sim);
                insert_to_main_memory(mem_sim, block_ptr, page);
                replace_char(mem_sim, offset, page, value);
            }
        }
        else     // D == 1, need to read new page from swap
        {
            int index = frame_available[frame_index_counter]; //the old page
            if (mem_sim->page_table[index].D == 1)
                move_to_swap(mem_sim, index);
            char * data_from_swap = read_from_swap(mem_sim, page);
            clear_page_v_f(mem_sim);
            insert_to_main_memory(mem_sim, data_from_swap, page);
            replace_char(mem_sim, offset, page, value);
        }
    }
}

/*close the exec_file and swap_file we opened, and free the pointer to the struct of the sim_database*/
void clear_system(struct sim_database* mem_sim)
{
    close(mem_sim->program_fd);
    close(mem_sim->swapfile_fd);
    free(mem_sim);
}


/*get the page needed from the exec_file*/
void getFromExetuable(struct sim_database *mem_sim, int page,char temp[])
{
    if(lseek(mem_sim->program_fd, page*PAGE_SIZE , SEEK_SET) == -1)   //move the pointer in the file to the place needed
        error("failed doing lseek()");
    ssize_t check = read(mem_sim->program_fd, temp, sizeof(char)*PAGE_SIZE);   //read 5 chars (when the first one is what we want)
    if(check < 0)
        error("can't open file");

}


/*inserts the wanted page to the main memory*/
void insert_to_main_memory(struct sim_database *mem_sim, char *copy, int page)
{
    for (int i = 0; i < FRAME_SIZE; i++)
        mem_sim->main_memory[(FRAME_SIZE*frame_index_counter)+i] = copy[i];

    frame_available[frame_index_counter] = page;   //update the frame_available table
    mem_sim->page_table[frame_available[frame_index_counter]].V = 1; //update the Valid char in the index needed in page table
    mem_sim->page_table[frame_available[frame_index_counter]].frame = frame_index_counter;  //update the frame in the index needed in page table
    frame_index_counter++;

    if (frame_index_counter == MEMORY_SIZE/FRAME_SIZE)// here we do the FIFO of the main memory, if we got to index 4 go back to index 0
        frame_index_counter = 0;
}


/*move the the page that was changed while it was in the main memory to the swap_file*/
void move_to_swap(struct sim_database *mem_sim, int page)
{
    char temp[FRAME_SIZE];
    for (int i = 0; i < FRAME_SIZE; i++)
    {
        temp[i] = mem_sim->main_memory[frame_index_counter*FRAME_SIZE+i];
    }

    if(lseek(mem_sim->swapfile_fd, page*PAGE_SIZE, SEEK_SET) == -1)
        error("failed doing lseek()");
    if(write(mem_sim->swapfile_fd, temp, sizeof(char)*FRAME_SIZE) == -1) //read 5 chars (when the first one is what we want)
        error("can't write to file");
    mem_sim->page_table[frame_available[frame_index_counter]].D = 1;
}


/*brings the page needed from the swap file*/
char* read_from_swap(struct sim_database *mem_sim, int page)
{
    char temp[FRAME_SIZE];

    if(lseek(mem_sim->swapfile_fd, page*PAGE_SIZE, SEEK_SET) == -1)
        error("failed doing lseek()");
    if(read(mem_sim->swapfile_fd, temp, sizeof(char)*FRAME_SIZE) == -1)
        error("can't read from file");
    char *ptr = temp;
    return ptr;
}


/*update the Valid bit to 0 and frame number to -1 when a page goes out from the main memory*/
void clear_page_v_f(struct sim_database *mem_sim)
{
    mem_sim->page_table[frame_available[frame_index_counter]].V = 0;
    mem_sim->page_table[frame_available[frame_index_counter]].frame = -1;
}


/*returns the char needed. we use it only in the load function*/
char return_char(struct sim_database *mem_sim, int offset, int page)
{
int location = mem_sim->page_table[page].frame * PAGE_SIZE + offset;
return mem_sim->main_memory[location];   //return the char was asked from the physical memory
}


/*replace the new char value with the old char value. we use it only in the store function*/
void replace_char(struct sim_database *mem_sim, int offset, int page, char value)
{
    int location = mem_sim->page_table[page].frame * PAGE_SIZE + offset;
    mem_sim->main_memory[location] = value;
    mem_sim->page_table[page].D = 1;
}

/*check if the place we need to insert the new page to the main memory is available or taken by another page*/
int placeAvailable(int index_to_add)
{
    if(frame_available[index_to_add] == -1)
        return 1;
    return NO_PLACE_AVAILABLE;
}


/*checks that the address we get in the main is a valid address indeed*/
int check_address(struct sim_database *mem_sim ,int address)
{
    int last_address = mem_sim->text_size + mem_sim->data_bss_size + mem_sim->heap_stack_size;
    if (address < 0 || address >= last_address)
    {
        error("Invalid address");
        return 0;
    }
    return 1;
}


/**************************************************************************************/
void print_memory(struct sim_database* mem_sim)
{
	int i;
	printf("\n Physical memory\n");
	for(i = 0; i < MEMORY_SIZE; i++) 
	{
		printf("[%c]\n", mem_sim->main_memory[i]);
	}
}
/************************************************************************************/
void print_swap(struct sim_database* mem_sim) 
{
	char str[PAGE_SIZE];
	int i;
	printf("\n Swap memory\n");
	lseek(mem_sim->swapfile_fd, 0, SEEK_SET); // go to the start of the file
	while(read(mem_sim->swapfile_fd, str, PAGE_SIZE) == PAGE_SIZE) 
	{
		for(i = 0; i < PAGE_SIZE; i++) 
		{
			printf("[%c]\t", str[i]);
		}
		printf("\n");
	}
}
/***************************************************************************************/
void print_page_table(struct sim_database* mem_sim) 
{
	int i;
	printf("\n page table \n");
	printf("Valid\t Dirty\t Permission \t Frame\n");
	for(i = 0; i < NUM_OF_PAGES; i++)
	{
		printf("[%d]\t[%d]\t[%d]\t[%d]\n", mem_sim->page_table[i].V,
		mem_sim->page_table[i].D,
		mem_sim->page_table[i].P, mem_sim->page_table[i].frame);
	}
}

