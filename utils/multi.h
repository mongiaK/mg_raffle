/*================================================================
*  
*  文件名称：main.cpp
*  创 建 者: mongia
*  创建日期：2021年03月08日
*  
================================================================*/
#pragma once

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h> 
#define gettid() syscall(__NR_gettid) 

#include <map>
#include <iostream>

typedef void (*TaskFunc)();
static int success = 0;
static int failed = 0;
static TaskFunc gfunc = NULL;

void register_func(TaskFunc func)
{
    gfunc = func;
}

static void *thread(void *param)
{
	pthread_detach(pthread_self());

    if(gfunc != NULL)
        (*gfunc)();

	pthread_exit(0);
	return 0;
}

void thread_create(int threadnum)
{
	pthread_t pid[threadnum];
	std::map<pthread_t, bool> pool;
	for (int i = 0; i < threadnum; i++){
		int ret = pthread_create(&pid[i], NULL, thread, NULL);
		if(ret != 0){
#ifdef DEBUG
			std::cout << "create thread failed! errno: " << errno << std::endl;
#endif
			++failed;
			continue;
		}
		pool[pid[i]] = true;
		++success;
	}

	while(pool.size() > 0){
		for(int i = 0; i < threadnum; i++){
			int ret = pthread_kill(pid[i], 0); // 判断线程是否还存在
			if(ret == 0){ // 线程不存在
				if(pool.find(pid[i]) != pool.end()){
					pool.erase(pid[i]);
				}
			}
		}
		sleep(1);
	}
#ifdef DEBUG
	std::cout << "child process [" << getpid() << "] exit, thread run success times: " << success  << " failed times: " << failed << std::endl;
#endif
}

void set_multi_env()
{
	struct rlimit limit;
	
	limit.rlim_cur = limit.rlim_max = 1024;
	if(setrlimit(RLIMIT_STACK, &limit) != 0){
		std::cout << "set system stack size failed." << std::endl;
		exit(0);
	}

	limit.rlim_cur = limit.rlim_max = 100000;
	if(setrlimit(RLIMIT_NOFILE, &limit) != 0){
		std::cout << "set system open file failed." << std::endl;
		exit(0);
	}

	getrlimit(RLIMIT_STACK, &limit);
	if(limit.rlim_cur != 1024){
		std::cout << "set system stack size failed." << std::endl;
		exit(0);
	}

	getrlimit(RLIMIT_NOFILE, &limit);
	if(limit.rlim_cur != 100000){
		std::cout << "set system open file failed." << std::endl;
		exit(0);
	}
}

void watch_process(std::map<int, bool>& pool)
{
    while(pool.size() > 0) {
        for(auto item = pool.begin(); item != pool.end();) {
            int ret = waitpid(item->first, NULL, 0); // 等待子进程执行完
            if (ret == item->first) {
                pool.erase(item++);
            } else {
                item++;
            }
        }

        std::cout << "main  process [" << getpid() << "] loop, wait child exit, alive: " << pool.size() << std::endl;
        sleep(1);
    }
}

void multi_create(int procnum, int threadnum)
{
	set_multi_env();

	int pid = -1;
	std::map<int, bool> pool;
	for(int i = 0; i < procnum; i++){
		pid = fork();
		if(pid <= 0){
			break;
		}
		pool[pid] = true;
	}

	if(pid == 0){
		thread_create(threadnum);
    }else{
        watch_process(pool);
    }
}
