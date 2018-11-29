// thead_safe_queue

#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <string>

#include "expression.hpp"
#include "semantic_error.hpp"

// So we can differentiate between error output and correct result output
struct output_type {
  bool isError;
  Expression exp_result;
  SemanticError err_result;

  output_type() : output_type(true, Expression(), SemanticError(std::string("Error"))) {};
  output_type(bool isErr, Expression exp, SemanticError error) : isError(isErr), exp_result(exp), err_result(error) {};
};

template<typename T>
class ThreadSafeQueue {
public:

  void push(T const& value);

  bool empty() const;

  bool try_pop(T& popped_value);

  void wait_and_pop(T& popped_value);

private:
  std::queue<T> thread_queue;
  mutable std::mutex the_mutex;
  std::condition_variable the_condition_variable;
};



#endif
