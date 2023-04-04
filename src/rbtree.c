#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_QUEUE_SIZE 20

void rbtree_insert_fixup(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *node);
node_t *get_next_node(const rbtree *t, node_t *p);

/* 1️⃣ RB tree 구조체 생성 */
// 새 트리를 생성하는 함수
rbtree *new_rbtree(void)
{
  rbtree *tree = (rbtree *)calloc(1, sizeof(rbtree));
  // NIL 노드
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  // tree의 nil과 root를 nil 노드로 설정 (tree가 빈 경우 root는 nil노드여야 한다.)
  tree->nil = tree->root = nil;
  nil->color = RBTREE_BLACK;
  return tree;
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  free(t);
}

/* 3️⃣ key 추가 */
// 노드를 삽입하고 불균형을 복구하는 함수
node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // 새 노드 생성
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node->key = key;
  new_node->color = RBTREE_RED;              // 항상 레드로 추가
  new_node->left = new_node->right = t->nil; // 추가한 노드의 자식들을 nil 노드로 설정

  // 새 노드를 삽입할 위치 탐색
  node_t *current = t->root;
  while (current != t->nil)
  {
    if (key < current->key)
    {
      if (current->left == t->nil)
      {
        current->left = new_node; // 새 노드를 왼쪽 자식으로 추가
        break;
      }
      current = current->left;
    }
    else
    {
      if (current->right == t->nil)
      {
        current->right = new_node; // 새 노드를 오른쪽 자식으로 추가
        break;
      }
      current = current->right;
    }
  }

  new_node->parent = current; // 새 노드의 부모 지정

  // root가 nil이면(트리가 비어있으면) 새 노드를 트리의 루트로 지정
  if (current == t->nil)
    t->root = new_node;

  // 불균형 복구
  rbtree_insert_fixup(t, new_node);

  return new_node;
}

// 노드 삽입 후 불균형을 복구하는 함수
void rbtree_insert_fixup(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *uncle;
  int is_left = node == parent->left; // 현재 노드가 왼쪽 자식인지 여부
  int is_parent_is_left;              // 부모가 왼쪽 자식인지 여부

  // 추가된 노드가 root 노드인 경우: 색만 변경
  if (node == t->root)
  {
    node->color = RBTREE_BLACK;
    return;
  }

  // 부모가 BLACK인 경우: 변경 없음
  if (parent->color == RBTREE_BLACK)
    return;

  is_parent_is_left = grand_parent->left == parent;
  uncle = (is_parent_is_left) ? grand_parent->right : grand_parent->left;

  // [CASE 1]: 부모와 부모의 형제가 모두 RED인 경우
  if (uncle->color == RBTREE_RED)
  {
    parent->color = RBTREE_BLACK;
    grand_parent->color = RBTREE_RED;
    uncle->color = RBTREE_BLACK;
    rbtree_insert_fixup(t, grand_parent);
    return;
  }

  if (is_parent_is_left)
  {
    if (is_left)
    // [CASE 2]: 부모의 형제가 BLACK & 부모가 왼쪽 자식 & 현재 노드가 왼쪽 자식인 경우
    {
      right_rotate(t, parent);
      parent->color = RBTREE_BLACK;
      parent->right->color = RBTREE_RED;
      return;
    }

    // [CASE 3]: 부모의 형제가 BLACK & 부모가 왼쪽 자식 & 현재 노드가 오른쪽 자식인 경우
    left_rotate(t, node);
    right_rotate(t, node);
    node->color = RBTREE_BLACK;
    node->right->color = RBTREE_RED;
    return;
  }

  if (is_left)
  {
    // [CASE 2]: 부모의 형제가 BLACK & 부모가 오른쪽 자식 & 현재 노드가 왼쪽 자식인 경우
    right_rotate(t, node);
    left_rotate(t, node);
    node->color = RBTREE_BLACK;
    node->left->color = RBTREE_RED;
    return;
  }

  // [CASE 3]: 부모의 형제가 BLACK & 부모가 오른쪽 자식 & 현재 노드가 오른쪽 자식인 경우
  left_rotate(t, parent);
  parent->color = RBTREE_BLACK;
  parent->left->color = RBTREE_RED;
}

// 오른쪽으로 회전하는 함수
void right_rotate(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_right = node->right;

  // 부모가 루트인 경우: 현재 노드를 루트로 지정 (노드를 삭제한 경우만 해당)
  if (parent == t->root)
    t->root = node;
  else
  { // 1-1) grand_parent의 자식을 현재 노드로 변경
    if (grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }
  node->parent = grand_parent; // 1-2) 노드의 부모를 grand_parent로 변경 (양방향 연결)
  parent->parent = node;       // 2-1) 부모의 부모를 현재 노드로 변경
  node->right = parent;        // 2-2) 부모를 현재 노드의 자식으로 변경 (양방향 연결)
  parent->left = node_right;   // 3-1) 현재 노드의 자식을 부모의 자식으로 변경
  node_right->parent = parent; // 3-2) 오른쪽 자식의 부모를 부모로 변경 (양방향 연결)
}

// 왼쪽으로 회전하는 함수
void left_rotate(rbtree *t, node_t *node)
{
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *node_left = node->left;

  // 부모가 루트인 경우: 현재 노드를 루트로 지정 (노드를 삭제한 경우만 해당)
  if (parent == t->root)
    t->root = node;
  else
  { // 1-1) grand_parent의 자식을 현재 노드로 변경
    if (grand_parent->left == parent)
      grand_parent->left = node;
    else
      grand_parent->right = node;
  }
  node->parent = grand_parent; // 1-2) 노드의 부모를 grand_parent로 변경 (양방향 연결)
  parent->parent = node;       // 2-1) 부모의 부모를 현재 노드로 변경
  node->left = parent;         // 2-2) 부모를 현재 노드의 자식으로 변경 (양방향 연결)
  parent->right = node_left;   // 3-1) 현재 노드의 자식을 부모의 자식으로 변경
  node_left->parent = parent;  // 3-2) 왼쪽 자식의 부모를 부모로 변경 (양방향 연결)
}

/* 4️⃣ key 탐색 */
// key에 해당하는 노드를 반환하는 함수
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *current = t->root;
  while (current != t->nil)
  {
    if (key == current->key)
      return current;
    else
      current = (key < current->key) ? current->left : current->right;
  }
  return NULL; // 해당 key값을 가진 노드가 없을 경우 NULL 반환
}

/* 5️⃣ 최소값을 가진 node 탐색 */
// key가 최소값에 해당하는 노드를 반환하는 함수
node_t *rbtree_min(const rbtree *t)
{
  node_t *current = t->root;
  while (current->left != t->nil)
    current = current->left;
  return current;
}

/* 6️⃣ 최대값을 가진 node 탐색 */
// key가 최대값에 해당하는 노드를 반환하는 함수
node_t *rbtree_max(const rbtree *t)
{
  node_t *current = t->root;
  while (current->right != t->nil)
    current = current->right;
  return current;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  return 0;
}

/* 7️⃣ array로 변환 */
// `t`를 inorder로 `n`번 순회한 결과를 `arr`에 담는 함수
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  node_t *current = rbtree_min(t);
  arr[0] = current->key;
  for (int i = 1; i < n; i++)
  {
    if (current == t->nil)
      break;
    current = get_next_node(t, current);
    if (current == t->nil)
      break;
    arr[i] = current->key;
  }
  return 0;
}

// 키 값을 기준으로 다음 노드를 반환하는 함수
node_t *get_next_node(const rbtree *t, node_t *p)
{
  node_t *current = p->right;
  if (current == t->nil) // 오른쪽 자식이 없으면
  {
    current = p;
    while (1)
    {
      if (current->parent->right == current) // current가 오른쪽 자식인 경우
        current = current->parent;           // 부모 노드로 이동 후 이어서 탐색
      else
        return current->parent; // current가 왼쪽 자식인 경우 부모 리턴
    }
  }
  while (current->left != t->nil) // 왼쪽 자식이 있으면
    current = current->left;      // 왼쪽 끝으로 이동
  return current;
}
