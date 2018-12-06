#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string & program){

  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);
  if(!ok){
    std::cerr << "Failed to parse: " << program << std::endl;
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE( "Test Interpreter parser with expected input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE( "Test Interpreter parser with numerical literals", "[interpreter]" ) {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};

  for(auto program : programs){
    std::istringstream iss(program);

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with truncated input", "[interpreter]" ) {

  {
    std::string program = "(f";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }

  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE( "Test Interpreter parser with extra input", "[interpreter]" ) {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with single non-keyword", "[interpreter]" ) {

  std::string program = "hello";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty input", "[interpreter]" ) {

  std::string program;
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with empty expression", "[interpreter]" ) {

  std::string program = "( )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with bad number string", "[interpreter]" ) {

  std::string program = "(1abc)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter parser with incorrect input. Regression Test", "[interpreter]" ) {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE( "Test Interpreter result with literal expressions", "[interpreter]" ) {

  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE( "Test Interpreter result with simple procedures (add)", "[interpreter]" ) {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }

  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}

TEST_CASE( "Test Interpreter special forms: begin and define", "[interpreter]" ) {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE( "Test a medium-sized expression", "[interpreter]" ) {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE("Test various lists", "[interpreter]") {
  std::string input1 = "(list 1 I \"hi\")";
  std::string input2 = "(list)";
  std::string input3 = "(list (first (list 1 2)) (rest (list 2 3 4 5)))";

  Interpreter interp;

  std::istringstream iss2(input2);

  bool ok2 = interp.parseStream(iss2);
  REQUIRE(ok2 == true);

  std::istringstream iss1(input1);

  bool ok1 = interp.parseStream(iss1);
  REQUIRE(ok1 == true);

  std::istringstream iss3(input3);

  bool ok3 = interp.parseStream(iss3);
  REQUIRE(ok3 == true);


}

TEST_CASE( "Test arithmetic procedures", "[interpreter]" ) {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
           "(+ -3 1 1)",
           "(- 1)",
           "(- 1 2)",
           "(* 1 -1)",
           "(* 1 1 -1)",
           "(/ -1 1)",
           "(/ 1 -1)"};

    for(auto s : programs){
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}


TEST_CASE( "Test some semantically invalid expresions", "[interpreter]" ) {

  std::vector<std::string> programs = {"(@ none)", // so such procedure
               "(- 1 1 2)", // too many arguments
               "(define begin 1)", // redefine special form
               "(define pi 3.14)"}; // redefine builtin symbol
    for(auto s : programs){
      Interpreter interp;

      std::istringstream iss(s);

      bool ok = interp.parseStream(iss);
      REQUIRE(ok == true);

      REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
    }
}

TEST_CASE("Test error handling in define and begin and lookup") {
  std::string input1 = "(define 1)";
  std::string input2 = "(define 3 2)";
  std::string input3 = "(define + 5)";
  std::string input4 = "(begin)";
  std::string input5 = "(a)";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);
  std::istringstream iss4(input4);
  std::istringstream iss5(input5);

  Interpreter interp;
  Expression result;

  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss4));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss5));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE("Testing error handling for apply", "[interpreter]") {
  std::string input1 = "(2 3 4)";
  std::string input2 = "(+ 2 hi)";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);

  Interpreter interp;
  Expression result;

  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE( "Test for exceptions from semantically incorrect input", "[interpreter]" ) {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test malformed define", "[interpreter]" ) {

    std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE( "Test using number as procedure", "[interpreter]" ) {
    std::string input = R"(
(1 2 3)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Testing use of lambda function in expression.cpp", "[interpreter]") {
  std::string input = "(define f (lambda (x) (+ 3 x I)))";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  std::string input2 = "(f 7)";
  std::istringstream iss2(input2);

  bool ok2 = interp.parseStream(iss2);
  REQUIRE(ok2 == true);

  Expression result2;
  REQUIRE_NOTHROW(result2 = interp.evaluate());

}

TEST_CASE("Testing use of map and range functions in expression.cpp", "[interpreter]") {
  std::string input = "(define my_map (map + (range -2 2 0.5)))";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());
}

TEST_CASE("Testing use of apply and range functions in expression.cpp", "[interpreter]") {
  std::string input = "(define my_map (apply / (range -2 2 0.5)))";
  Interpreter interp;
  std::istringstream iss(input);
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  Expression result;
  REQUIRE_THROWS(result = interp.evaluate());

  Interpreter interp2;
  std::string input2 = "(apply + (list 1 2 3 4 5))";
  std::istringstream iss2(input2);
  bool ok2 = interp2.parseStream(iss2);
  REQUIRE(ok2 == true);
  Expression result2;
  REQUIRE_NOTHROW(result2 = interp2.evaluate());
}

TEST_CASE("Error handling range", "[interpreter]") {
  std::string input1 = "(range 0)";
  std::string input2 = "(range 0 -1 2)";
  std::string input3 = "(range 0 1 -1)";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);

  Interpreter interp;
  Expression result;
  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE("Error handling map", "[interpreter]") {
  std::string input1 = "(map 0)";
  std::string input2 = "(map (list) 2)";
  std::string input3 = "(map 0 1)";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);

  Interpreter interp;
  Expression result;
  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE("Error handling apply", "[interpreter]") {
  std::string input1 = "(range 0)";
  std::string input2 = "(range (list) 2)";
  std::string input3 = "(range 0 1)";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);

  Interpreter interp;
  Expression result;
  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE("Testing several expression methods", "[interpreter]") {
  std::string input = "(define complexAsList (lambda (x) (list (real x) (imag x))))";
  std::string input2 = "(apply complexAsList (list (+ 1 (* 3 I))))";

  Interpreter interp;
  Interpreter interp2;

  std::istringstream iss(input);
  std::istringstream iss2(input2);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  bool ok2 = interp.parseStream(iss2);
  REQUIRE(ok2 == true);
  Expression result2;
  REQUIRE_NOTHROW(result2 = interp.evaluate());
}

TEST_CASE("Testing property lists of expressions", "[interpreter]") {
  std::string input = "(define my_point (set-property \"size\" 3 (make-point 2 3)))";
  Interpreter interp;
  std::istringstream iss(input);
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
  Expression result;
  REQUIRE_THROWS(result = interp.evaluate());

  std::string input2 = "(get-property \"object-name\" make-point)";
  Interpreter interp2;
  std::istringstream iss2(input2);
  bool ok2 = interp2.parseStream(iss2);
  REQUIRE(ok2 == true);
  Expression result2;
  REQUIRE_THROWS(result2 = interp2.evaluate());
}

TEST_CASE("Error handling for property lists", "[interpreter]") {
  std::string input1 = "(set-property 1 2)";
  std::string input2 = "(set-property 4 3 5)";
  std::string input3 = "(get-property 1 2 3)";
  std::string input4 = "(get-property 5 2)";

  std::stringstream iss1(input1);
  std::stringstream iss2(input2);
  std::stringstream iss3(input3);
  std::stringstream iss4(input4);

  Interpreter interp;
  Expression result;
  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss4));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE("Testing discrete-plot", "[interpreter]") {
  std::string input1 = "(define f (lambda (x) (list x (+ (* 2 x) 1))))";
  Interpreter interp;
  std::istringstream iss1(input1);
  bool ok1 = interp.parseStream(iss1);
  REQUIRE(ok1 == true);
  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  std::string input2 = "(discrete-plot (map f (range -2 2 0.5)) (list (list \"text-scale\" 3) (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1)))";
  Interpreter interp2;
  std::istringstream iss2(input2);
  bool ok2 = interp2.parseStream(iss2);
  REQUIRE(ok2 == true);
  Expression result2;
  REQUIRE_THROWS(result2 = interp2.evaluate());

  if(interp.parseStream(iss2))
    REQUIRE_NOTHROW(result = interp.evaluate());
}

TEST_CASE("Testing continuous-plot", "[interpreter]") {
  Interpreter interp;
  std::string input = "(continuous-plot 1 2)";
  std::istringstream iss(input);
  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);
}

TEST_CASE("Testing error handling for rest", "[interpreter]") {
  std::string input1 = "(rest 1 2)";
  std::string input2 = "(rest 1)";
  std::string input3 = "(rest (list))";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);

  Interpreter interp;
  Expression result;

  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE("Testing error handling for length", "[interpreter]") {
  std::string input1 = "(length 1 2)";
  std::string input2 = "(length 1)";
  std::string input3 = "(length (list))";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);

  Interpreter interp;
  Expression result;

  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_NOTHROW(result = interp.evaluate());
}

TEST_CASE("Testing various parts of append", "[interpreter]") {
  std::string input1 = "(append 0 1 2)";
  std::string input2 = "(append 1)";
  std::string input3 = "(append (list) (list))";
  std::string input4 = "(append (list 1) 4)";
  std::string input5 = "(append (list 1) I)";
  std::string input6 = "(append (list 1) hi)";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);
  std::istringstream iss4(input4);
  std::istringstream iss5(input5);
  std::istringstream iss6(input6);

  Interpreter interp;
  Expression result;

  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_NOTHROW(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss4));
  REQUIRE_NOTHROW(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss5));
  REQUIRE_NOTHROW(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss6));
  REQUIRE_THROWS(result = interp.evaluate());
}

TEST_CASE("Testing error handling for join", "[interpreter]") {
  std::string input1 = "(join 1)";
  std::string input2 = "(join 1 2)";
  std::string input3 = "(join (list) (list))";

  std::istringstream iss1(input1);
  std::istringstream iss2(input2);
  std::istringstream iss3(input3);

  Interpreter interp;
  Expression result;

  REQUIRE(interp.parseStream(iss1));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss2));
  REQUIRE_THROWS(result = interp.evaluate());
  REQUIRE(interp.parseStream(iss3));
  REQUIRE_NOTHROW(result = interp.evaluate());
}
