#include "catch.hpp"

#include "environment.hpp"
#include "semantic_error.hpp"


#include <cmath>
#include <string>

TEST_CASE( "Test default constructor", "[environment]" ) {

  Environment env;

  REQUIRE(env.is_known(Atom("pi")));
  REQUIRE(env.is_exp(Atom("pi")));

  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));

  REQUIRE(env.is_proc(Atom("+")));
  REQUIRE(env.is_proc(Atom("-")));
  REQUIRE(env.is_proc(Atom("*")));
  REQUIRE(env.is_proc(Atom("/")));
  REQUIRE(!env.is_proc(Atom("op")));

  REQUIRE(env.is_proc(Atom("sqrt")));
  REQUIRE(env.is_proc(Atom("^")));
  REQUIRE(env.is_proc(Atom("ln")));
  REQUIRE(env.is_proc(Atom("sin")));
  REQUIRE(env.is_proc(Atom("cos")));
  REQUIRE(env.is_proc(Atom("tan")));

}

TEST_CASE( "Test get expression", "[environment]" ) {
  Environment env;

  REQUIRE(env.get_exp(Atom("pi")) == Expression(std::atan2(0, -1)));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());

  REQUIRE(env.get_exp(Atom("e")) == Expression(std::exp(1)));

  std::complex<double> test_I(0,1);
  REQUIRE(env.get_exp(Atom("I")) == Expression(test_I));
}

TEST_CASE( "Test add expression", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  REQUIRE(env.is_known(Atom("one")));
  REQUIRE(env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == a);

  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);
  REQUIRE(env.is_known(Atom("hi")));
  REQUIRE(env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == b);

  REQUIRE_THROWS_AS(env.add_exp(Atom(1.0), b), SemanticError);
}

TEST_CASE( "Test get built-in procedure", "[environment]" ) {
  Environment env;

  INFO("default procedure")
  Procedure p1 = env.get_proc(Atom("doesnotexist"));
  Procedure p2 = env.get_proc(Atom("alsodoesnotexist"));
  REQUIRE(p1 == p2);
  std::vector<Expression> args;
  REQUIRE(p1(args) == Expression());
  REQUIRE(p2(args) == Expression());

  //args.clear();
  INFO("trying add procedure")
  Procedure padd = env.get_proc(Atom("+"));
  //REQUIRE_THROWS_AS(padd(args), SemanticError);
  args.emplace_back(1.0);

  std::complex<double> testcomp(1,1);
  //std::complex<double> testcomp1(1,0);
  args.emplace_back(testcomp);
  //args.emplace_back(testcomp1);
  std::complex<double>testcomp2(2,1);
  REQUIRE(padd(args) == Expression(testcomp2));

  args.clear();
  INFO("trying subneg procedure")
  Procedure psub = env.get_proc(Atom("-"));
  args.emplace_back(7);
  std::complex<double> testcomp1(3,2);
  std::complex<double> compres(4,2);
  args.emplace_back(testcomp1);
  REQUIRE(psub(args) == Expression(compres));

  args.clear();
  std::complex<double> testcomp3(3,2);
  REQUIRE_THROWS_AS(psub(args), SemanticError);
  args.emplace_back(testcomp3);
  REQUIRE(psub(args) == Expression(- testcomp3));

  /*args.clear();
  std::string wrong = "wrong";
  args.emplace_back(wrong);
  REQUIRE_THROWS_AS(padd(args), SemanticError);*/

}

TEST_CASE( "Test my mul, div, pow functions with Number & Complex types") {
  Environment env;
  std::vector<Expression> args;

  //args.clear();
  INFO("trying div procedure")
  Procedure pmul = env.get_proc(Atom("*"));
  //REQUIRE_THROWS_AS(pmul(args), SemanticError);
  //std::complex<double> testcomp0(2,3);
  args.emplace_back(3);
  std::complex<double> testcomp(3,2);
  //args.emplace_back(testcomp0);
  args.emplace_back(testcomp);
  std::complex<double> testcomp9(9,4);
  REQUIRE(pmul(args) == Expression(testcomp9));

  args.clear();
  INFO("trying div procedure")
  Procedure pdiv = env.get_proc(Atom("/"));
  REQUIRE_THROWS_AS(pdiv(args), SemanticError);
  std::complex<double> testcomp1(2,3);
  std::complex<double> testcomp2(3,2);
  args.emplace_back(testcomp1);
  args.emplace_back(testcomp2);
  std::complex<double> testcomp3(2/3,3/2);
  REQUIRE(pdiv(args) == Expression(testcomp3));

  args.clear();
  INFO("trying pow with Number and Complex types")
  Procedure ppow = env.get_proc(Atom("^"));
  REQUIRE_THROWS_AS(ppow(args), SemanticError);
  args.emplace_back(3); args.emplace_back(0);
  REQUIRE(ppow(args) == Expression(1));
  args.clear();
  args.emplace_back(3); args.emplace_back(1);
  REQUIRE(ppow(args) == Expression(3));
  args.clear();
  std::complex<double> testcomp5(3,0);
  std:;complex<double> compres(27,0);
  args.emplace_back(testcomp5); args.emplace_back(testcomp5);
  REQUIRE(ppow(args) == Expression(compres));

  args.clear();
  args.emplace_back(testcomp5);
  args.emplace_back(2);
  REQUIRE(ppow(args) == Expression(std::pow(testcomp5, 2)));

  args.clear();
  args.emplace_back(2);
  args.emplace_back(testcomp5);
  REQUIRE(ppow(args) == Expression(std::pow(2, testcomp5)));

}

TEST_CASE("Test my sqrt function w/Number & Complex types") {
  Environment env;
  std::vector<Expression> args;

  //args.clear();
  INFO("trying sqrt procedure")
  Procedure psqrt = env.get_proc(Atom("sqrt"));
  args.emplace_back(4.5);
  REQUIRE(psqrt(args) == Expression(std::sqrt(4.5)));
  args.clear();
  args.emplace_back(-1);
  std::complex<double> compres1(0,1);
  REQUIRE(psqrt(args) == Expression(compres1));

  args.clear();
  REQUIRE_THROWS_AS(psqrt(args), SemanticError);
  std::complex<double> testcomp4(2,-4);
  args.emplace_back(testcomp4);
  REQUIRE(psqrt(args) == Expression(std::sqrt(testcomp4)));
}

TEST_CASE("Test my nlog and trig functions w/Number type input") {
  Environment env;
  std::vector<Expression> args;

  args.clear();
  INFO("trying nlog w/Number & Complex")
  Procedure pln = env.get_proc(Atom("ln"));
  REQUIRE_THROWS_AS(pln(args), SemanticError);
  args.emplace_back(1);
  REQUIRE(pln(args) == Expression(0));

  args.clear(); args.emplace_back(-5.0);
  REQUIRE_THROWS_AS(pln(args), SemanticError);

  args.clear();
  INFO("trying sin function")
  Procedure psin = env.get_proc(Atom("sin"));
  REQUIRE_THROWS_AS(psin(args), SemanticError);
  args.emplace_back(0);
  REQUIRE(psin(args) == Expression(0));

  args.clear();
  INFO("trying cos function")
  Procedure pcos = env.get_proc(Atom("cos"));
  REQUIRE_THROWS_AS(pcos(args), SemanticError);
  args.emplace_back(0);
  REQUIRE(pcos(args) == Expression(1));

  args.clear();
  INFO("trying tan function")
  Procedure ptan = env.get_proc(Atom("tan"));
  REQUIRE_THROWS_AS(ptan(args), SemanticError);
  args.emplace_back(0);
  REQUIRE(ptan(args) == Expression(0));
}

TEST_CASE(" Test for real and imag functions") {
  Environment env;
  std::vector<Expression> args;

  INFO("Trying real function")
  Procedure preal = env.get_proc(Atom("real"));
  std::complex<double> testcomp(4,5);
  args.emplace_back(testcomp);
  REQUIRE(preal(args) == Expression(4));

  INFO("Trying imag function")
  Procedure pimag = env.get_proc(Atom("imag"));
  REQUIRE(pimag(args) == Expression(5));
}

TEST_CASE(" Test for my mag, arg, and conj functions") {
  Environment env;
  std::vector<Expression> args;

  INFO("Trying mag function")
  Procedure pmag = env.get_proc(Atom("mag"));
  std::complex<double> testcomp(2,1);
  args.emplace_back(testcomp);
  REQUIRE(pmag(args) == Expression(std::abs(testcomp)));
  args.clear();
  REQUIRE_THROWS_AS(pmag(args), SemanticError);
  args.emplace_back(3.0);
  REQUIRE_THROWS_AS(pmag(args), SemanticError);

  args.clear();
  INFO("Trying arg function")
  Procedure parg = env.get_proc(Atom("arg"));
  std::complex<double> testcomp1(1,2);
  args.emplace_back(testcomp1);
  REQUIRE(parg(args) == Expression(std::arg(testcomp1)));
  args.clear();
  REQUIRE_THROWS_AS(parg(args), SemanticError);
  args.emplace_back(3.0);
  REQUIRE_THROWS_AS(parg(args), SemanticError);

  args.clear();
  INFO("Trying conj function")
  Procedure pconj = env.get_proc(Atom("conj"));
  std::complex<double> testcomp2(3,-5);
  args.emplace_back(testcomp2);
  REQUIRE(pconj(args) == Expression(std::conj(testcomp2)));
  args.clear();
  REQUIRE_THROWS_AS(pconj(args), SemanticError);
  args.emplace_back(3.0);
  REQUIRE_THROWS_AS(pconj(args), SemanticError);

}

TEST_CASE( "Test reset", "[environment]" ) {
  Environment env;

  Expression a(Atom(1.0));
  env.add_exp(Atom("one"), a);
  Expression b(Atom("hello"));
  env.add_exp(Atom("hi"), b);

  env.reset();
  REQUIRE(!env.is_known(Atom("one")));
  REQUIRE(!env.is_exp(Atom("one")));
  REQUIRE(env.get_exp(Atom("one")) == Expression());
  REQUIRE(!env.is_known(Atom("hi")));
  REQUIRE(!env.is_exp(Atom("hi")));
  REQUIRE(env.get_exp(Atom("hi")) == Expression());
}

TEST_CASE( "Test semeantic errors", "[environment]" ) {

  Environment env;

  {
    Expression exp(Atom("begin"));

    REQUIRE_THROWS_AS(exp.eval(env), SemanticError);
  }
}

TEST_CASE("Testing lists and list functions", "[environment]") {
  Environment env;
  std::vector<Expression> args;

  Procedure plist = env.get_proc(Atom("list"));
  double one = 1;
  double two = 2;
  double three = 3;
  args.emplace_back(one);
  args.emplace_back(two);
  args.emplace_back(three);

  Expression test_list; test_list.setHeadList();
  test_list.append(1); test_list.append(2); test_list.append(3);

  //REQUIRE(plist(args) == test_list);

}
