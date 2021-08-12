#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"
/*
  Pointer implements a pointer type that uses
  garbage collection to release unused memory.
  A Pointer must only be used to point to memory
  that was dynamically allocated using new.
  When used to refer to an allocated array,
  specify the array size.
*/
template <class T, int size = 0>
class Pointer
{
private:
    // refContainer maintains the garbage collection list
    static std::list<PtrDetails<T>> refContainer;
    // addr points to the memory allocated  to which this   pointer
    // currently points to.
    T *addr;
    /*
        isArray is set to true if  memptr is pointing to 
        an allocated memory address which is the offset position 0
        depending on the data type
        else it is set false
    */
    bool isArray;
    // size of the array
    unsigned arraySize;
    // true when first Pointer is created
    static bool first;
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T>>::iterator findPtrInfo(T *ptr);

public:
    // Define an iterator for the Pointer<T> class
    typedef Iterator<T> GCiterator;
    Pointer()
    {
        Pointer(NULL);
    }
    Pointer(T *);
    // Copy Constructor
    Pointer(const Pointer &);
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    Pointer &operator=(const Pointer &rv);
    T *operator=(T *t);

    // Return a reference to the object pointed
    // to by this Pointer.
    T &
    operator*()
    {
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i)
    {
        return addr[i];
    }
    // Conversion /Type Cast to *Pointer<T>
    operator T *() { return addr; }
    // Conversion /Type Cast to Pointer<T>
    operator T() { return *addr; }
    // Return an Iter to the start of the allocated memory.
    Iterator<T> begin()
    {
        int _size;
        if (isArray)
        {
            _size = arraySize;
        }
        else
        {
            _size = 1;
        }
        return Iterator<T>(addr, addr, addr + _size);
    }

    Iterator<T> end()
    {
        int _size;
        if (isArray)
        {
            _size = arraySize;
        }
        else
        {
            _size = 1;
        }
        return Iterator<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer  for th
    static int refContainerSize()
    {
        return refContainer.size();
    }
    // A utility function that diplays the contents of refContainer.
    static void showlist();
    // Clear refContainer when the program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T>> Pointer<T, size>::refContainer;

template <class T, int size>
bool Pointer<T, size>::first = true;

template <class T, int size>
Pointer<T, size>::Pointer(T *t)
{
    if (first)
    {
        atexit(shutdown);
    }
    first = false;
    typename std::list<PtrDetails<T>>::iterator it;
    it = findPtrInfo(t);
    if (it != refContainer.end())
    {
        it->refCount++;
    }
    else
    {
        PtrDetails<T> obj(t, size);
        refContainer.push_back(obj);
    }

    addr = t;
    arraySize = size;
    if (size > 0)
    {
        isArray = true;
    }
    else
    {
        isArray = false;
    }
}

template <class T, int size>
Pointer<T, size>::Pointer(const Pointer &rv)
{
    typename std::list<PtrDetails<T>>::iterator it;
    it = findPtrInfo(rv.addr);
    addr = rv.addr;
    arraySize = rv.arraySize;
    if (arraySize > 0)
    {
        isArray = true;
    }
    else
    {
        isArray = false;
    }
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer()
{
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);

    if (p->refCount)
    {
        p->refCount--;
    }
    collect();
}

template <class T, int size>
bool Pointer<T, size>::collect()
{
    bool memfreed = false;
    typename std::list<PtrDetails<T>>::iterator it;
    do
    {
        for (it = refContainer.begin(); it != refContainer.end(); it++)
        {
            if (it->refCount > 0)
            {
                continue;
            }
            memfreed = true;
            refContainer.remove(*it);
            if (it->memPtr)
            {
                if (it->isArray)
                {
                    delete[] it->memPtr;
                }
                else
                {
                    delete it->memPtr;
                }
            }
            break;
        }
    } while (it != refContainer.end());
    return memfreed;
}

template <class T, int size>
T *Pointer<T, size>::operator=(T *t)
{
    typename std::list<PtrDetails<T>>::iterator it;
    it = findPtrInfo(addr);
    it->refCount++;
    it = findPtrInfo(t);
    if (it != refContainer.end())
    {
        it->refCount++;
    }
    else
    {
        PtrDetails<T> obj(t, size);
        refContainer.push_back(obj);
    }
    addr = t;
    return t;
}

template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(const Pointer &rv)
{
    typename std::list<PtrDetails<T>>::iterator it;
    it = findPtrInfo(addr);
    it->refCount--;
    it = findPtrInfo(rv.addr);
    it->refCount++;
    addr = rv.addr;
    return rv;
}

template <class T, int size>
void Pointer<T, size>::showlist()
{
    typename std::list<PtrDetails<T>>::iterator it;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";

    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }

    for (it = refContainer.begin(); it != refContainer.end(); it++)
    {
        std::cout << "[" << (void *)it->memPtr << "]"
                  << " " << it->refcount << " ";
        if (it->memPtr)
        {
            std::cout << " " << *it->memPtr;
        }
        else
        {
            std::cout << "---";
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T>>::iterator
Pointer<T, size>::findPtrInfo(T *ptr)
{
    typename std::list<PtrDetails<T>>::iterator it;
    // Find ptr in refContainer.
    for (it = refContainer.begin(); it != refContainer.end(); it++)
    {
        if (it->memPtr == ptr)
        {
            return it;
        }
    }
    return it;
}

// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown()
{
    if (refContainerSize() == 0)
    {
        return; // list is empty
    }
    typename std::list<PtrDetails<T>>::iterator it;
    for (it = refContainer.begin(); it != refContainer.end(); it++)
    {
        // Set all reference counts to zero
        it->refCount = 0;
    }
    collect();
}
