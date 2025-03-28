#include <ast.h>
#include <stdio.h>
#include <stdlib.h>

ast_t *ast_create_node (void) {
    ast_t *node = malloc (sizeof (ast_t));
    node->left = NULL;
    node->right = NULL;
    node->token = NULL;
    return node;
}

void ast_destroy_node (ast_t *node) {
    if (node == NULL) {
        return;
    }
    ast_destroy_node (node->left);
    ast_destroy_node (node->right);
    node->left = NULL;
    node->right = NULL;
    node->token = NULL;

    free (node);
    node = NULL;
}

static const char *ast_type_to_string (enum astype type) {
    switch (type) {
    case AST_COMMAND_TAIL:
        return "AST_COMMAND_TAIL";
        break;
    case AST_BACKGROUND:
        return "AST_BACKGROUND";
        break;
    case AST_REDIRECT_IN:
        return "AST_REDIRECT_IN";
        break;
    case AST_REDIRECT_OUT:
        return "AST_REDIRECT_OUT";
        break;
    case AST_PIPE:
        return "AST_PIPE";
        break;
    default:
        return "UNKNOWN";
        break;
    }
}

void ast_print (ast_t *node, int level) {
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < level; i++) {
        printf ("  ");
    }

    if (node->token == NULL) {
        printf ("%s\n", ast_type_to_string (node->type));
    } else {
        printf (
            "%s(%s)\n", node->token->lexeme, ast_type_to_string (node->type)
        );
    }

    ast_print (node->right, level + 1);
    ast_print (node->left, level + 1);
}
