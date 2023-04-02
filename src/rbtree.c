#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

node_t *rbtree_insert_fixup(rbtree *t, node_t *node);

rbtree *new_rbtree(void) {
  // 트리 생성
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  // sentinel 노드 생성
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  t->nil = nil;

  return t;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 추가할 노드 생성
  node_t *node = (node_t *)calloc(1, sizeof(node_t));
  node->key = key;
  node->color = RBTREE_RED; // 항상 레드로 추가
  node->left = node->right = t->nil;

  // tree가 비었으면 root로 지정
  if(t->root == NULL) {
    t->root = node;
    rbtree_insert_fixup(t, node);
    return t->root;
  }

  // tree에 노드가 있으면
  node_t *current_node = t->root; // 검사할 노드(루트부터 시작)

  while(1){
    if(key >= current_node->key) { // 추가할 노드가 더 크면
      if(current_node->right == t->nil) {
        current_node->right = node;
        node->parent = current_node;
        break;
      }
      // current_node의 오른쪽 자식노드로 검사할 노드 변경
      current_node = current_node->right;
    } else { // 추가할 노드가 더 작으면
      if(current_node->left == t->nil) {
        current_node->left = node;
        node->parent = current_node;
        break;
      }
      current_node = current_node->left; 
    }
  }
  rbtree_insert_fixup(t, node);

  return t->root;
}

node_t *rbtree_insert_fixup(rbtree *t, node_t *node) {
  if(node == t->root) {
    node->color = RBTREE_BLACK;
    return node;
  } else if(node->parent->color == RBTREE_BLACK) { // 부모가 검정 노드 || 부모가 루트 노드
    return node;
  }

  node_t *parent_node = node->parent;
  node_t *grand_parent_node = parent_node->parent;
  node_t *uncle_node;

  if(grand_parent_node->left == parent_node) {
    uncle_node = grand_parent_node->right;
  } else{
    uncle_node = grand_parent_node->left;
  }

  /* [CASE1] uncle_node->color가 RED인 경우 */
  if(uncle_node->color == RBTREE_RED) {
    grand_parent_node->color = RBTREE_RED;
    parent_node->color = RBTREE_BLACK;
    uncle_node->color = RBTREE_BLACK;
    rbtree_insert_fixup(t, grand_parent_node);
  }

  return t->root;
}



node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
int main(void){
  rbtree *tree = new_rbtree();
  rbtree_insert(tree, 3);
  rbtree_insert(tree, 1);
  rbtree_insert(tree, 33);
  rbtree_insert(tree, 23);
  rbtree_insert(tree, 12);
  rbtree_insert(tree, 4);
  rbtree_insert(tree, 2);
  rbtree_insert(tree, 1);
  rbtree_insert(tree, 1);
  rbtree_insert(tree, 5);
  rbtree_insert(tree, 6);
  rbtree_insert(tree, 9);

}
