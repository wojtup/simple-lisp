# Lisp Interpreter
Most basic [LISP](http://www.shido.info/lisp/idx_scm_e.html) interpreter (gc included).
It tries to be like Scheme (thus the link), but even more useless.
Code (and especially GC) is heavily inspired by ["Crafting Interpreters"](http://craftinginterpreters.com/) by Robert Nystorm.

# Symbols built in:
- math operators (*, +, -, /, <, >)
- equivalence operator (=)
- print and read
- car, cdr, cons (obviously)
- define
- if
- lambda
- list
- null?

# Some other notes:
- Here symbols are treated like variables in C-like languages: they are evaluated to their values, when they have none, it is an error (besides the builtins, so for example PRINT evaluates to itself),
- Only numerical values can be read using READ,
- We don't do lexical scoping in here (only currently defined globals and arguments are visible, so one can say it is dynamic scoping),
- Closures won't work (no warning or anything, probably will just throw random errors),
- You can make the GC shut up by setting GC_DEBUG to 0 in main.cpp
- There are probably hundreds unique ways in which you can crash it, but it will work if you show some good will.
