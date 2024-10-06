class Memory {
private:

    /* Struct that holds data for every pointer we distribute. */
    typedef struct {
        void* ptr;
        size_t size;
        size_t oloc;
    } Allocated;

    unsigned long set_size = 0; //Lowkey unnecessary two vars but wtv
    unsigned long total_bytes_available = 0;
    int ctr = 0; //Counter for Allocated* sizes array.
    void* main; //Actual memory storage.
    Allocated* sizes; //Stored data for every pointer we distribute.
    int* positions; //Array of integers representing every single byte we have.

public:
    Memory(unsigned long size_bytes) {
        main = malloc(size_bytes);

        positions = (int*)calloc(size_bytes, sizeof(int)); 
        set_size = size_bytes;
        total_bytes_available = size_bytes;
    }
    ~Memory() { //Free all memory on class destruction.
        free(main);
        free(positions);
    }
   
    void dealloc(void* ptr) {
        int dist = 0;
        if (ptr == nullptr) { //If granted a null pointer, there's nothing to free.
            return;
        }

        for (int i = 0; i < ctr; i++) {
            std::cout << ptr << " vs " << sizes[i].ptr << "\n";
            if (ptr == sizes[i].ptr) {
                //Found it.
                //Okay, we have no reason to actually change the values of the memory we're freeing. Just change the positions so it's available to give away once again.

                //Using the same values as before, set the taken bytes in our positions tracker to 0, "freeing" the memory for new use.
                for (int sp = 0; sp < (sizes[i].size); sp++) { 
                    positions[sizes[i].oloc + sp] = 0;
                }

                //Remove the data for the old memory.
                Allocated* new_sizes = (Allocated*)(malloc(sizeof(Allocated) * (ctr - 1)));
                memcpy(new_sizes, sizes, i * sizeof(Allocated));
                memcpy(new_sizes + i, sizes + i + 1, ((ctr - 1 - i) * sizeof(Allocated)));

                return;
            }
        }
        
        //We did not locate the pointer and cannot free it.
    }

    //alloc(5, sizeof(int)) -> 5 integers
    void* _cdecl alloc(unsigned int amt, unsigned int typesize) {
        unsigned long bytes_required = amt * typesize; //Calculate total required bytes.

        if (bytes_required > total_bytes_available) { //Not even worth searching for.
            return nullptr; 
        }

        //Locate available memory of that size.
        size_t last_unavail = 0;
        bool located = false;
        for (int i = 0; i <= total_bytes_available; i += typesize) {
            if (positions[(i / typesize)] != 0) {
                last_unavail = i+1; //Add 1 to prevent overlap.
            }
            if (((i - last_unavail + 1) / typesize) == amt) { //If the distance between the two points divided by the size is equal to the amount.
                located = true;
                break;
            }
        }

        if (located) { //If sufficient memory was found.
            for (int i = 0; i < (bytes_required); i++) {
                *(positions + (last_unavail / typesize) + i) = 1; //Set all the byte positions to taken.
            }

            Allocated* new_sizes = (Allocated*)(malloc(sizeof(Allocated) * (ctr + 1)));
            memcpy(new_sizes, sizes, sizeof(Allocated) * ctr);

            Allocated n; //Create new allocated struct.
            n.ptr = (void*)((char*)(main)+last_unavail); //Set the pointer to the same one we will return.
            n.size = bytes_required; //Set size to bytes taken.
            n.oloc = (last_unavail / typesize); //Set the original position location.
            new_sizes[ctr] = n; //Add it to the new sizes array.

            free(sizes); //Free old sizes array.
            sizes = new_sizes; //Change pointers.

            ctr++; //Increment "counter" for how many different blocks we have allocated (just a size of the "sizes" array).
            return (void*)((char*)(main)+last_unavail); //Return the memory pointer.
        }


        return nullptr;
    }

    //print out bytes array
    void dbg_printbytes() {
        printf("\n\n");
        for (int i = 0; i < total_bytes_available; i++) {
            std::cout << positions[i] << " ";
            if (!(i % 20) && i!=0) { printf("\n"); }
        }
    }
};
