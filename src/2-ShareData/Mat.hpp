#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;
using mugard = lock_guard<mutex>;

template<typename T = float, size_t N = 4>
class Mat {
public:
    using MatTN = Mat<T, N>;
    #define MAT_FOREACH(i, j, size) \
        for (size_t i = 0; i < size; i++) \
            for (size_t j = 0; j < size; j++)

    Mat () {
        MAT_FOREACH(i, j, N)
            data[i][j] = 0;
    }

    Mat (initializer_list<T[N]> m) {
        for (auto it = m.begin(); it != m.end(); ++it) {
            for (size_t j = 0; j < N; j++) {
                size_t i = it - m.begin();
                data[i][j] = (*it)[j];
            }
        }
    }

    MatTN operator * (const MatTN& m) const {
        MatTN res;
        auto& _data = data;
        vector<thread> threads;
        threads.reserve(N * N);

        MAT_FOREACH(i, j, N)
            threads.push_back(
                thread([this, &res, &m, i, j] () {
                    for (size_t k = 0; k < N; k++) {
                        res.data[i][j] += data[i][k] * m.data[k][j];
                    }
                })
            );

        for (auto& thread : threads)
            thread.join();
        return res;
    }

    void show () {
        cout << "Matrix:\n";
        MAT_FOREACH(i, j, N) {
            cout << data[i][j] << ", ";
            if (j == 3) cout << "\n";
        }
    }

    T sumSync () {
        T sum = 0;
        MAT_FOREACH(i, j, N)
            sum += data[i][j];
        return sum;
    }

    T sumLocalThread () {
        vector<thread> threads; threads.reserve(N);
        vector<T> sums; sums.reserve(N);
        for (size_t i = 0; i < N; i++) {
            threads.push_back(
                thread([this, &sums, i] () {
                    T sum = 0;
                    for (size_t j = 0; j < N; j++) {
                        sum += data[i][j];
                    }
                    sums.push_back(sum);
                })
            );
        }
        for (auto& thread : threads)
            thread.join();

        T sum = 0;
        for (auto& s : sums)
            sum += s;
        return sum;
    }

    T sumCommonThread () {
        T sum = 0;
        mutex mu;
        vector<thread> threads; threads.reserve(N);
        vector<T> sums; sums.reserve(N);

        for (size_t i = 0; i < N; i++) {
            threads.push_back(
                thread([&sum, this, &mu, i] () {
                    for (size_t j = 0; j < N; j++) {
                        mugard lock(mu);
                        sum += data[i][j];
                    }
                })
            );
        }
        for (auto& thread : threads)
            thread.join();
        return sum;
    }

private:
    T data[N][N];
};