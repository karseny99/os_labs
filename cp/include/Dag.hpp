#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <set>
#include <sys/wait.h>
#include <future>
#include "JsonParser.hpp"

#define EXEC_FAILURE -1

using namespace std;
const static int DEFAULT_PROC_NUM = 4;

map<int, thread> threadMap;

struct LogStack {
    std::vector<int> completed;
    size_t wasRead = 0;

    std::mutex mut;
    std::condition_variable cv;

    void Push(const int& id) {
        {
            unique_lock l(mut);
            completed.push_back(id);
        }
        cv.notify_one();
    }
};


class JobExecutor {

    public:

        JobExecutor(int _maxProc, LogStack* _log) : maxProc(_maxProc), log(_log) {}

        void Execute(const string& path, const int& name) {
            // thread t(&JobExecutor::jexec, this, name, path, log, executed_threads);
            // t.detach()

            threadMap[name] = thread(&JobExecutor::jexec, this, name, path, log);
            threadMap[name].detach();
        }

    private:
        // const static int DEFAULT_PROC_NUM = 4;

        int maxProc = DEFAULT_PROC_NUM;
        LogStack* log;


        int procNum = 0;
        mutex mpcountMutex;
        condition_variable mpcountcv;

        void jexec(const int& name, const string& path, LogStack* log) {
            {
                unique_lock<mutex> lk(mpcountMutex);

                if(maxProc == procNum) {
                    mpcountcv.wait(lk);
                }

                ++procNum;
            }

            try {
                if(exec(path, name) != 0) {
                    close_all_threads();
                    exit(EXEC_FAILURE);
                } 
            } catch (...) {
                close_all_threads();
                exit(EXEC_FAILURE);
            }

            {
                unique_lock<mutex> lk(mpcountMutex);
                --procNum;
            }

            mpcountcv.notify_one();

            log->Push(name);
        }

        static void close_all_threads() {
            
            cout << "All threads terminated" << endl;
            for(auto& el : threadMap) {
                el.second.~thread();
            }
        }

        static int exec(const string& path, const int name) {
            int pid = fork();
            if(pid == 0) {
                    // cout << path << ' ' << name << endl;
                if(execl(path.c_str(), path.c_str(), to_string(name).c_str(), (char*)0) == -1) {
                    cout << "Execution failed" << endl;
                }
            } else if(pid == -1) {
                throw logic_error("Fork failed");
            } else {
                int state;
                waitpid(pid, &state, 0);
                return state;
            }

            return 0;
        }
};


class DagExecutor {

    private:

        
        int maxProc;

        int target, current;

        int procNum;
        set<int> rdyToExecution;

        mutex mut;
        condition_variable cv;

        LogStack log;
        JobExecutor execute;

    public: 

        DagExecutor() : maxProc(DEFAULT_PROC_NUM), execute(maxProc, &log) {}
        DagExecutor(int _maxProc) : maxProc(_maxProc), execute(maxProc, &log) {}

        bool Execute(JsonParser& dag) {

            target = dag.Jobs.size();
            current = 0;
            
            // Nothing to execute
            if(target == 0) { 
                return true;
            }

            // Queue of all proccess
            set<int> waitQueue;
            for(const auto& p : dag.Jobs) { 
                waitQueue.insert(p.first);
            }

            std::map<int, int> countOfDeps;
            for (const auto& p : dag.dependences) {
                countOfDeps[p.first] = p.second.size();
            }

            // Independent jobs
            for(const auto& job : dag.Jobs) {
                if(dag.dependences[job.first].size() == 0) {
                    rdyToExecution.insert(job.first);
                    waitQueue.erase(job.first); // remove from wait queue;
                }
            }

            // Starting dag's execution
            procNum = 0;
            while(true) {
                
                // Execution completed
                if(current == target) {
                    return true;
                } else {

                    {
                        unique_lock lck(mut);
                        vector<int> executedJobs;

                        for(const int& job_id : rdyToExecution) {

                            executedJobs.push_back(job_id);
                            execute.Execute(dag.Jobs[job_id].path, job_id);

                        }

                        for(const int& job_id : executedJobs) 
                            rdyToExecution.erase(job_id);
                        
                    }

                    vector<int> completedJobs;
                    {
                        unique_lock l(log.mut);
                        
                        if(log.wasRead == log.completed.size()) {
                            log.cv.wait(l);
                        }

                        for(int i = log.wasRead; i < log.completed.size(); ++i) {
                            completedJobs.push_back(log.completed[i]);
                            current++;
                        }
                        log.wasRead = log.completed.size();
                    }

                    for(const int& job_id : completedJobs) {
                        for(const auto& depend : dag.revDependences[job_id]) {
                            --countOfDeps[depend];

                            if(countOfDeps[depend] == 0) {
                                rdyToExecution.insert(depend);
                                waitQueue.erase(depend);
                            }
                        }
                    }
                }
            }

        }

};