enum tokenType {
  GENERAL = -1,
  PIPE = '|',
  AMPERSAND = '&',
  QOUTE = '\'',
  DQUOTE = '\"',
  SEMICOLON = ';',
  ESCAPESEQUENCE = '\\',
  GREATER = '>',
  LESSER = '<',
  NULL_C = 0,
  NORMAL = -1,
};

typedef struct token {
  char *text;
  enum tokenType type;
  struct token *next;
} token;

typedef struct lexer {
  token *root;
} lexer;

int createLexer(char *input, lexer *l);
void destroyLexer(lexer *l);
