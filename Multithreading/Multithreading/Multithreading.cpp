//============================================================================
// Name        : Multithreading.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Very basic intro to C++ multithreading constructs
// Important   : thread, join(), 
//				 this_thread::get_id(), this_thread::sleep_for(),
//				 future<type>, async(launch::async, AsyncFunction, 200)
//				 future.valid(), future.get()
//				 promise<type>, promise.get_future(), promise.set_value()
//				 lock_guard<type>, unique_guard<type>(can also do lock()/unlock())
//               condition_variable.wait(), condition_variable.notify_all()
//============================================================================

#include <iostream>
#include <initializer_list>
#include <thread>
#include <future>
#include <condition_variable>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Basic thread creation with callback or lambda
//
void ThreadFunc(int &data)
{
    cout << "Inside second thread with id " << this_thread::get_id() << " and with value: " << data << endl;
    data++;
}

void ThreadWithFunctionAndLambda()
{
    cout << "Hello from main thread with id " << this_thread::get_id() << endl;
    int data = 10;
    thread t1{ ThreadFunc, ref(data) };

    thread t2{ [&]() {
        cout << "Inside thread thread with id " << this_thread::get_id() << " and value: " << data << endl;
        data++;
    } };

    t1.join();
    t2.join();

    cout << "Value after completion of other threads: " << data << endl;
}

//
// Template callback function
//
template <typename Type>
void ThreadTFunction()
{
    cout << "\nInside second thread with id " << this_thread::get_id() << " and type:" << typeid(Type).name() << endl;
}

void ThreadsWtihTemplates()
{
    thread t1{ ThreadTFunction<int> };
    this_thread::sleep_for(chrono::milliseconds(1000));
    thread t2{ ThreadTFunction<char> };

    t1.join();
    t2.join();
}

//
// Thread function with STL
//
void ThreadStlFunction(initializer_list<int> il)
{
    cout << "Size of initializer list: " << il.size() << endl;
}

void ThreadsWtihStlElements()
{
    initializer_list<int> il = { 1, 2, 3, 4, 5 };
    thread t1{ ThreadStlFunction , il };

    // Implicit conversion does not happen, so you need to pass object or reference otherwise compilation error.
    //thread t1{ ThreadStlFunction ,{ 1, 2, 3, 4, 5 } };

    t1.join();
}

//
// Async function
//
int AsyncFunction(int value)
{
    cout << "I am inside an async ... with id: " << this_thread::get_id() << endl;
    return 100 + value;
}

void ThreadsAndFuture()
{
    // Future: async programming way of C++11. So you get things executed in a separate thread, without explicity creating a separate thread.
    //          e.g. HTTP response handler.
    // Promise: container of future.

    cout << "Main thread id: " << this_thread::get_id() << endl;
    future<int> fn = async(launch::async, AsyncFunction, 200); // launched in a sepratre thread and excutes independant of the main thread.

                                                               // If async() was called with launch::async    -> get() waits till AsyncFunction has finished and get the returned value.
                                                               // If async() was called with launch::deffered -> get() calls AsyncFunction and gets the returned value.
                                                               // Calling get() more than once on a future, crashes the program. So always check validity of future
    if (fn.valid())
    {
        cout << "Value returned from future: " << fn.get() << endl;
    }

    if (fn.valid())
    {
        cout << "Value returned from future(seond time): " << fn.get() << endl;
    }
    else
    {
        cout << "Future is invalid after get()" << endl;
    }

}

//
// Promise
//
void ThreadPromiseFunc(promise<int> &prm)
{
    this_thread::sleep_for(chrono::seconds(2));
    prm.set_value(200);
    this_thread::sleep_for(chrono::seconds(2));
}

void ThreadsAndPromise()
{
    promise<int> prm;
    future<int> fn = prm.get_future();

    thread t{ ThreadPromiseFunc, ref(prm) };

    if (fn.valid())
    {
        // The moment value is set we are unblocked, no matter the other thread has finished or not.
        cout << "Value in future is: " << fn.get() << endl;
    }

    if (fn.valid())
    {
        cout << "Value in future is(second time): " << fn.get() << endl;
    }
    else
    {
        cout << "Future is invalid after get()" << endl;
    }

    t.join();
}

//
// Mutex
//
void ThreadMutexFunction(mutex &mtx)
{
    lock_guard<mutex> lock(mtx);
    cout << "Thread " << this_thread::get_id() << " locked on mutex" << endl;
    this_thread::sleep_for(chrono::seconds(5));
}

void ThreadsAndMutex()
{
    // Concept: while main thread is sleeping, second thread locks mutex and then main thread is blocked till second thread releases mutex
    mutex mtx;
    thread t{ ThreadMutexFunction , ref(mtx) };
    this_thread::sleep_for(chrono::seconds(1));

    unique_lock<mutex> lock(mtx);
    cout << "Main thread " << this_thread::get_id() << " acquired lock on mutex" << endl;

    t.join();
}

//
//
//
void ThreadConVarFunction(mutex &mtx, condition_variable &cond_var)
{
    unique_lock<mutex> lock(mtx);
    cout << "Inside thread with id " << this_thread::get_id() << endl;
    cond_var.wait(lock); // only works with unique_lock
    cout << "Retruning from  thread with id: " << this_thread::get_id() << endl;
}

void ThreadsAndConditionVariables()
{
    cout << "Inside main thread with id: " << this_thread::get_id() << endl;
    mutex mtx;
    condition_variable cond_var;
    thread t{ ThreadConVarFunction , ref(mtx), ref(cond_var)};

    this_thread::sleep_for(chrono::seconds(10));

    cond_var.notify_all();

    t.join();

    cout << "End of threads and cond variables" << endl;
}

int main()
{
    //Basic
    //ThreadWithFunctionAndLambda();

    // Templated thread function
    //ThreadsWtihTemplates();

    // With stl
    //ThreadsWtihStlElements();	

    // Future
    //ThreadsAndFuture();

    // Promise
    //ThreadsAndPromise();

    // Mutex
    //ThreadsAndMutex();

    // Condition variable
    ThreadsAndConditionVariables();

    return 0;
}

