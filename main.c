/*
saarwe
Author: Saar Weitzman
ID: 204175137*/

#include "mem_sim.h"

main()
{
	char val;
	struct sim_database *mem_sim = init_system ("exec_file.txt", "swap_file" ,25, 50, 50);
	val = load (mem_sim , 64);
	val = load (mem_sim , 66);
	val = load (mem_sim , 2);
	store(mem_sim , 98,'X');
	val = load (mem_sim ,16);
	val = load (mem_sim ,70);
	store(mem_sim ,32,'Y');
	store (mem_sim ,15,'Z');
	val = load (mem_sim ,23);
	print_memory(mem_sim);
	print_swap(mem_sim);
	clear_system(mem_sim);
}



/*{
    char val;
    struct sim_database * mem_sim = init_system ("exec_file", "swap" ,25, 50, 50);
    val = load(mem_sim,98);
    printf("%c\n",val);
    store(mem_sim , 98,'X');
    store(mem_sim , 120,'c');
    store(mem_sim , 121,'h');
    store(mem_sim , 122,'e');
    store(mem_sim , 123,'c');
    val = load (mem_sim , 120);
    printf("%c\n",val);
    val = load (mem_sim , 66);
    printf("%c\n",val);
    val = load (mem_sim , 2);
    printf("%c\n",val);
    store(mem_sim , 97,'X');
    val = load (mem_sim ,16);
    printf("%c\n",val);
    val = load (mem_sim ,70);
    printf("%c\n",val);
    store(mem_sim ,32,'Y');
    store (mem_sim ,15,'Z');
    val = load (mem_sim , 98);
    printf("%c\n",val);
    val = load (mem_sim ,23);
    printf("%c\n",val);
    val = load (mem_sim , 66);
    printf("%c\n",val);
    store(mem_sim , 124,'k');

    print_memory(mem_sim);
    print_page_table(mem_sim);
    print_swap(mem_sim);
    clear_system(mem_sim);
}*/







/*{
    char val;
    struct sim_database* mem_sim = init_system ("exec_file.txt", "swap_file.txt" ,25, 50, 50);
    val = load (mem_sim , 64);
    printf("%c\n", val);
    val = load (mem_sim , 66);
    printf("%c\n", val);
    store(mem_sim ,64,'Y');
    val = load (mem_sim , 70);
    val = load (mem_sim , 77);
    val = load (mem_sim , 83);
    val = load (mem_sim , 84);
    printf("%c\n", val);
    store (mem_sim ,15,'Z');
    store (mem_sim ,54,'S');
    val = load (mem_sim , 83);
    printf("%c\n", val);
    val = load (mem_sim , 2);
    store(mem_sim , 98,'X');
    val = load (mem_sim ,16);
    val = load (mem_sim ,64);
    store(mem_sim ,32,'Y');
    val = load (mem_sim ,23);
    store(mem_sim , 105,'T');
    store(mem_sim , 105,'T');
    store(mem_sim ,52,'Q');
    val = load (mem_sim ,111);
    printf("%c\n", val);
    store(mem_sim , 97,'L');


    print_memory(mem_sim);
    print_page_table(mem_sim);
    print_swap(mem_sim);
    clear_system(mem_sim);


}*/
