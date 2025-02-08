#include <stdio.h>
#include <stdlib.h>
#include "headers.h" // Include the BuddyMemory and PCB definitions

int main()
{
    // Initialize root memory block
    BuddyMemory *root = malloc(sizeof(BuddyMemory));
    root->memsize = 1024;
    root->start = 0;
    root->is_free = true;
    root->pcbID = -1;
    root->left = root->right = NULL;

    // Define PCBs with updated structure
    PCB pcb1 = {
        .id = 1,
        .arrival_time = 4,
        .runtime = 11,
        .priority = 9,
        .state = 0, // Example: 0 = Ready
        .remaining_time = 11,
        .waiting_time = 0,
        .pid = -1,
        .start_time = -1,
        .finished_time = -1,
        .stopped_time = -1,
        .restarted_time = -1,
        .remainingTimeAfterStop = -1,
        .memsize = 32,
        .start_address = -1,
        .end_address = -1};

    PCB pcb2 = {
        .id = 2,
        .arrival_time = 7,
        .runtime = 2,
        .priority = 8,
        .state = 0,
        .remaining_time = 2,
        .waiting_time = 0,
        .pid = -1,
        .start_time = -1,
        .finished_time = -1,
        .stopped_time = -1,
        .restarted_time = -1,
        .remainingTimeAfterStop = -1,
        .memsize = 398,
        .start_address = -1,
        .end_address = -1};

    PCB pcb3 = {
        .id = 3,
        .arrival_time = 8,
        .runtime = 28,
        .priority = 0,
        .state = 0,
        .remaining_time = 28,
        .waiting_time = 0,
        .pid = -1,
        .start_time = -1,
        .finished_time = -1,
        .stopped_time = -1,
        .restarted_time = -1,
        .remainingTimeAfterStop = -1,
        .memsize = 383,
        .start_address = -1,
        .end_address = -1};

    PCB pcb4 = {
        .id = 4,
        .arrival_time = 13,
        .runtime = 7,
        .priority = 6,
        .state = 0,
        .remaining_time = 7,
        .waiting_time = 0,
        .pid = -1,
        .start_time = -1,
        .finished_time = -1,
        .stopped_time = -1,
        .restarted_time = -1,
        .remainingTimeAfterStop = -1,
        .memsize = 60,
        .start_address = -1,
        .end_address = -1};

    PCB pcb5 = {
        .id = 5,
        .arrival_time = 22,
        .runtime = 7,
        .priority = 8,
        .state = 0,
        .remaining_time = 7,
        .waiting_time = 0,
        .pid = -1,
        .start_time = -1,
        .finished_time = -1,
        .stopped_time = -1,
        .restarted_time = -1,
        .remainingTimeAfterStop = -1,
        .memsize = 64,
        .start_address = -1,
        .end_address = -1};

    // Test allocations
    printf("Allocating PCB 4 (60 bytes): %s", allocate(root, &pcb4) ? "Success" : "Failed");
    printf(" in start = %d\n", pcb4.start_address);
    printf("Allocating PCB 3 (383 bytes): %s", allocate(root, &pcb3) ? "Success" : "Failed");
    printf(" in start = %d\n", pcb3.start_address);
    printf("Allocating PCB 5 (64 bytes): %s", allocate(root, &pcb5) ? "Success" : "Failed");
    printf(" in start = %d\n", pcb5.start_address);
    printf("Allocating PCB 1 (32 bytes): %s", allocate(root, &pcb1) ? "Success" : "Failed");
    printf(" in start = %d\n", pcb1.start_address);
    printf("Allocating PCB 2 (398 bytes): %s", allocate(root, &pcb2) ? "Success" : "Failed");
    printf(" in start = %d\n", pcb2.start_address);

    // Display memory tree
    printf("\nMemory Tree:\n");
    displayTree(root, 0, "Root");

    //Deallocate PCB 2
    printf("\nDeallocating PCB 1:\n");
    deallocate(root, pcb1.start_address);

    //Display memory tree after deallocation
    printf("\nMemory Tree After Deallocation:\n");
    displayTree(root, 0,"Root");

        //Deallocate PCB 2
    printf("\nDeallocating PCB 5:\n");
    deallocate(root, pcb5.start_address);

    //Display memory tree after deallocation
    printf("\nMemory Tree After Deallocation:\n");
    displayTree(root, 0,"Root");

        //Deallocate PCB 2
    printf("\nDeallocating PCB 4:\n");
    deallocate(root, pcb4.start_address);

    //Display memory tree after deallocation
    printf("\nMemory Tree After Deallocation:\n");
    displayTree(root, 0,"Root");

    //Deallocate PCB 2
    printf("\nDeallocating PCB 3:\n");
    deallocate(root, pcb3.start_address);

    //Display memory tree after deallocation
    printf("\nMemory Tree After Deallocation:\n");
    displayTree(root, 0,"Root");


    // Free the root block
    free(root);
    return 0;
}
