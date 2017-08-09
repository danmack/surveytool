#include "queue.h"

template<class T> QUEUE<T>::QUEUE(int s)
{
  v = p = new T[sz = s];
}

template<class T> QUEUE<T>::~QUEUE()
{
  delete[] v;
}

void QUEUE<T>::add(T a)
{
  *p++ = a;
}

template<class T> T QUEUE<T>::pop()
{
  if (p > v)
    return *--p;
  else
    return (T) 0;
}

template<class T> T QUEUE<T>::next()
{
  if (p > v)
  {
    temp = *v;
    b = v;
    while (b < (p-1))
    {
      *b = *(b+1);
      b++;
    }
    --p;
    return temp;
  }
  return (T) 0;
}

template<class T> int QUEUE<T>::size()
{
  return p-v;
}
