#include <iostream>
#include <string>
#include <dlfcn.h>


using namespace std;

const char path1[] = "/mnt/c/Users/Arsen/OneDrive/Документы/study/os_labs/lab4/libpi.so";
const char path2[] = "/mnt/c/Users/Arsen/OneDrive/Документы/study/os_labs/lab4/libder.so";

const char pi_func1[] = "Pi1";

int main() {
    
    void* pi_lib = dlopen(path1, RTLD_LAZY);
    void* der_lib = dlopen(path2, RTLD_LAZY);
    int k = 100;

    if (!pi_lib) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }
    if (!der_lib) {
        cerr << "Cannot open library: " << dlerror() << '\n';
        return 1;
    }


    typedef float (*sym_t)(int);
    while(1) {
        cout << "Choose func: " << endl;
        sym_t lib_func;
        lib_func = (sym_t) dlsym(pi_lib, pi_func1);
        if(lib_func == nullptr) {
            return -1;
        }
        cout << "Input number of steps: " << endl;
        cin >> k;
        float res = (*lib_func)(k);
        cout << "ans is " << res << endl;
    }

    dlclose(pi_lib);
    dlclose(der_lib);
}