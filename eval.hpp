#ifndef EVAL_HPP
#define EVAL_HPP

#include <string>
#include <unordered_map>
#include "object.hpp"

using std::string;
using std::unordered_map;

/* Evaluator evironment - global variables and locals stack */
struct Environment {
	/* Global variables */
	unordered_map<string, Object*> variables;
	/* Locals stack (function arguments) */
	vector<Object*> bind_stack;
	/* Each function only will use its own arguments, so we need to
	   remember how many it has */
	vector<int> arity_stack;
};

/* Evaluate S-expr in current environment */
Object* eval(Object* obj);

#endif
