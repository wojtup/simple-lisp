#include <climits>
#include <stdio.h>
#include <string.h>
#include "eval.hpp"

Environment env;

/* Just for convenience */
enum {
	SYM_MUL = 0,
	SYM_ADD,
	SYM_MINUS,
	SYM_DIVIDE,
	SYM_LESS,
	SYM_EQUAL,
	SYM_GREATER,
	SYM_CAR,
	SYM_CDR,
	SYM_CONS,
	SYM_DEFINE,
	SYM_IF,
	SYM_LAMBDA,
	SYM_LIST,
	SYM_NULL,
	SYM_PRINT,
	SYM_READ,
	NUMBER_OF_BUILTINS
};

/* List of builtins' strings - every string's index matches enum above
   (eg. "-" has index SYM_MINUS) */
const char* builtin[] = {
	"*", "+", "-", "/", "<", "=", ">", "CAR", "CDR", "CONS", "DEFINE",
	"IF", "LAMBDA", "LIST", "NULL?", "PRINT", "READ"
};

/* Evaluate special form (ie. builtin) */
Object* eval_form(Object* sym, vector<Object*> args)
{
	/* Find what symbol are we calling */
	int index = -1;
	for (int i = SYM_MUL; i < NUMBER_OF_BUILTINS; i++) {
		Object* temp = new_sym(builtin[i], strlen(builtin[i]));
		if (eqv(sym, temp)) {
			index = i;
			break;
		}
	}

	/* Evaluate the symbol, evaluating arguments */
	switch (index) {
	case SYM_MUL: {
		if (args.size() == 0)
			throw "No arguments to multiply";
		long long res = 1;
		for (Object* x : args) {
			x = eval(x);
			if (x->type != OBJ_INT)
				throw "Multiplying non-numeric values";
			res *= x->ival;
		}
		return new_int(res);
	}
	case SYM_ADD: {
		if (args.size() == 0)
			throw "No arguments to add";
		long long res = 0;
		for (Object* x : args) {
			x = eval(x);
			if (x->type != OBJ_INT)
				throw "Adding non-numeric values";
			res += x->ival;
		}
		return new_int(res);
	}
	case SYM_MINUS: {
		if (args.size() == 0)
			throw "No arguments to subtract";
		long long res = eval(args[0])->ival;
		for (size_t i = 1; i < args.size(); i++) {
			Object* x = args[i];
			x = eval(x);
			if (x->type != OBJ_INT)
				throw "Subtracting non-numeric values";
			res -= x->ival;
		}
		return new_int(res);
	}
	case SYM_DIVIDE: {
		if (args.size() == 0)
			throw "No arguments to divide";
		long long res = eval(args[0])->ival;
		for (size_t i = 1; i < args.size(); i++) {
			Object* x = args[i];
			x = eval(x);
			if (x->type != OBJ_INT)
				throw "Dividing non-numeric values";
			if (x->ival == 0)
				throw "Dividing by zero";
			res /= x->ival;
		}
		return new_int(res);
	}
	case SYM_LESS: {
		if (args.size() < 2)
			throw "Not enough arguments to compare";
		bool res = false;
		long long compare = LLONG_MIN;
		for (Object* x : args) {
			x = eval(x);
			if (x->type != OBJ_INT)
				throw "Comparing non-numeric values";
			res = (compare < x->ival);
			compare = x->ival;
			if (!res)
				break;
		}
		return new_bool(res);
	}
	case SYM_EQUAL: {
		if (args.size() < 2)
			throw "Not enough arguments to compare";
		bool res = false;
		Object* first = eval(args[0]);
		for (Object* x : args) {
			x = eval(x);
			res = eqv(first, x);
			if (!res)
				break;
		}
		return new_bool(res);
	}
	case SYM_GREATER: {
		if (args.size() < 2)
			throw "Not enough arguments to compare";
		bool res = false;
		long long compare = LLONG_MAX;
		for (Object* x : args) {
			x = eval(x);
			if (x->type != OBJ_INT)
				throw "Comparing non-numeric values";
			res = (compare > x->ival);
			compare = x->ival;
			if (!res)
				break;
		}
		return new_bool(res);
	}
	case SYM_CAR: {
		if (args.size() != 1)
			throw "Wrong arity for CAR";
		return car(eval(args[0]));
	}
	case SYM_CDR: {
		if (args.size() != 1)
			throw "Wrong arity for CDR";
		return cdr(eval(args[0]));
	}
	case SYM_CONS: {
		if (args.size() != 2)
			throw "Wrong arity for CONS";
		return cons(eval(args[0]), eval(args[1]));
	}
	case SYM_DEFINE: {
		if (args.size() != 2)
			throw "Wrong arity for DEFINE";
		if (args[0]->type != OBJ_SYMBOL)
			throw "Trying to define non-symbol";
		Object* x = eval(args[1]);
		env.variables[string(args[0]->name, args[0]->len)] = x;
		return x;
	}
	case SYM_IF: {
		if (args.size() != 3)
			throw "Wrong arity for IF";
		Object* cond = eval(args[0]);
		if (cond->type != OBJ_BOOL)
			throw "No condition for IF";

		if (cond->bval)
			return eval(args[1]);
		else
			return eval(args[2]);
	}
	case SYM_LAMBDA: {
		if (args.size() < 2)
			throw "No body or arguments for LAMBDA";
		Object* ls_args = from_vector(args);
		vector<Object*> arg_list = to_vector(ls_args->car);
		vector<Object*> body = to_vector(ls_args->cdr);
		return new_lambda(arg_list, body);
	}
	case SYM_LIST: {
		for (auto& x : args)
			x = eval(x);
		return from_vector(args);
	}
	case SYM_NULL: {
		if (args.size() != 1)
			throw "Wrong arity for NULL?";

		if (eval(args[0]) == NULL)
			return new_bool(true);
		return new_bool(false);
	}
	case SYM_PRINT: {
		if (args.size() != 1)
			throw "Wrong arity for PRINT";
		Object* val = eval(args[0]);
		print(val);
		printf("\n");
		return val;
	}
	case SYM_READ: {
		if (args.size() != 0)
			throw "READ takes no arguments";
		long long val;
		scanf("%lld", &val);
		return new_int(val);
	}
	default:
		throw "wtf???";
	}

	return NULL;
}

/* Invokes a lambda object (func), given arguments args */
Object* invoke(Object* func, vector<Object*> args)
{
	vector<Object*> arg_list = to_vector(func->arg_list);
	if (arg_list.size() != args.size())
		throw "Lambda procedure called with wrong arity";

	/* We evaluate arguments while binding, so we need to put binds on
	   temporary stack - otherwise they will mess up evaluation of other
	   arguments (this happens especially in recursive functions) */
	vector<Object*> temp_binds;
	for (size_t i = 0; i < args.size(); i++) {
		Object* bind = cons(arg_list[i], eval(args[i]));
		temp_binds.push_back(bind);
	}

	/* Put binds on stack */
	env.arity_stack.push_back(args.size());
	for (auto b : temp_binds)
		env.bind_stack.push_back(b);

	/* Evaluate the body */
	vector<Object*> body = to_vector(func->body);
	Object* ret;
	for (Object* expr : body)
		ret = eval(expr);

	/* Clear the stack */
	env.arity_stack.pop_back();
	for (size_t i = 0; i < args.size(); i++)
		env.bind_stack.pop_back();

	return ret;
}

/* Main evaluator function */
Object* eval(Object* obj)
{
	if (obj == NULL)
		return NULL;

	if (obj->type == OBJ_INT or obj->type == OBJ_BOOL or
	    obj->type == OBJ_LAMBDA)
		return obj;

	/* When evaluating list, evaluate its first element and call it */
	if (obj->type == OBJ_PAIR) {
		Object* func = eval(obj->car);
		if (func == NULL)
			return NULL;

		if (func->type == OBJ_SYMBOL)
			return eval_form(func, to_vector(obj->cdr));

		if (func->type == OBJ_LAMBDA)
			return invoke(func, to_vector(obj->cdr));

		else
			throw "Trying to invoke non-symbol and non-lambda";
	}

	/* By now we can only evaluate symbol, so check current binds too */
	vector<Object*> binds;
	if (env.arity_stack.size() != 0) {
		int bind_size = env.bind_stack.size();
		int arity = env.arity_stack[env.arity_stack.size() - 1];

		for (int i = bind_size - arity; i < bind_size; i++)
			binds.push_back(env.bind_stack[i]);
	}

	while (obj->type == OBJ_SYMBOL) {
		/* Check if it is a builtin symbol */
		for (int i = SYM_MUL; i < NUMBER_OF_BUILTINS; i++) {
			Object* temp = new_sym(builtin[i], strlen(builtin[i]));
			if (eqv(obj, temp))
				return obj;
		}

		/* Or if it is in bind list (binds overshadow globals) */
		for (Object* x : binds)
			if (eqv(obj, x->car))
				return x->cdr;

		/* Nothing found, look in globals */
		auto it = env.variables.find(string(obj->name, obj->len));
		if (it == env.variables.end())
			throw "Accessing undefined variable";

		obj = it->second;
	}

	return obj;
}
