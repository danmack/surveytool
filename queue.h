// queue implementation from strousoup

template<class T>
class QUEUE {
private:
  int foo;
  T* v;
  T* p;
  T* b;
  T  temp;
  int sz;
public:
  QUEUE(int s)
  {
    v = p = new T[sz = s];
  }
  ~QUEUE()
  {
    delete[] v;
  }
  void add(T a)
  {
    *p++ = a;
  }
  T pop()
  {
    if (p > v)
      return *--p;
    else
      return (T) 0;
  }

  T next()
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

  int size() const
  {
    return p-v;
  }
};



