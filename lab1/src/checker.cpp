

bool IsPrime(int x) {
    if(x < 0) {
        return true;
    } else if(x == 0 or x == 1) {
        return false;
    } 

    for(int i = 2; i * i <= x; ++i) {
        if(x % i == 0) {
            return false;
        }
    }
    
    return true;
}