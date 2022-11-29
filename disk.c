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

struct RCB NULLRCB() {
    struct RCB nullrcb;
    nullrcb.request_id = 0; 
    nullrcb.arrival_timestamp = 0; 
    nullrcb.cylinder = 0; 
    nullrcb.address = 0;
    nullrcb.process_id = 0; 
    return nullrcb;
};

struct RCB NEWRCB() {
        int request_id;
        int arrival_timestamp;
        int cylinder;
        int address;
        int process_id;
    };

struct RCB rcb_queue_remove(struct RCB * request_queue, int *queue_cnt, int index);

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
   struct RCB nullrcb = NULLRCB(); //creating a null RCB struct
   
   if(current_request.request_id == nullrcb.request_id && current_request.arrival_timestamp == nullrcb.arrival_timestamp && current_request.cylinder == nullrcb.cylinder && current_request.address == nullrcb.address && current_request.process_id == nullrcb.process_id){ //checking if current request queue is empty
       return new_request;
   }else{
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;
   }
   return current_request; 
}
struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX],int *queue_cnt){
    struct RCB nullrcb = NULLRCB();
    struct RCB earliestrcb = NEWRCB();
    struct RCB requestrcb = NEWRCB();

    int earliest = 0; 
    
    if(*queue_cnt == 0){
        return nullrcb; 
    }

    for(int i = 0; i < *queue_cnt; i++){
        if(request_queue[i].arrival_timestamp < request_queue[earliest].arrival_timestamp){
            earliest = i;                      
        }   
    }
    earliestrcb = request_queue[earliest]; 
    rcb_queue_remove(request_queue, queue_cnt, earliest);
    (*queue_cnt)--;
    return earliestrcb;
}
struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
    struct RCB nullrcb = NULLRCB();
   
   if(current_request.request_id == nullrcb.request_id && current_request.arrival_timestamp == nullrcb.arrival_timestamp && current_request.cylinder == nullrcb.cylinder && current_request.address == nullrcb.address && current_request.process_id == nullrcb.process_id){
       return new_request;
   }else{
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;
   }
   return current_request; 
}
struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt,int current_cylinder){
    struct RCB nullrcb = NULLRCB();
    struct RCB closestrcb = NEWRCB();
    struct RCB requestrcb = NEWRCB();

    int closest = 0; 
    
    if(*queue_cnt == 0){
        return nullrcb; 
    }

    for(int i = 0; i < *queue_cnt; i++){
        if(abs(current_cylinder-request_queue[i].cylinder) == abs(current_cylinder-request_queue[closest].cylinder)){ //checking if the cylinders are equal
            if(request_queue[i].arrival_timestamp < request_queue[closest].arrival_timestamp){ //checking for the earliest arrival timestamp
                closest = i; //storing index of the closest cylinder
            }                 
        }else{
            if(abs(current_cylinder-request_queue[i].cylinder) < abs(current_cylinder-request_queue[closest].cylinder)){ //checking for the cylinder that closest to current
                closest = i; //storing index of the closest cylinder
            }
        }
    }

    closestrcb = request_queue[closest]; //storing closet cylinder in a new RCB
    rcb_queue_remove(request_queue, queue_cnt, closest); //removing the choosen cylinder from the request quee
    (*queue_cnt)--; //reducing queue count
    return closestrcb; //returning closest RCB
}
struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
    struct RCB nullrcb = NULLRCB(); 
    
    if(current_request.request_id == nullrcb.request_id && current_request.arrival_timestamp == nullrcb.arrival_timestamp && current_request.cylinder == nullrcb.cylinder && current_request.address == nullrcb.address && current_request.process_id == nullrcb.process_id){
       return new_request;
   }else{
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;
   }
   return current_request; 
}
struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX],int *queue_cnt, int current_cylinder, int scan_direction){
    struct RCB nullrcb = NULLRCB(); 
    struct RCB closestrcb = NEWRCB();
    struct RCB requestrcb = NEWRCB();

    int closest = -1; 

    if(*queue_cnt == 0){
        return nullrcb; 
    }

    for(int i = 0; i < *queue_cnt; i++){
        if(request_queue[i].cylinder == current_cylinder){ //checking if the cylinders are equal
            if(closest == -1){
                closest = i;
            }else{
                if(request_queue[i].arrival_timestamp < request_queue[closest].arrival_timestamp){ //checking for the earliest arrival timestamp
                    closest = i; //storing index of the closest cylinder
                }
            }
        }    
    }
    //checking if scan direction is set to 1
    if(closest == -1){
        if(scan_direction == 1){
            for(int j = 0; j < *queue_cnt; j++){
                if(request_queue[j].cylinder > current_cylinder){
                        if(abs(current_cylinder-request_queue[j].cylinder) < abs(current_cylinder-request_queue[closest].cylinder)){ //checking for the cylinder that closest to current
                            closest = j; //storing index of the closest cylinder
                    }else if(abs(current_cylinder-request_queue[j].cylinder) == abs(current_cylinder-request_queue[closest].cylinder)){
                            if(request_queue[j].arrival_timestamp < request_queue[closest].arrival_timestamp){ //checking for the earliest arrival timestamp
                                closest = j; //storing index of the closest cylinder
                        }
                    } 
                }
            }  
        }
    }
    if(closest == -1){
        if(scan_direction == 1){
            for(int j = 0; j < *queue_cnt; j++){
                if(request_queue[j].cylinder < current_cylinder){
                    if(abs(current_cylinder-request_queue[j].cylinder) < abs(current_cylinder-request_queue[closest].cylinder)){ //checking for the cylinder that closest to current
                        closest = j; //storing index of the closest cylinder
                    }else if(abs(current_cylinder-request_queue[j].cylinder) == abs(current_cylinder-request_queue[closest].cylinder)){
                            if(request_queue[j].arrival_timestamp < request_queue[closest].arrival_timestamp){ //checking for the earliest arrival timestamp
                                closest = j; //storing index of the closest cylinder
                        }
                    } 
                }
            }  
        }
    }  
    //checking if scan direction is set to 0
    if(closest == -1){
        if(scan_direction == 0){
            for(int n = 0; n < *queue_cnt; n++){
                if(request_queue[n].cylinder < current_cylinder){
                    if(abs(current_cylinder-request_queue[n].cylinder) < abs(current_cylinder-request_queue[closest].cylinder)){ //checking for the cylinder that closest to current
                        closest = n; //storing index of the closest cylinder
                    }else if(abs(current_cylinder-request_queue[n].cylinder) == abs(current_cylinder-request_queue[closest].cylinder)){
                        if(request_queue[n].arrival_timestamp < request_queue[closest].arrival_timestamp){ //checking for the earliest arrival timestamp
                            closest = n; //storing index of the closest cylinder
                        }
                    }
                }
            }          
        }
    }

    if(closest == -1){
        if(scan_direction == 0){
            for(int n = 0; n < *queue_cnt; n++){
                if(request_queue[n].cylinder > current_cylinder){
                    if(abs(current_cylinder-request_queue[n].cylinder) < abs(current_cylinder-request_queue[closest].cylinder)){ //checking for the cylinder that closest to current
                        closest = n; //storing index of the closest cylinder
                    }else if(abs(current_cylinder-request_queue[n].cylinder) == abs(current_cylinder-request_queue[closest].cylinder)){
                        if(request_queue[n].arrival_timestamp < request_queue[closest].arrival_timestamp){ //checking for the earliest arrival timestamp
                            closest = n; //storing index of the closest cylinder
                        }
                    }
                }
            }          
        }
    } 

    closestrcb = request_queue[closest]; //storing closet cylinder in a new RCB
    rcb_queue_remove(request_queue, queue_cnt, closest); //removing the choosen cylinder from the request quee
    (*queue_cnt)--; //reducing queue count
    return closestrcb; //returning closest RCB
}
struct RCB rcb_queue_remove(struct RCB * request_queue, int *queue_cnt, int index){
    index++; //incrementing index to remove correct element
    for(int i = index-1; i < *queue_cnt-1; i++){
        request_queue[i] = request_queue[i+1];
    }
    return *request_queue;
}