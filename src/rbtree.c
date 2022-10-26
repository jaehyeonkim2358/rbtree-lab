#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * RB tree 구조체 생성 함수
*/
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

/**
 * node N을 root로 갖고 NIL을 sentinel로 하는 RB tree의 
 * NIL을 제외한 모든 node에 할당된 메모리를 반환하는 함수.
*/
void delete_sub_node(node_t *n, node_t *nil) {
  if(n==nil) return;
  delete_sub_node(n->left, nil);
  delete_sub_node(n->right, nil);
  free(n);
  n=NULL;
}

/**
 * RB tree 구조체가 차지했던 메모리를 반환하는 함수.
 * 
 * T가 사용한 모든 메모리를 반환한다.
*/
void delete_rbtree(rbtree *t) {
  delete_sub_node(t->root, t->nil);

  free(t->nil);
  t->nil = NULL;

  free(t);
  t = NULL;
}

/**
 * T의 node X와 X의 right child를 rotate 하는 함수
 * 이진 탐색 트리의 특성은 유지된다.
*/
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

/**
 * T의 node X와 X의 left child를 rotate 하는 함수
 * 이진 탐색 트리의 특성은 유지된다.
*/
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

/**
 * T에 node를 insert한 후 rbtree 특성을 복구하는 함수.
*/
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

/**
 * KEY를 갖는 node를 생성하여 T에 insert하는 함수.
*/
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

  if(parentNode == t->nil) {                      // 1. parentNode가 nil인 경우 (newNode가 root)
    t->root = newNode;
  } else if(newNode->key < parentNode->key) {     // 2. newNode가 parentNode의 왼쪽 자식 노드인 경우
    parentNode->left = newNode;
  } else {                                        // 3. newNode가 parentNode의 오른쪽 자식 노드인 경우
    parentNode->right = newNode;
  }

  newNode->right = t->nil;
  newNode->left = t->nil;
  newNode->color = RBTREE_RED;

  rbtree_insert_fixup(t, newNode);

  return newNode;
}

/**
 * KEY를 갖는 node의 pointer를 T에서 찾아 return.
 * T에 KEY를 갖는 node가 존재하지 않으면 NULL을 return.
*/
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

/**
 * N을 root로 하는 subtree에서 최소 key값을 갖는 node를 return.
 * sentinel을 NIL로 설정한다.
*/
node_t *node_min(node_t *n, node_t *nil) {
  node_t *min = n;
  while(n != nil) {
    min = n;
    n = n->left;
  }
  return min;
}

/**
 * N을 root로 하는 subtree에서 최대 key값을 갖는 node를 return.
 * sentinel을 NIL로 설정한다.
*/
node_t *node_max(node_t *n, node_t *nil) {
  node_t *max = n;
  while(n != nil) {
    max = n;
    n = n->right;
  }
  return max;
}

/**
 * T에서 최소 key값을 갖는 node를 return.
*/
node_t *rbtree_min(const rbtree *t) {
  return node_min(t->root, t->nil);
}

/**
 * T에서 최대 key값을 갖는 node를 return.
*/
node_t *rbtree_max(const rbtree *t) {
  return node_max(t->root, t->nil);
}

/**
 * U와 U의 parent node와의 관계에서 U를 V로 대체하는 함수
*/
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

/**
 * T의 node를 erase한 후 rbtree 특성을 복구하는 함수.
*/
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

/**
 * T에서 TARGET node를 삭제하는 함수
*/
int rbtree_erase(rbtree *t, node_t *target) {
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

    /** case 1
        target의 자리를 right subtree의 min node로 대체 **/
    // y = node_min(target->right, t->nil);
    /** end of case 1 **/

    /** case 2 
        target의 자리를 left subtree의 max node로 대체 **/
    y = node_max(target->left, t->nil);
    /** end of case 2 **/

    y_color = y->color;

    /** case 1
        target의 자리를 right subtree의 min node로 대체 **/
    // tmp = y->right;
    // if(y->parent == target) {
    //   tmp->parent = y;
    // } else {
    //   trans_plant(t, y, y->right);
    //   y->right = target->right;
    //   y->right->parent = y;
    // }
    // trans_plant(t, target, y);
    // y->left = target->left;
    // y->left->parent = y;
    // y->color = target->color;
    /** end of case 1 **/

    /** case 2
        target의 자리를 left subtree의 max node로 대체 **/
    tmp = y->left;
    if(y->parent == target) {
      tmp->parent = y;
    } else {
      trans_plant(t, y, y->left);
      y->left = target->left;
      y->left->parent = y;
    }
    trans_plant(t, target, y);
    y->right = target->right;
    y->right->parent = y;
    y->color = target->color;
    /** end of case 2 **/
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

/**
 * CUR를 root node로 갖는 subtree의 모든 node를 오름차순으로 ARR에 저장하는 함수.
 * sentinel을 NIL로 설정하고, INDEX는 ARR에 저장을 시작할 첫번째 인덱스로 지정한다.
*/
void get_array(node_t *cur, node_t *nil, key_t *arr, int *index) {
  if(cur == nil) return;
  get_array(cur->left, nil, arr, index);
  arr[(*index)++] = cur->key;
  get_array(cur->right, nil, arr, index);
}

/**
 * T의 node를 배열 ARR에 오름차순으로 저장하는 함수.
 * N은 배열 ARR의 크기이다.
*/
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int *index = calloc(1, sizeof(int));
  get_array(t->root, t->nil, &arr[0], index);
  free(index);
  return 0;
}
