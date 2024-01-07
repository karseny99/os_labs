#include <json/json.h>
#include <iostream>
#include "../include/JsonParser.hpp"
#include "../include/Dag.hpp"

using namespace std;


/*
g++ -std=c++20 src/main.cpp -ljsoncpp -o main && ./main < data.json
*/


int main(int argc, char** argv) {

    cout << "====================" << endl;;
    cout << "Every node after launching goes sleep for it's id_number seconds" << endl;

    Json::Value data;
    cin >> data;
    JsonParser d(data);
    cout << "====================" << endl;;
    cout << "Graph of given json file:" << endl;
    cout << "====================" << endl;;
    d.print_graph();
    cout << "====================" << endl;;

    cout << "Starting execution of DAG" << endl;
    cout << "====================" << endl;;
    int ProcNum = 4;
    if(argc > 2) {
        ProcNum = stoi(argv[2]);
    }

    DagExecutor executor(ProcNum);
    executor.Execute(d);
    cout << "====================" << endl;
    cout << "Dag's execution is done!" << endl;
}