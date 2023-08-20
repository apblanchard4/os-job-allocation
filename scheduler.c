#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct job {
    int id ;
    int length ;
    int start_time, end_time; //use for policy analysis
    int robin_complete; //use to see if job is complete, 0 means still needs to be run, 1 means complete
    int accessed; //how many times the round robin has run
    int time_left;
    struct job * next ;

}job;

job *head; //use for LL of jobs

int num_jobs = 0; //keep track of the total num of jobs
int running_time; //keep track of total running time, used for analysis

void FIFO (void) { //first in first out, the first job is ran first
    running_time = 0;
    // Return if list is empty
    if(head == NULL)    {
        printf("List is empty.");
        return;
    }

    job *node;
    node = head;
    while(node != NULL) //traverse the list

    {
        node ->start_time = running_time; //starting now, set start time
        int run_time =  node->length;
        int job_num = node->id;
        printf("Job %d ran for: %d\n", job_num, run_time);  //"run" the job
        running_time += run_time; //add the time it took to run to the running time
        node->end_time = running_time; //set the end to the running time after finished
        node = node ->next;                 // Move to next node
    }

    printf("End of execution with FIFO.\n");
}

void SJF (void) { //shortest job first, list is given sorted already
    running_time= 0;
    job *node;
    // Return if list is empty
    if(head == NULL){
        printf("List is empty.");
        return;
    }

    node = head;
    while(node != NULL) { //traverse the list
        node ->start_time = running_time; //starting now, set start time
        int run_time =  node->length;
        int job_num = node->id;
        printf("Job %d ran for: %d\n", job_num, run_time);
        running_time += run_time; //add the time it took to run to the running time
        node->end_time = running_time; //set the end to the running time after finished
        node = node ->next;                 // Move to next node
    }

    printf("End of execution with SJF.\n");
}

void RR (int time_slice) { //round robin using given time slice
    running_time = 0;
    // Return if list is empty, error

    if (head == NULL) {
        printf("List is empty.");
        return;
    }
    //loop through the list until all jobs are complete, do not remove elements
    //keep track if complete through the complete jobs variable
    job *node;
    node = head;
    int complete_jobs =0; //increases each time
    while (num_jobs > complete_jobs) {  //while the list still needs contains jobs that need to be run:
        if (node->accessed == 0) {
            node->start_time = running_time; //if its the first time this job is running, set the start time
            node->time_left = node->length;
            node->accessed++;
        }

        if (node->robin_complete == 0) { //check if this job has already been completed, 0 means still needs to be run
            //this job needs to be ran, get info:
            int job_length =  node->length;
            int job_length_left = node->time_left; //get the time left for this job
            int job_num = node->id; //set the runtime to the time slice
            int run_time = time_slice;
            //check if the job length is less than the time slice
            if (job_length_left < time_slice) {
                run_time = job_length_left; //set the runtime to job length since don't need whole time slice
            }
            printf("Job %d ran for: %d\n", job_num, run_time);

            running_time += run_time;//set the running_time to add the time for this job


            //calculate the new length
            node->time_left = job_length_left - run_time;  //subtract the runtime from the old length


            //check if the job is completed
            if (node->time_left == 0) { //if equal to 0, complete
                complete_jobs +=1;
                if (node->robin_complete ==0) {
                node->robin_complete = 1; //set the variable to mark if complete to 1
                node->end_time = running_time; //set the runtime
                }

                if(complete_jobs == num_jobs) {
                    printf("End of execution with RR.\n");
                    return ;
                }
            }
            //continue to other jobs
            node = node->next;

            if (node == NULL ) { //if its the end of the list, check if all jobs are complete
                if (num_jobs > complete_jobs) { //if total number of jobs is equal to the completed, continue to loop
                    node = head; //restarts at the beginning, make sure they all fully complete
                }
            }
        }

        if(node->robin_complete == 1) { //checks if job has already been complete
            node = node->next;
            if(node == NULL){
                if(num_jobs > complete_jobs){
                    node = head; }
            }
        }
    }
}



void swap_sort () { //use to sort the elements
    int swapped; //use to keep track of if swapped
    struct job *node; //current node
    struct job *last_node = NULL; //used to make sure ends

    if (head == NULL) { //if the list is empty:
        printf("file can't be opened \n");
        return;
    }

    while (swapped) { //while there is nodes to be swapped
        swapped = 0; //set to 0
        node = head; //set the dummy node to the job list
        while (node->next != last_node) { //if the next is not the last node
            if (node->length > node->next->length)  { //if the length is greater than the next next length,
                swap(node, node->next); //swap the node and the next one
                swapped = 1;
            }
            node = node->next; //go to the next element
        }
        last_node = node; //the last node is set
    }
}


/* function to swap data of two nodes a and b*/

void swap(struct job *job_1, struct job *job_2) {          //use with swap sort, switch the two lengths and ids, the rest of info can remain the same
    int temp_length = job_1->length; //use temp length to hold job 1 length
    int temp_id = job_1->id; //hold job 1 ID
    job_1->length = job_2->length; //set 2 to 1
    job_1->id = job_2->id;
    job_2->length = temp_length; //set 2 to temp which is the info stored in 1
    job_2->id = temp_id;

}


void read_input (char** job_trace) { //function to read in file, added to the linked list
    //create the job linked list before starting the policy since we want them all to be said to arrive at the same time
    char const* const file_name = *job_trace; //gets name of the file
    //printf("FILE NAME: %s",file_name );
    FILE* file = fopen(file_name, "r"); //open the file
    char line[256];
    // Opening file in reading mode


    if (file == NULL) { //error message file cannot be opened
        printf("file can't be opened \n");
        return;
    }

    //get contents inside of the in file

    int i=0; //used to keep track of the job id

    while(fgets(line, sizeof(line), file)){ //each line at a time

        num_jobs++;
        struct job *node; //create node to go through the list
        node  = (struct job*)malloc(sizeof(struct job));
        node->id = i; //input the job id based on the line of the file
        node->length = atoi(line); //input length of job based on the file
        //set all the variables to defaults
        node->start_time = 0;
        node->end_time=0; //use for policy analysis
        node -> robin_complete = 0;
        node-> accessed =0;
        node->time_left=0;
        node->next =NULL;
        i++; //increase the number of file line



        if (head == NULL){ //if there is nothing in the list, set to node
            head = node;
        }

        else { //something in the list already, add to the end

            job *temp = head; //create dummy node

            while(temp->next!=NULL) { //there is something next, set temp to next node
                temp=temp->next;
            }
            // adding the new node after the last node
            temp->next=node;
        }
    }

    fclose(file); //close the file

}



void analysis_FIFO_SJF () { //used for the second part to analyze
    job *node;
    node = head;
    double avg_response =0;
    double avg_turn =0;
    double avg_wait = 0;
    double total =0;

    while(node != NULL) { //traverse the list
        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", node->id, node->start_time, node->start_time + node->length, node->start_time);
        avg_response += node->start_time;
        avg_turn +=node->start_time + node->length;
        avg_wait += node->start_time;
        node = node->next;
    }
    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", avg_response/((double)(num_jobs)), avg_turn/((double)(num_jobs)), avg_wait/((double)(num_jobs)));
}



void analysis_RR () { //RR has special function to analyze since it requires more work
    job *node;
    node = head;
    double avg_response =0;
    double avg_turn =0;
    double avg_wait = 0;
    int response = 0;

    while(node != NULL) //traverse the list
    {
        int node_id = node->id;
        response = node->start_time; //start is when it was first responded to
        int turnaround = node->end_time;  //turnaround is end - start
        int wait = turnaround - (node->length);



        printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n", node_id, response, turnaround, wait);
        avg_response += response;
        avg_turn += turnaround;
        avg_wait += wait;
        node = node->next;
    }
    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", avg_response/num_jobs, avg_turn/num_jobs, avg_wait/num_jobs);
}


int main(int argc, char* argv[]){
    //use variables to hold command line input
    char *policy_name;
    char *job_trace;
    int time_slice;

    //read the input

    for(int i = 1; i<argc; i++) { //don't want to read argv[0]

        if (i == 1) { //input for policy name
            policy_name = argv[1];
            //printf("Policy name: %s \n", policy_name );
        }

        if (i == 2) { //input for job trace
            job_trace = argv[2];
            //printf("Job Trace: %s \n", job_trace );

        }

        if (i == 3) { //input for time slice to use with RR
            char *input = argv[3];
            time_slice = atoi(input);
            //printf("Time Slice: %d \n", time_slice );

        }
    }
    read_input(&job_trace); //read the input from the job trace file, used to create the link list of jobs

    if (0 == (strcmp(policy_name, "FIFO"))) { //if policy is FIFO, run FIFO and analyze
        printf("Execution trace with FIFO:\n");
        FIFO(); //run FIFO
        printf("Begin analyzing FIFO:\n");
        analysis_FIFO_SJF();
        printf("End analyzing FIFO.\n");
    }

    else if (0 == (strcmp(policy_name, "SJF"))) {  //if policy is SJF, run SJF and analyze
        swap_sort(); //must sort before
        printf("Execution trace with SJF:\n");
        SJF(); //run SJF
        printf("Begin analyzing SJF:\n");
        analysis_FIFO_SJF(); //run analysis
        printf("End analyzing SJF.\n");
    }

    else if (0 == (strcmp(policy_name, "RR"))) {
        printf("Execution trace with RR:\n");
        RR(time_slice); //run RR using the given time slice from the input
        printf("Begin analyzing RR:\n");
        analysis_RR();
        printf("End analyzing RR.\n");
    }

    else { //invalid input for policy name, print error message
        fprintf(stderr, "Invalid policy name\n");
        return -1;
    }

}

