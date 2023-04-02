#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_QUEUE_SIZE 20

node_t *rbtree_insert_fixup(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *node);

/* 트리 생성 */
rbtree *new_rbtree(void) {
  rbtree *tree = (rbtree *)calloc(1, sizeof(rbtree));
  // NIL 노드 생성
  node_t *nil = (node_t *)malloc(sizeof(node_t));
  nil->color = RBTREE_BLACK;
  tree->nil = tree->root = nil; // tree의 nil과 root를 nil 노드로 설정
  return tree;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

/* 노드 추가 */
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 1. 추가할 노드 생성
  node_t *node = (node_t *)calloc(1, sizeof(node_t));
  node->key = key;
  node->color = RBTREE_RED; // 항상 레드로 추가
  node->left = node->right = t->nil; // 추가한 노드의 자식들은 nil 노드로 설정

  // 2. tree가 비어있으면 루트 노드를 추가할 노드로 지정
  if(t->root == t->nil) {
    t->root = node;
    node->parent = t->nil;
    rbtree_insert_fixup(t, node); // 재정렬 (node 색 블랙으로 변경)
    return node;
  }

  // 3. 추가할 위치 탐색
  node_t *current_node = t->root; // 검사할 노드 지정(루트부터 시작)
  while(1) {
    if(key >= current_node->key) { 
    // 추가할 노드가 검사 중인 노드보다 더 크면 오른쪽 자식으로 이동
      if(current_node->right == t->nil) { // 오른쪽 자식이 nil 노드이면
        // 4. 노드 삽입
        current_node->right = node;
        node->parent = current_node;
        break;
      }
      current_node = current_node->right;
    } else { 
    // 3'. 추가할 위치 탐색(반대 경우)
      if(current_node->left == t->nil) {
        // 4'. 노드 삽입(반대 경우)
        current_node->left = node;
        node->parent = current_node;
        break;
      }
      current_node = current_node->left; 
    }
  }
  // 5. 재정렬
  rbtree_insert_fixup(t, node); 
  return node;
}

/* 재정렬 함수 */
node_t *rbtree_insert_fixup(rbtree *t, node_t *node) {
  
  // 1. 추가된 노드가 root 노드인 경우
  if(node == t->root) { 
    node->color = RBTREE_BLACK;
    return node;
  }

  // 2. 부모가 BLACK 노드인 경우
  node_t *parent_node = node->parent;
  if(parent_node->color == RBTREE_BLACK) { 
    return node;
  }

  node_t *grand_parent_node_node = parent_node->parent;
  node_t *uncle_node;
  int is_parent_is_left; // 부모가 왼쪽 자식인지 여부
  int is_left_child; // 현재 노드가 왼쪽 자식인지 여부

  if(grand_parent_node_node->left == parent_node) {
    uncle_node = grand_parent_node_node->right;
    is_parent_is_left = 1;
  }
  else {
    uncle_node = grand_parent_node_node->left;
    is_parent_is_left = 0;
  }

  if(node == parent_node->left) is_left_child = 1;
  else is_left_child = 0;

  /* 3. 삼촌 노드가 RED인 경우 */
  if(uncle_node->color == RBTREE_RED) {
    grand_parent_node_node->color = RBTREE_RED;
    parent_node->color = RBTREE_BLACK;
    uncle_node->color = RBTREE_BLACK;
    rbtree_insert_fixup(t, grand_parent_node_node);
  }

  /* 4. 삼촌 노드가 BLACK인 경우 */
  else {
    if(is_parent_is_left) {
      /* 4-1. 부모가 왼쪽 자식 && 내가 왼쪽 자식 */
      if(is_left_child) { 
        right_rotate(t, node->parent);
        node->parent->color = RBTREE_BLACK;
        node->parent->right->color = RBTREE_RED;
      }
      /* 4-2. 부모가 왼쪽 자식 && 내가 오른쪽 자식 */
      else {
        left_rotate(t, node);
        right_rotate(t, node);
        node->color = RBTREE_BLACK;
        node->right->color = RBTREE_RED;
      }
    } 
    else {
      if(is_left_child) { 
        /* 4-3. 부모가 오른쪽 자식 && 내가 왼쪽 자식 */
        right_rotate(t, node);
        left_rotate(t, node);
        node->color = RBTREE_BLACK;
        node->left->color = RBTREE_RED;
      }
      else {
        /* 4-4. 부모가 오른쪽 자식 && 내가 오른쪽 자식 */
        left_rotate(t, node->parent);
        node->parent->color = RBTREE_BLACK;
        node->parent->left->color = RBTREE_RED;
      }
    }
  }
  return t->root;
}

/* 오른쪽으로 회전하는 함수 */
void right_rotate(rbtree *t, node_t *node) {
  node_t *parent_node = node->parent;
  node_t *right_child = node->right;
  parent_node->left = right_child;
  right_child->parent = parent_node;
  
  if(parent_node == t->root) {
    t->root = node;
    node->parent = t->nil;
  } else {
    node_t *grand_parent_node = parent_node->parent;

    int is_parent_is_left; // 부모가 왼쪽 자식인지 여부
    if(grand_parent_node->left == parent_node) is_parent_is_left = 1;
    else is_parent_is_left = 0;

    if(is_parent_is_left) grand_parent_node->left = node;
    else grand_parent_node->right = node;
    node->parent = grand_parent_node;
  }
  parent_node->parent = node;
  node->right = parent_node;
}

/* 왼쪽으로 회전하는 함수 */
void left_rotate(rbtree *t, node_t *node) {
  node_t *parent_node = node->parent;
  node_t *left_child = node->left;

  parent_node->right = left_child;
  left_child->parent = parent_node;

  node->left = parent_node;
  if(parent_node == t->root) {
    t->root = node;
    node->parent = t->nil;
  }
  else {
    node_t *grand_parent_node = parent_node->parent;
    int is_parent_is_left;
    if(grand_parent_node->left == parent_node) is_parent_is_left = 1;
    else is_parent_is_left = 0;

    if(is_parent_is_left) grand_parent_node->left = node;
    else grand_parent_node->right = node;
    node->parent = grand_parent_node;
  }
  parent_node->parent = node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *current_node = t->root;
  while(key != current_node->key) {
    if(current_node == t->nil) return NULL;
    if(key < current_node->key) current_node = current_node->left;
    else current_node = current_node->right;
  }
  return current_node;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *current_node = t->root;
  while(current_node->left != t->nil) {
    current_node = current_node->left;
  }
  return current_node;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *current_node = t->root;
  while(current_node->right != t->nil) {
    current_node = current_node->right;
  }
  return current_node;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}


// TODO: implement to_array
node_t *next_node(const rbtree *t, node_t *p);

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *current_node = rbtree_min(t);
  arr[0] = current_node->key;
  for(int i = 1; i < n; i++) {
    current_node = next_node(t, current_node);
    arr[i] = current_node->key;
  }
  return 0;
}

node_t *next_node(const rbtree *t, node_t *p) {
  if(p->right == t->nil) {
    node_t *current_node = p;
    int is_left = 0;
    while(1) {
      if(current_node->parent->right == current_node) current_node = current_node->parent;
      else return current_node->parent;
    }
  }
  node_t *current_node = p->right;
  while(current_node->left != t->nil) {
    current_node = current_node->left;
  }
  return current_node;
}

/* TEST를 위한 큐 */
int front = -1;
int rear = 0;
node_t *queue[MAX_QUEUE_SIZE];
int depthq[MAX_QUEUE_SIZE];

void enqueue(node_t *value, int depth) {
    if (rear == MAX_QUEUE_SIZE - 1) {
        printf("Queue Overflow\n");
        return;
    }
    if (front == -1) front = 0;
    queue[rear] = value;
    depthq[rear] = depth;
    rear++;
}

node_t *dequeue() {
    if (front == -1 || front >= rear) {
        printf("Queue Underflow\n");
        return NULL;
    }
    node_t *value = queue[front];
    front++;
    return value;
}

int get_depth() {
  return depthq[front-1];
}
/*
int main(void){
  rbtree *tree = new_rbtree();
  rbtree_insert(tree, 10);
  rbtree_insert(tree, 5);
  rbtree_insert(tree, 5);
  rbtree_insert(tree, 34);
  rbtree_insert(tree, 6);
  rbtree_insert(tree, 23);
  rbtree_insert(tree, 12);
  rbtree_insert(tree, 12);
  rbtree_insert(tree, 6);
  rbtree_insert(tree, 12);
  
  // 테스트 : rbtree_to_array
  int arr[8];
  rbtree_to_array(tree, arr, 8);
  for(int i = 0; i < 8; i++) {
    printf("%d ", arr[i]);
  }
 
  node_t *current = tree->root;
  char *red = "RED";
  char *black = "BLACK";
  
  enqueue(current, 1);
  while(front < rear) {
    node_t *tmp = dequeue();
    int dep = get_depth();
    // printf("\nkey %d color %s depth %d parent %d left %d right %d", tmp->key, tmp->color ? black : red, dep, tmp->parent->key, tmp->left->key, tmp->right->key);
    printf("\nkey %d color %s depth %d", tmp->key, tmp->color ? black : red, dep);
    if(tmp->left != tree->nil) enqueue(tmp->left, dep+1);
    if(tmp->right != tree->nil) enqueue(tmp->right, dep+1);
  }

  return 0;
}
*/
