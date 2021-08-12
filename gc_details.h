/**
 * This class represent an element stored in memory but as a 
 * garbage collection information List
 * 
*/

template <class T>
class PtrDetails
{
public:
    // current reference count
    unsigned refCount;
    // pointer to allocated memory
    T *memPtr;
    /*
            isArray is set to true if  memptr is pointing to 
            an allocated memory address which is the offset position 0
            depending on the data type
            else it is set false
        */
    bool isArray = false;
    /*
        if array then the size of the array is keep for deallocation
        purpose later 
        */
    unsigned arraySize; // cannot be negative
    PtrDetails(T *mPtr, unsigned size = 0)
    {
        refCount = 1;
        memPtr = mPtr;
        arraySize = size;
        if (size != 0)
        {
            isArray = true;
        }
    }
};

// Overloading to check if the two <PtrDetails> points to the same to same
// memory address

template <class T>
bool operator==(const PtrDetails<T> &ptr__1, const PtrDetails<T> &ptr__2)
{
    return (ptr__1.memPtr == ptr__2.memPtr);
}