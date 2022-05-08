#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>

using std::vector;

/* Type of the object - note: OBJ_LAMBDA is not used during parsing, it is
   only during evaluation phase */
enum ObjType {
	OBJ_INT,
	OBJ_BOOL,
	OBJ_SYMBOL,
	OBJ_PAIR,
	OBJ_LAMBDA
};

/* Object is general container for everything interpreter uses */
struct Object {
	/* Type of the object */
	ObjType type;
	/* Required for GC */
	bool marked;
	Object* next;

	/* Every object only uses one of those fields, so we can put them in
	   union, saving a little bit of memory */
	union {
		/* type == OBJ_INT */
		long long ival;

		/* type == OBJ_BOOL */
		bool bval;

		/* type == OBJ_SYMBOL */
		struct {
			const char* name;
			int len;
		};

		/* type == OBJ_PAIR */
		struct {
			Object* car;	/* "first" */
			Object* cdr;	/* "second" */
		};

		/* type == OBJ_LAMBDA */
		struct {
			Object* arg_list;
			Object* body;
		};
	};
};

/* Mark object (and all possible children of it) as used */
void mark_object(Object* obj);

/* Mostly self-explanatory, except for eqv, which checks if objects are
   equivalent (so literals are the same, pairs have equivalent car and cdr,
   lambdas have equivalent bodies and arguments). */
void print(Object* obj);
bool is_list(Object* obj);
bool eqv(Object* a, Object* b);
vector<Object*> to_vector(Object* obj);
Object* from_vector(vector<Object*> vec);

/* Construct new object of type */
Object* new_int(long long val);
Object* new_bool(bool val);
Object* new_sym(const char* name, int len);
Object* new_lambda(vector<Object*> args, vector<Object*> body);

/* Pair-related code */
Object* cons(Object* car, Object* cdr);
Object* car(Object* cons);
Object* cdr(Object* cons);

#endif
