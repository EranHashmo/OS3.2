
#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "Barrier.h"
#include <map>
#include <iostream>
#include <atomic>
#include <pthread.h>
#include <algorithm>
#include <queue>

//
#include "IncClient.cpp"

void errMsg(const std::string& message)
{
    std::cerr << "system error: " << message << std::endl;
}

typedef void* JobHandle;


struct ThreadContext
{
    int id;
//    void * c;
    std::vector<IntermediatePair> interVec;
    pthread_mutex_t tMutex;
};

struct JobContext
{
//    pthread_t *threads;
//    pthread_t shuffleThread;
    const MapReduceClient &_client;
    const InputVec &inVec;
    IntermediateMap *interMap;
    std::vector<K2*> K2Vec;
    OutputVec &outVec;
    JobState *_jobState;
    Barrier *_barrier;
    int _multiThreadLvl;
//    std::map<pthread_t*, ThreadContext*> tc;
    pthread_mutex_t reduceMutex = PTHREAD_MUTEX_INITIALIZER;
//    pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
//    int count;
    std::vector<ThreadContext*> tcs;
    std::queue<int> finishedThreadsQ;
    std::atomic<int> mapAtomic;
    std::atomic<int> reduceAtomic;
    std::atomic<int> finishedThreadsNum;
    std::atomic<int> runningThreadId;

    JobContext(const MapReduceClient &client, const InputVec &inputVec, OutputVec &outputVec,
            IntermediateMap *intermediateMap, JobState *jobState,
               Barrier *barrier, int multiThreadLevel)
            : _client(client), inVec(inputVec), outVec(outputVec), interMap(intermediateMap), K2Vec(),
              _jobState(jobState),
              _barrier(barrier), _multiThreadLvl(multiThreadLevel), mapAtomic(0), reduceAtomic(0),
              runningThreadId(0), finishedThreadsQ(), finishedThreadsNum(0), tcs(multiThreadLevel)
    {
        pthread_mutex_init(&reduceMutex, NULL);
    }
};

void safeLock(pthread_mutex_t *mutex)
{
    if (pthread_mutex_lock(mutex) != 0){
        errMsg("error on pthread_mutex_lock");
        exit(1);
    }
}

void safeUnlock(pthread_mutex_t *mutex)
{
    if (pthread_mutex_unlock(mutex) != 0){
        errMsg("error on pthread_mutex_unlock");
        exit(1);
    }
}

bool key2Comp(IntermediatePair p1, IntermediatePair p2)
{
    return *p1.first < *p2.first;
}


void emit2 (K2* key, V2* value, void* context)
{
    IntermediatePair pair(key, value);
    auto *tc = (ThreadContext*)context;
    safeLock(&tc->tMutex);
    tc->interVec.push_back(pair);
    safeUnlock(&tc->tMutex);
}

void emit3 (K3* key, V3* value, void* context)
{
    auto job = (JobContext*) context;
    OutputPair pair(key, value);

    safeLock(&job->reduceMutex);
    job->outVec.push_back(pair);
    safeUnlock(&job->reduceMutex);
}

void *startRoutine(void* arg)
{
    auto* job = (JobContext*)arg;
    job->_jobState->stage = MAP_STAGE;

     std::vector<IntermediatePair> interVec;
     std::vector<K2*> K2Vec;
//    auto *threadVector = new std::vector<IntermediatePair>;
    auto *tc = new ThreadContext({job->runningThreadId++, interVec, pthread_mutex_t()});
    pthread_mutex_init(&tc->tMutex, NULL);
    safeLock(&tc->tMutex);
    job->tcs[tc->id] = tc;
    safeUnlock(&tc->tMutex);

    int oldVal = 0;
    int shuffleT = 0;
    if (tc->id == shuffleT)
    {
        int id;
        while(true)  //  Shuffle
                //and job->_jobState->percentage != 100)
        {
            if (job->finishedThreadsQ.empty()  )
            {
                if (job->finishedThreadsNum == job->_multiThreadLvl-1)
                {
                    break; //  shuffle finished
                }
                continue;


            }
            id = job->finishedThreadsQ.front();
            safeLock(&tc->tMutex);
            job->finishedThreadsQ.pop();
            safeUnlock(&tc->tMutex);

            if (job->tcs[id]->interVec.empty())
            {
                continue;
            }
            safeLock(&tc->tMutex);
            for (auto pair : (job->tcs[id])->interVec)
            {
                K2* k = pair.first;
                if (job->interMap->find(pair.first) == job->interMap->end())
                {
                    std::vector <V2 *> vector;
                    (*job->interMap)[pair.first] = vector;

                    (job->K2Vec).push_back(k);
                }
                (*job->interMap)[pair.first].push_back(pair.second);
            }
            safeUnlock(&tc->tMutex);

//            for (auto item: *(job->tcs)[job->runningThreadId]->interVec)
//            {
//                // maybe it would be better in some other function? so this one will be smaller.
//                // todo add something about percentage and maybe atomic variable
//                K2 *key =item.first;
//                auto it = (job ->interMap)->find(key);
//                if (it == job->interMap->end())
//                {
//                    std::vector <V2 *> vector;
//                    vector.push_back(item.second);
//                    std::pair <K2*, std::vector<V2 *>> pair(item.first, vector);
//                    job->interMap->insert(pair);
//                }
//                else
//                {
//                    it->second.push_back(item.second);
//                }
//            }
        }
    }
    else
        {
        while (true)
        {
            oldVal = (job->mapAtomic)++;
            if (oldVal >= (job->inVec).size())
            {
//                job->_jobState->percentage = 100;

                break;
            }
//            job->_client.map(job->inVec[oldVal].first, job->inVec[oldVal].second, job);
            job->_client.map(job->inVec[oldVal].first, job->inVec[oldVal].second, tc);

            safeLock(&tc->tMutex);
            job->_jobState->percentage = (float)(oldVal / job->inVec.size()) * 100;
            safeUnlock(&tc->tMutex);

        }
        std::sort((tc->interVec).begin(),
                (tc->interVec).end(), key2Comp);      //sort
    }

    job->finishedThreadsNum++;
    safeLock(&tc->tMutex);
    job->finishedThreadsQ.push(tc->id);
    safeUnlock(&tc->tMutex);

    job->_barrier->barrier();

    job->_jobState->stage = SHUFFLE_STAGE;
    job->_jobState->percentage = 0.0;


    // Reduce

    if (job->interMap->empty())
    {
        printf("EMPTY\n");
    }
    while (true)
    {
        oldVal = (job->reduceAtomic)++;
        if (oldVal >= job->K2Vec.size())
        {
            break;
        }
        K2* key2 = (job->K2Vec)[oldVal];
        std::vector<V2*> v = (*job->interMap)[key2];
        job->_client.reduce(key2, v, job);
        safeLock(&tc->tMutex);
        job->_jobState->percentage = (float)(oldVal / job->inVec.size()) * 100;
        safeUnlock(&tc->tMutex);
    }

    delete tc;

    job->_barrier->barrier();

    return 0;
}

JobHandle startMapReduceJob(const MapReduceClient& client, const InputVec& inputVec, OutputVec& outputVec, int multiThreadLevel)
{
//    std::vector<pthread_t*> threads(multiThreadLevel);
    auto *threads = new pthread_t[multiThreadLevel];

    ThreadContext tcs[multiThreadLevel];
    auto *jobContext = new JobContext(client, inputVec, outputVec, new IntermediateMap(),
                                      new JobState({UNDEFINED_STAGE, 0}),
                                      new Barrier(multiThreadLevel), multiThreadLevel);

//    jobContext->threads = threads;
//    jobContext->shuffleThread = &threadsArr[0];
    for (int i = 0; i < multiThreadLevel; ++i)
    {
        pthread_create(threads +i, NULL, startRoutine, jobContext);
    }

    for (int i = 0; i < multiThreadLevel; ++i)
    {
        pthread_join(*(threads + i), NULL);
    }

    printf("after join\n");

    if (outputVec.empty())
    {
        printf("EMPTY\n");
    }
    for (auto pair : outputVec)
    {
        ((Intgrk*)pair.first)->printk();
        ((Intgrv*)pair.second)->printv();
    }

//    for (auto pair : *jobContext->interMap)
//    {
//        ((Intgrk*)pair.first)->printk();
//        std::vector< V2*> v = pair.second;
//        for (auto item : v)
//        {
//            ((Intgrv*)item)->printv();
//        }
//    }

    delete jobContext->interMap;
    delete jobContext->_jobState;
    delete jobContext->_barrier;

    return jobContext;
}

void waitForJob(JobHandle job);
void getJobState(JobHandle job, JobState* state);
void closeJobHandle(JobHandle job);
