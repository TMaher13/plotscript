/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty)
list of expressions called the tail.
 */
class Expression {
public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression();

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  Expression(const Atom & a);

  //Expression(const std::vector<Atom> & a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression & a);

  // Constructor for list
  Expression(const std::vector<Expression> & a);

  /// deep-copy assign an expression  (recursive)
  Expression & operator=(const Expression & a);

  /// return a reference to the head Atom
  Atom & head();

  /// return a const-reference to the head Atom
  const Atom & head() const;

  /// Sets head of Expression from Atom
  void setHead(const Atom & a);

  // Set the head of the expression to list type
  void setHeadList();

  // set the head of the expression to lambda type
  void setHeadLambda();

  /// append Atom to tail of the expression
  void append(const Atom & a);

  /// append Expression to tail of the expression
  void append(const Expression& a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression * tail();

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  // Returns the tail of the expression
  std::vector<Expression> getTail() const;

  // Clear the tail, used in discrete-plot
  void clearTail() {
    m_tail.clear();
  }

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convenience member to determineif head atom is a number
  bool isHeadComplex() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// convienience member to determine if head atom is a list
  bool isHeadList() const noexcept;

  /// convienience member to determine if head atom is a lambda
  bool isHeadLambda() const noexcept {return m_head.isLambda();};

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment & env);

  // Helper function to add property to list
  void add_property(const std::string & key, Expression & value);

  Expression get_property(const std::string & key);

  /// Method for creating a copy of the environment for lambda functions
  //Expression shadow_copy(Atom & op, std::vector<Expression> & args, Environment & env);

  Expression apply(Atom & op, std::vector<Expression> & args, Environment & env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression & exp) const noexcept;

  std::map<std::string, Expression> property_list;

private:

  // the head of the expression
  Atom m_head;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;

  bool isList;

  // internal helper methods
  Expression handle_lookup(const Atom & head, const Environment & env);
  Expression handle_define(Environment & env);
  Expression handle_begin(Environment & env);

  // Implementation special form for handling lambda functions
  Expression handle_lambda(Environment & env);
  Expression handle_apply(Environment & env);
  Expression handle_map(Environment & env);

  // Implementation of special forms for setting/getting properties for an expression
  Expression handle_set_prop(Environment & env);
  Expression handle_get_prop(Environment & env);

  // Implemented for plots in the GUI
  Expression handle_discrete_plot(Environment & env);
  Expression handle_continuous_plot(Environment & env);

};

/// Render expression to output stream
std::ostream & operator<<(std::ostream & out, const Expression & exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression & left, const Expression & right) noexcept;

#endif
