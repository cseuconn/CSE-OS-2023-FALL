#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MEM_SIZE 8192
#define PAGE_SIZE 4096

//Basic page structure
struct page
{
    int isused;
    int start_address;
    int pagesused;
    int sizealloc;
};

//Basic structure for a memory table
struct page_table
{
    int total_pages;
    int pages_available;
    int size_available;
    struct page pages[MEM_SIZE/PAGE_SIZE];
} *my_page_table;

//Allocations happen here, give it size of allocation and the page_table
void *my_alloc(int request_size, struct page_table *my_table)
{   
    int pages_requested = (request_size/PAGE_SIZE) + 1;
    printf("Pages left: %d Requested: %d\n", my_table->pages_available, pages_requested);
    //If there is not enough room
    if (request_size > my_table->size_available || pages_requested > my_table->pages_available)
    {
        printf("Allocation failed, request size too large\n");
        return NULL;
    }
    //Otherwise we want to loop through memory looking for a contiguous spot to sit
    int page_num = 0;

    //Go until it searches the entire table unsuccessfully or it returns a page number
    while (page_num != my_table->total_pages)
    {
        int empty_blocks = 0;
        int current_page = page_num;
        //This loop checks if there is enough space from the current page number
        while(empty_blocks < pages_requested)
        {
            //Check to see if block is empty, if it's not break
            //Otherwise check the next block
            if (my_table->pages[current_page].isused == 0)
            {
                empty_blocks++;
            } else
            {
                break;
            }
        }

        //Check to see if there are enough blocks to leave the outer loop
        if (empty_blocks >= pages_requested)
        {
            break;
        } else {
            page_num++;
        }
    }

    //No contiguous spot in memory found return NULL
    if (page_num == my_table->total_pages){
        printf("Allocation failed, no spot is available\n");
        return NULL;
    }

    int start_addr = page_num*PAGE_SIZE;
    //Otherwise it holds the spot in memory, and we must update the pages to show it's being used
    my_table->pages_available -= pages_requested;
    my_table->size_available -= request_size;
    for (int i = 0; i < pages_requested; i++)
    {
        my_table->pages[page_num].isused = 1;
        my_table->pages[page_num].start_address = start_addr;
        my_table->pages[page_num].pagesused = pages_requested;
        my_table->pages[page_num].sizealloc = request_size;
    }
    printf("Successfully Allocated!\n");
    return NULL;
}

//Deallocating function, you give it the starting page and the page_table and it updates the pages to show they're not used
void *my_dealloc(int start_page, struct page_table *my_table)
{
    //Check to make sure there is something actually allocated at the address (a soft check, this is a dangerous dealloc)
    if (my_table->pages[start_page].pagesused == 0)
    {
        printf("Deallocation failed, page given is already empty\n");
        return NULL;
    }

    //Set the current page
    int cur_page = start_page;
    my_table->pages_available += my_table->pages[start_page].pagesused;
    my_table->size_available += my_table->pages[start_page].sizealloc;
    //Starting at the page given until the number of pages allocated, fix the page information
    for(int i = 0; i < my_table->pages[start_page].pagesused; i++)
    {
        my_table->pages[cur_page].isused = 0;
        my_table->pages[cur_page].start_address = -1;
        my_table->pages[cur_page].pagesused = 0;
        my_table->pages[cur_page].sizealloc = 0;
    }
    printf("Successfully Deallocated!\n");
    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    struct page_table *my_page_table = (struct page_table*)malloc(sizeof(struct page_table));
    //Initialize the page_table
    printf("Starting main MemSize: %d PageSize: %d\n", MEM_SIZE, PAGE_SIZE);
    my_page_table->pages_available = MEM_SIZE/PAGE_SIZE;
    printf("%d\n", MEM_SIZE/PAGE_SIZE);
    my_page_table->size_available = MEM_SIZE;
    my_page_table->total_pages = my_page_table->pages_available;
    printf("Pages available: %d\n", my_page_table->pages_available);
    for(int i = 0; i < MEM_SIZE/PAGE_SIZE; i++)\
    {
        my_page_table->pages[i].isused = 0;
        my_page_table->pages[i].start_address = -1;
        my_page_table->pages[i].pagesused = 0;
    }
    //////////////////////////////////////////////

    my_alloc(3023, my_page_table);
    my_dealloc(0, my_page_table);
    my_alloc(5000, my_page_table);
    my_dealloc(0, my_page_table);
    my_alloc(2000, my_page_table);

    //Might want to have actual threads to show use of the page_table and alloc/dealloc functions
    return 0;
}