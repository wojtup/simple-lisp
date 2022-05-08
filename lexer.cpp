#include <ctype.h>
#include <string.h>
#include "lexer.hpp"

/* Internal lexer state */
static int pos;
static const char* code;

/* Needed for lookahead */
static bool wait = false;
static Token cache;

void init_lexer(char* c)
{
	pos = 0;
	/* Letters' case doesn't matter, so set it to upper */
	for (size_t i = 0; i < strlen(c); i++) {
		c[i] = toupper(c[i]);
	}
	code = c;
}

/* This function doesn't convert anything yet, simply separates tokens */
Token next()
{
	if (wait) {
		wait = false;
		return cache;
	}

	Token ret;

	next:
	switch (code[pos]) {
	case '\0':
		ret.type = TOK_END;
		ret.val = NULL;
		ret.len = 0;
		break;
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		pos++;
		goto next;
	case '(':
		ret.type = TOK_LPAREN;
		ret.val = code + pos;
		ret.len = 1;
		pos++;
		break;
	case ')':
		ret.type = TOK_RPAREN;
		ret.val = code + pos;
		ret.len = 1;
		pos++;
		break;
	case '#':
		if (code[pos + 1] != 'T' and code[pos + 1] != 'F')
			throw "Invalid boolean";

		ret.type = TOK_BOOL;
		ret.val = code + pos;
		ret.len = 2;
		pos += 2;
		break;
	default:
		/* If it starts with digit it is a number */
		if (isdigit(code[pos])) {
			int len = 0;
			while (isdigit(code[pos + len])) {
				len++;
			}

			ret.type = TOK_INT;
			ret.val = code + pos;
			ret.len = len;
			pos += len;
			break;
		}

		/* Else it is a symbol */
		else {
			int len = 0;
			/* Both ( and ) break the symbol */
			char c = code[pos + len];
			while (c != ')' and c != '(') {
				if (isalnum(c) or ispunct(c)) {
					len++;
					c = code[pos + len];
				}
				else
					break;
			}

			ret.type = TOK_SYM;
			ret.val = code + pos;
			ret.len = len;
			pos += len;
			break;
		}
	}

	return ret;
}

Token lookahead()
{
	cache = next();
	wait = true;

	return cache;
}
