#include "expression.hpp"

#include <sstream>
#include <list>
#include <iostream>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression() {}

Expression::Expression(const Atom & a){

  m_head = a;
}

// recursive copy
Expression::Expression(const Expression & a) {

  m_head = a.m_head;
  for(auto e : a.m_tail){
    m_tail.push_back(e);
  }
  property_list = a.property_list;

}

Expression::Expression(const std::vector<Expression> & a) {
  //m_head.setList();
  //isList = true;
  for(auto e : a){
    m_tail.push_back(e);
  }
}

Expression & Expression::operator=(const Expression & a) {

  // prevent self-assignment
  if(this != &a){
    m_head = a.m_head;
    m_tail.clear();
    for(auto e : a.m_tail){
      m_tail.push_back(e);
    }
    property_list = a.property_list;

  }

  return *this;
}


Atom & Expression::head(){
  return m_head;
}

const Atom & Expression::head() const{
  return m_head;
}

bool Expression::isHeadNumber() const noexcept{
  return m_head.isNumber();
}

bool Expression::isHeadComplex() const noexcept {
  return m_head.isComplex();
}

bool Expression::isHeadList() const noexcept {
  if(m_head.isSymbol())
    return (m_head.asSymbol() == "list");
  else
    return m_head.isList();
}

bool Expression::isHeadSymbol() const noexcept{
  return m_head.isSymbol();
}

void Expression::setHeadList() {
  m_head.setList();
}

void Expression::setHeadLambda() {
  m_head.setLambda();
}


void Expression::append(const Atom & a){
  m_tail.emplace_back(a);
}

void Expression::append(const Expression& a) {
  m_tail.push_back(a);
}


Expression * Expression::tail(){
  Expression * ptr = nullptr;

  if(m_tail.size() > 0){
    ptr = &m_tail.back();
  }

  return ptr;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept{
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept{
  return m_tail.cend();
}

std::vector<Expression> Expression::getTail() const {
  return m_tail;
}

Expression Expression::apply(Atom & op, std::vector<Expression> & args, Environment & env) {

  if(!(op.isSymbol() || op.isString()) && !op.isLambda()) {
    //if(op.asString() != "list")
    throw SemanticError("Error during evaluation: procedure name not symbol or lambda.");
  }

  // must map to a proc or exp
  if(!env.is_proc(op) && !env.is_exp(op)){
    throw SemanticError("Error during evaluation: symbol does not name a procedure or lambda.");
  }

  // If atom is placeholder for lambda
  if(env.is_exp(op)) {

    Expression lambda = env.get_exp(op);
    std::vector<Expression> lambda_tail = lambda.getTail();
    std::vector<Expression> lambda_args = args;
    Expression lambda_list = lambda_tail[0];

    //for(Expression::IteratorType it = lambda.m_tail[0].begin(); it != lambda.m_tail[0].end(); ++it, i++){
    if(lambda_list.getTail().size() != lambda_args.size())
      throw SemanticError("Error in call to lambda function: invalid number of arguments.");

    Environment copyEnv = env;
    //copyEnv.setLambda();

    for(int j = 0; j < lambda_list.getTail().size(); j++) {
      copyEnv.add_exp(lambda_list.m_tail[j].head(), lambda_args[j], true);
    }

    return lambda_tail[1].eval(copyEnv);
  }
  else {
    // map from symbol to proc
    Procedure proc = env.get_proc(op);

    // call proc with args
    return proc(args);
  }
}

Expression Expression::handle_lookup(const Atom & head, const Environment & env){
    if(head.asString().front() == '\"') {
      return Expression(head);
    }

    //std::cout << head.asSymbol() << '\n';

    if(head.isSymbol()){ // if symbol is in env return value
      if(head.asSymbol() == "list") {
        Expression toReturn = Expression();
        //toReturn.head().setList();
        return toReturn;
      }

      if(env.is_exp(head)){
        return env.get_exp(head);
      }
      else if(env.is_proc(head)) {
        return Expression(head);
      }
      else{
         throw SemanticError("Error during evaluation: unknown symbol");
      }
    }
    else if(head.isNumber() || head.isComplex()){
      return Expression(head);
    }
    else{
      throw SemanticError("Error during evaluation: Invalid type in terminal expression");
    }
}

Expression Expression::handle_begin(Environment & env){

  if(m_tail.size() == 0){
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
    result = it->eval(env);
  }

  return result;
}


Expression Expression::handle_define(Environment & env){
  //defined = true;

  // tail must have size 3 or error
  if(m_tail.size() != 2)
    throw SemanticError("Error during evaluation: invalid number of arguments to define");

  // tail[0] must be symbol
  if(!m_tail[0].isHeadSymbol())
    throw SemanticError("Error during evaluation: first argument to define not symbol");

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if((s == "define") || (s == "begin") || (s == "lambda"))
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");

  if(env.is_proc(m_head))
    throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");

  // eval tail[1]
  Expression result = m_tail[1].eval(env);

  if(env.is_exp(m_head))
    throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");

  //if(m_tail[1].head().asSymbol() == "lambda")
    //envmap.emplace(m_tail[0].head(), EnvResult(ProcedureType, lambda));
  //else {
    //and add to env
  env.add_exp(m_tail[0].head(), result);
  //}

  return result;
}

// Special form method to handle a lambda function created by the user
Expression Expression::handle_lambda(Environment & env) {

  if(m_tail.size() != 2)
    throw SemanticError("Error during evaluation: invalid number of arguments to define");

  // create the list of arguments to be created
  Expression result;// = m_tail[1];
  m_tail[0].m_tail.insert(m_tail[0].m_tail.begin(), Expression(m_tail[0].head())); // push input argument in head into tail as part of list
  m_tail[0].setHeadList(); // Set as list

  // Create returnable lambda expression
  result.m_tail.push_back(m_tail[0]);
  result.m_tail.push_back(m_tail[1]);

  result.setHeadLambda();

  return result;
}

Expression Expression::handle_apply(Environment & env) {
  if(m_tail.size() != 2)
    throw SemanticError("Error in call to apply: invalid number of arguments");

  if(!m_tail[1].isHeadList())
    throw SemanticError("Error in call to apply: invalid list argument");

  if(m_tail[0].m_tail != std::vector<Expression>())
    throw SemanticError("Error in call to apply: invalid symbol argument");

  if(env.is_proc(m_tail[0].head())) {

    Procedure proc = env.get_proc(m_tail[0].head());

    Expression exp_tail = m_tail[1];
    std::vector<Expression> pass_exp;

    for(Expression::IteratorType it = exp_tail.m_tail.begin(); it != exp_tail.m_tail.end(); ++it) {
      pass_exp.push_back((*it).eval(env));
    }

    Expression result = proc(pass_exp);

    return result;
  }
  else if(env.is_exp(m_tail[0].head())) {

    Expression args_eval = m_tail[1].eval(env);

    return apply(m_tail[0].head(), args_eval.m_tail, env);
  }
  else
    throw SemanticError("Error in call to apply: invalid symbol argument");
}

// Similar to apply but run procedure/expression on each item in list
Expression Expression::handle_map(Environment & env) {
  if(m_tail.size() != 2)
    throw SemanticError("Error in call to map: invalid number of arguments");

  //std::cout << m_tail[1] << '\n';

  if(!m_tail[1].isHeadList() && m_tail[1].head().asString() != "range")
    throw SemanticError("Error in call to mapy: invalid list argument");

  if(m_tail[0].m_tail != std::vector<Expression>())
    throw SemanticError("Error in call to map: invalid symbol argument");

  if(env.is_proc(m_tail[0].head())) {

    Procedure proc = env.get_proc(m_tail[0].head());

    Expression exp_tail = m_tail[1];
    std::vector<Expression> pass_exp;

    for(Expression::IteratorType it = exp_tail.m_tail.begin(); it != exp_tail.m_tail.end(); ++it) {
      pass_exp.push_back((*it).eval(env));
    }

    Expression return_exp;
    return_exp.setHeadList();

    for(auto & a: pass_exp)
      return_exp.append(proc(std::vector<Expression>(1,a)));

    return return_exp;
  }
  else if(env.is_exp(m_tail[0].head())) {

    Expression args_eval = m_tail[1].eval(env);

    Expression return_exp;
    return_exp.setHeadList();

    for(int i = 0; i < args_eval.m_tail.size(); i++) {
      std::vector<Expression> toPass(1,args_eval.m_tail[i]);
      return_exp.append(apply(m_tail[0].head(), toPass, env));
    }

    return return_exp;
  }
  else
    throw SemanticError("Error in call to apply: invalid symbol argument.");
}


///*
// Methods for setting and getting properties
//*/
void Expression::add_property(const std::string & key, Expression & value) {
  //Expression temp = value.eval(env);
  //stored_values.push_back(value);
  //std::cout << "size: " << stored_values.size() << '\n';
  //property_list[key] = &stored_values.back();
  property_list[key] = value;
}

Expression Expression::get_property(const std::string & key) {
  return property_list[key];
}

Expression Expression::handle_set_prop(Environment & env) {
  if(m_tail.size() != 3)
    throw SemanticError("Error in call to set-property: invalid number of arguments.");

  if((m_tail[0].head().asSymbol().front() != '\"'))
    throw SemanticError("Error in call to set-property: invalid argument.");

  //std::cout << "Key: " << m_tail[0].head().asString() << '\n';
  //std::cout << "Value: " << m_tail[1].head() << '\n';

  Expression returnExp = m_tail[2].eval(env);
  Expression value = m_tail[1].eval(env);

  //std::cout << m_tail[1] << ' ' << m_tail[2] << '\n';
  //Expression temp = value.eval(env);
  returnExp.add_property(m_tail[0].head().asString(), value);


  return returnExp;
}

Expression Expression::handle_get_prop(Environment & env) {
  if(m_tail.size() != 2)
    throw SemanticError("Error in call to set-property: invalid number of arguments.");

  if((m_tail[0].head().asSymbol().front() != '\"'))
    throw SemanticError("Error in call to set-property: invalid argument.");

  Expression temp;
  //if(env.is_exp(m_tail[1].head()))
  //temp = env.get_exp(m_tail[1].head());
  temp = m_tail[1].eval(env);

  std::map<std::string,Expression>::const_iterator isFound = temp.property_list.find(m_tail[0].head().asString());

  if(isFound == temp.property_list.end()) {
    return Expression();
  }
  else
    return temp.get_property(m_tail[0].head().asString());
}


Expression Expression::handle_discrete_plot(Environment & env) {

  double scaleVal = 1;
  bool isScaled = false;

  double minY = m_tail[1].getTail()[0].head().asNumber();
  double maxY = m_tail[1].getTail()[0].head().asNumber();
  double minX = m_tail[0].getTail()[0].head().asNumber();
  double maxX = m_tail[0].getTail()[0].head().asNumber();

  Expression toReturn;
  toReturn.setHeadList();

  std::string objName = "\"object-name\"";
  std::string textType = "\"text\"";
  std::string lineType = "\"line\"";
  std::string pointType = "\"point\"";
  Expression type2(pointType);
  Expression type1(lineType);
  Expression type(textType);
  std::string optionProp = "\"option\"";

  if(m_tail.size() != 2)
    throw SemanticError("Error in call to discrete-plot: invalid number of arguments.");

  //std::cout << "Head: " << m_tail[0].head() << '\n';
  if(!m_tail[0].isHeadList() && m_tail[0].head().asSymbol() != "map" && m_tail[0].head().asSymbol() != "range")
    throw SemanticError("Error in call to discrete-plot: argument 1 not a list.");

  if(!m_tail[1].isHeadList())
    throw SemanticError("Error in call to discrete-plot: argument 2 not a list.");

  if(m_tail[0].size() == 0 || m_tail[1].size() == 0)
    throw SemanticError("Error in call to discrete-plot: empty list argument.");

  if(m_tail[1].getTail().size() != 0) {
    for(auto & option: m_tail[1].getTail()) {
      if(!option.isHeadList())
        throw SemanticError("Error in call to discrete-plot: argument not a list.");

      if(option.getTail().at(0).head().asString() == "\"text-scale\"") {
        isScaled = true;
        scaleVal = option.getTail().at(1).head().asNumber();
      }
      else {

        Expression optionName(option.getTail().at(0).head());

        //std::cout << "item: " << option.getTail().at(1).head().asString() << '\n';
        Expression newOption = Expression(option.getTail().at(1).head().asString());
        //std::cout << newOption << ' ';
        newOption.add_property(objName, type);
        //std::cout << newOption.get_property(objName) << '\n';
        newOption.add_property(optionProp, optionName);

        // Location of the option in the graph
        Expression textLocation;
        textLocation.clearTail(); // Just to make sure tail is empty
        textLocation.setHeadList();
        textLocation.add_property(objName, type2);

        if(option.getTail().at(0).head().asString() == "\"title\"") {
          textLocation.append(0);
          textLocation.append(-13);
        }
        else if(option.getTail().at(0).head().asString() == "\"abscissa-label\"") {
          textLocation.append(0);
          textLocation.append(13);
        }
        else if(option.getTail().at(0).head().asString() == "\"ordinate-label\"") {
          textLocation.append(-13);
          textLocation.append(0);

          Expression rotation(4.71);
          newOption.add_property(std::string("\"text-rotation\""), rotation);
        }

        newOption.add_property(std::string("\"position\""), textLocation);

        toReturn.append(newOption);
      }
    }
  }

  Expression point1;
  point1.setHeadList();
  point1.add_property(objName, type1);
  point1.append(10);
  point1.append(10);

  Expression point2;
  point2.setHeadList();
  point2.append(-10);
  point2.append(10);

  Expression point3;
  point3.setHeadList();
  point3.append(10);
  point3.append(-10);

  Expression point4;
  point4.setHeadList();
  point4.append(-10);
  point4.append(-10);


  // Lines for bounding box
  Expression line1; line1.setHeadList();
  line1.add_property(std::string("\"object-name\""), type1);
  line1.append(point1); line1.append(point2);

  Expression line2; line2.setHeadList();
  line2.add_property(std::string("\"object-name\""), type1);
  line2.append(point2); line2.append(point4);

  Expression line3; line3.setHeadList();
  line3.add_property(std::string("\"object-name\""), type1);
  line3.append(point4); line3.append(point3);

  Expression line4; line4.setHeadList();
  line4.add_property(std::string("\"object-name\""), type1);
  line4.append(point3); line4.append(point1);

  toReturn.append(line1);
  toReturn.append(line2);
  toReturn.append(line3);
  toReturn.append(line4);

  if(m_tail[0].getTail().size() != 0) {
    for(auto & list: m_tail[0].getTail()) {
      if(!list.isHeadList())
        throw SemanticError("Error in call to discrete-plot: argument not a list.");

      Expression newPoint;
      Expression pointSize(0.5);
      newPoint.add_property(std::string("\"object-name\""), type2);
      newPoint.add_property(std::string("\"size\""), pointSize);
      newPoint.append(list.getTail().at(0).head().asNumber());
      newPoint.append(list.getTail().at(1).head().asNumber());

      if(list.getTail().at(0).head().asNumber() < minX)
        minX = list.getTail().at(0).head().asNumber();
      else if(list.getTail().at(0).head().asNumber() > maxX)
        maxX = list.getTail().at(0).head().asNumber();

      if(list.getTail().at(1).head().asNumber() < minY)
        minY = list.getTail().at(1).head().asNumber();
      else if(list.getTail().at(1).head().asNumber() > maxY)
        maxY = list.getTail().at(1).head().asNumber();

      Expression xAxis1; Expression xAxis2;
      xAxis1.add_property(std::string("\"object-name\""), type2); xAxis2.add_property(std::string("\"object-name\""), type2);
      Expression yAxis1; Expression yAxis2;
      yAxis1.add_property(std::string("\"object-name\""), type1); yAxis2.add_property(std::string("\"object-name\""), type1);
      xAxis1.setHeadList(); xAxis2.setHeadList();
      yAxis1.setHeadList(); yAxis2.setHeadList();
      xAxis1.append(-10+(20/(maxX-minX))*(-minX)); xAxis1.append(-10);
      xAxis2.append(-10+(20/(maxX-minX))*(-minX)); xAxis2.append(10);
      yAxis1.append(-10); yAxis1.append(-10+(20/(maxY-minY))*(-minY));
      yAxis2.append(10); yAxis2.append(-10+(20/(maxY-minY))*(-minY));

      Expression xAxis; Expression yAxis;
      xAxis.add_property(std::string("\"object-name\""), type1); yAxis.add_property(std::string("\"object-name\""), type1);
      xAxis.append(xAxis1); xAxis.append(xAxis2);
      yAxis.append(yAxis1); yAxis.append(yAxis2);

      if(minY < 0)
        toReturn.append(yAxis);
      if(minX < 0)
        toReturn.append(xAxis);

      toReturn.append(newPoint);
    }
  }
  /*else {

  }*/
  //std::cout << toReturn << '\n';

  return toReturn;
}


Expression Expression::handle_continuous_plot(Environment & env) {

  if(!isHeadList())
    throw SemanticError("Error in call to set-property: invalid number of arguments.");

  return Expression();
}



void Expression::setHead(const Atom & a) {
  m_head = a;
}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment & env) {

  if(m_tail.empty()) {


    return handle_lookup(m_head, env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "begin") { // handle begin special-form
    return handle_begin(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "define") { // handle define special-form
    return handle_define(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "lambda") { // handle lambda special-form
    return handle_lambda(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "apply") { // handle apply special-form
    return handle_apply(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "map") { // handle map special-form
    return handle_map(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "set-property") {
    return handle_set_prop(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "get-property") {
    return handle_get_prop(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "discrete-plot") {
    return handle_discrete_plot(env);
  }
  else if(m_head.isSymbol() && m_head.asSymbol() == "continuous-plot") {
    return handle_continuous_plot(env);
  }
  else { // else attempt to treat as procedure
    std::vector<Expression> results;
    for(Expression::IteratorType it = m_tail.begin(); it != m_tail.end(); ++it){
      results.push_back(it->eval(env));
    }
    return apply(m_head, results, env);
  }
}


std::ostream & operator<<(std::ostream & out, const Expression & exp) {

  if(exp == Expression()) {
    out << "NONE";
    return out;
  }

  out << "(";
  out << exp.head();
  bool begin = true; // To know when to add spaces to output
  if(exp.head().isSymbol() && (exp.getTail() != std::vector<Expression>()))
    out << " ";

  for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
    if(begin == false) // Kind of wonky but oh well
      out << " ";

    out << *e;
    begin = false;
  }

  out << ")";



  return out;
}

bool Expression::operator==(const Expression & exp) const noexcept{

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if(result){
    for(auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
  (lefte != m_tail.end()) && (righte != exp.m_tail.end());
  ++lefte, ++righte){
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool operator!=(const Expression & left, const Expression & right) noexcept{

  return !(left == right);
}
