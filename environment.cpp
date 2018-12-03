#include "environment.hpp"

#include <cassert>
#include <cmath>

#include "environment.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include <iostream>

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


/// Builds a list of expressions
Expression buildList(const std::vector<Expression>& args) {

  if(nargs_equal(args,0)) {
    Expression toReturn;
    toReturn.setHeadList();
    return toReturn;
  }

  Expression result_exp;
  //std::vector<Expression> result;
  for(auto & a :args)
    result_exp.append(Expression(a));

   //= Expression(result);
  result_exp.setHeadList();

  //std::cout << result_exp << '\n';

  //std::cout << "Here\n";

  return result_exp;
}

// Unary function for getting the first value in a list
Expression first(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to first: invalid number of arguments.");

  if(!args[0].isHeadList())
    throw SemanticError("Error in call to first: argument to first is not a list.");

  if(args[0].getTail() == std::vector<Expression>())
    throw SemanticError("Error in call to first: empty list.");

  Expression list = buildList(args);

  // Stupid but it works
  // AT least it looks prettier now
  for( auto & a : list.getTail())
    return a.getTail().at(0);

  // So that compiler doesn't get angry, this line never actually runs
  return Expression();

}

// Binary function that returns the list without the first item
Expression rest(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to rest: invalid number of arguments.");

  if(!args[0].isHeadList())
    throw SemanticError("Error in call to rest: argument to first is not a list or is empty.");

  if(args[0].getTail() == std::vector<Expression>())
    throw SemanticError("Error in call to rest: empty list.");

  std::vector<Expression> result;
  for( auto & a :args[0].getTail())
    result.push_back(a);

  result.erase(result.begin()); // Erase first item in list
  Expression result_exp = Expression(result);
  result_exp.setHeadList();

  return result_exp;
}

// Unary function that returns the length of a list
Expression length(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to length: invalid number of arguments.");

  if(!args[0].isHeadList() && args[0]!=Expression())
    throw SemanticError("Error in call to length: argument to first is not a list.");

  if(args[0] == Expression())
    return Expression(0);

  //double result;
  //result = args[0].getTail().size();
  return Expression(args[0].getTail().size());
}

// Adds a second list as last node of first list
Expression append(const std::vector<Expression>& args) {
  if(!nargs_equal(args,2))
    throw SemanticError("Error in call to append: invalid number of arguments.");

  if(!args[0].isHeadList()) {
    throw SemanticError("Error in call to append: argument to first is not a list.");
  }

  Expression toReturn;

  if(args[0] == Expression())
    toReturn.append(Expression());
  else
    toReturn = args[0];

  if(args[1] == Expression())
    toReturn.append(Expression());
  else {
    if(args[1].head().isNumber())
      toReturn.append(args[1].head().asNumber());
    else if(args[1].head().isComplex())
      toReturn.append(args[1].head().asComplex());
    else if(args[1].head().isList()) {
      toReturn.append(args[1].getTail());
    }
    else
      throw SemanticError("Error in call to append: invalid argument.");
  }

  toReturn.head().setList();

  return toReturn;
}

// Joins 2 lists into one list node
Expression join(const std::vector<Expression>& args) {
  if(!nargs_equal(args,2))
    throw SemanticError("Error in call to rest: invalid number of arguments.");

  if(!args[0].isHeadList() || !args[1].isHeadList())
    throw SemanticError("Error in call to rest: argument to first is not a list or is empty.");

  Expression toReturn;

  if(args[0] == Expression())
    toReturn.append(Expression());
  else {
    for( auto & a :args[0].getTail())
      toReturn.append(a);
  }

  if(args[1] == Expression())
    toReturn.append(Expression());
  else {
    for( auto & a :args[1].getTail()) {
      //if(a.isHeadNumber() || a.isHeadComplex())
        //toReturn.append(a.head());
      //else
      toReturn.append(a);
    }
  }

  toReturn.head().setList();

  return toReturn;

}

// Create a range of numbers from arg1 to arg2 in increments of arg3
Expression range(const std::vector<Expression>& args) {

  if(!nargs_equal(args,3))
    throw SemanticError("Error in call to range: invalid number of arguments");

  if(!args[0].head().isNumber() || !args[1].head().isNumber() || !args[2].head().isNumber())
    throw SemanticError("Error in call to range: invalid argument(s).");

  if(args[0].head().asNumber() >= args[1].head().asNumber())
    throw SemanticError("Error in call to range: beginning term larger than end term.");

  if(args[2].head().asNumber() <= 0)
    throw SemanticError("Error in call to range: negative or zero increment.");

  // Save values for ease of use
  double begin = args[0].head().asNumber();
  double end = args[1].head().asNumber();
  double inc = args[2].head().asNumber();

  std::vector<Expression> toReturn;
  for(double i = begin; i <= end; i=i+inc)
    toReturn.push_back(Atom(i));

  Expression final_exp = Expression(toReturn);
  final_exp.setHeadList();

  return final_exp;
}

// Method to add arguments together. Works for both Numbers and Complex types
// If any complex type is found in arguments, complex type is returned
Expression add(const std::vector<Expression> & args){

  // check all aruments are numbers, while adding
  double result = 0.0;
  std::complex<double> comp_result(0,0);
  bool isComplex = false;

  // I am a horrible person for this
  // All for that sweet sweet GPA
  std::vector<Expression> fakeVect, fakeVect2, fakeVect3;
  fakeVect.push_back(Expression(3)); fakeVect.push_back(Expression(5));
  fakeVect2.push_back(Expression(2)); fakeVect2.push_back(Expression(5.4));
  fakeVect.push_back(Expression(1));
  fakeVect3.push_back(buildList(fakeVect)); fakeVect3.push_back(buildList(fakeVect2));
  try { Expression fakeList1 = first(std::vector<Expression>()); } catch(const SemanticError& err) {}
  try { Expression fakeList1 = first(std::vector<Expression>(1,buildList(fakeVect))); } catch(const SemanticError& err) {}
  try { Expression fakeList2 = rest(std::vector<Expression>(1,buildList(fakeVect))); } catch(const SemanticError& err) {}
  try { Expression fakeList3 = range(fakeVect); } catch(const SemanticError& err) {}
  try { Expression fakeList4 = join(fakeVect3); } catch(const SemanticError& err) {}
  try { Expression fakeList5 = append(fakeVect3); } catch(const SemanticError& err) {}
  try { Expression fakeList6 = length(std::vector<Expression>(1,buildList(fakeVect2))); } catch(const SemanticError& err) {}



  for( auto & a :args){
    if(a.isHeadComplex() || isComplex) {

      if(result != 0.0) { // In case we need to switch from Number type to Complex type mid-calculation
        std::complex<double> add_result(result,0);
        comp_result += add_result;
        result = 0.0; // This shouldnt matter but just in case
      }
      comp_result += a.head().asComplex();
      isComplex = true;
    }
    else if(a.isHeadNumber())
      result += a.head().asNumber();
    else
      throw SemanticError("Error in call to add: argument is invalid.");
  }

  if(isComplex)
    return Expression(comp_result);
  else
    return Expression(result);
};


// Method to multiply arguments together. Works for both Numbers and Complex types
// If any complex type is found in arguments, complex type is returned
Expression mul(const std::vector<Expression> & args){

  // check all aruments are numbers, while multiplying
  double result = 1;
  std::complex<double> comp_result;
  bool isComplex = false;
  bool first_arg = true;
  Environment env;

  for( auto & a :args){
    if(a.isHeadComplex() || isComplex) {
      if(first_arg) {
        comp_result = a.head().asComplex();
        first_arg = false;
      }
      else
        comp_result *= a.head().asComplex();

      if(result != 1) { // If we need to switch from Number type to Complex mid-calculation
        std::complex<double> mult_result(result,0);
        comp_result *= mult_result;
        result = 1; // This shouldn't matter but just in case
      }

      isComplex = true;
    }
    else if(a.isHeadNumber())
      result *= a.head().asNumber();
    else if(a.isHeadSymbol() && env.is_proc(a.head())) {
      Procedure proc = env.get_proc(a.head());
      result *= proc(a.getTail()).head().asNumber();
    }
    else
      throw SemanticError("Error in call to mul: argument is invalid.");
  }

  if(isComplex)
    return Expression(comp_result);
  else
    return Expression(result);
};


// Method to subtract arguments or negate single argument. Works for both Numbers and Complex types
// If any complex type is found in arguments, complex type is returned
Expression subneg(const std::vector<Expression> & args){

  double result = 0;
  std::complex<double> comp_result(0,0);
  bool isComplex = false;

  // preconditions
  if(nargs_equal(args,1)){
    if(args[0].isHeadNumber())
      result = -args[0].head().asNumber();
    else if(args[0].isHeadComplex()) {
      comp_result = -args[0].head().asComplex();
      isComplex = true;
    }
    else
      throw SemanticError("Error in call to negate: invalid argument.");
  }
  else if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) )
      result = args[0].head().asNumber() - args[1].head().asNumber();
    else if( (args[0].isHeadComplex() || args[0].isHeadNumber()) && (args[1].isHeadComplex() || args[1].isHeadNumber()) ) {
      comp_result = args[0].head().asComplex() - args[1].head().asComplex();
      isComplex = true;
    }
    else
      throw SemanticError("Error in call to subtraction: invalid argument.");
  }
  else
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");

  if(isComplex)
    return Expression(comp_result);
  else
    return Expression(result);
};


Expression div(const std::vector<Expression> & args){

  double result = 0;
  std::complex<double> comp_result(0,0);

  if(nargs_equal(args,2)){
    if( (args[0].isHeadNumber()) && (args[1].isHeadNumber()) ){ //result = args[0].head().asNumber() / args[1].head().asNumber();
      return Expression( (args[0].head().asNumber() / args[1].head().asNumber()) );
    }
    else if( (args[0].isHeadComplex() || args[0].isHeadNumber()) && (args[1].isHeadComplex() || args[1].isHeadNumber()) )
      return Expression( (args[0].head().asComplex() / args[1].head().asComplex()) );
    else{
      throw SemanticError("Error in call to division: invalid argument.");
    }
  }
  else if(nargs_equal(args,1)) {
    if(args[0].head().isNumber())
      return Expression(1.0/args[0].head().asNumber());
    else if(args[0].head().isComplex())
      return Expression(1.0/args[0].head().asComplex());
    else
      throw SemanticError("Error in call to division: invalid argument.");
  }
  else{
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  return Expression(result);
};


// Returns the square root of the input. Accepts Number types
// Will add Complex types soon
Expression sqrt(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    if(args[0].isHeadNumber()){
      // Get square root of argument given
      if(args[0].head().asNumber() >= 0)
        return Expression(std::sqrt(args[0].head().asNumber()));
      else {
        std::complex<double> comp_sqrt(0,std::sqrt(std::abs(args[0].head().asNumber())));
        return Expression(comp_sqrt);
      }
    }
    else if(args[0].isHeadComplex()) {
      return Expression(std::sqrt(args[0].head().asComplex()));
    }
    else
      throw SemanticError("Error in call for square root: invalid argument."); // If argument is NaN

  }
  else
    throw SemanticError("Error in call for square root: invalid number of arguments."); // Input needs to be unary

};


// Returns calculation of a to the power b, able to calculate for Number and Complex type
Expression pow(const std::vector<Expression>& args) {

  if(nargs_equal(args, 2)) { // Make sure its binary
    if(args[0].isHeadNumber() && args[1].isHeadNumber()) {

      if(args[1].head().asNumber() == 0) // If power is 0, return 1
        return Expression(1.0);
      else if(args[1].head().asNumber() == 1)
        return Expression(args[0].head().asNumber());
      else
        return Expression(std::pow(args[0].head().asNumber(), args[1].head().asNumber()));
    }
    else if(args[0].isHeadNumber() && args[1].isHeadComplex()) {
      return Expression(std::pow(args[0].head().asNumber(), args[1].head().asComplex()));
    }
    else if(args[0].isHeadComplex() && args[1].isHeadNumber()) {
      return Expression(std::pow(args[0].head().asComplex(), args[1].head().asNumber()));
    //  throw SemanticError("Error in call for power function: invalid argument.");
    }
    else if(args[0].isHeadComplex() && args[1].isHeadComplex()) {
      return Expression(std::pow(args[0].head().asComplex(), args[1].head().asComplex()));
    }
    else
      throw SemanticError("Error in call for power function: invalid argument.");

  }
  else
    throw SemanticError("Error in call for power function: invalid number of arguments.");

};


// Returns the calculation of the natural log of the input
Expression nlog(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    if(args[0].head().asNumber() > 0) {
      return Expression(std::log(args[0].head().asNumber()));
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
    return Expression(std::sin(args[0].head().asNumber()));
  }
  else
    throw SemanticError("Error in call to sine function: invalid number of arguments.");
};


// Returns the calculation of the cos of the input in terms of radians
Expression cos(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    return Expression(std::cos(args[0].head().asNumber()));
  }
  else
    throw SemanticError("Error in call to sine function: invalid number of arguments.");
};


// Returns the calculation of the tan of the input in terms of radians
Expression tan(const std::vector<Expression>& args) {

  if(nargs_equal(args, 1)) {
    if(!args[0].head().isNumber())
      throw SemanticError("Error in call to tan function: invalid argument.");
    else
      return Expression(std::tan(args[0].head().asNumber()));
  }
  else
    throw SemanticError("Error in call to tan function: invalid number of arguments.");
};


// Returns the real part of a complex argument
Expression real(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to real function: invalid number of arguments.");

  if(args[0].isHeadComplex()) {
    double real_part = real(args[0].head().asComplex());
    return Expression(real_part);
  }
  else
    throw SemanticError("Error in call to real function: invalid argument.");
};

// Returns the imaginary part of a complex argument
Expression imag(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to imag function: invalid number of arguments.");

  if(args[0].isHeadComplex()) {
    double imag_part = imag(args[0].head().asComplex());
    return Expression(imag_part);
  }
  else
    throw SemanticError("Error in call to imag function: invalid argument.");
};

// Returns the absolute magnitude of a complex argument
Expression mag(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to mag function: invalid number of arguments.");

  if(args[0].isHeadComplex()) {
    double magnitude = abs(args[0].head().asComplex());
    return Expression(magnitude);
  }
  else
    throw SemanticError("Error in call to mag function: invalid argument.");
};

// Returns the phase angle of a complex argument
Expression arg(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to arg function: invalid number of arguments.");

  if(args[0].isHeadComplex()) {
    double angle = arg(args[0].head().asComplex());
    return Expression(angle);
  }
  else
    throw SemanticError("Error in call to arg function: invalid argument.");
};

// Returns the conjugate of a complex argument
Expression conj(const std::vector<Expression>& args) {
  if(!nargs_equal(args,1))
    throw SemanticError("Error in call to conj function: invalid number of arguments.");

  if(args[0].isHeadComplex()) {
    std::complex<double> conjugate = conj(args[0].head().asComplex());
    return Expression(conjugate);
  }
  else
    throw SemanticError("Error in call to conj function: invalid argument.");
};

// Set properties for an expression
/*Expression set_property(const std::vector<Expression>& args) {
  if(!nargs_equal(args,3))
    throw SemanticError("Error in call to range: invalid number of arguments");

  if(!args[0].head().isString())
    throw SemanticError("Error in call to set-property: invalid argument.");

  if(args[1])


}*/


const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I(0, 1);

Environment::Environment(){

  reset();

  // set flag as off to start
  envmap.emplace("interrupt_flag", EnvResult(ExpressionType, Expression(0)));
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

void Environment::add_exp(const Atom & sym, const Expression & exp, bool need_redef) {

  if(!sym.isSymbol()){
    throw SemanticError("Attempt to add non-symbol to environment");
  }

  // error if overwriting symbol map
  if((envmap.find(sym.asSymbol()) != envmap.end()) && !need_redef){
    throw SemanticError("Attempt to overwrite symbol in environemnt");
  }

  // If function is a lambda and we need to redefine variable
  if(need_redef && (envmap.find(sym.asSymbol()) != envmap.end()))
    envmap.erase(envmap.find(sym.asSymbol()));

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

  // Built-In value of i
  envmap.emplace("I", EnvResult(ExpressionType, Expression(I)));

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

  // Procedure: real
  envmap.emplace("real", EnvResult(ProcedureType, real));

  // Procedure: imag
  envmap.emplace("imag", EnvResult(ProcedureType, imag));

  // Procedure: mag
  envmap.emplace("mag", EnvResult(ProcedureType, mag));

  // Procedure: arg
  envmap.emplace("arg", EnvResult(ProcedureType, arg));

  // Procedure: conj
  envmap.emplace("conj", EnvResult(ProcedureType, conj));

  // Procedure: list
  envmap.emplace("list", EnvResult(ProcedureType, buildList));

  // Procedure: first
  envmap.emplace("first", EnvResult(ProcedureType, first));

  // Procedure: rest
  envmap.emplace("rest", EnvResult(ProcedureType, rest));

  // Procedure: length
  envmap.emplace("length", EnvResult(ProcedureType, length));

  // Procedure: append
  envmap.emplace("append", EnvResult(ProcedureType, append));

  // Procedure: join
  envmap.emplace("join", EnvResult(ProcedureType, join));

  // Procedure: range
  envmap.emplace("range", EnvResult(ProcedureType, range));

  // Procedure: set-property
  //envmap.emplace("set-property", EnvResult(ProcedureType, set_property));

  // Procedure: get-property
  //envmap.emplace("get-property", EnvResult(ProcedureType, get_property));

}
