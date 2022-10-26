#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  nil->key = 0;
  nil->parent = NULL;
  nil->left = NULL;
  nil->right = NULL;

  p->nil = nil;
  p->root = p->nil;
  
  return p;
}

void delete_sub_node(node_t *n, node_t *nil) {
  if(n==nil) return;
  delete_sub_node(n->left, nil);
  delete_sub_node(n->right, nil);
  free(n);
  n=NULL;
}

void delete_rbtree(rbtree *t) {
  delete_sub_node(t->root, t->nil);

  free(t->nil);
  t->nil = NULL;

  free(t);
  t = NULL;
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left;
  if(y->left != t->nil) {
    y->left->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil) {
    t->root = y;
  } else if(x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;
  if(y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil) {
    t->root = y;
  } else if(x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->right = x;
  x->parent = y;
}

void rbtree_insert_fixup(rbtree *t, node_t *n) {
  while(n->parent->color == RBTREE_RED) {
    if(n->parent == n->parent->parent->left) {
      node_t *uncle = n->parent->parent->right;
      if(uncle->color == RBTREE_RED) {
        n->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        n = n->parent->parent;
      } else {
        if(n == n->parent->right) {
          n = n->parent;
          left_rotate(t, n);
        }
        n->parent->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        right_rotate(t, n->parent->parent);
      }
    } else {
      node_t *uncle = n->parent->parent->left;
      if(uncle->color == RBTREE_RED) {
        n->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        n = n->parent->parent;
      } else {
        if(n == n->parent->left) {
          n = n->parent;
          right_rotate(t, n);
        }
        n->parent->color = RBTREE_BLACK;
        n->parent->parent->color = RBTREE_RED;
        left_rotate(t, n->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *newNode = (node_t *)calloc(1, sizeof(node_t));
  newNode->key = key;

  node_t *parentNode = t->nil;
  node_t *tmp = t->root;
  while(tmp != t->nil) {
    parentNode = tmp;
    if(newNode->key < tmp->key) {
      tmp = tmp->left;
    } else {
      tmp = tmp->right;
    }
  }
  newNode->parent = parentNode;

  /**
   * 3가지 경우
   * 
   * 1. parentNode가 nil인 경우 (newNode가 root)
   * 2. newNode가 parentNode의 왼쪽 자식 노드인 경우
   * 3. newNode가 parentNode의 오른쪽 자식 노드인 경우
   */
  if(parentNode == t->nil) {
    t->root = newNode;
  } else if(newNode->key < parentNode->key) {
    parentNode->left = newNode;
  } else {
    parentNode->right = newNode;
  }

  newNode->right = t->nil;
  newNode->left = t->nil;
  newNode->color = RBTREE_RED;

  rbtree_insert_fixup(t, newNode);

  return newNode;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cur = t->root;
  while(cur != t->nil) {
    if(key < cur->key) {
      cur = cur->left;
    } else if(key > cur->key) {
      cur = cur->right;
    } else {
      break;
    }
  }
  
  if(cur == t->nil) {
    return NULL;
  } else {
    return cur;
  }
}

node_t *node_min(node_t *n, node_t *nil) {
  node_t *min = n;
  while(n != nil) {
    min = n;
    n = n->left;
  }
  return min;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *min = node_min(t->root, t->nil);
  return min;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *max = t->root;
  node_t *cur = t->root;
  while(cur != t->nil) {
    max = cur;
    cur = cur->right;
  }
  return max;
}

void trans_plant(rbtree *t, node_t *u, node_t *v) {
  if(u->parent == t->nil) {
    t->root = v;
  } else if(u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *cursor) {
  while(cursor != t->root && cursor->color == RBTREE_BLACK) {
    if(cursor == cursor->parent->left) {
      node_t *sibling_node = cursor->parent->right;
      if(sibling_node->color == RBTREE_RED) {
        sibling_node->color = RBTREE_BLACK;
        cursor->parent->color = RBTREE_RED;
        left_rotate(t, cursor->parent);
        sibling_node = cursor->parent->right;
      } 
      if(sibling_node->left->color == RBTREE_BLACK && sibling_node->right->color == RBTREE_BLACK) {
        sibling_node->color = RBTREE_RED;
        cursor = cursor->parent;
      } else {
        if(sibling_node->right->color == RBTREE_BLACK) {
          sibling_node->left->color = RBTREE_BLACK;
          sibling_node->color = RBTREE_RED;
          right_rotate(t, sibling_node);
          sibling_node = cursor->parent->right;
        }
        sibling_node->color = cursor->parent->color;
        cursor->parent->color = RBTREE_BLACK;
        sibling_node->right->color = RBTREE_BLACK;
        left_rotate(t, cursor->parent);
        cursor = t->root;
      }
    } else {
      node_t *sibling_node = cursor->parent->left;
      if(sibling_node->color == RBTREE_RED) {
        sibling_node->color = RBTREE_BLACK;
        cursor->parent->color = RBTREE_RED;
        right_rotate(t, cursor->parent);
        sibling_node = cursor->parent->left;
      } 
      if(sibling_node->left->color == RBTREE_BLACK && sibling_node->right->color == RBTREE_BLACK) {
        sibling_node->color = RBTREE_RED;
        cursor = cursor->parent;
      } else {
        if(sibling_node->left->color == RBTREE_BLACK) {
          sibling_node->right->color = RBTREE_BLACK;
          sibling_node->color = RBTREE_RED;
          left_rotate(t, sibling_node);
          sibling_node = cursor->parent->left;
        }
        sibling_node->color = cursor->parent->color;
        cursor->parent->color = RBTREE_BLACK;
        sibling_node->left->color = RBTREE_BLACK;
        right_rotate(t, cursor->parent);
        cursor = t->root;
      }
    }
  }
  cursor->color = RBTREE_BLACK;
}

void node_free(node_t *n) {
  free(n);
  n = NULL;
}

int rbtree_erase(rbtree *t, node_t *target) {
  // TODO: implement erase
  node_t *y = target;
  color_t y_color = y->color;

  node_t *tmp;
  if(target->left == t->nil) {
    tmp = target->right;
    trans_plant(t, target, target->right);
  } else if(target->right == t->nil) {
    tmp = target->left;
    trans_plant(t, target, target->left);
  } else {
    y = node_min(target->right, t->nil);
    y_color = y->color;
    tmp = y->right;
    if(y->parent == target) {
      tmp->parent = y;
    } else {
      trans_plant(t, y, y->right);
      y->right = target->right;
      y->right->parent = y;
    }
    trans_plant(t, target, y);
    y->left = target->left;
    y->left->parent = y;
    y->color = target->color;
  }

  free(target);
  target = NULL;

  if(y_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, tmp);
  }
  t->nil->parent = NULL;
  t->nil->right = NULL;
  t->nil->left = NULL;
  t->nil->color = RBTREE_BLACK;
  
  return 0;
}

void get_array(node_t *n, node_t *nil, key_t *arr, int *index) {
  if(n == nil) return;
  get_array(n->left, nil, arr, index);
  arr[(*index)++] = n->key;
  get_array(n->right, nil, arr, index);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int *index = calloc(1, sizeof(int));
  get_array(t->root, t->nil, arr, index);
  free(index);
  return 0;
}
