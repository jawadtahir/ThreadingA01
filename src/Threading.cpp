//============================================================================
// Name        : Threading.cpp
// Author      : Jawad Tahir
// Version     :
// Copyright   : GPLv2
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <ucontext.h>
#include <list>
using namespace std;

list<ucontext_t*> ready_q, thread_created;
ucontext_t main_context;


void mythread_init()
{
	getcontext(&main_context);
	//cout<<"R"<<endl;
	main_context.uc_stack.ss_sp = new char[SIGSTKSZ];
	main_context.uc_stack.ss_size = SIGSTKSZ;
}
int mythread_fork()
{
	int ret_val = 0, a= 0;
	int* resumed = &a;
	*resumed = 0;
	ucontext_t new_context;
	getcontext(&new_context);
	//getcontext(&main_context);
	new_context.uc_link = &main_context;
	new_context.uc_stack.ss_sp = new char[SIGSTKSZ];
	new_context.uc_stack.ss_size = SIGSTKSZ;
	//swapcontext(&new_context, &main_context);
	thread_created.push_back(&new_context);
	ready_q.push_back(&main_context);
	getcontext(&new_context);
	new_context.uc_link = &main_context;
	getcontext(&main_context);
	cout<<"Hello"<<endl;
	if (*resumed == 0)
	{
		*resumed = 1;
		ret_val = 0;
	}
	else
	{
		*resumed = 0;
		ret_val = thread_created.size();
	}


	//getcontext(&main_context);

	return ret_val;

}
void mythread_exit(){}
void mythread_yield()
{
	ucontext_t new_context;
	ucontext_t* temp_context;
	getcontext(&new_context);

	ready_q.push_back(&new_context);
	temp_context = ready_q.front();
	ready_q.pop_front();
	setcontext(temp_context);

	//swapcontext(&new_context, temp_context);
}
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
	setcontext(&main_context);
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
