#include "environment.hpp"

#include <cassert>
#include <cmath>
//#include <math.h>

#include "environment.hpp"
#include "semantic_error.hpp"

/***********************************************************************
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> & args, unsigned nargs){
  return args.size() == nargs;
}

/***********************************************************************
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> & args){
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> & args){

  // check all aruments are numbers, while adding
  double result = 0;
  for( auto & a :args){
    if(a.isHeadNumber()){
      result += a.head().asNumber();
    }
    else{
      throw SemanticError("Error in call to add, argument not a number");
    }
  }

  return Expression(result);
};

Expression mul(const std::vector<Expression> & args){

  // check all aruments are numbers, while multiplying
  double result = 1;
  for( auto & a :args){
    if(a.isHeadNumber()){
      result *= a.head().asNumber();
    }
    else{
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }

  return Expression(result);
};

Expression subneg(const std::vector<Expression> & args){

  double result = 0;

  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber()){
      result = -args[0].head().asNumber();
    }
    else{
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  }
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = args[0].head().asNumber() - args[1].head().asNumber();
    }
    else{
      throw SemanticError("Error in call to subtraction: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }

  return Expression(result);
};

Expression div(const std::vector<Expression> & args){

  double result = 0;

  if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){
      result = args[0].head().asNumber() / args[1].head().asNumber();
    }
    else{
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else{
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  return Expression(result);
};

Expression sqrt(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    if(args[0].isHeadNumber()){
      // Get square root of argument given
      if(args[0].head().asNumber() >= 0)
        return std::sqrt(args[0].head().asNumber());
      else {
        // FOr now throw error, in task 4 change to account for imaginary parts
        throw SemanticError("Error in call for square root: negative number.");
      }

    }
    else
      throw SemanticError("Error in call for square root: invalid argument."); // If argument is NaN

  }
  else
    throw SemanticError("Error in call for square root: invalid number of arguments."); // Input needs to be unary

};

Expression pow(const std::vector<Expression>& args) {

  if(nargs_equal(args, 2)) { // Make sure its binary
    if(args[0].isHeadNumber() && args[1].isHeadNumber()) {

      if(args[1].head().asNumber() == 0) // If power is 0, return 1
        return 1.0;
      else if(args[1].head().asNumber() == 1)
        return args[0].head().asNumber();
      else
        return std::pow(args[0].head().asNumber(), args[1].head().asNumber());
    }
    else
      throw SemanticError("Error in call for power function: invalid argument.");

  }
  else
    throw SemanticError("Error in call for power function: invalid number of arguments.");

};

Expression nlog(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    if(args[0].head().asNumber() > 0) {
      return std::log(args[0].head().asNumber());
    }
    else
      throw SemanticError("Error in call to natural log function: invalid argument.");
  }
  else
    throw SemanticError("Error in call to natural log function: invalid number of arguments.");
};

// Returns the calculation of the sin of the input in terms of radians
Expression sin(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    return std::sin(args[0].head().asNumber());
  }
  else
    throw SemanticError("Error in call to sine function: invalid number of arguments.");
};

// Returns the calculation of the cos of the input in terms of radians
Expression cos(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    return std::cos(args[0].head().asNumber());
  }
  else
    throw SemanticError("Error in call to sine function: invalid number of arguments.");
};

// Returns the calculation of the tan of the input in terms of radians
Expression tan(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    return std::tan(args[0].head().asNumber());
  }
  else
    throw SemanticError("Error in call to sine function: invalid number of arguments.");
};

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);

Environment::Environment(){

  reset();
}

bool Environment::is_known(const Atom & sym) const{
  if(!sym.isSymbol()) return false;

  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom & sym) const{
  if(!sym.isSymbol()) return false;

  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom & sym) const{

  Expression exp;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ExpressionType)){
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom & sym, const Expression & exp){

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }

  // error if overwriting symbol map
  if(envmap.find(sym.asSymbol()) != envmap.end()){
    throw SemanticError("Attempt to overwrite symbol in environemnt");
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_proc(const Atom & sym) const{
  if(!sym.isSymbol()) return false;

  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom & sym) const{

  //Procedure proc = default_proc;

  if(sym.isSymbol()){
    auto result = envmap.find(sym.asSymbol());
    if((result != envmap.end()) && (result->second.type == ProcedureType)){
      return result->second.proc;
    }
  }

  return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset(){

  envmap.clear();

  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));

  // Built-In value of e
  envmap.emplace("e", EnvResult(ExpressionType, Expression(EXP)));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add));

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg));

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul));

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div));

  // Procedure: sqrt
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: pow
  envmap.emplace("^", EnvResult(ProcedureType, pow));

  // Procedure: nlog
  envmap.emplace("ln", EnvResult(ProcedureType, nlog));

  //Procedure: sin
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  // Procedure: cos
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  //Procedure: tan
  envmap.emplace("tan", EnvResult(ProcedureType, tan));
}
