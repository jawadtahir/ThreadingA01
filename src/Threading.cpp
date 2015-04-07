//============================================================================
// Name        : Threading.cpp
// Author      : Jawad Tahir
// Version     :
// Copyright   : GPLv2
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <ucontext.h>
#include <signal.h>
#include <list>
#include <map>
using namespace std;

struct tcb_t {
	int thread_id;
	ucontext_t context;
	int status;
	stack_t stack;
};

map<int, tcb_t*> threads;
list<tcb_t*> ready_q;
tcb_t main_context_tcb;
int counter = 0, use_timer = 0;
int* running_thread_id = &counter;

void sig_timer(int sig_no) {
	ucontext_t* temp_context;
	tcb_t* new_tcb_t = threads[*running_thread_id];
	tcb_t* temp_context_tcb;
	//ucontext_t* new_context = new_tcb_t.context;
	if (new_tcb_t) {
		new_tcb_t->status = 0;
		getcontext(&new_tcb_t->context);

		ready_q.push_back(new_tcb_t);
		temp_context_tcb = ready_q.front();
		ready_q.pop_front();
		temp_context_tcb->status = 1;
		*running_thread_id = temp_context_tcb->thread_id;
		//sigaltstack(&temp_context->uc_stack, &new_context.uc_stack);
		//setcontext(temp_context);

		swapcontext(&new_tcb_t->context, &temp_context_tcb->context);
		//memcpy(&new_context.uc_stack, &new_context.uc_stack, sizeof(new_context.uc_stack));
	}

}

void mythread_init(int argc) {
	if (argc == 2) {
		use_timer = 1;
		signal(SIGALRM, sig_timer);
		ualarm(100, 50);
	}
	ucontext_t temp_context;
	getcontext(&temp_context);
	//cout<<"R"<<endl;
//	new_context.uc_stack.ss_sp = new char[SIGSTKSZ];
//	new_context.uc_stack.ss_size = SIGSTKSZ;
	main_context_tcb.context = temp_context;
	main_context_tcb.stack = temp_context.uc_stack;
	main_context_tcb.status = 1;
	main_context_tcb.thread_id = 0;
	threads[0] = &main_context_tcb;
	*running_thread_id = 0;
}

int mythread_fork() {
	int ret_val = 0, a = 0;
	int* resumed = &a;
	*resumed = 0;
	ucontext_t new_context;
	tcb_t* new_context_tcb = (tcb_t*) malloc(sizeof(tcb_t));
	tcb_t* curr_context_tcb;
	curr_context_tcb = threads[*running_thread_id];
	curr_context_tcb->status = 0;
	getcontext(&new_context);
	getcontext(&curr_context_tcb->context);
//	new_context.uc_link = main_context_tcb.context;
//	new_context.uc_stack.ss_sp = new char[SIGSTKSZ];
//	new_context.uc_stack.ss_size = SIGSTKSZ;
	(*new_context_tcb).context = new_context;
	(*new_context_tcb).stack = new_context.uc_stack;
	(*new_context_tcb).status = 1;
	(*new_context_tcb).thread_id = threads.size();
	//swapcontext(&new_context, &main_context);
	threads[threads.size()] = new_context_tcb;
	ready_q.push_back(curr_context_tcb);
	getcontext(&new_context);
	(*new_context_tcb).context.uc_link = &curr_context_tcb->context;
	getcontext(&curr_context_tcb->context);
	if (*resumed == 0) {
		*resumed = 1;
		ret_val = 0;
		*running_thread_id = new_context_tcb->thread_id;
	} else {
		*resumed = 0;
		ret_val = new_context_tcb->thread_id;
		*running_thread_id = 0;
	}

	//getcontext(&main_context);

	return ret_val;

}
void mythread_exit() {
	tcb_t* new_context_tcb = threads[*running_thread_id];
	new_context_tcb->status = 2;
	if (ready_q.size() > 0) {
		tcb_t* temp_context = ready_q.front();
		ready_q.pop_front();
		*running_thread_id = temp_context->thread_id;
		temp_context->status = 1;
		setcontext(&temp_context->context);
	} else {
		exit(EXIT_SUCCESS);
	}
}
void mythread_yield() {
	if (!use_timer) {
		ucontext_t* temp_context;
		tcb_t* new_tcb_t = threads[*running_thread_id];
		tcb_t* temp_context_tcb;
		//ucontext_t* new_context = new_tcb_t.context;
		new_tcb_t->status = 0;
		getcontext(&new_tcb_t->context);

		ready_q.push_back(new_tcb_t);
		temp_context_tcb = ready_q.front();
		ready_q.pop_front();
		temp_context_tcb->status = 1;
		*running_thread_id = temp_context_tcb->thread_id;
		//sigaltstack(&temp_context->uc_stack, &new_context.uc_stack);
		//setcontext(temp_context);

		swapcontext(&new_tcb_t->context, &temp_context_tcb->context);
		//memcpy(&new_context.uc_stack, &new_context.uc_stack, sizeof(new_context.uc_stack));
	}
}

int mythread_join(int threadId) {
	int ret_val = 0;
	int i = 0;
	tcb_t* curr_context_tcb = threads[*running_thread_id];
	tcb_t* temp_context_tcb;
	for (list<tcb_t*>::iterator it = ready_q.begin(); it != ready_q.end();
			it++) {

		if ((*it)->thread_id == threadId) {
			i = 1;
			temp_context_tcb = (*it);
			ready_q.insert(it++, curr_context_tcb);
			break;
		}

	}
	if (i) {
		temp_context_tcb = ready_q.front();
		ready_q.pop_front();
		curr_context_tcb->status = 0;
		*running_thread_id = temp_context_tcb->thread_id;
		temp_context_tcb->status = 1;
		swapcontext(&curr_context_tcb->context, &temp_context_tcb->context);
	}

	return ret_val;
}

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

int main(int argc, char* argv[]) {
	mythread_init(argc);
	std::cout << "A" << std::endl;
	int thread1 = mythread_fork();
	//setcontext(&main_context);
	if (thread1 == 0)
		MyExampleThread1();
	std::cout << "C" << std::endl;
	int thread2 = mythread_fork();
	if (thread2 == 0)
		MyExampleThread2();
	std::cout << "F" << std::endl;
	mythread_join(thread1);
	std::cout << "G" << std::endl;
	mythread_join(thread2);
	std::cout << "I" << std::endl;
	return 0;
}
