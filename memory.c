#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "oslabs.h"
#define QUEUEMAX 10
#define MAPMAX 10
#define TABLEMAX 10
#define POOLMAX 10
#define REFERENCEMAX 20
#define MAX( a, b ) ( ( a > b) ? a : b ) 
#define MIN( a, b ) ( ( a > b) ? b : a ) 

struct MEMORY_BLOCK NULLBLOCK() {
    struct MEMORY_BLOCK nullblock;
    nullblock.start_address = 0; 
    nullblock.end_address = 0; 
    nullblock.segment_size = 0; 
    nullblock.process_id = 0; 
    return nullblock;
};

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    //definind structs
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    //defining variables for the free memory map entry
    int rmem, segment_size, free_block, pos;
    int mcnt = *map_cnt; //storing map count
    //loop through memory map array
    for(int i = 0; i < *map_cnt; i++){
        //printf("inside first for loop \n");
        if(memory_map[i].segment_size == request_size && memory_map[i].process_id == 0){
            //printf("seg size is equal to request size \n");
            memory_map[i].process_id = process_id;
            return memory_map[i];
        }
    }

    for(int i = 0; i < *map_cnt; i++){
        //printf("inside second for loop \n");
        if(memory_map[i].process_id == 0){
            //printf("inside process id == 0 if statement \n");
            free_block = i; //storing the index of the free memory block
            //printf("%d\n", free_block);
        }
    }

    for(int i = 0; i < *map_cnt; i++){
        //printf("inside third for loop \n");
        if(memory_map[i].segment_size < memory_map[free_block].segment_size && memory_map[i].process_id == 0){
            //printf("inside checking segment sizes if statement \n");
            free_block = i; //storing the index of the free memory block
            //printf("%d\n", free_block);
        }
    }

    if(memory_map[free_block].segment_size > request_size && memory_map[free_block].process_id == 0){
        //printf("inside checking segment size is greater than request size if statement \n");
        pos = free_block + 1;
        
        rmem = memory_map[free_block].segment_size - request_size;

        memory_map[free_block].segment_size = request_size;
        memory_map[free_block].end_address = memory_map[free_block].start_address + request_size - 1;
        memory_map[free_block].process_id = process_id;
        (*map_cnt)++;

        for(int i = *map_cnt; i >= pos; i--){
            memory_map[i] = memory_map[i-1];
        }

        memory_map[pos].segment_size = rmem;
        memory_map[pos].start_address = memory_map[free_block].end_address + 1;
        memory_map[pos].end_address = memory_map[pos].start_address + memory_map[pos].segment_size - 1;
        memory_map[pos].process_id = 0;
        return memory_map[free_block];
    } else{ 
        return nullblock; 
    }  
} 
struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    int end_address, segment_size, free_block, pos;
    int mcnt = *map_cnt; //storing map count
    //loop through memory map array
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0){ //checking for free memory block
            free_block = i; //storing the index of the free memory block
            pos = free_block + 1;//insert position for free space of memory
            if(memory_map[free_block].segment_size == request_size){
                memory_map[free_block].process_id = process_id;
                return memory_map[free_block];
            }

            if(memory_map[i].segment_size > request_size){
                segment_size = memory_map[free_block].segment_size; //storing original segment size

                memory_map[free_block].start_address = 0;
                memory_map[free_block].end_address = request_size - 1;
                memory_map[free_block].segment_size = request_size;
                memory_map[free_block].process_id = process_id;
            
                for(int i = *map_cnt; i >= pos; i--){
                    memory_map[i] = memory_map[i-1];
                }
                memory_map[pos].start_address = request_size;
                memory_map[pos].end_address = segment_size - 1;
                memory_map[pos].segment_size = segment_size - request_size;
                memory_map[pos].process_id = 0;
                (*map_cnt)++;
                return memory_map[free_block];
            } 
        }
    }
    return nullblock;   
} 
struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    int free_block, pos, rmem;    
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].segment_size == request_size && memory_map[i].process_id == 0){
            memory_map[i].process_id = process_id;
            return memory_map[i];
        }
    }
    free_block = -1;
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0){
            if(memory_map[i].segment_size > request_size){
                if(free_block == -1){
                    free_block = i; 
                }else{
                    if(memory_map[i].segment_size > memory_map[free_block].segment_size){
                        free_block = i;
                    }
                }
            }
        }
    }
    if(free_block == -1 ){
        return nullblock;
    }else{
        pos = free_block + 1;

        rmem = memory_map[free_block].segment_size - request_size;

        memory_map[free_block].segment_size = request_size;
        memory_map[free_block].end_address = memory_map[free_block].start_address + request_size - 1;
        memory_map[free_block].process_id = process_id;
        (*map_cnt)++;

        for(int i = *map_cnt; i >= pos; i--){
            memory_map[i] = memory_map[i-1];
        }

        memory_map[pos].segment_size = rmem;
        memory_map[pos].start_address = memory_map[free_block].end_address + 1;
        memory_map[pos].end_address = memory_map[pos].start_address + memory_map[pos].segment_size - 1;
        memory_map[pos].process_id = 0;
        return memory_map[free_block];
    }
}
struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id, int last_address){
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    int free_block, pos, rmem;    
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].segment_size == request_size && memory_map[i].process_id == 0){
            memory_map[i].process_id = process_id;
            return memory_map[i];
        }
    }
    free_block = -1;
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0){
            if(memory_map[i].segment_size > request_size){
                if(free_block == -1){
                    free_block = i; 
                }else{
                    if(memory_map[i].segment_size > memory_map[free_block].segment_size){
                        free_block = i;
                    }
                }
            }
        }
    }
    if(free_block == -1 ){
        return nullblock;
    }else{
        pos = free_block + 1;

        rmem = memory_map[free_block].segment_size - request_size;

        memory_map[free_block].segment_size = request_size;
        memory_map[free_block].end_address = memory_map[free_block].start_address + request_size - 1;
        memory_map[free_block].process_id = process_id;
        (*map_cnt)++;

        for(int i = *map_cnt; i >= pos; i--){
            memory_map[i] = memory_map[i-1];
        }

        memory_map[pos].segment_size = rmem;
        memory_map[pos].start_address = memory_map[free_block].end_address + 1;
        memory_map[pos].end_address = memory_map[pos].start_address + memory_map[pos].segment_size - 1;
        memory_map[pos].process_id = 0;
        return memory_map[free_block];
    }
}
void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt){
    int free_block_pos;
    int merged = 0; 

    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == freed_block.process_id){
            memory_map[i].process_id = 0;
            free_block_pos = i;
            break;
        }
    }
    if(free_block_pos > 0 && free_block_pos + 1 < *map_cnt){
        if(memory_map[free_block_pos-1].process_id == 0){
            memory_map[free_block_pos-1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos-1].segment_size;
            memory_map[free_block_pos-1].end_address = memory_map[free_block_pos].start_address + memory_map[free_block_pos].segment_size - 1;
            (*map_cnt)--;
            merged = 1;
            for(int i = free_block_pos; i < *map_cnt; i++){
                memory_map[i] = memory_map[i+1];   
            } 
        }
        if(merged == 1){
            if(memory_map[free_block_pos].process_id == 0){
            memory_map[free_block_pos-1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos-1].segment_size;
            memory_map[free_block_pos-1].end_address = memory_map[free_block_pos].start_address + memory_map[free_block_pos].segment_size - 1;
            (*map_cnt)--;
            for(int i = free_block_pos; i < *map_cnt; i++){
                memory_map[i] = memory_map[i+1];   
            }
            return;
            }
        }else if(memory_map[free_block_pos + 1].process_id == 0){
            memory_map[free_block_pos+1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos+1].segment_size;
            memory_map[free_block_pos+1].process_id = memory_map[free_block_pos].process_id;
            memory_map[free_block_pos+1].start_address = memory_map[free_block_pos].start_address;
            (*map_cnt)--;

            for(int i = free_block_pos; i < *map_cnt; i++){
                memory_map[i] = memory_map[i+1];   
            }
            return;
        }else {
            return;
        }
    }else if(free_block_pos == 0){
        if(memory_map[free_block_pos+1].process_id == 0){
            memory_map[free_block_pos+1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos+1].segment_size;
            memory_map[free_block_pos+1].process_id = memory_map[free_block_pos].process_id;
            memory_map[free_block_pos+1].start_address = memory_map[free_block_pos].start_address;
            (*map_cnt)--;
        }
        
        for(int i = free_block_pos; i < *map_cnt; i++){
            memory_map[i] = memory_map[i+1];
        }
        return; 
    }else if(free_block_pos+1 >= *map_cnt) {
        if(memory_map[free_block_pos-1].process_id == 0){
            memory_map[free_block_pos-1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos-1].segment_size;
            memory_map[free_block_pos-1].end_address = memory_map[free_block_pos].start_address + memory_map[free_block_pos].segment_size - 1;
            (*map_cnt)--;
        }
        for(int i = free_block_pos; i < *map_cnt; i++){
            memory_map[i] = memory_map[i+1];   
        }
        return; 
    }
}      

