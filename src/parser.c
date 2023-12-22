#include <ast.h>
#include <lexer.h>
#include <parser.h>
#include <stdlib.h>

// Grammar(for now):
// Full Commnad ::= Pipe ';' Full Command | Pipe ';' | Pipe '&' | Pipe '&' Full
// Command | Pipe
// Pipe ::= Command | Command '|' Command
// Command ::= Simple Command '>' Filename | Simple Command '<' Filename
// Simple Command ::= Executable | Executable Arguments

token *curtok = NULL;

void parse(lexer *l) { (void)l; }
