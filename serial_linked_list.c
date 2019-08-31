#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define RANDOM_LIMIT 65536
#define SAMPLE_SIZE 10      //Initial sample size assumption

int n;                      //Number of nodes in the linked list
int m;                      //Number of operations to be performed on linked list
double m_member;            //Fraction of Member operations
double m_insert;            //Fraction of Insert operations
double m_delete;            //Fraction of Delete operations

//Define Nodewdsz
struct list_node_s {
    int data;
    struct list_node_s *next;
};

//Functions
void validateInputs(int n, int m, double m_member, double m_insert, double m_delete);

int Member(int value, struct list_node_s *head_p);

int Insert(int value, struct list_node_s **head_pp);

int Delete(int value, struct list_node_s **head_pp);

int isEmpty(struct list_node_s *head_p);

void freeList(struct list_node_s **head_pp);

double runOperations(int m, double m_member, double m_insert, double m_delete, struct list_node_s *head);

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Number of arguments does not match. Please re-enter command\n");
        exit(0);
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]);
    m_member = atof(argv[3]);
    m_insert = atof(argv[4]);
    m_delete = atof(argv[5]);

    validateInputs(n, m, m_member, m_insert, m_delete);

    int sampleSize = SAMPLE_SIZE;

    //This while loop is repeated until a sufficient sample size is obtained
    while (1) {
        int count = -1;
        double totalTime = 0;
        double elapsedTime = 0;
        double times[sampleSize];
        double squareSum = 0;
        double stdDeviation;

        printf("sample size = %d \n", sampleSize);


        while (count < (sampleSize)) {
            struct list_node_s *head = NULL;
            int nodeCount = 0;

            //Populate the linked list with random unique values between 1-2^16
            while (nodeCount < n) {
                if (Insert((rand() % RANDOM_LIMIT), &head) == 1) {
                    nodeCount++;
                }
            }

            elapsedTime = runOperations(m, m_member, m_insert, m_delete, head);

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
        printf("Minimum sample size = %d \n \n", minSampleSize);
    }
    return 0;
}

//Validate the input arguments
void validateInputs(int n, int m, double m_member, double m_insert, double m_delete) {
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
}

//Linked List Membership function
int Member(int value, struct list_node_s *head_p) {
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
int Insert(int value, struct list_node_s **head_pp) {
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
int Delete(int value, struct list_node_s **head_pp) {
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

//This function runs the random linked_list operations
double runOperations(int m, double m_member, double m_insert, double m_delete, struct list_node_s *head) {
    //Get total number of each operations to be performed
    double member_ops = m_member * m;
    double insert_ops = m_insert * m;
    double delete_ops = m_delete * m;

    int member_count = 0;
    int insert_count = 0;
    int delete_count = 0;

    int total_count = 0;

    clock_t t = clock();

    while (total_count < m) {

        //Generate a random value to select which option to use
        int operation = rand() % 3;

        /*
         * The operation related to randomly selected value is performed if the current
         * number of operations executed is less than the required number of times.
         */
        if (operation == 0 && member_count < member_ops) {
            Member((rand() % RANDOM_LIMIT), head);
            member_count++;
            total_count++;
        } else if (operation == 1 && delete_count < delete_ops) {
            Delete((rand() % RANDOM_LIMIT), &head);
            delete_count++;
            total_count++;
        } else if (operation == 2 && insert_count < insert_ops) {
            Insert((rand() % RANDOM_LIMIT), &head);
            insert_count++;
            total_count++;
        }
    }

    t = clock() - t;
    double time_taken = ((double) t) / CLOCKS_PER_SEC;
    freeList(&head);
    return time_taken;
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
