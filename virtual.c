#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "oslabs.h"
#define QUEUEMAX 10
#define MAPMAX 10
#define TABLEMAX 10
#define POOLMAX 10
#define REFERENCEMAX 20
#define MAX( a, b ) ( ( a > b) ? a : b ) 
#define MIN( a, b ) ( ( a > b) ? b : a ) 

int process_page_access_fifo(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){
    int rframe_num, invalid_page;
    invalid_page = -1;
    rframe_num = 0;

    if(page_table[page_number].is_valid == true){
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return page_table[page_number].frame_number;
    }else if(page_table[page_number].is_valid == false && *frame_cnt > 0){
        rframe_num = frame_pool[*frame_cnt - 1];
        (*frame_cnt)--;

        page_table[page_number].frame_number = rframe_num;
        page_table[page_number].arrival_timestamp = current_timestamp; 
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].is_valid = true; 
        page_table[page_number].reference_count = 1;
        return page_table[page_number].frame_number;
    }else {
        for(int i = 0; i < *table_cnt; i++){
            if(page_table[i].is_valid == true){
                if(invalid_page == -1){
                    invalid_page = i;
                }else{
                    if(page_table[i].arrival_timestamp < page_table[invalid_page].arrival_timestamp){
                        invalid_page = i;
                    }
                }
            }
        }
        //setting metadata for newly referenced page
        page_table[page_number].is_valid = true; 
        page_table[page_number].frame_number = page_table[invalid_page].frame_number; //setting frame number
        page_table[page_number].arrival_timestamp = current_timestamp; 
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;

        //invalidating entry with smallest arrival timestamp
        page_table[invalid_page].is_valid = false;
        page_table[invalid_page].frame_number = -1; 
        page_table[invalid_page].arrival_timestamp = -1; 
        page_table[invalid_page].last_access_timestamp = -1; 
        page_table[invalid_page].reference_count = -1;
        
        return page_table[page_number].frame_number;      
    }
} 
int count_page_faults_fifo(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int invalid_page, time_stamp, page_faults, frame;
    time_stamp = 1; 
    frame = 0; 
    page_faults = 0;
    int selection = 0; 

    for(int i = 0; i < reference_cnt; i++){
        invalid_page = -1;
        int page_number = refrence_string[i]; //setting page number to number being refereneced
        int free_frame = 0;
        time_stamp++;

        if(page_table[page_number].is_valid == true){
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count += 1;
        }else if(page_table[page_number].is_valid == false && frame_cnt > 0){
            free_frame = frame_pool[frame_cnt - 1];
            frame_cnt--;

            page_table[page_number].is_valid = true; 
            page_table[page_number].frame_number = free_frame+1;
            page_table[page_number].arrival_timestamp = time_stamp; 
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count = 1;
            page_faults++;
        }else {
            for(int j = 0; j < table_cnt; j++){
                if(page_table[j].is_valid == true){
                    if(invalid_page == -1){
                        invalid_page = j;
                    }else{
                        if(page_table[j].arrival_timestamp < page_table[invalid_page].arrival_timestamp){
                            invalid_page = j;
                        }
                    }
                }
            }
            //setting metadata for newly referenced page
            page_table[page_number].is_valid = true; 
            page_table[page_number].frame_number = page_table[invalid_page].frame_number; //setting frame number
            page_table[page_number].arrival_timestamp = time_stamp; 
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count = 1;
            page_faults++; 

            //invalidating entry with smallest arrival timestamp
            page_table[invalid_page].is_valid = false;
            page_table[invalid_page].frame_number = -1; 
            page_table[invalid_page].arrival_timestamp = -1; 
            page_table[invalid_page].last_access_timestamp = -1; 
            page_table[invalid_page].reference_count = -1;  
        }
    }
    return page_faults;
}
int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){
    int free_frame, invalid_page;
    invalid_page = -1;
    free_frame = 0;

    if(page_table[page_number].is_valid == true){
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return page_table[page_number].frame_number;
    }else if(page_table[page_number].is_valid == false && *frame_cnt > 0){
        free_frame = frame_pool[*frame_cnt - 1];
        (*frame_cnt)--;

        page_table[page_number].frame_number = free_frame;
        page_table[page_number].arrival_timestamp = current_timestamp; 
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].is_valid = true; 
        page_table[page_number].reference_count = 1;
        return page_table[page_number].frame_number;
    }else {
        for(int j = 0; j < *table_cnt; j++){
            if(page_table[j].is_valid == true){
                if(invalid_page == -1){
                    invalid_page = j;
                }else{
                    if(page_table[j].last_access_timestamp < page_table[invalid_page].last_access_timestamp){
                        invalid_page = j;
                    }
                }
            }
        }
        //setting metadata for newly referenced page
        page_table[page_number].is_valid = true; 
        page_table[page_number].frame_number = page_table[invalid_page].frame_number; //setting frame number
        page_table[page_number].arrival_timestamp = current_timestamp; 
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;

        //invalidating entry with smallest arrival timestamp
        page_table[invalid_page].is_valid = false;
        page_table[invalid_page].frame_number = -1; 
        page_table[invalid_page].arrival_timestamp = -1; 
        page_table[invalid_page].last_access_timestamp = -1; 
        page_table[invalid_page].reference_count = -1;
        
        return page_table[page_number].frame_number;      
    }
} 
int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int invalid_page, time_stamp, page_faults, frame;
    time_stamp = 1; 
    frame = 0; 
    page_faults = 0;
    int selection = 0; 

    for(int i = 0; i < reference_cnt; i++){
        invalid_page = -1;
        int page_number = refrence_string[i]; //setting page number to number being refereneced
        int free_frame = 0;
        time_stamp++;

        if(page_table[page_number].is_valid == true){
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count += 1;
        }else if(page_table[page_number].is_valid == false && frame_cnt > 0){
            free_frame = frame_pool[frame_cnt - 1];
            frame_cnt--;

            page_table[page_number].is_valid = true; 
            page_table[page_number].frame_number = free_frame+1;
            page_table[page_number].arrival_timestamp = time_stamp; 
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count = 1;
            page_faults++;
        }else {
            for(int j = 0; j < table_cnt; j++){
                if(page_table[j].is_valid == true){
                    if(invalid_page == -1){
                        invalid_page = j;
                    }else{
                        if(page_table[j].last_access_timestamp < page_table[invalid_page].last_access_timestamp){
                            invalid_page = j;
                        }
                    }
                }
            }
            //setting metadata for newly referenced page
            page_table[page_number].is_valid = true; 
            page_table[page_number].frame_number = page_table[invalid_page].frame_number; //setting frame number
            page_table[page_number].arrival_timestamp = time_stamp; 
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count = 1;
            page_faults++; 

            //invalidating entry with smallest arrival timestamp
            page_table[invalid_page].is_valid = false;
            page_table[invalid_page].frame_number = -1; 
            page_table[invalid_page].arrival_timestamp = -1; 
            page_table[invalid_page].last_access_timestamp = -1; 
            page_table[invalid_page].reference_count = -1;  
        }
    }
    return page_faults;
}
int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){
    int free_frame, invalid_page;
    invalid_page = -1;
    free_frame = 0;

    if(page_table[page_number].is_valid == true){
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        return page_table[page_number].frame_number;
    }else if(page_table[page_number].is_valid == false && *frame_cnt > 0){
        free_frame = frame_pool[*frame_cnt - 1];
        (*frame_cnt)--;

        page_table[page_number].frame_number = free_frame;
        page_table[page_number].arrival_timestamp = current_timestamp; 
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].is_valid = true; 
        page_table[page_number].reference_count = 1;
        return page_table[page_number].frame_number;
    }else {
        for(int j = 0; j < *table_cnt; j++){
            if(page_table[j].is_valid == true){
                if(invalid_page == -1){
                    invalid_page = j;
                }else{
                    if(page_table[j].reference_count == page_table[invalid_page].reference_count){
                        if(page_table[j].arrival_timestamp < page_table[invalid_page].arrival_timestamp){
                            invalid_page = j;
                        }
                        
                    }else {
                        if(page_table[j].reference_count < page_table[invalid_page].reference_count){
                            invalid_page = j;
                        }
                    }
                }
            }
        }
        //setting metadata for newly referenced page
        page_table[page_number].is_valid = true; 
        page_table[page_number].frame_number = page_table[invalid_page].frame_number; //setting frame number
        page_table[page_number].arrival_timestamp = current_timestamp; 
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;

        //invalidating entry with smallest arrival timestamp
        page_table[invalid_page].is_valid = false;
        page_table[invalid_page].frame_number = -1; 
        page_table[invalid_page].arrival_timestamp = -1; 
        page_table[invalid_page].last_access_timestamp = -1; 
        page_table[invalid_page].reference_count = -1;
        
        return page_table[page_number].frame_number;      
    }
} 
int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int invalid_page, time_stamp, page_faults, frame;
    time_stamp = 0; 
    frame = 0; 
    page_faults = 0;
    int selection = 0; 
    

    for(int i = 0; i < reference_cnt; i++){
        invalid_page = -1;
        int page_number = refrence_string[i]; //setting page number to number being refereneced
        int free_frame = 0;
        time_stamp++;

        if(page_table[page_number].is_valid == true){
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count += 1;
        }else if(page_table[page_number].is_valid == false && frame_cnt > 0){
            free_frame = frame_pool[frame_cnt - 1];
            frame_cnt--;

            page_table[page_number].is_valid = true; 
            page_table[page_number].frame_number = free_frame+1;
            page_table[page_number].arrival_timestamp = time_stamp; 
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count = 1;
            page_faults++;
        }else {
            for(int j = 0; j < table_cnt; j++){
                if(page_table[j].is_valid == true){
                    if(invalid_page == -1){
                        invalid_page = j;
                    }else{
                        if(page_table[j].reference_count == page_table[invalid_page].reference_count){
                            if(page_table[j].arrival_timestamp < page_table[invalid_page].arrival_timestamp){
                                invalid_page = j;
                            }   
                        }else {
                            if(page_table[j].reference_count < page_table[invalid_page].reference_count){
                                invalid_page = j;
                            }
                        }
                    }
                }
            }
            //setting metadata for newly referenced page
            page_table[page_number].is_valid = true; 
            page_table[page_number].frame_number = page_table[invalid_page].frame_number; //setting frame number
            page_table[page_number].arrival_timestamp = time_stamp; 
            page_table[page_number].last_access_timestamp = time_stamp;
            page_table[page_number].reference_count = 1;
            page_faults++; 

            //invalidating entry with smallest arrival timestamp
            page_table[invalid_page].is_valid = false;
            page_table[invalid_page].frame_number = -1; 
            page_table[invalid_page].arrival_timestamp = -1; 
            page_table[invalid_page].last_access_timestamp = -1; 
            page_table[invalid_page].reference_count = -1;  
        }
    }
    return page_faults;
}