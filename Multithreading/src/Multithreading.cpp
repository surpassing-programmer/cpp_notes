//============================================================================
// Name        : Multithreading.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
/*
 * NOTE: With mingw, copy all the migw.* files from https://github.com/meganz/mingw-std-threads.git
 *       to the location where you have thread.h, e.g.
 *       git clone https://github.com/meganz/mingw-std-threads.git
 *       cd mingw-std-threads
 *       cp mingw.* ~/gcc/include/c++/7.3.0/
 * */
//#include <thread>
#include <mingw.thread.h>

using namespace std;

void ThreadFunction(int value)
{
	cout << "Hello from thread with value " << value << endl;
}

int main()
{
	thread t1 {ThreadFunction, 100};
	t1.join();
	return 0;
}
