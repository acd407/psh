#include <ast.h>
#include <lexer.h>
#include <parser.h>
#include <stdlib.h>

// Grammar(for now):
// Full Commnad ::= Pipe ';' Full Command | Pipe ';' | Pipe '&' | Pipe '&' Full
// Command | Pipe
// Pipe ::= Command | Command '|' Command
// Command ::= Simple Command '>' Filename | Simple Command '<' Filename |
// Simple Command
// Simple Command ::= Executable | Executable Arguments

astree *FULL_COMMAND(void);
astree *FULL_COMMAND1(void);
astree *FULL_COMMAND2(void);
astree *FULL_COMMAND3(void);
astree *PIPE1(void);
astree *PIPE2(void);
astree *COMMAND1(void);
astree *COMMAND2(void);
astree *COMMAND3(void);
astree *SIMPLE_COMMAND1(void);
astree *SIMPLE_COMMAND2(void);

token *curtok = NULL;
