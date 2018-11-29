// Implementation of thread safe queue
#include "thread_safe_queue.hpp"

// Check if the queue is empty
template<typename T>
bool ThreadSafeQueue<T>::empty() const {
  std::lock_guard<std::mutex> lock(the_mutex);
  return thread_queue.empty();
}


// Push a message onto the queue
template<typename T>
void ThreadSafeQueue<T>::push(const T& value) {
  std::unique_lock<std::mutex> lock(the_mutex);
  thread_queue.push(value);

  // Manually unlock and notify one thread that queue has new item
  lock.unlock();
  the_condition_variable.notify_one();
}


// Try and pop value off top of queue, return false if queue is empty
template<typename T>
bool ThreadSafeQueue<T>::try_pop(T& popped_value) {
  std::lock_guard<std::mutex> lock(the_mutex);
  if(thread_queue.empty())
    return false;

  popped_value = thread_queue.front();
  thread_queue.pop();
  return true;
}


template<typename T>
void ThreadSafeQueue<T>::wait_and_pop(T& popped_value) {
  std::unique_lock<std::mutex> lock(the_mutex);

  while(thread_queue.empty())
    the_condition_variable.wait(lock);

  popped_value = thread_queue.front();
  thread_queue.pop();
}

template class ThreadSafeQueue<std::string>;
template class ThreadSafeQueue<output_type>;
