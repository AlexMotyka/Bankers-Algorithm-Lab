/*
1. Assign our args
2. Generate our matrices
3. Create threads for requests
    4. Try to allocate resources using banker algorithm
5. Done
*/

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>

// May be any values >= 0
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

//mutex locks
pthread_mutex_t mutexavail;
pthread_mutex_t mutexalloc;
pthread_mutex_t mutexneed;

// Available amount of each resource
int available[NUMBER_OF_RESOURCES];
// Maximum demand of each customer
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
// Amount currently allocated to each customer
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
// Remaining need of each customer
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];


int request_res(int n_customer, int request[]);
int release_res(int n_customer, int request[]);
void print_matrix(int M[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES],char *name);

//we use rand to gen a maximum 
void generate_maximum(){
    int i,j;
    srand(time(NULL));
    for(i=0;i<NUMBER_OF_CUSTOMERS;i++){
        for(j=0;j<NUMBER_OF_RESOURCES;j++){
            
            maximum[i][j] = rand() % available[j];
            allocation[i][j] = 0;
        }

    }

}

//use the bankers algorithm to check if a given "transaction" is safe
int checkIfSafe(){

    int i,j, work[NUMBER_OF_RESOURCES],finish[NUMBER_OF_CUSTOMERS];
    int success = 0;

    for(i=0;i<NUMBER_OF_RESOURCES;i++){

        work[i] = available[i];

    }

    for(i=0;i<NUMBER_OF_CUSTOMERS;i++){

        finish[i] = 0;

    }

    for(i=0;i<NUMBER_OF_CUSTOMERS;i++){

        if(finish[i]==0){

            for(j=0;j<NUMBER_OF_RESOURCES;j++){

                if(need[i][j] > work[j]){
                    //this gets returned if its unsafe to allocate
                    return -1;
                }

            }

            for(j=0;j<NUMBER_OF_RESOURCES;j++){

                work[j] += allocation[i][j];
            }

            success = 1;

        }
    }

    return success;

}

int request_res(int n_customer, int request[]){

    int k;
    for(k=0;k<NUMBER_OF_RESOURCES;k++){

        //cant do the request
        if(request[k] > need[n_customer][k]){
            return -1;
        }
    }
    //try to allocate resources
    for(k=0;k<NUMBER_OF_RESOURCES;k++){

        pthread_mutex_lock(&mutexalloc);
        allocation[n_customer][k] += request[k];
        pthread_mutex_unlock(&mutexalloc);
        pthread_mutex_lock(&mutexavail);
        available[k] -= request[k];
        pthread_mutex_unlock(&mutexavail);
        pthread_mutex_lock(&mutexneed);
        need[n_customer][k] -= request[k];
        pthread_mutex_unlock(&mutexneed);
    }

    //if the request isn't safe revert back to the previous state(basically undo the above for loop^)
    if(checkIfSafe()<0){

        for(k=0;k<NUMBER_OF_RESOURCES;k++){

            pthread_mutex_lock(&mutexalloc);
            allocation[n_customer][k] -= request[k];
            pthread_mutex_unlock(&mutexalloc);
            pthread_mutex_lock(&mutexavail);
            available[k] += request[k];
            pthread_mutex_unlock(&mutexavail);
            pthread_mutex_lock(&mutexneed);
            need[n_customer][k] += request[k];
            pthread_mutex_unlock(&mutexneed);
        }

        return -1;
    }

    return 0;
}

int release_res(int n_customer, int request[]){

    int k;

    for(k=0;k<NUMBER_OF_RESOURCES;k++){

        pthread_mutex_lock(&mutexalloc);
        allocation[n_customer][k] -= request[k];
        pthread_mutex_unlock(&mutexalloc);
        pthread_mutex_lock(&mutexavail);
        available[k] += request[k];
        pthread_mutex_unlock(&mutexavail);
        pthread_mutex_lock(&mutexneed);
        need[n_customer][k] = maximum[n_customer][k] + allocation[n_customer][k];
        pthread_mutex_unlock(&mutexneed);
    }

    return 1;

}



void *thread_create(void *cno){

    int i,j,request[NUMBER_OF_RESOURCES],request_flag=0;
    int cust_no = (int)cno;

    //Create a request of random size(always less than the maximum)
    for(i=0;i<NUMBER_OF_RESOURCES;i++){
        request[i] = rand() % available[i];
    }

    if(request_res(cust_no,request)<0){

        printf("\n Customer number is %d ", cust_no);

        for(j=0;j<NUMBER_OF_RESOURCES;j++){

            printf("%d ", request[j]);
        }
        printf("DENIED\n");

    }else{

        request_flag = 1;
        printf("\n Customer number is %d ", cust_no);

        for(j=0;j<NUMBER_OF_RESOURCES;j++){

            printf("%d ", request[j]);
        }

        printf("ACCEPTED\n");

    }

    

    if(request_flag==1){
        sleep(rand() % 10);
        release_res(cust_no, request); //release the resource if it was allocated
        printf("\n Customer number %d released some resources", cust_no);
    }
    return 0;
}

void print_matrix(int M[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES],char *name){

    int i,j;
    printf("\n-----%s Matrix----\n",name);

    for(i=0;i<NUMBER_OF_CUSTOMERS;i++){

        for(j=0;j<NUMBER_OF_RESOURCES;j++){

            printf("%d ",M[i][j]);
        }
    printf("\n");

    }

}

int main(int argc, char **argv){

    int i,j,run_count = 50;
    pthread_t thread_id;

    if(argc==(NUMBER_OF_RESOURCES+1)){
        //assign the args
        printf("\n Available Matrix \n");

        for(i=1;i<=NUMBER_OF_RESOURCES;i++){

            available[i-1] = abs(atoi(argv[i]));

            printf("%d ",available[i-1]);

        }

        printf("\n");
    }
    generate_maximum(); //gen our matrix

    printf("\n Maximum matrix is: \n");

    for(i=0; i<NUMBER_OF_CUSTOMERS;i++){

        for(j=0; j<NUMBER_OF_RESOURCES; j++){

            need[i][j] = maximum[i][j] - allocation[i][j]; //calculate our needs matrix
            printf("%d ",maximum[i][j]); //print the max matrix
        }

        printf("\n");
    }

    printf("\n Need Matrix\n");

    for(i=0; i< NUMBER_OF_CUSTOMERS;i++){

        for(j=0;j<NUMBER_OF_RESOURCES;j++){

            printf("%d ",need[i][j]); //matrix showing the resources we need
        }

        printf("\n");
    }

    for(i=0;i<run_count;i++){

        for(j=0;j<NUMBER_OF_CUSTOMERS;j++){

            pthread_create(&thread_id,NULL,thread_create,(void *)j); //create our threads and see if we can accept the requests
        }
    }

    printf("\n Finished jobs.");

    return 0;

}

