#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "log.hpp"
using namespace std;
using mugard = lock_guard<mutex>;
#include "Mat.hpp"

const int MAX = 1000;
mutex mu0;
mutex mu1;

void add0 () {
    cout << "start0,";
    for (int i = 0; i <= MAX; i += 2) {
        mu0.lock();
        cout << i << ",";
        mu1.unlock();
        mu0.try_lock();
    }
}

void add1 () {
    cout << "start1,";
    for (int i = 1; i <= MAX; i += 2) {
        mu1.lock();
        cout << i << ",";
        mu0.unlock();
        mu1.try_lock();
    }
}

void playOddEven () {
    mu1.lock();
    cout << "\n\n";
    auto t0 = thread(add0);
    auto t1 = thread(add1);

    t0.join(); t1.join();
}

void playMats () {
    auto i = Mat{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    i.show();

    auto res2 = i * i;
    res2.show();

    auto m3 = Mat{{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}};
    (m3 * m3).show();
    (m3 * i).show();

    LOG("m3.sumSync()", m3.sumSync());
    LOG("m3.sumLocalThread()", m3.sumLocalThread());
    LOG("m3.sumCommonThread()", m3.sumCommonThread());
}

int main() {
    playMats();
    playOddEven();
}
