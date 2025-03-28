#include <glob.h>
#include <lexer.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>

lexer_t *lexer_create (void) {
    lexer_t *l = xmalloc (sizeof (lexer_t));
    l->root = NULL;
    l->error = NULL;
    return l;
}

static token_t *add_token (token_t *ptok, char *lexeme, enum token_type type) {
    token_t *t = xmalloc (sizeof (token_t));
    if (lexeme) {
        t->lexeme = xstrdup (lexeme);
    } else {
        t->lexeme = NULL;
    }
    t->type = type;
    t->next = NULL;
    if (ptok) {
        ptok->next = t;
    }
    return t;
}

static enum token_type get_type (char ch) {
    switch (ch) {
    case '|':
        return TOKEN_PIPE;
        break;
    case '&':
        return TOKEN_AMPERSAND;
        break;
    case '<':
        return TOKEN_LESSER;
        break;
    case '>':
        return TOKEN_GREATER;
        break;
    case ';':
        return TOKEN_SEMICOLON;
        break;
    default:
        return TOKEN_ARGUMENT;
        break;
    }
}

char *token_to_string (token_t *t) {
    switch (t->type) {
    case TOKEN_PIPE:
        return "|";
        break;
    case TOKEN_AMPERSAND:
        return "&";
        break;
    case TOKEN_LESSER:
        return "<";
        break;
    case TOKEN_GREATER:
        return ">";
        break;
    case TOKEN_SEMICOLON:
        return ";";
        break;
    case TOKEN_ARGUMENT:
        return t->lexeme;
        break;
    default:
        return NULL;
        break;
    }
}

static int advace_string (const char *from, char *to) {
    char type = *from;
    int i = 1;
    while (from[i] != '\0') {
        if (from[i] == type) {
            return i + 1;
        }
        to[i - 1] = from[i];
        i++;
    }

    return 0;
}

static void add_error (lexer_t *l, char *message, int character) {
    if (l->error) {
        free (l->error);
    }
    l->error = xmalloc (sizeof (lexical_error_t));
    l->error->msg = message;
    l->error->character = character;
}

static token_t *
add_lexeme (lexer_t *l, token_t *prev, char *lexeme, int *lexeme_i) {
    if (*lexeme_i <= 0) {
        return prev;
    }

    lexeme[*lexeme_i] = '\0';
    *lexeme_i = 0;

    glob_t globbuf;
    glob (lexeme, GLOB_TILDE | GLOB_NOCHECK | GLOB_NOSORT, NULL, &globbuf);

    for (size_t i = 0; i < globbuf.gl_pathc; i++) {
        prev = add_token (prev, globbuf.gl_pathv[i], TOKEN_ARGUMENT);
    }

    if (!l->root) {
        l->root = prev;
    }

    globfree (&globbuf);

    return prev;
}

token_t *lex (lexer_t *l, char *input) {
    int len = (int) strlen (input) + 1;
    char *lexeme = xmalloc (len);
    token_t *t = NULL;
    int add = 0;

    int i = 0;
    int lexeme_i = 0;
    while (i < len - 1) {
        switch (input[i]) {
        case ' ':
        case '\t':
        case '\n':
            t = add_lexeme (l, t, lexeme, &lexeme_i);
            break;
        case '&':
        case '>':
        case '<':
        case ';':
        case '|':
            t = add_lexeme (l, t, lexeme, &lexeme_i);
            t = add_token (t, NULL, get_type (input[i]));
            break;
        case '\'':
        case '\"':
            add = advace_string (input + i, lexeme + lexeme_i);
            if (add == 0) {
                add_error (l, "lexical error: unmatched quote", i);
                return NULL;
            }
            lexeme_i += add - 2;
            i += add;
            break;
        case '\\':
            if (input[i + 1] == '\0') {
                add_error (
                    l, "lexical error: escape sequence at end of input", i
                );
                return NULL;
            }
            lexeme[lexeme_i++] = input[i + 1];
            i++;
            break;
        default:
            lexeme[lexeme_i++] = input[i];
            break;
        }
        i++;
    }

    t = add_lexeme (l, t, lexeme, &lexeme_i);
    free (lexeme);
    return l->root;
}

void lexer_destroy (lexer_t *l) {
    token_t *t = l->root;
    while (t) {
        token_t *next = t->next;
        free (t->lexeme);
        free (t);
        t = next;
    }
    if (l->error) {
        free (l->error);
    }
    l->error = NULL;

    free (l);
    l = NULL;
}
