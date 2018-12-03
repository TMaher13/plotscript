// output_thread.hpp

#ifndef OUTPUT_THREAD_HPP
#define OUTPPUT_THREAD_HPP

#include "expression.hpp"
#include "semantic_error.hpp"
#include "thread_safe_queue.hpp"

#include <iostream>


class OutputThread {

public:

  void endEventLoop() {
    endLoop = true;
  }

  OutputThread(ThreadSafeQueue<output_type>* output_queue_ptr) {
    outputQueuePtr = output_queue_ptr;
    endLoop = false;
  };

  // Event loop for output thread
  void operator()() const {
    output_type result;
    while(1) {

      if(endLoop) {
        std::cout << "Ending output thread.\n";
        break;
      }

      if(outputQueuePtr->try_pop(result)) {
        //std::cout << "Message received\n";
        if(result.isError) {
          std::cout << result.err_result.what() << '\n';
          std::cout << "\nplotscript> ";
        }
        else {
          std::cout << result.exp_result << '\n';
          std::cout << "\nplotscript> ";
        }


        //break;
      }
    }
  };


private:

  ThreadSafeQueue<output_type>* outputQueuePtr;

  bool endLoop;

};


#endif
