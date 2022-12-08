## C S 3113 - Project 3 - *Advanced CPU Scheduling*
### *Brady Carden*
-------------------
## How To Run
- Open a terminal in the folder where all files are located
- Run the command `make ready`. This custom command runs `make clean` and `make all` in one user instruction
- Run `./project3 q p N ALGO` with appropriate values for each

## Bugs 
- When dealing with numerous large input files, the scheduler seems to grow incorrect

## Assumptions
- the values for `q` will be from 1-8
- the values for `p` will be from  1-8
- the value for `ALGO` will be "FIRSTFIT", implementation for BuddyFit is not complete

## Structs
`scriptFile`
- Used to store data about a script file
- Contains data such as the file pointer, whether or not the file is completed, and whether or not the last allocate command failed

`Chunk`
- This represents a chunk of memory.
- Contains a name for what occupies the chunk of memory and the size it occupies

`fileNode`
- Circular Linked List used to store script files

`listNode`
- Doubly Linked List used to store memory

## Functions
`void assignFilePtrs()`
- This functions takes values from input and creates a circular linked list of the script files 


`void removeFile(struct FileNode* toRemove)`
- This is used to remove a completed file from the linked list of files

`void removeHead()`
- Used to remove the head of doubly linked list used to store memory


`void removeTail()`
- Used to remove the tail of doubly linked list used to store memory

`void removeNode(struct ListNode* this)`
- Removes `this` from memory

`void addHead(struct ListNode* toAdd)`
- adds `toAdd` to the front of memory

`void addEnd(struct ListNode* toAdd)`
- adds `toAdd` to the back of memory

`struct ListNode* addBefore(char* string, int size, struct ListNode* addedBefore)`
- This is used to insert data into memory directly infront of `addedBefore`

`void initializeList(int size)`
- Initializes and sets size of memory linked list

`void readInput(int argc, char** argv)`
- Reads input from `./project3 q p N ALGO` 

`void printInput()`
- Prints what was read from `readInput()`, used in testing

`void printList()`
- Prints values for previous, data, and next of every node in memory
- Used in testing

`void printNode(struct ListNode* node)`
- Prints values for previous, data, and next of a node in memory
- Used in testing

`void printFileNode(struct FileNode* node)`
- Does the same as `printNode()` but is slightly edited to handle the FileNodes
- Used in testing

`void printMemory()`
- Prints name and size of every node in memory

- `listAvailable`, `listAssigned`, `find`, `release`, and `requestFirstFit` all behave as described in project description

- `compactMemory`, `requestBuddySystem`, `splitMemory` is the unfinished implementation of BUDDY algorithm
---------

## Choosing the Quantum
Choice of quantum is important as it affects the user's perception of a computer's multitasking ability. If the quantum is large, meaning many instructions of one script are executed before changing to a different script, the user may see these programs performing in a choppy manner. Each program being rotated through the CPU scheduler will perform well for a brief second before stopping while other programs run.

If the quantum is a smaller value, the user will see each program running more consistently and smoothly. No program is able to take much time from another as each are being executed for a very brief amount of time.