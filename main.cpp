#include <stdio.h>
#include <string.h>
#include "object.hpp"
#include "eval.hpp"
#include "gc.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#define GC_DEBUG 1

char* read_file(const char* filename)
{
	/* Open file */
	FILE* f = fopen(filename, "rb");
	if (f == NULL)
		throw "Couldn't open source file";

	/* Check its length */
	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	rewind(f);

	/* Allocate memory for source code (add one more byte for NUL) */
	char* source = (char*) safe_malloc(len + 1);

	/* Read in from file */
	fread(source, sizeof(char), len, f);

	/* Append NUL */
	source[len] = '\0';
	fclose(f);
	return source;
}

int main(int argc, char** argv)
{
	/* All errors are signaled using exceptions - thus "try" */
	try {
		char* code;

		/* Read in source code */
		if (argc == 1) {
			printf("Enter source filename: ");
			char fname[128];
			scanf("%s", fname);
			code = read_file(fname);
		}
		else {
			code = read_file(argv[1]);
		}

		/* Set lexer to use it */
		init_lexer(code);

		/* While we have another S-expr, parse and evaluate it */
		while (lookahead().type != TOK_END) {
			Object* x = eval(list());
			printf("=> ");
			print(x);
			printf("\n");
			int before = get_mem_usage();
			reclaim();
			int after = get_mem_usage();

#if GC_DEBUG
			printf("GC: Reclaimed %d bytes.\n", before - after);
#endif
		}

		safe_free(code, strlen(code));
	}
	catch (const char* msg) {
		printf("Error: %s\n", msg);
	}

	return 0;
}
