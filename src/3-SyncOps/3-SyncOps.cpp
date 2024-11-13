#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
using namespace std;

const int NUM_INGREDIENTS = 10;
const int MEAL_SIZE = 3;

// random gen
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> ingredientDist(0, NUM_INGREDIENTS - 1);
uniform_int_distribution<> timeDist(100, 500);

template <typename T>
class ThreadQueue {
public:
  mutex m;
  condition_variable c;
  queue<T> q;

  T wait() {
    unique_lock<mutex> lock(m);
    c.wait(lock, [this] { return !q.empty(); });
    auto val = q.front();
    q.pop();
    return val;
  }

  void push (T val) {
    unique_lock<mutex> lock(m);

    // doing some work
      this_thread::sleep_for(chrono::milliseconds(timeDist(gen)));

    q.push(val);
    c.notify_all();
  }
};

class ThreadUser {
public:
  thread t;

  ThreadUser() : t(&ThreadUser::threadLoop, this) {}

  void threadLoop () {
    while (true) {
      if (!loop())
        break;
    }
  }

  virtual bool loop() = 0;
};

ThreadQueue<vector<int>> orderQueue; // Customer -> Waiter
ThreadQueue<vector<int>> cookQueue;  // Waiter -> Cooker
ThreadQueue<vector<int>> mixQueue;   // Cooker -> Chief
ThreadQueue<vector<int>> serveQueue; // Chief -> Waiter

class Customer : public ThreadUser {
public:
  virtual bool loop() override {
    vector<int> meal;
    while (meal.size() < MEAL_SIZE) {
      int ingredient = ingredientDist(gen);
      if (find(meal.begin(), meal.end(), ingredient) == meal.end()) {
        meal.push_back(ingredient);
      }
    }

    LOG("Customer ordered meal with ingredients", meal);
    orderQueue.push(meal);

    vector<int> servedMeal = serveQueue.wait();
    LOG("Customer is eating meal with ingredients", servedMeal);

    LOG("Customer finished eating and exits the restaurant.");
    return false;
  }
};

class Waiter : public ThreadUser {
  virtual bool loop() override {
    vector<int> order = orderQueue.wait();
    LOG("Waiter received order for ingredients", order);
    cookQueue.push(order);

    vector<int> meal = mixQueue.wait();
    LOG("Waiter is serving the meal with ingredients", meal);
    serveQueue.push(meal);

    return true;
  }
};

class Cooker : public ThreadUser {
  virtual bool loop() override {
    vector<int> order = cookQueue.wait();
    LOG("Cooker is preparing ingredients", order);
    mixQueue.push(order);

    return true;
  }
};

class Chief : public ThreadUser {
  virtual bool loop() override {
    vector<int> ingredients = mixQueue.wait();
    LOG("Chief is mixing ingredients", ingredients);
    serveQueue.push(ingredients);

    return true;
  }
};

int main() {
  Customer customer;
  Waiter waiter;
  Cooker cooker;
  Chief chief;

  customer.t.join();
  waiter.t.detach();
  cooker.t.detach();
  chief.t.detach();

  return 0;
}