/**
 * Exception class :
 * useage  : thrown when an Iterator exceed the range of underlying 
 * object
*/
#include <iostream>
#include <stdexcept>

class OutOfRangeException : std::out_of_range
{
    OutOfRangeException(std::string msg = "Out of Range Exception") : std::out_of_range(msg)
    {
        std::cout << "Out of Range Exception" << std::endl;
    }
};

template <class T>
/* An iterator-like class for cycling through arrays
 that are pointed to by GCPtrs. Iter pointers
 ** do not ** participate in or affect garbage
 collection. Thus, an Iter pointing to
 some object does not prevent that object
 from being recycled.
*/
class Iterator
{
    // current pointer value
    T *ptr;
    // points to element the end+1
    T *end;
    // points to the begin
    T *begin;
    // sequence length
    unsigned length;

public:
    Iterator()
    {
        ptr = end = begin = NULL;
        length = 0;
    }

    Iterator(T *addr, T *first, T *last)
    {
        ptr = addr;
        end = last;
        begin = first;
        length = last - first; //offset difference distance
    }

    /**
     * Return length of sequence to which this Iterator points
     * */
    unsigned size() { return length; }

    // Return value pointed to by ptr.
    // Do not allow out-of-bounds access
    T &operator*()
    {
        if (ptr >= end || ptr < begin)
        {
            throw OutOfRangeException();
        }
        return *ptr;
    }
    // Return address contained to by ptr.
    // Do not allow out-of-bounds access
    T *operator->()
    {
        if (ptr >= end || ptr < begin)
        {
            throw OutOfRangeException();
        }
        return ptr;
    }

    Iterator &operator++()
    {
        if (ptr >= end || ptr < begin)
        {
            throw OutOfRangeException();
        }
        ptr++;
        return *this;
    }
    //Post-increment operator
    Iterator &operator++(int)
    {
        T *tmp = ptr;
        ptr++;
        return Iterator<T>(tmp, begin, end);
    }

    Iterator &operator--()
    {
        if (ptr >= end || ptr < begin)
        {
            throw OutOfRangeException();
        }
        ptr--;
        return *this;
    }

    //Post-decement operator
    Iterator &operator--(int)
    {
        T *tmp = ptr;
        ptr--;
        return Iterator<T>(tmp, begin, end);
    }

    // Return a reference to the object at the
    // specified index. Do not allow out-of-bounds
    // access.
    T &operator[](int i)
    {
        if ((i < 0) || (i >= (end - begin)))
        {
            throw OutOfRangeException();
        }
    }
    bool operator==(Iterator op2)
    {
        return ptr == op2.ptr;
    }
    bool operator!=(Iterator op2)
    {
        return ptr != op2.ptr;
    }
    bool operator<(Iterator op2)
    {
        return ptr < op2.ptr;
    }
    bool operator<=(Iterator op2)
    {
        return ptr <= op2.ptr;
    }
    bool operator>(Iterator op2)
    {
        return ptr > op2.ptr;
    }
    bool operator>=(Iterator op2)
    {
        return ptr >= op2.ptr;
    }

    // Subtract an integer from an Iter.
    Iterator operator-(int n)
    {
        ptr -= n;
        return *this;
    }
    // Add an integer to an Iter.
    Iterator operator+(int n)
    {
        ptr += n;
        return *this;
    }
    // Return number of elements between two Iters.
    int operator-(Iterator<T> &itr2)
    {
        return ptr - itr2.ptr;
    }
};