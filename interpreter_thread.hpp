// interpreter_thread.hpp

#ifndef INTERPRETER_THREAD_HPP
#define INTERPRETER_THREAD_HPP

#include <string>
#include "interpreter.hpp"
#include "thread_safe_queue.hpp"
#include "semantic_error.hpp"

class InterpreterThread {

private:
  Interpreter interp;

  ThreadSafeQueue<std::string>* inputQueuePtr;
  ThreadSafeQueue<output_type>* outputQueuePtr;

public:

  InterpreterThread(ThreadSafeQueue<std::string>* input_queue_ptr, ThreadSafeQueue<output_type>* output_queue_ptr, Interpreter& interpreter) {
    inputQueuePtr = input_queue_ptr;
    outputQueuePtr = output_queue_ptr;
    interp = interpreter;
    std::cout << "Interpreter thread created.\n";
  };

  // Event loop for interpreter thread
  void operator()() {
    //int i = 0;
    //std::cout << "In Interpreter thread.\n";
    while(1) {
      std::string m;
      //std::cout << i << '\n';
      if(inputQueuePtr->try_pop(m)) {
        std::istringstream expression(m);
        output_type toSend;

        if(!interp.parseStream(expression)) {
          //std::cout << i << '\n';
          toSend.isError = true;
          toSend.err_result = SemanticError(std::string("Error: Invalid Expression. Could not parse."));
          outputQueuePtr->push(toSend);
          //std::cout << i << '\n';
          // transport in the output queue that you sent an expression or an error
        }
        else{
          try{
            //std::cout << i << i << '\n';
            Expression exp = interp.evaluate();
            //std::cout << exp << std::endl;
            toSend.isError = false;
            toSend.exp_result = exp;
            outputQueuePtr->push(toSend);
            //std::cout << i << i << '\n';
          }
          catch(const SemanticError & ex) {
            toSend.isError = true;
            toSend.err_result = SemanticError(ex.what());
            outputQueuePtr->push(toSend);
            //std::cerr << ex.what() << std::endl;
          }
        }

      }
    }

  };

};

#endif
