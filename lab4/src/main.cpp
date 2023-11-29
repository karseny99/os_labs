#include <iostream>
#include <string>
#include <dlfcn.h>
#include <vector>

using namespace std;

const char path1[] = "/mnt/c/Users/Arsen/OneDrive/Документы/study/os_labs/lab4/libpi.so";
const char path2[] = "/mnt/c/Users/Arsen/OneDrive/Документы/study/os_labs/lab4/libder.so";

vector<string> pi_func = {"Pi1", "Pi2"};
vector<string> der_func = {"Der1", "Der2"};

int main() {
    cout.precision(16);
    void* pi_lib = dlopen(path1, RTLD_NOW);
    void* der_lib = dlopen(path2, RTLD_NOW);

    if (!pi_lib) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }
    if (!der_lib) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }


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
            typedef float (*sym_t)(int);
            sym_t lib_func;
            lib_func = (sym_t) dlsym(pi_lib, pi_func[current_func].c_str());
            cin >> k;
            cout << "the value of pi with " << current_func + 1 << "-function, is " << (*lib_func)(k) << endl;
        } else if(cmd == 2) {
            typedef float (*sym_t)(float, float);
            sym_t lib_func;
            lib_func = (sym_t) dlsym(der_lib, der_func[current_func].c_str());
            cin >> x >> dx;
            cout << "derivative value of cos(x) at a point x=" << x << " with dx=" << dx  << "\nis " << (*lib_func)(x, dx) << " with usage of " << current_func + 1 << "-function" << endl;
        }
        cout << "==================================" << endl;
    }
    dlclose(pi_lib);
    dlclose(der_lib);

}