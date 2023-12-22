#include <ast.h>
#include <stdlib.h>

astree *newAst(void) {
  astree *node = malloc(sizeof(astree));
  node->type = 0;
  node->left = NULL;
  node->right = NULL;
  return node;
}

void setData(astree *node, char *data) { node->data = data; }
void setType(astree *node, enum astype type) { node->type = type; }
void setChilds(astree *node, astree *l, astree *r) {
  node->left = l;
  node->right = r;
}
