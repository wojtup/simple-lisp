#include <stdio.h>
#include <string.h>
#include "object.hpp"
#include "gc.hpp"

/* Mark object and its children as used */
void mark_object(Object* obj)
{
	if (obj == NULL)
		return;

	if (obj->type == OBJ_INT or obj->type == OBJ_BOOL or
	    obj->type == OBJ_SYMBOL) {
		obj->marked = true;
		return;
	}

	if (obj->type == OBJ_PAIR) {
		mark_object(obj->car);
		mark_object(obj->cdr);
		obj->marked = true;
		return;
	}

	if (obj->type == OBJ_LAMBDA) {
		mark_object(obj->arg_list);
		mark_object(obj->body);
		obj->marked = true;
	}
}

/* Check recursively if a pair is a list */
bool is_list(Object* obj)
{
	if (obj == NULL)
		return true;

	if (obj->type == OBJ_PAIR) {
		if (obj->cdr == NULL)
			return true;
		return is_list(obj->cdr);
	}

	return false;
}

/* Check for equivalence */
bool eqv(Object* a, Object* b)
{
	if (a == b)
		return true;

	if (a == NULL or b == NULL)
		return false;

	if (a->type != b->type)
		return false;

	if (a->type == OBJ_INT)
		return (a->ival == b->ival);

	if (a->type == OBJ_BOOL)
		return (a->bval == b->bval);

	if (a->type == OBJ_SYMBOL)
		return (strncmp(a->name, b->name, a->len) == 0);

	if (a->type == OBJ_PAIR)
		return (eqv(a->car, b->car) and eqv(a->cdr, b->cdr));

	if (a->type == OBJ_LAMBDA)
		return (eqv(a->arg_list, b->arg_list) and
			eqv(a->body, b->body));

	return false;
}

/* Form list object from vector */
Object* from_vector(vector<Object*> vec)
{
	int size = vec.size();
	if (size == 0)
		return NULL;

	Object* ret = cons(vec[size - 1], NULL);
	for (int i = size - 2; i >= 0; i--)
		ret = cons(vec[i], ret);

	return ret;
}

/* And the other way around too */
vector<Object*> to_vector(Object* obj)
{
	vector<Object*> ret;

	while (obj != NULL) {
		ret.push_back(obj->car);
		obj = obj->cdr;
	}

	return ret;
}

/* Print an object */
void print(Object* obj)
{
	if (obj == NULL)
		printf("()");

	else if (obj->type == OBJ_INT)
		printf("%lld", obj->ival);

	else if (obj->type == OBJ_BOOL)
		printf("%s", (obj->bval) ? "#T" : "#F");

	else if (obj->type == OBJ_SYMBOL)
		printf("%.*s", obj->len, obj->name);

	else if (obj->type == OBJ_LAMBDA)
		printf("#<lambda>");

	else if (is_list(obj)) {
		vector<Object*> ls = to_vector(obj);
		printf("(");
		print(ls[0]);
		for (size_t i = 1; i < ls.size(); i++) {
			printf(" ");
			print(ls[i]);
		}
		printf(")");
	}

	else {
		printf("(");
		print(obj->car);
		printf(" . ");
		print(obj->cdr);
		printf(")");
	}
}

/* Linked from gc.cpp */
extern Object* objects;

Object* new_int(long long val)
{
	Object* ret = (Object*) safe_malloc(sizeof(Object));
	ret->type = OBJ_INT;
	ret->ival = val;
	ret->next = objects;
	objects = ret;
	return ret;
}

Object* new_bool(bool val)
{
	Object* ret = (Object*) safe_malloc(sizeof(Object));
	ret->type = OBJ_BOOL;
	ret->bval = val;
	ret->next = objects;
	objects = ret;
	return ret;
}

Object* new_sym(const char* name, int len)
{
	Object* ret = (Object*) safe_malloc(sizeof(Object));
	ret->type = OBJ_SYMBOL;
	ret->name = name;
	ret->len = len;
	ret->next = objects;
	objects = ret;
	return ret;
}

Object* new_lambda(vector<Object*> args, vector<Object*> body)
{
	Object* ret = (Object*) safe_malloc(sizeof(Object));
	ret->type = OBJ_LAMBDA;
	ret->arg_list = from_vector(args);
	ret->body = from_vector(body);
	ret->next = objects;
	objects = ret;
	return ret;
}

Object* cons(Object* car, Object* cdr)
{
	Object* ret = (Object*) safe_malloc(sizeof(Object));
	ret->type = OBJ_PAIR;
	ret->car = car;
	ret->cdr = cdr;
	ret->next = objects;
	objects = ret;
	return ret;
}

Object* car(Object* cons)
{
	if (cons == NULL)
		throw "CAR called on NULL";
	if (cons->type != OBJ_PAIR)
		throw "CAR called on atomic value";

	return cons->car;
}

Object* cdr(Object* cons)
{
	if (cons == NULL)
		throw "CDR called on NULL";
	if (cons->type != OBJ_PAIR)
		throw "CDR called on atomic value";

	return cons->cdr;
}
