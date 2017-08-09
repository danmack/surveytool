// stack implementation from strousoup

template<class T>
class STACK {
  T* v;
  T* p;
  int sz;

public:
  STACK(int s) {
    v = p = new T[sz = s];
  }
  ~STACK() {
    delete[] v;
  }

  void push(T a) {
    *p++ = a;
  }

  T pop() {
    if (p > v)
      return *--p;
    else
      return (T) 0;
  }

  int size() const {
    return p-v;
  }
};
