#include <stdlib.h>
#include <string.h>
#include "object.hpp"
#include "gc.hpp"
#include "eval.hpp"

/* Linked list of all allocated objects - new ones are appended to the end */
Object* objects = NULL;

/* Linked from eval.cpp */
extern Environment env;

/* Memory usage */
static int mem_used = 0;

/* Allocates memory, throws exception when there is none */
void* safe_malloc(size_t size)
{
	mem_used += size;
	void* ret = malloc(size);
	if (ret == NULL)
		throw "Not enough memory";

	memset(ret, 0, size);

	return ret;
}

/* Frees memory and performs the bookkeeping */
void safe_free(void* addr, size_t size)
{
	mem_used -= size;
	free(addr);
}

int get_mem_usage()
{
	return mem_used;
}

/* Mark all reachable objects (that is those in global variables) */
void mark()
{
	for (auto x : env.variables)
		mark_object(x.second);
}

/* Go through all objects, delete those unmarked */
void sweep()
{
	Object* prev = NULL;
	Object* obj = objects;
	while (obj != NULL) {
		if (obj->marked) {
			obj->marked = false;
			prev = obj;
			obj = obj->next;
		}
		else {
			Object* to_free = obj;
			obj = obj->next;
			if (prev != NULL)
				prev->next = obj;
			else
				objects = obj;

			safe_free(to_free, sizeof(Object));
		}
	}
}

/* Mark and sweep all objects */
void reclaim()
{
	mark();
	sweep();
}
