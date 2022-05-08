#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>

using std::vector;

/* Parse one atom */
Object* atom();

/* Parse list (so it can parse entire S-expr) */
Object* list();

#endif
