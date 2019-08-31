### Multi-Threaded Linked List Operations
This repository includes the following linked list implementations (insert, delete, member operations).

- serial linked list implementation
- linked list implementation with one mutex for the entire linked list
- linked list implementation with read-write lock

To run "serial linked list implementation" (File "serial_linked_list.c"):

    1. Compile using : gcc -g -Wall -o serial serial_linked_list.c -lm
    2. Execute using : ./serial <n> <m> <mMember> <mInsert> <mDelete>
       (Ex : ./serial 1000 10000 0.9 0.05 0.05)


To run "linked list implementation with one mutex for the entire linked list" (File "one_mutex_linked_list.c"):

    1. Compile using : gcc -g -Wall -o mutex one_mutex_linked_list.c -lpthread -lm
    2. Execute using : ./mutex <n> <m> <mMember> <mInsert> <mDelete> <no_of_threads>
       (Ex : ./mutex 1000 10000 0.9 0.05 0.05 4)


To run "linked list implementation with read-write lock" (File "read_write_linked_list.c"):

    1. Compile using : gcc -g -Wall -o read_write read_write_linked_list.c -lpthread -lm
    2. Execute using : ./read_write <n> <m> <mMember> <mInsert> <mDelete> <no_of_threads>
       (Ex : ./read_write 1000 10000 0.9 0.05 0.05 4)
