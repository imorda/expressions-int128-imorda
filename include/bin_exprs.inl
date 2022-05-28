#if !defined(BIN_EXPR)
#error You need to define BIN_EXPR macro
#else

BIN_EXPR(Divide, /)
BIN_EXPR(Multiply, *)
BIN_EXPR(Add, +)
BIN_EXPR(Subtract, -)

#undef BIN_EXPR

#endif
