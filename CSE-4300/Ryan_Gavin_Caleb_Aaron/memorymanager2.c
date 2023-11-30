#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MEM_SIZE 8192
#define PAGE_SIZE 4096

// Basic page structure
struct page
{
    int isused;
    int start_address;
    int pagesused;
    int sizealloc;
};

// Basic structure for a memory table
struct page_table
{
    int total_pages;
    int pages_available;
    int size_available;
    struct page pages[MEM_SIZE / PAGE_SIZE];
} *my_page_table;
// Mutex for thread synchronization
pthread_mutex_t mutex;

// Function to initialize the page table
void initialize_page_table(struct page_table *my_table)
{
    my_table->pages_available = MEM_SIZE / PAGE_SIZE;
    my_table->size_available = MEM_SIZE;
    my_table->total_pages = my_table->pages_available;

    for (int i = 0; i < MEM_SIZE / PAGE_SIZE; i++)
    {
        my_table->pages[i].isused = 0;
        my_table->pages[i].start_address = -1;
        my_table->pages[i].pagesused = 0;
        my_table->pages[i].sizealloc = 0;
    }
}

// Function to allocate memory
void *my_alloc(int request_size, struct page_table *my_table)
{
    pthread_mutex_lock(&mutex);
    int pages_requested = (request_size / PAGE_SIZE) + 1;
    printf("Pages left: %d Requested: %d\n", my_table->pages_available, pages_requested);

    // If there is not enough room
    if (request_size > my_table->size_available || pages_requested > my_table->pages_available)
    {
        printf("Allocation failed, request size too large\n");

        // Release the lock
        pthread_mutex_unlock(&mutex);

        return NULL;
    }

    // Acquire the lock
    

    // Find a contiguous spot in memory
    int page_num = 0;

    while (page_num != my_table->total_pages)
    {
        int empty_blocks = 0;
        int current_page = page_num;

        // Check if there is enough space from the current page number
        while (empty_blocks < pages_requested)
        {
            if (my_table->pages[current_page].isused == 0)
            {
                empty_blocks++;
            }
            else
            {
                break;
            }
            current_page++;
        }

        // Check if there are enough blocks to allocate
        if (empty_blocks >= pages_requested)
        {
            break;
        }
        else
        {
            page_num++;
        }
    }

    // No contiguous spot in memory found, return NULL
    if (page_num == my_table->total_pages)
    {
        printf("Allocation failed, no spot is available\n");

        // Release the lock
        pthread_mutex_unlock(&mutex);

        return NULL;
    }

    int start_addr = page_num * PAGE_SIZE;

    // Update the page table to show the allocated memory
    my_table->pages_available -= pages_requested;
    my_table->size_available -= request_size;

    for (int i = 0; i < pages_requested; i++)
    {
        my_table->pages[page_num].isused = 1;
        my_table->pages[page_num].start_address = start_addr;
        my_table->pages[page_num].pagesused = pages_requested;
        my_table->pages[page_num].sizealloc = request_size;
        page_num++;
    }

    printf("Successfully Allocated!\n");

    // Release the lock
    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Function to deallocate memory
void *my_dealloc(int start_page, struct page_table *my_table)
{
    pthread_mutex_lock(&mutex);

    // Check if there is something actually allocated at the address
    if (my_table->pages[start_page].pagesused == 0)
    {
        printf("Deallocation failed, nothing allocated at this address\n");

        // Release the lock
        pthread_mutex_unlock(&mutex);

        return NULL;
    }

    // Acquire the lock
    

    // Update the page table to show the deallocated memory
    int pages_used = my_table->pages[start_page].pagesused;
    int size_alloc = my_table->pages[start_page].sizealloc;

    my_table->pages_available += pages_used;
    my_table->size_available += size_alloc;

    for (int i = 0; i < pages_used; i++)
    {
        my_table->pages[start_page].isused = 0;
        my_table->pages[start_page].start_address = -1;
        my_table->pages[start_page].pagesused = 0;
        my_table->pages[start_page].sizealloc = 0;
        start_page++;
    }

    printf("Successfully Deallocated!\n");

    // Release the lock
    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Thread function to allocate and deallocate memory at random intervals, with random sizes.
void *thread_func(void *arg)
{
    int *thread_num = (int *)arg;
    int request_size = 0;
    int sleep_time = 0;

    // Allocate and deallocate memory 5 times



    for (int i = 0; i < 5; i++)
    {
        // Generate a random request size
        request_size = rand() % 10000 + 1;

        // Generate a random sleep time
        sleep_time = rand() % 5 + 1;


        // Allocate memory
        printf("Thread %d: Allocating %d bytes\n", *thread_num, request_size);
        my_alloc(request_size, my_page_table);
        printf("Thread %d: Pages available: %d\n", *thread_num, my_page_table->pages_available);
        printf("Thread %d: Size available: %d\n", *thread_num, my_page_table->size_available);
        printf("\n");

        // Sleep for a random amount of time
        sleep(sleep_time);


        // Deallocate memory
        printf("Thread %d: Deallocating starting at page %d\n", *thread_num, 0);
        my_dealloc(0, my_page_table);
        printf("Thread %d: Pages available: %d\n", *thread_num, my_page_table->pages_available);
        printf("Thread %d: Size available: %d\n", *thread_num, my_page_table->size_available);
        printf("\n");



        // Sleep for a random amount of time
        sleep(sleep_time);
    }

    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    my_page_table = (struct page_table *)malloc(sizeof(struct page_table));

    // Initialize the page table
    printf("Starting main MemSize: %d PageSize: %d\n", MEM_SIZE, PAGE_SIZE);
    initialize_page_table(my_page_table);
    printf("Pages available: %d\n", my_page_table->pages_available);

    // Initialize the mutex
    pthread_mutex_init(&mutex, NULL);

    printf("Test Case 1: Allocate 1 page\n");
    my_alloc(2000, my_page_table);
    printf("Pages available: %d\n", my_page_table->pages_available);
    printf("Size available: %d\n", my_page_table->size_available);
    printf("\n");

    printf("Test Case 2: Allocate memory that exceeds available size\n");
    my_alloc(10000, my_page_table);
    printf("Pages available: %d\n", my_page_table->pages_available);
    printf("Size available: %d\n", my_page_table->size_available);
    printf("\n");

    printf("Test Case 3: Deallocate memory\n");
    my_dealloc(0, my_page_table);
    printf("Pages available: %d\n", my_page_table->pages_available);
    printf("Size available: %d\n", my_page_table->size_available);
    printf("\n");

    printf("Test Case 4: Deallocate already deallocated memory\n");
    my_dealloc(0, my_page_table);
    printf("Pages available: %d\n", my_page_table->pages_available);
    printf("Size available: %d\n", my_page_table->size_available);
    printf("\n");

    printf("Test Case 5: Allocate memory after deallocation\n");
    my_alloc(3000, my_page_table);
    printf("Pages available: %d\n", my_page_table->pages_available);
    printf("Size available: %d\n", my_page_table->size_available);
    printf("\n");

    printf("Deallocating all memory\n");
    my_dealloc(0, my_page_table);

    printf("\n");

    printf("Test Case 6: Allocate memory that requires multiple pages\n");
    my_alloc(6000, my_page_table);
    printf("Pages available: %d\n", my_page_table->pages_available);
    printf("Size available: %d\n", my_page_table->size_available);
    printf("\n");

    printf("Deallocating all memory\n");
    my_dealloc(0, my_page_table);
    my_dealloc(1, my_page_table);



    // Use 2 threads to allocate and deallocate memory using the function *thread_func*
    pthread_t thread1, thread2;
    int thread_num1 = 1;
    int thread_num2 = 2;

    printf("Test Case 7: Using threads to allocate and deallocate memory\n");
    pthread_create(&thread1, NULL, thread_func, &thread_num1);
    pthread_create(&thread2, NULL, thread_func, &thread_num2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    
    return 0;
}

