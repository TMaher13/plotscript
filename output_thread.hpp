// output_thread.hpp

#ifndef OUTPUT_THREAD_HPP
#define OUTPPUT_THREAD_HPP

#include "expression.hpp"
#include "semantic_error.hpp"
#include "thread_safe_queue.hpp"

#include <iostream>

class OutputThread {

public:

  OutputThread(ThreadSafeQueue<output_type>* output_queue_ptr) {
    outputQueuePtr = output_queue_ptr;
  };

  // Event loop for output thread
  void operator()() const {
    output_type result;
    while(1) {
      if(outputQueuePtr->try_pop(result)) {
        if(result.isError)
          std::cout << result.err_result.what() << '\n';
        else
          std::cout << result.exp_result << '\n';

        break;
      }
    }
  };


private:

  ThreadSafeQueue<output_type>* outputQueuePtr;

};


#endif
