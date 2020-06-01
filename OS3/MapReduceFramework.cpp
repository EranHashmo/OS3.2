
#include "MapReduceFramework.h"
#include "MapReduceClient.h"
#include "Barrier.h"
#include <map>
#include <iostream>
#include <atomic>
#include <pthread.h>
#include <algorithm>
#include <queue>
#include <unistd.h>

//
//#include "IncClient.cpp"

void errMsg(const std::string& message)
{
    std::cerr << "system error: " << message << std::endl;
}

typedef void* JobHandle;


struct ThreadContext
{
    int id;
//    void * c;
    std::vector<IntermediatePair> *interVec;
    pthread_mutex_t *tMutex;
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
    pthread_mutex_t *emit3Mutex;
    pthread_mutex_t *stateMutex;
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
        emit3Mutex = new pthread_mutex_t();       //TODO make deletable
        pthread_mutex_init(emit3Mutex, NULL);
        stateMutex = new pthread_mutex_t();       //TODO make deletable
        pthread_mutex_init(stateMutex, NULL);
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
//    IntermediatePair pair(key, value);
    auto *tc = (ThreadContext*)context;
    safeLock(tc->tMutex);
    tc->interVec->emplace_back(key, value);
    safeUnlock(tc->tMutex);
}

void emit3 (K3* key, V3* value, void* context)
{
    auto job = (JobContext*) context;
    safeLock(job->emit3Mutex);
    job->outVec.emplace_back(key, value);
    safeUnlock(job->emit3Mutex);
}

void *startRoutine(void* arg)
{
    auto* job = (JobContext*)arg;
    job->_jobState->stage = MAP_STAGE;

     auto *interVec = new std::vector<IntermediatePair>();
//     std::vector<K2*> K2Vec;
//    auto *threadVector = new std::vector<IntermediatePair>;
    int newID = job->runningThreadId++;
//    auto *tc = new ThreadContext({newID, interVec, new pthread_mutex_t()});
    auto *tc = new ThreadContext();
    auto *mutex = new pthread_mutex_t();
    pthread_mutex_init(mutex, nullptr);
    tc->interVec = interVec;
    tc->tMutex = mutex;
    tc->id = newID;

    job->tcs[tc->id] = tc;

    int oldVal = 0;
    int shuffleT = 0;

    while (true)
    {
        oldVal = job->mapAtomic++;
        if (oldVal >= (job->inVec).size())
        {
//                job->_jobState->percentage = 100;
            break;
        }
        job->_client.map(job->inVec[oldVal].first, job->inVec[oldVal].second, tc);

            safeLock(job->stateMutex);
            job->_jobState->percentage = (float)(oldVal / job->inVec.size()) * 100;
            safeUnlock(job->stateMutex);

    }
    if (!tc->interVec->empty())
    {
        std::sort((tc->interVec)->begin(),
                  (tc->interVec)->end(), key2Comp);   //sort
    }

    job->finishedThreadsNum++;
    job->finishedThreadsQ.push(tc->id);


    if (tc->id == shuffleT)
    {
        int id;
        while(true)  //  Shuffle
            //and job->_jobState->percentage != 100)
        {
            if (job->finishedThreadsQ.empty()  )
            {
                if (job->finishedThreadsNum >= job->_multiThreadLvl)
                {
                    break; //  shuffle finished
                }
                continue;
            }
            id = job->finishedThreadsQ.front();
            safeLock(job->tcs[id]->tMutex);
            job->finishedThreadsQ.pop();
            std::vector<IntermediatePair> *curInterVec = job->tcs[id]->interVec;

            if (curInterVec->empty())
            {
                continue;
            }
            for (auto pair : *curInterVec)
            {
                if (job->interMap->find(pair.first) == job->interMap->end())
                {
                    std::vector <V2 *> vector;
                    (*job->interMap)[pair.first] = vector;
                    (job->K2Vec).push_back(pair.first);
                }
                (*job->interMap)[pair.first].push_back(pair.second);
            }
            safeUnlock(job->tcs[id]->tMutex);

        }
    }

    if (tc->id == shuffleT)
    {
        job->_jobState->stage = REDUCE_STAGE;
        job->_jobState->percentage = 0.0;
    }
    job->_barrier->barrier();
//    int x = 9;
//    job->_barrier->barrier();
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
        job->_client.reduce(job->K2Vec[oldVal], (*job->interMap)[job->K2Vec[oldVal]], job);
        safeLock(job->stateMutex);
        job->_jobState->percentage = ((float)oldVal / job->K2Vec.size()) * 100;
        safeUnlock(job->stateMutex);
    }

    job->_barrier->barrier();
    if (tc->id == shuffleT)
    {
        job->_jobState->stage = REDUCE_STAGE;
        job->_jobState->percentage = 100.0;
    }
    delete tc->interVec;
    pthread_mutex_destroy(tc->tMutex);
    delete tc->tMutex;
    return nullptr;
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
        pthread_create(threads +i, nullptr, startRoutine, jobContext);
    }

    for (int i = 0; i < multiThreadLevel; ++i)
    {
        pthread_join(*(threads + i), nullptr);
    }

    printf("after join\n");
//
//    if (outputVec.empty())
//    {
//        printf("EMPTY\n");
//    }
//    for (auto pair : outputVec)
//    {
//        ((Intgrk*)pair.first)->printk();
//        ((Intgrv*)pair.second)->printv();
//    }

//    for (auto pair : *jobContext->interMap)
//    {
//        ((Intgrk*)pair.first)->printk();
//        std::vector< V2*> v = pair.second;
//        for (auto item : v)
//        {
//            ((Intgrv*)item)->printv();
//        }
//    }



    return jobContext;
}

void waitForJob(JobHandle job)
{
    auto* state = ((JobContext*)job)->_jobState;
    while(state->stage != REDUCE_STAGE || state->percentage != 100.0)
    {
        usleep(1000);
        // todo check if after waiting state can change!!!
    }
}


void getJobState(JobHandle job, JobState* state)
{
    state->stage = ((JobContext*)job)->_jobState->stage;
    state->percentage = ((JobContext*)job)->_jobState->percentage;
}


void closeJobHandle(JobHandle job)
{
    auto *state = ((JobContext *) job)->_jobState;
    if (state->stage != REDUCE_STAGE && state->percentage != 100.0) {
        //todo not sure what exactly we should free here
        delete ((JobContext *) job)->interMap;
        ((JobContext *) job)->interMap = nullptr;
        delete ((JobContext *) job)->_jobState;
        ((JobContext *) job)->_jobState = nullptr;
        delete ((JobContext *) job)->_barrier;
        ((JobContext *) job)->_barrier = nullptr;
    } else {
        waitForJob(job);
    }
}