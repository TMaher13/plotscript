// interpreter_thread.hpp
#ifndef INTERPRETER_THREAD_HPP
#define INTERPRETER_THREAD_HPP

#include <string>
#include <iostream>

#include "interpreter.hpp"
#include "thread_safe_queue.hpp"
#include "semantic_error.hpp"

extern bool interpRestart;

class InterpreterThread {

private:
  Interpreter interp;

  ThreadSafeQueue<std::string>* inputQueuePtr;
  ThreadSafeQueue<output_type>* outputQueuePtr;

public:

  InterpreterThread() {};

  void resetInterp(Interpreter& newInterp) {
    interp = newInterp;
  }

  InterpreterThread(ThreadSafeQueue<std::string>* input_queue_ptr, ThreadSafeQueue<output_type>* output_queue_ptr, Interpreter& interpreter) {
    inputQueuePtr = input_queue_ptr;
    outputQueuePtr = output_queue_ptr;
    interp = interpreter;
    //interpRestart = false;
  };

  // Event loop for interpreter thread
  void operator()() {
    //install_handler();
    while(1) {
      std::string m;

      output_type toSend; // Object to send to output_queue

      if(inputQueuePtr->try_pop(m)) {

        if(m=="%stop" || m=="%reset" || m=="%exit")
          break;

        /*if(m == "%interrupt") {
          interp = Interpreter();
          toSend.isError = true;
          toSend.err_result = SemanticError(std::string("Error: interpreter kernel interrupted"));
          outputQueuePtr->push(toSend);
          continue;
        }*/

        //std::cout << "Flag status: " << interrupt_flag << '\n';

        std::istringstream expression(m);

        if(!interp.parseStream(expression)) {
          toSend.isError = true;
          toSend.err_result = SemanticError(std::string("Error: Invalid Expression. Could not parse."));
          outputQueuePtr->push(toSend);
          // transport in the output queue that you sent an expression or an error
        }
        else{
          try{
            Expression exp = interp.evaluate();
            toSend.isError = false;
            toSend.exp_result = exp;
            outputQueuePtr->push(toSend);

          }
          catch(const SemanticError & ex) {
            toSend.isError = true;
            toSend.err_result = SemanticError(ex.what());
            outputQueuePtr->push(toSend);
          }
        }

      }
    }

  };

};

#endif
