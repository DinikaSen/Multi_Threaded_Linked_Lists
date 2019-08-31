#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>

#define RANDOM_LIMIT 65536
#define SAMPLE_SIZE 10      //Initial sample size assumption

int n;                      //Number of nodes in the linked list
int m;                      //Number of operations to be performed on linked list
double m_member;            //Fraction of Member operations
double m_insert;            //Fraction of Insert operations
double m_delete;            //Fraction of Delete operations
int no_of_threads;          //Number of threads to be executed

//Define Node
struct list_node_s {
    int data;
    struct list_node_s *next;
};

//Define no of each operations assigned per a single thread as a struct
struct operations {
    int memberFuncs;
    int inserts;
    int deletes;
};

//Initialize linked list head
struct list_node_s *head_p = NULL;

int total_count = 0;
double member_ops, insert_ops, delete_ops;

pthread_rwlock_t rwlock;

//Functions
void validateInputs(int n, int m, double m_member, double m_insert, double m_delete, int no_of_threads);

int Member(int value);

int Insert(int value);

int Delete(int value);

int isEmpty(struct list_node_s *head_p);

void freeList(struct list_node_s **head_pp);

void *threadFunction(void *dummyPt);

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Number of arguments does not match. Please re-enter command\n");
        exit(0);
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]);
    m_member = atof(argv[3]);
    m_insert = atof(argv[4]);
    m_delete = atof(argv[5]);
    no_of_threads = atoi(argv[6]);

    validateInputs(n, m, m_member, m_insert, m_delete, no_of_threads);

    //Get total number of each operations to be performed
    member_ops = m_member * m;
    insert_ops = m_insert * m;
    delete_ops = m_delete * m;

    //Initialize an array to store number of operations assigned per each thread
    struct operations operationCountList[no_of_threads];

    //Calculate and assign no of member, insert, delete operations to be done by each thread
    for (int i = 0; i < no_of_threads; i++) {
        //A struct per each thread
        struct operations ops = {0, 0, 0};

        /*
         * Here, if total member/delete/insert operations is divisible by number of threads, each thread is assigned an equal number
         * of operations. If not, the remaining operations are assigned to the threads - one operation per thread starting
         * from the first thread.
         */
        if ((int) member_ops % no_of_threads == 0 || (int) member_ops % no_of_threads <= i) {
            ops.memberFuncs = (int) member_ops / no_of_threads;
        } else if ((int) member_ops % no_of_threads > i) {
            ops.memberFuncs = (int) member_ops / no_of_threads + 1;
        }

        if ((int) insert_ops % no_of_threads == 0 || (int) insert_ops % no_of_threads <= i) {
            ops.inserts = (int) insert_ops / no_of_threads;
        } else if ((int) insert_ops % no_of_threads > i) {
            ops.inserts = (int) insert_ops / no_of_threads + 1;
        }

        if ((int) delete_ops % no_of_threads == 0 || (int) delete_ops % no_of_threads <= i) {
            ops.deletes = (int) delete_ops / no_of_threads;
        } else if ((int) delete_ops % no_of_threads > i) {
            ops.deletes = (int) delete_ops / no_of_threads + 1;
        }

        //Store number of operations assigned per each thread in the array
        operationCountList[i] = ops;
    }

    int sampleSize = SAMPLE_SIZE;
    while (1) {
        int count = -1;
        double totalTime = 0;
        double elapsedTime = 0;
        double times[sampleSize];
        double squareSum = 0;
        double stdDeviation;

        printf("sample size = %d \n", sampleSize);

        //Populate the linked list with random unique values between 1-2^16
        while (count < sampleSize) {
            head_p = NULL;
            int nodeCount = 0;

            while (nodeCount < n) {
                if (Insert((rand() % RANDOM_LIMIT)) == 1) {
                    nodeCount++;
                }
            }

            long thread;
            pthread_t *thread_handles;
            thread_handles = malloc(no_of_threads * sizeof(pthread_t));
            pthread_rwlock_init(&rwlock, NULL);

            clock_t t = clock();

            //Create threads
            for (thread = 0; thread < no_of_threads; thread++) {
                struct operations op = operationCountList[(int) thread];
                pthread_create(&thread_handles[thread], NULL, threadFunction, (void *) &op);
            }

            //Join threads
            for (thread = 0; thread < no_of_threads; thread++) {
                pthread_join(thread_handles[thread], NULL);
            }

            t = clock() - t;

            pthread_rwlock_destroy(&rwlock);
            free(thread_handles);
            freeList(&head_p);

            elapsedTime = ((double) t) / CLOCKS_PER_SEC;

            if (count != -1) {
                printf("Elapsed time : %f \n",elapsedTime);
                totalTime += elapsedTime;
                times[count] = elapsedTime;
            }
            count++;
        }

        //Calculate average time and standard deviation
        double averageTime = (double) (totalTime / sampleSize);
        int count2;
        for (count2 = 0; count2 < sampleSize; count2++) {
            squareSum += (times[count2] - averageTime) * (times[count2] - averageTime);
        }
        stdDeviation = sqrt(squareSum / sampleSize);

        //Calculate the minimum number of samples required to obtain the expected accuracy and confidence interval
        double sizeValue = (100 * 1.96 * stdDeviation) / (averageTime * 5);
        sizeValue = sizeValue * sizeValue;
        int minSampleSize = round(sizeValue);

        //If the used number of samples is less than the minimunm required amount, the operations are run with a different sample size
        //If not, the results are printed and execution is terminated
        if (sampleSize - minSampleSize < 0) {
            sampleSize = minSampleSize;
        } else {
            printf("TotalTime = %f \n", totalTime);
            printf("Average = %f \n", averageTime);
            printf("Standard Deviation = %f \n", stdDeviation);
            printf("Minimum size = %d \n", minSampleSize);
            printf("Finished !!!\n");
            break;
        }

        printf("TotalTime = %f \n", totalTime);
        printf("Average = %f \n", averageTime);
        printf("Standard Deviation = %f \n", stdDeviation);
        printf("Minimum size = %d \n", minSampleSize);
    }
    return 0;
}

//Validate the input arguments
void validateInputs(int n, int m, double m_member, double m_insert, double m_delete, int no_of_threads) {
    if (n <= 0) {
        printf("Number of nodes in the linked list cannot be zero or negative\n");
        exit(0);
    }
    if (m <= 0) {
        printf("Number of operations to be performed on the linked list cannot be zero or negative\n");
        exit(0);
    }
    if (m_member + m_insert + m_delete != (double) 1) {
        printf("Fractions of the operations should add upto 1\n");
        exit(0);
    }
    if (no_of_threads <= 0) {
        printf("Number of threads cannot be zero or negative\n");
        exit(0);
    }
}

//Linked List Membership function
int Member(int value) {
    struct list_node_s *current_p = head_p;

    while (current_p != NULL && current_p->data < value) {
        current_p = current_p->next;
    }

    if (current_p == NULL || current_p->data > value) {
        return 0;
    } else
        return 1;
}

//Linked List Insertion function
int Insert(int value) {
    struct list_node_s **head_pp = &head_p;
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL)
            *head_pp = temp_p;
        else
            pred_p->next = temp_p;

        return 1;
    } else
        return 0;
}

//Linked List Deletion function
int Delete(int value) {
    struct list_node_s **head_pp = &head_p;
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) {
            *head_pp = curr_p->next;
            free(curr_p);
        } else {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    } else
        return 0;
}

//This function is executed by all the threads
void *threadFunction(void *dummyPt) {
    struct operations *operationStruct = (struct operations *) dummyPt;
    int local_member_total = operationStruct->memberFuncs;
    int local_delete_total = operationStruct->deletes;
    int local_insert_total = operationStruct->inserts;
    int local_total = local_member_total + local_delete_total + local_insert_total;

    int member_count = 0;
    int insert_count = 0;
    int delete_count = 0;
    int local_count = 0;

    //These flags are used to identify whether each operation is done required number of times
    bool memberOpDone = false;
    bool insertOpDone = false;
    bool deleteOpDone = false;

    while (local_count < local_total) {

        //Generate a random value to select which option to use
        int operation = rand() % 3;

        /*
         * If the flag for a particular operation is false either the
         * read_lock(for member operation) or write_lock(for insert and delete operations)
         * is obtained until the operation is done. Then the lock is unlocked.
         * After that, the local operation count and total count are incremented.
         */
        if (operation == 0 && !memberOpDone) {
            if (member_count < local_member_total) {
                pthread_rwlock_rdlock(&rwlock);
                Member((rand() % RANDOM_LIMIT));
                pthread_rwlock_unlock(&rwlock);
                member_count++;
                local_count++;
            } else {
                memberOpDone = true;
            }
        } else if (operation == 1 && !deleteOpDone) {
            if (delete_count < local_delete_total) {
                pthread_rwlock_wrlock(&rwlock);
                Delete((rand() % RANDOM_LIMIT));
                pthread_rwlock_unlock(&rwlock);
                delete_count++;
                local_count++;
            } else {
                deleteOpDone = true;
            }
        } else if (operation == 2 && !insertOpDone) {
            if (insert_count < local_insert_total) {
                pthread_rwlock_wrlock(&rwlock);
                Insert((rand() % RANDOM_LIMIT));
                pthread_rwlock_unlock(&rwlock);
                insert_count++;
                local_count++;
            } else {
                insertOpDone = true;
            }
        }
    }
    return NULL;
}

//Free the memory used for linked list
void freeList(struct list_node_s **head_pp) {
    struct list_node_s *curr_p;
    struct list_node_s *succ_p;

    if (isEmpty(*head_pp)) return;
    curr_p = *head_pp;
    succ_p = curr_p->next;
    while (succ_p != NULL) {
        free(curr_p);
        curr_p = succ_p;
        succ_p = curr_p->next;
    }
    free(curr_p);
    *head_pp = NULL;
}

//Check if a linked list is empty
int isEmpty(struct list_node_s *head_p) {
    if (head_p == NULL)
        return 1;
    else
        return 0;
}
