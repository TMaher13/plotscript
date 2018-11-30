#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "thread_safe_queue.hpp"
#include "interpreter_thread.hpp"
#include "output_thread.hpp"

void prompt(){
  std::cout << "\nplotscript> ";
}

std::string readline(){
  std::string line;
  std::getline(std::cin, line);

  return line;
}

void error(const std::string & err_str){
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string & err_str){
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream & stream, bool isFromFile=false){

  Interpreter interp;

  if(isFromFile) {
    std::ifstream start_stream(STARTUP_FILE);
    //if(!start_stream){
    //  error("Could not open startup file for reading.");
    //  return EXIT_FAILURE;
    //}
    if(interp.parseStream(start_stream))
      Expression startup_eval = interp.evaluate();
    //error("Startup file failed to parse.");
    //else
  }

  if(!interp.parseStream(stream)){
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  }
  else{
    try{
      Expression exp = interp.evaluate();
      std::cout << exp << std::endl;
    }
    catch(const SemanticError & ex){
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int eval_from_file(std::string filename){

  std::ifstream ifs(filename);

  if(!ifs){
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }

  return eval_from_stream(ifs, true);
}

int eval_from_command(std::string argexp){

  std::istringstream expression(argexp);

  return eval_from_stream(expression);
}


// A REPL is a repeated read-eval-print loop
void repl(){
  Interpreter interp;

  // Startup file for points, lines, and text in GUI
  std::ifstream ifs(STARTUP_FILE);


  if(interp.parseStream(ifs))
    Expression startup_exp = interp.evaluate();

  // Make thread queues and interpreter thread
  ThreadSafeQueue<std::string> input_queue;
  ThreadSafeQueue<output_type> output_queue;
  InterpreterThread interpThread(&input_queue, &output_queue, interp);

  bool InterpRunning = true;

  std::thread int_th(interpThread);

  while(!std::cin.eof()){

    prompt();
    std::string line = readline();

    if(line.empty()) continue;

    if(line == "%start") {
      if(!InterpRunning)
        int_th = std::thread(interpThread);
      InterpRunning = true;
    }
    else if(line == "%stop") {
      if(InterpRunning) {
        input_queue.push(line);
        int_th.join();
      }
      InterpRunning = false;
    }
    else if(line == "%reset") {
      if(InterpRunning) {
        input_queue.push(line);
        int_th.join();
      }
      int_th = std::thread(interpThread);
      InterpRunning = true;

    }
    else { // For any normal plotscript command

      if(!InterpRunning)
        error("interpreter kernel not running");
      else {
        input_queue.push(line);

        OutputThread outThread(&output_queue);
        std::thread out_th(outThread);

        out_th.join();
      }
    }

  }

  if(InterpRunning)
    int_th.join();
}

int main(int argc, char *argv[])
{
  if(argc == 2){
    return eval_from_file(argv[1]);
  }
  else if(argc == 3){
    if(std::string(argv[1]) == "-e"){
      return eval_from_command(argv[2]);
    }
    else{
      error("Incorrect number of command line arguments.");
    }
  }
  else{
    repl();
  }

  return EXIT_SUCCESS;
}
