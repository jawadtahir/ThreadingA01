//============================================================================
// Name        : Threading.cpp
// Author      : Jawad Tahir
// Version     :
// Copyright   : GPLv2
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <ucontext.h>
using namespace std;

void mythread_init()
{
	ucontext_t new_context;
}
int mythread_fork(){}
void mythread_exit(){}
void mythread_yield(){}
int mythread_join(int threadId){}

void MyExampleThread1() {
  cout << "B" << std::endl;
  mythread_yield();
  cout << "E" << std::endl;
  mythread_exit();
}

void MyExampleThread2() {
  cout << "D" << std::endl;
  mythread_yield();
  cout << "H" << std::endl;
  mythread_exit();
}

int main() {
	mythread_init();
	  std::cout << "A" << std::endl;
	  int thread1 = mythread_fork();
	  if (thread1==0)
	    MyExampleThread1();
	  std::cout << "C" << std::endl;
	  int thread2 = mythread_fork();
	  if (thread2==0)
	    MyExampleThread2();
	  std::cout << "F" << std::endl;
	  mythread_join(thread1);
	  std::cout << "G" << std::endl;
	  mythread_join(thread2);
	  std::cout << "I" << std::endl;
	return 0;
}


