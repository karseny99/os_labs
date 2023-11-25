#include <iostream>
#include <string>
#include <vector>
#include "../include/der.hpp"
#include "../include/pi.hpp"

using namespace std;


int main() {
    cout.precision(16);


    int cmd;
    int current_func = 0;
    int k;
    float x, dx;

    while(1) {
        cout << "Choose command: (0 - switch func in lib,\n 1 - func from pi_lib,\n 2 - func from der_lib,\n 3 - exit)" << endl;
        cout << "==================================" << endl;
        if(!(cin >> cmd)) {
            break;
        }
        if(cmd == 3) {
            break;
        } else if(cmd == 0) {
            current_func ^= 1;
        } else if(cmd == 1) {
            cin >> k;
            float res;
            if(current_func == 0) {
                res = Pi1(k);
            } else {
                res = Pi2(k);
            }
            cout << "the value of pi with " << current_func + 1 << "-function, is " << res << endl;
        } else if(cmd == 2) {

            float res;
            cin >> x >> dx;
            if(current_func == 0) {
                res = Der1(x, dx);
            } else {
                res = Der2(x, dx);
            }
            cout << "derivative value of cos(x) at a point x=" << x << " with dx=" << dx  << "\nis " << res << " with usage of " << current_func + 1 << "-function" << endl;
        }
        cout << "==================================" << endl;
    }
}