#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//-----------------------------
// Variables
//-----------------------------
char* memAvailable = "EMPTY";
char* endOfList = "ENDOFLIST";
char* algo = "FIRSTFIT";
char* null = "NULL";

int quantum;
int processes;
int space;
int listLength = 0;

//-----------------------------
// Structs
//-----------------------------

struct File {
    int lastExecuted;
    int completed;
};

struct File* File_new() {
    struct File* ptr = malloc(sizeof (struct File));
    ptr->lastExecuted = 0;
    ptr->completed = 0;
    return ptr;
}

struct Chunk {
    char string[17];
    int size;
};

struct Chunk* Chunk_new(char* aString, int aSize) {
    struct Chunk* ptr = malloc(sizeof (struct Chunk));
    strcpy(ptr->string, aString);
    ptr->size = aSize;
    return ptr;
}


//-----------------------------
// ListNode (LinkedList) struct
//-----------------------------
struct ListNode {
    struct ListNode* previous;
    struct Chunk* data;
    struct ListNode* next;
};

struct ListNode* head;
struct ListNode* tail;

struct ListNode* ListNode_new(struct ListNode* previous, struct Chunk* data, struct ListNode* next) {
    struct ListNode* ptr = malloc(sizeof (struct ListNode));
    ptr->previous = previous;
    ptr->data = data;
    ptr->next = next;
    return ptr;
}

struct ListNode* ListNode_newData(struct Chunk* data) {
    struct ListNode* ptr = malloc(sizeof (struct ListNode));
    ptr->previous = NULL;
    ptr->data = data;
    ptr->next = NULL;
    return ptr;
}

void removeHead() {
    head->next->previous = NULL;
    free(head);
    head = head->next;
    listLength--;
}

void removeTail() {
    tail->previous->next = NULL;
    tail = tail->previous;
    listLength--;
}

void removeNode(struct ListNode* this) {
    if(this == head) {
        removeHead();
    } else if(this == tail) {
        removeTail();
    } else {
        this->previous->next = this->next;
        this->next->previous = this->previous;
        free(this);
        listLength--;
    }
}

void addHead(struct ListNode* toAdd) {
    head->previous = toAdd;
    toAdd->next = head;
    toAdd->previous = NULL;
    head = toAdd;
    listLength++;
}

void addEnd(struct ListNode* toAdd) {
    tail->next = toAdd;
    toAdd->previous = tail;
    tail = tail->next;
    listLength++;
}

void addBefore(char* string, int size, struct ListNode* addedBefore) {
    struct Chunk* chunk = Chunk_new(string, size);
    struct ListNode* node = ListNode_newData(chunk);

    if(addedBefore == head) {
        addHead(node);
        head->next->data->size -= size;

    //This code works for all non-head nodes    
    } else {
        node->next = addedBefore;
        node->previous = addedBefore->previous;
        node->previous->next = node;
        addedBefore->previous = node;
        addedBefore->data->size -= size;
        listLength++;
    }
}

//-----------------------------
// Input Processing + Printing
//-----------------------------
void readInput(int argc, char** argv) {
    if(argc >= 4){
        quantum = strtol(argv[1], NULL, 10);
        processes = strtol(argv[2], NULL, 10); 
        space = strtol(argv[3], NULL, 10);
    }    
}

void printInput() {
    printf("QUANTUM: %d\nPROCESSES: %d\nSPACE: %d\nALGO: %s\n", quantum, processes, space, algo);
}

void printList() {
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {
        if(current->previous != NULL) {
            printf("PREV: %s, ", current->previous->data->string);
        } else {
            printf("PREV: NULL, ");
        }
        printf("DATA: %s, ", current->data->string);
        if(current->next != NULL) {
            printf("NEXT: %s\n", current->next->data->string);
        } else {
            printf("NEXT: NULL\n");
        }
        current = current->next;
    }
}


void printNode(struct ListNode* node) {
    if(node->previous != NULL) {
        printf("PREVIOUS: %s, ", node->previous->data->string);
    } else {
        printf("PREVIOUS: NULL, ");
    }
    printf("DATA: %s, ", node->data->string);
    if(node->next != NULL) {
        printf("NEXT: %s\n", node->next->data->string);
    } else {
        printf("NEXT: NULL\n");
    }
}

void printMemory() {
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {
        printf("%s : %d\n", current->data->string, current->data->size);
        current = current->next;
    }
}

//-----------------------------
// Input Commands
//-----------------------------
int listAvailable() {
    int ret = 0;
    int currentIndex = 0;
    int firstPrint = 1;
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {
        if(strcmp(memAvailable, current->data->string) == 0) {
            if(firstPrint) {
                printf("(%d, %d)", current->data->size, currentIndex);
                firstPrint = 0;
                ret = 1;
            } else {
                printf(" (%d, %d)", current->data->size, currentIndex);
                ret = 1;
            }
        }
        currentIndex += current->data->size;
        current = current->next;
    }

    if(!firstPrint) {
        printf("\n");
    }
    return ret;
}

int listAssigned() {
    int ret = 0;
    int currentIndex = 0;
    int firstPrint = 1;
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {
        if(strcmp(memAvailable, current->data->string) != 0) {
            if(firstPrint) {
                printf("(%s, %d, %d)", current->data->string, currentIndex, current->data->size);
                firstPrint = 0;
                ret = 1;
            } else {
                printf(" (%s, %d, %d)", current->data->string, currentIndex, current->data->size);
                ret = 1;
            }
        }
        currentIndex += current->data->size;
        current = current->next;
    }
    return ret;
}

int find(char* string) {
    int currentIndex = 0;
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {
        if(strcmp(string, current->data->string) == 0) {
            printf("(%s, %d, %d)\n", current->data->string, current->data->size, currentIndex);
            return 1;
        }
        currentIndex += current->data->size;
        current = current->next;
    }
    return 0;
}

void release(char* string) {
    int released = 0;
    int releasedLength = 0;
    int currentIndex = 0;
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++){
        if(strcmp(string, current->data->string) == 0) {

            //Only the tail and head nodes should have a pointer to NULL
            if(current != tail && current != head) {

                //if previous and next are empty
                if(strcmp(memAvailable, current->previous->data->string) == 0 && strcmp(memAvailable, current->next->data->string) == 0) {
                    current->previous->data->size += (current->next->data->size + current->data->size);
                    releasedLength = current->data->size;
                    released = 1;

                    struct ListNode* temp = current;
                    removeNode(temp->next);
                    removeNode(temp);
                    current = current->previous;
                    break;

                //if ONLY next is empty
                } else if(strcmp(memAvailable, current->next->data->string) == 0) {
                    current->data->size += current->next->data->size;
                    releasedLength = current->data->size;
                    released = 1;
                    
                    struct ListNode* temp = current;
                    current = current->previous;
                    removeNode(temp);
                    break;
                
                //if ONLY previous is empty
                } else if (strcmp(memAvailable, current->previous->data->string) == 0) {
                    current->previous->data->size += current->data->size;
                    releasedLength = current->data->size;
                    released = 1;

                    struct ListNode* temp = current;
                    current = current->previous;
                    removeNode(temp);
                    break;
                
                //if previous and next are different and non-empty
                } else {
                    strcpy(current->data->string, memAvailable);
                    releasedLength = current->data->size;
                    released = 1;
                    break;
                }
            
            //next two cases are for when current is tail
            } else if(current == tail) {  

                //if previous is empty 
                if(strcmp(current->previous->data->string, memAvailable) == 0) {
                    current->previous->data->size += current->data->size;
                    releasedLength = current->data->size;
                    released = 1;

                    removeTail();
                    break;

                //if previous is non-empty
                } else {
                    strcpy(current->data->string, memAvailable);
                    releasedLength = current->data->size;
                    released = 1;
                    break;
                }

            //next two case are for when current is head   
            } else if(current == head) {
                
                //if next is empty 
                if(strcmp(current->next->data->string, memAvailable) == 0) {
                    current->next->data->size += current->data->size;
                    releasedLength = current->data->size;
                    released = 1;

                    removeHead();
                    break; 
                
                //if next is non-empty
                } else {
                    strcpy(current->data->string, memAvailable);
                    releasedLength = current->data->size;
                    released = 1;
                    break;
                }
            }

        }
        currentIndex += current->data->size;
        current = current->next;
    }

    if(released) {
        printf("FREE %s %d %d\n", string, releasedLength, currentIndex);
    } else {
        printf("FAIL RELEASE %s\n", string);
    }
}

void compactMemory() {
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {

        //if current is head
        if(current == head) {
            if(strcmp(current->data->string, current->next->data->string) == 0) {
                current->data->size += current->next->data->size;
                removeNode(current->next);
            }

        //if current is tail    
        } else if(current == tail) {
            if(strcmp(current->data->string, current->previous->data->string) == 0) {
                current->previous->data->size += current->data->size;
                removeTail();
            }
        } else {

            //if next and previous are equal to current
            if(strcmp(current->data->string, current->previous->data->string) == 0 && strcmp(current->data->string, current->next->data->string) == 0) {
                current->previous->data->size += (current->data->size + current->next->data->size);
                struct ListNode* temp = current;
                current = current->previous;
                removeNode(temp->next);
                removeNode(temp);

            //if ONLY previous and current are the same    
            } else if(strcmp(current->data->string, current->previous->data->string) == 0) {
                current->previous->data->size += current->data->size;
                struct ListNode* temp = current;
                current = current->previous;
                removeNode(temp);

            //if ONLY current and next are the same    
            } else if(strcmp(current->data->string, current->next->data->string) == 0) {
                current->data->size += current->next->data->size;
                removeNode(current->next);
            }
            
        }
        current = current->next;
    }
}

void requestFirstFit(char* string, int size) {
    int success = 0;
    int currentIndex = 0;

    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {
        if(strcmp(current->data->string, memAvailable) == 0 && current->data->size >= size) {
            addBefore(string, size, current);
            success = 1;
            break;
        }
        currentIndex += current->data->size;
        current = current->next;
    }
    if(success) {
        printf("ALLOCATED %s %d\n", string, currentIndex);
    } else {
        printf("FAIL REQUEST %s %d\n", string, size);
    }
}

//-----------------------------
// Main Method
//-----------------------------
int main(int argc, char** argv) {

    readInput(argc, argv);

    head = ListNode_new(NULL, Chunk_new(memAvailable,10), NULL);
    listLength++;
    tail = head;

    struct Chunk* chunk2 = Chunk_new("2", 34);
    struct Chunk* chunk3 = Chunk_new(memAvailable, 30);
    struct Chunk* chunk4 = Chunk_new("4", 28);
    struct ListNode* temp2 = ListNode_newData(chunk2);
    struct ListNode* temp3 = ListNode_newData(chunk3);
    struct ListNode* temp4 = ListNode_newData(chunk4);
    
    addEnd(temp2);
    addEnd(temp3);
    addEnd(temp4);
    
    printf("\n-----------------------\nMemory...\n");
    printMemory();

    /*
    printf("\n-----------------------\nAssigned...\n");
    if(listAssigned() == 0) {
        printf("NONE ASSIGNED\n");
    }

    printf("\n-----------------------\nAvailable...\n");
    if(listAvailable() == 0) {
        printf("ALL FULL\n");
    }

    char* str = "THAT";
    printf("\n-----------------------\nFinding %s...\n", str);
    if(find(str) == 0) {
        printf("\"%s\" NOT FOUND\n", str);
    }

    char* toRemove = "C";
    printf("\n-----------------------\nReleasing %s...\n", toRemove);
    release(toRemove);

    printf("\n");
    printMemory();

    printf("\n-----------------------\nCompacting...\n");
    compactMemory();
    printMemory();

    char* toAdd = "3.5";
    char* after = "4";
    printf("\n-----------------------\nAdding %s before %s...\n", toAdd, after);
    addBefore(toAdd, 10, temp4);
    printMemory();
    */

    char* toAdd = "NEW";
    printf("\n--------------\nRequesting FIRSTFIT for %s...\n", toAdd);
    requestFirstFit(toAdd, 100);
    printMemory();




}