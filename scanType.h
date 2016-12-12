#ifndef SCANTYPEH
#define SCANTYPEH
typedef struct TokenData {
  int tokenClass;
  int lineNum;
  char* input;
  char const* tokenName;
  int value;
} TokenData;
#endif
