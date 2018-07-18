#include <iostream>
using namespace std;

typedef int index;
template<typename unit>
class maxstack
{
private:
    unit *stackarray;
    int maxlen;
    int stack_size;
    index topdx;
    index base;
public:
    maxstack(int _maxlen):maxlen(_maxlen)
    {
        if(maxlen > 0){
           stackarray = new unit[maxlen];
           base = 0;
           topdx  = 0;
           stack_size = 0;
        }
    }
    void pop();
    void push(unit);
    bool empty();
//    void print()
//    {
//        for(int index = 0; index < maxlen ; index ++){
//            cout<<stackarray[index]<<" ";
//        }
//        cout<<endl;
//    }
    unit & top();
};

template<typename unit>
bool maxstack<unit>::empty()
{
    return stack_size == 0;
}


template<typename unit>
void maxstack<unit>::pop()
{
    if(stack_size > 0){
       topdx = (topdx - 1 + maxlen) % maxlen;
       stack_size -= 1;
    }
    else {
       throw "stack empty";
    }
}

template<typename unit>
void maxstack<unit>::push(unit x)
{
    if(stack_size < maxlen){
       stackarray[topdx] = x;
       stack_size += 1;
       topdx = (topdx + 1) % maxlen;
    }
    else{
       stackarray[topdx] = x;
       base  = (base + 1) % maxlen;
       topdx = (topdx  + 1) % maxlen;
    }
}

template<typename unit>
unit & maxstack<unit>::top()
{
    if(stack_size > 0){
        return stackarray[((topdx  - 1 + maxlen) % maxlen)];
    }
    else{
        throw "stack empty";
    }
}

int main()
{
    maxstack<int> mystack(10);
    for(int index = 0; index < 12; index++){
        mystack.push(index);
       // mystack.print();
    }
    while(!mystack.empty()){
        cout<<mystack.top()<<endl;
        mystack.pop();
       // mystack.print();
    }
    try{
        mystack.pop();
        cout<<"stack is not empty"<<endl;
    }
    catch (const char* e){
        cout<<e<<endl;
    }
    return 0;
}

