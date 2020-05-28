//
// Created by owner on 5/25/2020.
#include <atomic>
#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "IncClient.cpp"
#include "Barrier.h"

#include <iostream>

struct ThreadContext
{
    int id;
    void *c;
    pthread_mutex_t tMut = PTHREAD_MUTEX_INITIALIZER;
};

struct Job
{
    pthread_t* threads;
    int _threadNum;
    std::atomic<int> _threadServed;
    std::atomic<int> shuffleCounter;
    std::atomic<int> counter;
    Barrier b;
    bool done;
    ThreadContext *_tcs;
    pthread_mutex_t *mut;

    std::vector<int>& _inVec;
    std::map<int, void *> interMap;
    std::vector<int> outVec;

    Job(int threadNum, ThreadContext* tcs, std::vector<int> &inVec)
            : threads(new pthread_t[threadNum]), _threadNum(threadNum), _inVec(inVec) ,
              shuffleCounter(0), counter(0), done(false), b(threadNum), _tcs(tcs), _threadServed(0)
    {
//        _inVec = inVec;
        mut = new pthread_mutex_t;
//        pthread_mutex_init(mut, nullptr);
    }
};

void *threadRun(void * arg)
{
    Job *job = (Job*)arg;

    auto *tc = new ThreadContext;
    auto *c = new std::vector<int>;
    tc->c = c;
    tc->id = job->_threadServed++;

    int oldVal = 0;
    while(true)
    {

        oldVal = (job->counter)++;
        if (oldVal >= (job->_inVec).size())
        {
            break;
        }

        ((std::vector<int> *)tc->c)->push_back(job->_inVec.at(oldVal) + 100);        //map
        for (int i = 0; i < 1000000; i++){}

    }
    job->interMap[tc->id] = tc->c;
    job->b.barrier();
//    delete mapi;
    job->done = true;
    delete tc;
    return nullptr;
}

int main(int argc, char *argv[])
{
    int numOfThreads = 5;
    auto *inVec = new std::vector<int>;
    for (int i = 0; i< 20; i++)
    {
        inVec->push_back(i);
    }
    auto *threads = new pthread_t[numOfThreads];
    auto *contexts = new ThreadContext[numOfThreads];

    Job* job = new Job(numOfThreads, contexts, *inVec);
    job->threads = threads;

    for (int i = 0; i < numOfThreads; ++i)
    {
        pthread_create(threads + i, NULL, threadRun, job);
    }

    for (int i = 0; i < numOfThreads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    for (auto item: job->interMap)
    {
        printf("%d\n", item.first);
        for (auto i: *(std::vector<int>*)item.second)
        {
            printf("%d:\t%d,\t", item.first, i);
        }
        printf("\n");
    }

    delete[] threads;
    delete[] contexts;
    delete job;

    return 0;
}
//

