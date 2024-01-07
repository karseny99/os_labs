#pragma once

#include <json/json.h>
#include <iostream>
#include <queue>
#include <string>

#define all(x) x.begin(), x.end()

using namespace std;
using graph = map<int, vector<int> >;


// enum job_type {
//     LECTURES,
//     TEST,
//     LABS,
//     COURSE_WORK,
//     EXAMS,
//     FINAL_MARK
// };


struct Job {
    string path;
    int name;
};



class JsonParser {

    public:

        map<int, Job> Jobs;
        map<int, vector<int>> dependences;
        map<int, vector<int>> revDependences;
        


        JsonParser(Json::Value jsonData)
        {

            if(!check_graph(jsonData)) { 
                throw std::logic_error("Wrong json data");
            }

            for(auto job : jsonData["jobs"]) {

                Jobs[job["jobs_id"].asInt()].path = job["path"].asString(); // Filling map of names and paths
                Jobs[job["jobs_id"].asInt()].name = job["jobs_id"].asInt();

                if(job["parents"][0].asInt() == -1) {
                    continue;
                }

                Json::Value json_dep = job["parents"]; // getting array of childs for certain job
                dependences[job["jobs_id"].asInt()].reserve(json_dep.size());
                std::transform(all(json_dep), back_inserter(dependences[job["jobs_id"].asInt()]), [](const auto& el) { return el.asInt(); });
            }

            get_rDep(); // getting reverse dependeces

        }

        void print_graph() {
            for(const auto& job_id : revDependences) {
                for(auto element : job_id.second) {
                    cout << job_id.first << ' ' << element << endl;
                }
            }
        }


    private:
        graph mapNode, undirectedGraph;
        vector<Json::Value> start_jobs, end_jobs;

        void get_rDep() {
            for(auto parent : dependences) {
                for(auto child : parent.second) {
                    
                    revDependences[child].push_back(parent.first);

                }
            }
        }




        bool check_graph(Json::Value jsonData) {
            
            for(auto job : jsonData["jobs"]) {
                if(job["child"].asInt() != -1) {
                    mapNode[job["job_id"].asInt()].push_back(job["child"].asInt());

                    undirectedGraph[job["job_id"].asInt()].push_back(job["child"].asInt());
                    undirectedGraph[job["child"].asInt()].push_back(job["job_id"].asInt());

                } else {
                    end_jobs.push_back(job);
                }

                if(job["parents"][0] == -1) {
                    start_jobs.push_back(job);
                }
            }


            if(start_jobs.size() == 0 or end_jobs.size() == 0) {
                cout << "There are no start or end jobs" << endl;
                return false;
            }

            if(has_cycles()) {
                cout << "There is cycle" << endl;
                return false;
            }

            if(many_components()) {
                cout << "There are more than one component of relations" << endl;
                return false;
            }

            return true;
        }


        bool many_components() {
            return comps_num() != 1;
        }

        void bfs(int start , graph& g, map<int, bool>& visited) {
            queue<int> q;
            visited[start] = true;
        
            q.push(start);
        
            while(!q.empty()) {
                int u = q.front();
                q.pop(); 
        
                for(int v : g[u]) {
                    if(visited[v] == 0) {
                        q.push(v);
                        visited[v] = true;
                    }
                }
            }
        
        }
        
        
        int comps_num() {
            int cnt = 0;
            map<int, bool> visited; 
            for(auto node : undirectedGraph) {
                if(!visited[node.first]) {
                    bfs(node.first, undirectedGraph, visited);
                    cnt++;
                }
            }
            return cnt;
        
        }

        bool has_cycles() {
            vector<int> path;
            map<int, int> color;
            vector<int> cycle;
            // cout << start_jobs[0]["job_id"].asInt() << endl;
            bool has_cycle = false;

            for(auto start : start_jobs) {
                has_cycle = has_cycle or dfs(start["job_id"].asInt(), -1, mapNode, color, path, cycle);
            }
            return has_cycle;
        }


        static bool dfs(int u, int prev, graph& g, map<int, int>& color, vector<int>& path, vector<int>& cycle) {
            if(color[u] == 2) {
                return false;
            } 
        
            color[u] = 1;
            path.push_back(u);
        
            
            for(int v : g[u]) {
                if(prev != v) {
                    if(color[v] == 1) {
                        cycle.push_back(v);
                        int ind = path.size() - 1;
                        while(path[ind] != v) {
                            cycle.push_back(path[ind]);
                            --ind;
                        }
                        reverse(cycle.begin(), cycle.end());
                        return true;
                    }
        
                    if(dfs(v, u, g, color, path, cycle)) 
                        return true;
                }
        
            }
        
            color[u] = 2;
            path.pop_back();
            return false;
        }



};