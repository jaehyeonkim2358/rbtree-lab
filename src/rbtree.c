#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

void init_nil(node_t *nil) {
  nil->color = RBTREE_BLACK;
  nil->key = 0;
  nil->parent = NULL;
  nil->left = NULL;
  nil->right = NULL;
}

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  init_nil(nil);
  p->nil = nil;
  p->root = p->nil;
  return p;
}

/**
 * node N을 root로 갖고 NIL을 sentinel로 하는 RB tree의 
 * NIL을 제외한 모든 node에 할당된 메모리를 반환하는 함수.
*/
void delete_sub_node(node_t *n, node_t *nil) {
  if(n == nil) return;
  delete_sub_node(n->left, nil);
  delete_sub_node(n->right, nil);
  free(n);
}

/**
 * T가 사용한 모든 메모리를 반환하는 함수.
*/
void delete_rbtree(rbtree *t) {
  delete_sub_node(t->root, t->nil);
  free(t->nil);
  free(t);
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

/**
 * insert이후 T의 rbtree 특성을 복구하는 함수.
*/
void rbtree_insert_fixup(rbtree *t, node_t *cursor) {
  // 종료 조건: cursor 부모 노드의 color (RBTREE_BLACK or RBTREE_RED)
  while(cursor->parent->color == RBTREE_RED) {
    // 분기 1: cursor 부모 노드의 위치 (left child or right child)
    if(cursor->parent == cursor->parent->parent->left) {
      node_t *uncle = cursor->parent->parent->right;
      // 분기 2: cursor 삼촌 노드(uncle)의 color (RBTREE_BLACK or RBTREE_RED)
      if(uncle->color == RBTREE_RED) {
        cursor->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        cursor->parent->parent->color = RBTREE_RED;
        cursor = cursor->parent->parent;
      } else {
        // 분기 3: cursor의 위치 (left child or right child)
        if(cursor == cursor->parent->right) {
          cursor = cursor->parent;
          left_rotate(t, cursor);
        }
        cursor->parent->color = RBTREE_BLACK;
        cursor->parent->parent->color = RBTREE_RED;
        right_rotate(t, cursor->parent->parent);
      }
    } else {
      node_t *uncle = cursor->parent->parent->left;
      if(uncle->color == RBTREE_RED) {
        cursor->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        cursor->parent->parent->color = RBTREE_RED;
        cursor = cursor->parent->parent;
      } else {
        if(cursor == cursor->parent->left) {
          cursor = cursor->parent;
          right_rotate(t, cursor);
        }
        cursor->parent->color = RBTREE_BLACK;
        cursor->parent->parent->color = RBTREE_RED;
        left_rotate(t, cursor->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

/**
 * KEY를 갖는 node를 생성 후 return하는 함수.
*/
node_t *create_new_node(const key_t key, node_t *nil) {
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->right = nil;
  new_node->left = nil;
  new_node->color = RBTREE_RED;
  return new_node;
}

/**
 * T에 KEY를 갖는 node를 삽입하는 함수.
*/
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // rbtree에 저장할 new_node 생성
  node_t *new_node = create_new_node(key, t->nil);

  // insert 위치 탐색
  node_t *parent_node = t->nil;
  node_t *cursor = t->root;
  while(cursor != t->nil) {
    parent_node = cursor;
    if(new_node->key < cursor->key) {
      cursor = cursor->left;
    } else {
      cursor = cursor->right;
    }
  }

  // new_node와 parent_node의 자식-부모 관계 설정
  new_node->parent = parent_node;
  if(parent_node == t->nil) {                     // 1. parent_node가 nil인 경우 (newNode가 root)
    t->root = new_node;
  } else if(new_node->key < parent_node->key) {   // 2. new_node가 parent_node의 왼쪽 자식 노드인 경우
    parent_node->left = new_node;
  } else {                                        // 3. new_node가 parent_node의 오른쪽 자식 노드인 경우
    parent_node->right = new_node;
  }

  // rbtree 복구
  rbtree_insert_fixup(t, new_node);

  // 생성 후 insert한 노드의 pointer를 반환
  return new_node;
}

/**
 * T에서 KEY를 갖는 node의 pointer를 찾는 함수.
 * 
 * KEY를 갖는 node가 존재하면 node를 return.
 * KEY를 갖는 node가 존재하지 않으면 NULL을 return.
*/
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *cursor = t->root;
  while(cursor != t->nil) {
    if(key < cursor->key) {
      cursor = cursor->left;
    } else if(key > cursor->key) {
      cursor = cursor->right;
    } else {
      break;
    }
  }
  
  if(cursor == t->nil) {
    return NULL;
  } else {
    return cursor;
  }
}

/**
 * CURSOR가 root인 subtree에서 최소 key값을 갖는 node를 return하는 함수.
*/
node_t *subtree_min(node_t *cursor, node_t *nil) {
  node_t *min = cursor;
  while(cursor != nil) {
    min = cursor;
    cursor = cursor->left;
  }
  return min;
}

/**
 * T에서 key값이 최소인 node를 return하는 함수.
*/
node_t *rbtree_min(const rbtree *t) {
  return subtree_min(t->root, t->nil);
}

/**
 * CURSOR가 root인 subtree에서 최대 key값을 갖는 node를 return하는 함수.
*/
node_t *subtree_max(node_t *cursor, node_t *nil) {
  node_t *max = cursor;
  while(cursor != nil) {
    max = cursor;
    cursor = cursor->right;
  }
  return max;
}

/**
 * T에서 key값이 최대인 node를 return하는 함수.
*/
node_t *rbtree_max(const rbtree *t) {
  return subtree_max(t->root, t->nil);
}

/**
 * 부모와의 관계에서, old_child를 new_child로 대체하는 함수.
*/
void trans_plant(rbtree *t, node_t *old_child, node_t *new_child) {
  if(old_child->parent == t->nil) {
    t->root = new_child;
  } else if(old_child == old_child->parent->left) {
    old_child->parent->left = new_child;
  } else {
    old_child->parent->right = new_child;
  }
  new_child->parent = old_child->parent;
}

/**
 * erase 후 rbtree 특성을 복구하는 함수.
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
 * T에서 TARGET node를 삭제하는 함수.
*/
int rbtree_erase(rbtree *t, node_t *target) {
  node_t *y = target;
  color_t y_color = y->color;

  node_t *x;
  if(target->left == t->nil) {
    x = target->right;
    trans_plant(t, target, target->right);
  } else if(target->right == t->nil) {
    x = target->left;
    trans_plant(t, target, target->left);
  } else {
    /** case 1 **/
    /** 삭제 될 target의 자리를 right subtree의 min node로 대체 **/
    // y = subtree_min(target->right, t->nil);
    // y_color = y->color;
    // x = y->right;
    // if(y->parent == target) {
    //   x->parent = y;
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

    /** case 2 **/
    /** 삭제 될 target의 자리를 left subtree의 max node로 대체 **/
    y = subtree_max(target->left, t->nil);
    y_color = y->color;
    x = y->left;
    if(y->parent == target) {
      x->parent = y;
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

  if(y_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, x);
  }

  init_nil(t->nil);
  
  return 0;
}

/**
 * 배열 ARR에 ROOT_NODE의 모든 자손 node들을 오름차순으로 저장하는 함수.
 * NIL은 sentinel node 이며, INDEX에는 ARR의 첫번째 인덱스로 지정할 값을 갖는 pointer를 전달한다.
*/
void set_array(node_t *root_node, node_t *nil, key_t *arr, size_t *index) {
  if(root_node == nil) return;
  set_array(root_node->left, nil, arr, index);
  arr[(*index)++] = root_node->key;
  set_array(root_node->right, nil, arr, index);
}

/**
 * 길이가 N인 pointer ARR에 T의 node들을 오름차순으로 저장하는 함수.
*/
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  size_t index = 0;
  set_array(t->root, t->nil, arr, &index);
  return 0;
}
