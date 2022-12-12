
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//-----------------------------
// Variables
//-----------------------------
char* memAvailable = "EMPTY";
char* algo = "FIRSTFIT";
char* null = "NULL";

char* comment = "#";
char* request = "REQUEST";
char* release_ = "RELEASE";
char* list = "LIST";
char* find_ = "FIND";

int quantum;
int processes;
int space;
int listLength = 0;
int incompleteFiles;

char* fileNames[8] = {"0.ins", "1.ins", "2.ins", "3.ins", "4.ins", "5.ins", "6.ins", "7.ins"};

//-----------------------------
// Structs
//-----------------------------
struct scriptFile {
    int completed;
    FILE* filePtr;
    int lastAllocFailed;
    char* lastAllocFailedName;
    int lastAllocFailedSize;
};

struct scriptFile* scriptFile_new(char* fileName) {
    struct scriptFile* ptr = malloc(sizeof (struct scriptFile));
    ptr->completed = 0;
    ptr->filePtr = fopen(fileName, "r");
    ptr->lastAllocFailed = 0;
    ptr->lastAllocFailedName = memAvailable;
    ptr->lastAllocFailedSize = 0;
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
// FileNode (Circular LinkedList)
//-----------------------------
struct FileNode {
    struct FileNode* previous;
    struct scriptFile* data;
    struct FileNode* next;
};

struct FileNode* fileHead;
struct FileNode* fileTail;

struct FileNode* FileNode_new(struct scriptFile* data) {
    struct FileNode* ptr = malloc(sizeof (struct FileNode));
    ptr->previous = NULL;
    ptr->data = data;
    ptr->next = NULL;
    return ptr;
}

void assignFilePtrs() {
    fileHead = FileNode_new(scriptFile_new(fileNames[0]));
    fileHead->next = fileHead;
    fileHead->previous = fileTail;
    fileTail = fileHead;

    for(int i = 1; i < processes; i++) {
        fileTail->next = FileNode_new(scriptFile_new(fileNames[i]));
        fileTail->next->previous = fileTail;
        fileTail = fileTail->next;
        fileTail->next = fileHead;
        fileHead->previous = fileTail;
    }
}

void removeFile(struct FileNode* toRemove) {
    if(toRemove == fileHead) {
        fileHead = fileHead->next;
        fileTail->next = fileHead;
        free(toRemove);
    } else if(toRemove == fileTail) {
        fileTail->previous->next = fileHead;
        struct FileNode* temp = fileTail;
        fileTail = fileTail->previous;
        free(temp);
        fileHead->previous = fileTail;
    } else {
        toRemove->previous->next = toRemove->next;
        toRemove->next->previous = toRemove->previous;
    }
    incompleteFiles--;
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

struct ListNode* ListNode_new(struct Chunk* data) {
    struct ListNode* ptr = malloc(sizeof (struct ListNode));
    ptr->previous = NULL;
    ptr->data = data;
    ptr->next = NULL;
    return ptr;
}

void removeHead() {
    head->next->previous = NULL;
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
    struct ListNode* node = ListNode_new(chunk);

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
        if(addedBefore->data->size == 0) {
            removeNode(addedBefore);
        }
        listLength++;
    }
}

void initializeList(int size) {
    head = ListNode_new(Chunk_new(memAvailable, size));
    tail = head;
    listLength++;
}

//-----------------------------
// Input Processing + Printing
//-----------------------------
void readInput(int argc, char** argv) {
    if(argc >= 4){
        quantum = strtol(argv[1], NULL, 10);
        processes = strtol(argv[2], NULL, 10); 
        space = strtol(argv[3], NULL, 10);
        algo = argv[4];
        incompleteFiles = processes;
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

void printFileNode(struct FileNode* node) {
    if(node->previous != NULL) {
        printf("PREVIOUS: %d, ", node->previous->data->completed);
    } else {
        printf("PREVIOUS: NULL, ");
    }
    printf("DATA: %d, ", node->data->completed);
    if(node->next != NULL) {
        printf("NEXT: %d\n", node->next->data->completed);
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
    if(ret == 0) {
        printf("FULL");
    }
    printf("\n");
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
                printf("(%s, %d, %d)", current->data->string, current->data->size,  currentIndex);
                firstPrint = 0;
                ret = 1;
            } else {
                printf(" (%s, %d, %d)", current->data->string, current->data->size,  currentIndex);
                ret = 1;
            }
        }
        currentIndex += current->data->size;
        current = current->next;
    }
    printf("\n");
    if(!ret) {
        printf("NONE\n");
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
        if(released == 0) {
            currentIndex += current->data->size;
            current = current->next;
        }
    }

    //Determines printed output
    if(released) {
        printf("FREE %s %d %d\n", string, releasedLength, currentIndex);
    } else {
        printf("FAIL RELEASE %s\n", string);
    }
}

int requestFirstFit(char* string, int size) {
    int success = 0;
    int currentIndex = 0;
    struct ListNode* current = head;
    for(int i = 0; i < listLength; i++) {
        if(strcmp(current->data->string, memAvailable) == 0 && current->data->size >= size) {
            if(current->data->size == size) {
                strcpy(current->data->string, string);
            } else {
                addBefore(string, size, current);
            }
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

    return success;
}

//-----------------------------
// Main Method
//-----------------------------
int main(int argc, char** argv) {

    readInput(argc, argv);
    initializeList(space);
    assignFilePtrs();

    char func[10];
    char name[10];
    int size;
    
    struct FileNode* currentFile = fileHead;
    while(incompleteFiles > 0) {
        int instrsRun = 0;
        while(instrsRun < quantum) {
            
            //Handles deadlock check -> Requests again if failed previously
            if(currentFile->data->lastAllocFailed == 1) {
                if(requestFirstFit(currentFile->data->lastAllocFailedName, currentFile->data->lastAllocFailedSize) == 0) {
                    printf("DEADLOCK DETECTED\n");
                    return 0;
                } else {
                    instrsRun++;
                }
            } 

            //EOF
            if(fscanf(currentFile->data->filePtr, "%s", func) == EOF) {
                currentFile->data->completed = 1;
                removeFile(currentFile);
                instrsRun++;
                break;
            }
                 
            //Comment
            else if(strcmp(func, comment) == 0) {

                //Skips current line
                fscanf(currentFile->data->filePtr, "%*[^\n]\n");

            //Find
            } else if(strcmp(func, find_) == 0) {
                fscanf(currentFile->data->filePtr, "%s", name);
                find(name);
                instrsRun++;

            //Request
            } else if(strcmp(func, request) == 0) {
                fscanf(currentFile->data->filePtr, "%s %d", name, &size);

                //If can't allocate...
                if(requestFirstFit(name, size) == 0) {
                    currentFile->data->lastAllocFailedName = "A";
                    //strcpy(currentFile->data->lastAllocFailedName, name);
                    currentFile->data->lastAllocFailedSize = size;

                    //Single thread case
                    if(processes == 1) {
                        requestFirstFit(name, size);
                        printf("DEADLOCK DETECTED\n");
                        return 0;
                    }
                    
                    /*
                    //Deadlock case
                    if(currentFile->data->lastAllocFailed == 1) {
                        printf("DEADLOCK DETECTED\n");
                        return 0;
                    }
                    */
                     currentFile->data->lastAllocFailed = 1;
                    break;
                } else {
                    currentFile->data->lastAllocFailed = 0;
                    currentFile->data->lastAllocFailedName = memAvailable;
                    //strcpy(currentFile->data->lastAllocFailedName, memAvailable);
                    currentFile->data->lastAllocFailedSize = 0;
                    instrsRun++;
                }

            //Release
            } else if(strcmp(func, release_) == 0) {
                fscanf(currentFile->data->filePtr, "%s", name);
                release(name);
                instrsRun++;

            //List
            } else if(strcmp(func, list) == 0) {
                fscanf(currentFile->data->filePtr, "%s", name);
                if(strcmp(name, "AVAILABLE") == 0 ) {
                    listAvailable(); 
                } else if(strcmp(name, "ASSIGNED") == 0) {
                    listAssigned();
                }
                instrsRun++;
            }
        }
        currentFile = currentFile->next;
    }
    return 0;
}
