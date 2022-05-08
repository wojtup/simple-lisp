#ifndef LEXER_HPP
#define LEXER_HPP

/* Type of the token */
enum TokType {
	TOK_END,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_BOOL,
	TOK_SYM,
	TOK_INT
};

/* Token itself - for now they are stored as strings */
struct Token {
	TokType type;
	/* Tokens point to original source code - no need for double storage */
	const char* val;
	int len;
};

/* Initialize lexer to use given code */
void init_lexer(char* code);

/* Fetch next token or only look at it, without consuming */
Token next();
Token lookahead();

#endif
