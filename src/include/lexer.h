enum tokenType {
  GENERAL = -1,
  PIPE = '|',
  AMPERSAND = '&',
  QUOTE = '\'',
  DQUOTE = '\"',
  SEMICOLON = ';',
  ESCAPESEQUENCE = '\\',
  NEWLINE = '\n',
  GREATER = '>',
  LESSER = '<',
  WHITESPACE = ' ',
  NULL_C = 0,
  NORMAL = -1,
};

enum state {
  IN_GENERAL,
  IN_QUOTE,
  IN_DQUOTE,
  IN_ESCAPESEQUENCE,
};
typedef struct token {
  char *text;
  int size;
  enum tokenType type;
  struct token *next;
} token;

typedef struct lexer {
  token *root;
} lexer;

int createLexer(char *input, lexer *l);
void destroyLexer(lexer *l);
