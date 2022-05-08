#include <stdlib.h>
#include <string.h>
#include "object.hpp"
#include "parser.hpp"
#include "lexer.hpp"
#include "gc.hpp"

Object* atom()
{
	Token t = lookahead();

	/* We shouldn't reach EOF when we expect atom */
	if (t.type == TOK_END)
		throw "Expected atom, got EOF";

	if (t.type == TOK_RPAREN)
		throw "Expected atom, got ')'";

	if (t.type == TOK_LPAREN)
		return list();

	/* We can read in the token */
	t = next();
	if (t.type == TOK_INT)
		return new_int(strtoll(t.val, NULL, 10));

	if (t.type == TOK_BOOL)
		return new_bool(strncmp(t.val, "#T", 2) == 0 ? true : false);

	if (t.type == TOK_SYM)
		return new_sym(t.val, t.len);

	return NULL;
}

Object* list()
{
	Token t = lookahead();

	if (t.type == TOK_LPAREN) {
		vector<Object*> ls;
		t = next();
		while (lookahead().type != TOK_RPAREN) {
			if (lookahead().type == TOK_END)
				throw "Expected ')' or atom, got EOF";
			ls.push_back(atom());
		}

		/* Skip matching ) */
		t = next();
		return from_vector(ls);
	}

	return atom();
}
