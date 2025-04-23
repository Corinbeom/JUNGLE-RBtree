#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  t->nil = nil;
  nil->left = nil->right = nil->parent = nil;
  t->root = nil;

  return t;
}

// 현재 노드가 부모 노드의 왼쪽 자식인지 판별하는 함수
int is_node_left(node_t *p) {
  return (p->parent->left == p);
}

// 두 노드의 color를 바꿔주는 함수
void exchange_color(node_t *p1, node_t *p2) {
  int tmp_color = p1->color;
  p1->color = p2->color;
  p2->color = (tmp_color == RBTREE_BLACK) ? RBTREE_BLACK : RBTREE_RED;
}

// node의 메모리를 해제하는 함수
void node_is_free(rbtree *t, node_t *p) {
  if (p->left != t->nil)
    node_is_free(t, p->left);
  if (p->right != t->nil)
    node_is_free(t, p->right);
  free(p);
}

void delete_rbtree(rbtree *t) {
  node_t *start = t->root;
  if (start != t->nil)
    node_is_free(t, start);
  free(t->nil);
  free(t);
}

// 특정 노드 p의 후속 노드(successor)를 반환
// inorder 순서로 현재 노드의 다음 노드를 찾아주는 함수
node_t *get_successor(const rbtree *t, node_t *p) {
  if (p == t->nil) return t->nil;  // 널 넣고 호출했을 때 방지

  // p가 최대값이면 후속 노드 없음 (nil 반환)
  if (p == rbtree_max(t)) {
    return t->nil;
  }
  node_t *curr;

  // 오른쪽 자식이 없는 경우 (오른쪽 서브트리가 없는 경우)
  if (p->right == t->nil) {
    curr = p;
    while (1) {
      // 현재 노드의 부모가 왼쪽 자식이면, 부모가 후속 노드
      if (is_node_left(curr)) {
        return curr->parent;
      }
      else {  // 아니라면 위로 올라가면서 왼쪽 자식일 때 까지 탐색
        curr = curr->parent;
      }
    }
  }

  // 오른쪽 자식이 있는 경우
  // 오른쪽 서브트리의 최소값이 후속 노드
  curr = p->right;
  while (curr->left != t->nil) {
    curr = curr->left;
  }

  return curr;
}

// 우회전 함수: 트리에서 왼쪽 자식을 위로 올리는 회전
void right_rotate(rbtree *t, node_t *node) {
  node_t *parent_node = node->parent;                 // 회전 대상의 부모
  node_t *right_child = node->right;                  // 노드의 오른쪽 자식
  node_t *grand_parent_node = parent_node->parent;    // 조부모 노드

  // 부모 노드의 왼쪽 자식을 노드의 오른쪽 자식으로 대체
  parent_node->left = right_child;
  right_child->parent = parent_node;

  // 부모 노드가 루트인 경우, 회전 후 루트를 업데이트
  if (parent_node == t->root) {
    t->root = node;
  }
  else if (parent_node != t->root) {
    // 부모 노드가 루트가 아닌 경우, 조부모와의 연결을 수정
    if (is_node_left(parent_node)) {
      grand_parent_node->left = node;
    }
    else {
      grand_parent_node->right = node;
    }
    node->parent = grand_parent_node;
  }

  // 회전 마무리: 노드와 부모의 관계 뒤집기
  parent_node->parent = node;
  node->right = parent_node;
}

// 좌회전 함수 : 트리에서 오른쪽 자식을 위로 올리는 회전
void left_rotate(rbtree *t, node_t *node) {
  node_t *parent_node = node->parent;               // 회전 대상의 부모
  node_t *left_child = node->left;                  // 노드의 오른쪽 자식
  node_t *grand_parent_node = parent_node->parent;  // 조부모 노드

  // 부모 노드의 오른쪽 자식을 노드의 왼쪽 자식으로 대체
  parent_node->right = left_child;
  left_child->parent = parent_node;

  // 부모 노드가 루트인 경우, 회전 후 루트를 업데이트
  if (parent_node == t->root) {
    t->root = node;
  }
  // 부모 노드가 루트가 아닌 경우
  else if (parent_node != t->root) {
    if (is_node_left(parent_node)) {
      grand_parent_node->left = node;
    }
    else {
      grand_parent_node->right = node;
    }
    node->parent = grand_parent_node;
  }

  // 회전 마무리 : 노드와 부모의 관계 뒤집기
  node->left = parent_node;
  parent_node->parent = node;
}

// insert 리밸런싱 함수
void rbtree_insert_fixup(rbtree *t, node_t *node) {
  // case 1: 삽입 노드가 루트인 경우 -> 블랙으로 설정
  if (node == t->root) {
    node->color = RBTREE_BLACK;
    return;
  }

  node_t *parent_node = node->parent;
  node_t *grand_parent_node = parent_node->parent;
  node_t *uncle_node;

  // case 2: 부모가 블랙이면 문제 없음 -> 리턴
  if (parent_node->color == RBTREE_BLACK)
    return;

  // uncle 노드 설정
  if (is_node_left(parent_node)) {
    uncle_node = grand_parent_node->right;
  }
  else {
    uncle_node = grand_parent_node->left;
  }

  // uncle이 RED인 경우 -> recoloring + 재귀 fixup
  if (uncle_node->color == RBTREE_RED) {
    grand_parent_node->color = RBTREE_RED;
    parent_node->color = RBTREE_BLACK;
    uncle_node->color = RBTREE_BLACK;
    rbtree_insert_fixup(t, grand_parent_node);
  }

  //  case 4 : uncle이 블랙 -> rotation 필요
  else if (uncle_node->color == RBTREE_BLACK) {

    //  case 4-1 : 왼쪽-왼쪽 (LL) -> right rotate
    if (is_node_left(parent_node) && is_node_left(node)) {
      right_rotate(t, node->parent);
      node->parent->color = RBTREE_BLACK;
      node->parent->right->color = RBTREE_RED;
    }

    // case 4-2 : 왼쪽-오른쪽 (LR) -> left + right rotate
    else if (is_node_left(parent_node) && !is_node_left(node)) {
      left_rotate(t, node);
      right_rotate(t, node);
      node->color = RBTREE_BLACK;
      node->right->color = RBTREE_RED;
    }

    // case 4-3 : 오른쪽-왼쪽 (RL) -> right + left rotate
    else if (!is_node_left(parent_node) && is_node_left(node)) {
      right_rotate(t, node);
      left_rotate(t, node);
      node->color = RBTREE_BLACK;
      node->left->color = RBTREE_RED;
    }

    // case 4-4 : 오른쪽-오른쪽 (RR) -> left rotate
    else if (!is_node_left(parent_node) && !is_node_left(node)) {
      left_rotate(t, node->parent);
      node->parent->color = RBTREE_BLACK;
      node->parent->left->color = RBTREE_RED;
    }
  }
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 새 노드 메모리 할당 및 기본 설정 (RED, nil 자식)
  node_t *node = (node_t *)malloc(sizeof(node_t));
  node_t *curr = t->root;

  node->key = key;
  node->color = RBTREE_RED;
  node->left = node->right = t->nil;

  // BST 삽입할 위치 탐색
  while (curr != t->nil)
  {
    if (curr->key <= key)
    {
      // 오른쪽 자식이 없으면 거기에 삽입
      if (curr->right == t->nil)
      {
        curr->right = node;
        break;
      }
      curr = curr->right;
    }
    else
    {
      // 왼쪽 자식이 없으면 거기에 삽입
      if (curr->left == t->nil)
      {
        curr->left = node;
        break;
      }
      curr = curr->left;
    }
  }

  // 부모 연결
  node->parent = curr;

  // 트리가 비어있다면, 루트로 설정
  if (curr == t->nil)
    t->root = node;

  // 삽입 이후 리밸런싱
  rbtree_insert_fixup(t, node);
  return node;
}

// key 값을 가지는 노드를 탐색하여 반환하는 함수
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *curr = t->root;

  // key가 일치하지 않고, nil 노드가 아닐 때까지 순회
  while (key != curr->key && curr != t->nil)
  {
    if (key < curr->key) {
      curr = curr->left;  // key가 작으면 왼쪽 자식으로 이동
    }
    else {
      curr = curr->right; // key가 크면 오른쪽 자식으로 이동
    }
  }

  // 찾은 노드가 nil이 아니면 해당 노드, 아니면 NULL 반환
  return (curr != t->nil) ? curr : NULL;
}

// 트리에서 최솟값을 가진 노드를 반환하는 함수
node_t *rbtree_min(const rbtree *t)
{
  node_t *curr = t->root;

  // 왼쪽 자식이 있을 때까지 계속 이동
  while (curr != t->nil && curr->left != t->nil)
    curr = curr->left;

  return curr; // 가장 왼쪽 노드 == 최소값
}

// 트리에서 최댓값을 가진 노드를 반환하는 함수
node_t *rbtree_max(const rbtree *t)
{
  node_t *curr = t->root;

  // 오른쪽 자식이 있을 때까지 계속 이동
  while (curr != t->nil && curr->right != t->nil)
    curr = curr->right;

  return curr; // 가장 오른쪽 노드 == 최대값
}

// erase 리밸런싱 함수
void rbtree_erase_fixup(rbtree *t, node_t *parent_node, int is_left)
{
  // is_left를 통해 extra black의 위치가 왼쪽 자식인지 오른쪽 자식인지 확인
  node_t *sibling = is_left ? parent_node->right : parent_node->left;
  node_t *out_child = is_left ? sibling->right : sibling->left; // sibling node의 바깥쪽 자식
  node_t *in_child = is_left ? sibling->left : sibling->right;  // sibling node의 안쪽 자식

  // case 1 : 형제가 RED면 -> 부모와 색 교환 + 회전으로 Black 형제로 변환
  if (sibling->color == RBTREE_RED)
  {
    if (is_left) { // 형제가 오른쪽 -> 왼쪽 회전
      left_rotate(t, sibling);
    }
    else { // 형제가 왼쪽 -> 오른쪽 회전
      right_rotate(t, sibling);
    }

    exchange_color(sibling, parent_node); // 부모와 형제의 색을 교환
    rbtree_erase_fixup(t, parent_node, is_left); // 재귀로 다시 fixup
  }

  // case 2 : 형제가 Black일 떄
  else if (sibling->color == RBTREE_BLACK)
  {
    // case 2-1 : 형제의 바깥 자식이 RED
    if (out_child->color == RBTREE_RED)
    {
      if (is_left) {
        left_rotate(t, sibling);
      }
      else {
        right_rotate(t, sibling);
      }
      exchange_color(sibling, parent_node); // 부모와 형제의 색 교환
      out_child->color = RBTREE_BLACK;      // 바깥 자식을 Black으로 변경
    }

    // case 2-2 : 형제의 바깥 자식은 Black, 안쪽 자식은 ReD
    else if (out_child->color == RBTREE_BLACK && in_child->color == RBTREE_RED)
    {
      if (is_left) {
        right_rotate(t, in_child); // 왼쪽 자식을 부모 방향으로 회전
      }
      else {
        left_rotate(t, in_child);
      }

      exchange_color(sibling, in_child);  // 형제와 안쪽 자식 색 교환
      rbtree_erase_fixup(t, parent_node, is_left);  // 다시 fixup
    }

    // case 2-3 : 형제의 자식 둘 다 Black -> 형제를 Red로 바꾸고 부모 fixup
    else if (out_child->color == RBTREE_BLACK && in_child->color == RBTREE_BLACK)
    {
      sibling->color = RBTREE_RED;

      // 부모가 root면 끝, 아니면 doubly black이 부모로 전이
      if (parent_node == t->root)
        return;
      rbtree_erase_fixup(t, parent_node->parent, is_node_left(parent_node));
    }
  }
}

// remove node를 successor로 대체하는 함수
// 노드 p를 successor로 덮어쓰기하고, successor를 트리에서 제거하는 과정
node_t *replace_to_successor(rbtree *t, node_t *p, node_t *successor, node_t *removed_node_parent)
{
  node_t *replace_node = successor->right; // successor는 무조건 오른쪽 자식만 가질 수 있음

  // 삭제 대상 노드 p의 key를 successor의 key로 덮어씀 (데이터만 이동)
  p->key = successor->key;

  // successor 제거 : 부모 노드의 자식 포인터를 successor의 오른쪽 자식으로 변경
  if (successor == removed_node_parent) {
    // 바로 아래가 successor니까 그냥 처리 가능
    if (successor == removed_node_parent->left)
      removed_node_parent->left = successor->right;
    else
      removed_node_parent->right = successor->right;
  } else {
    if (is_node_left(successor))
      removed_node_parent->left = successor->right;
    else
      removed_node_parent->right = successor->right;
  }

  // 새로운 부모 설정
  if (successor->right != t->nil) {
    successor->right->parent = removed_node_parent;
  }

  // 실제 트리에서 제거되는 successor의 위치에 있던 노드를 반환
  // 이 노드가 fixup 대상
  return replace_node;
}

// remove node를 child로 대체하는 함수
// 삭제되는 노드 p가 자식 한명만 가질 때 사용하는 방식
node_t *replace_to_child(rbtree *t, node_t *p, node_t *removed_node_parent)
{
  node_t *left_node = p->left;
  node_t *right_node = p->right;
  node_t *replace_node;

  // 자식 노드가 하나거나 둘 다 nil인 경우 처리
  replace_node = right_node;
  if (left_node != t->nil) {
    replace_node = left_node; // 왼쪽 자식이 존재하면 왼쪽 자식으로 대체
  }

  // 부모 노드의 포인터를 자식 노드로 연결
  if (is_node_left(p)) {
    removed_node_parent->left = replace_node;
  }
  else {
    removed_node_parent->right = replace_node;
    }

  // 자식 노드의 부모를 새로운 부모로 설정
  if (replace_node != t->nil) {
    replace_node->parent = removed_node_parent;
  }
  // 삭제 노드 메모리 해제
  free(p);
  // fixup을 위한 대체된 노드 반환
  return replace_node;
}

// rbtree에서 특정 노드 p를 삭제하는 함수
int rbtree_erase(rbtree *t, node_t *p)
{
  node_t *right_node = p->right;
  node_t *left_node = p->left;
  node_t *removed_node_parent, *successor_node, *replace_node;

  int is_removed_black = 0;  // 삭제되는 노드가 black인지
  int is_left;           // 삭제되는 노드가 부모 기준 왼쪽인지

  // case 1 : 삭제할 노드가 자식이 둘인 경우
  if (right_node != t->nil && left_node != t->nil)
  {
    // 후속 노드를 찾고 관련 정보 저장
    successor_node = get_successor(t, p);
    is_left = is_node_left(successor_node);
    is_removed_black = successor_node->color ? 1 : 0;  // 0 : RED, 1 : Black
    removed_node_parent = successor_node->parent;

    // 후속 노드로 대체
    replace_node = replace_to_successor(t, p, successor_node, removed_node_parent);

    // 후속 노드 메모리 해제
    free(successor_node);
  }

  // case 2 : 삭제할 노드가 자식이 하나거나 없는 경우
  else if (right_node == t->nil || left_node == t->nil)
  {
    // 삭제 노드가 루트인 경우 (직접 처리)
    if (p == t->root)
    {
      // 자식 중 하나만 존재하므로, 그 자식이 루트가 됨
      t->root = (left_node == t->nil) ? right_node : left_node;
      t->root->color = RBTREE_BLACK;
      free(p);
      return 0;
    }

    // 일반 노드일 경우
    is_left = is_node_left(p);
    is_removed_black = p->color ? 1 : 0;
    removed_node_parent = p->parent;

    // 자식 노드로 대체
    replace_node = replace_to_child(t, p, removed_node_parent);
  }

  // 삭제된 노드가 black인데 대체 노드가 red면 그냥 색깔만 바꿔줌
  if (is_removed_black && replace_node->color == RBTREE_RED) {
    replace_node->color = RBTREE_BLACK;
    return 0;
  }
  // 삭제된 노드도 black이고, 대체 노드도 black이면 fixup 필요
  else if (is_removed_black) {
    if (replace_node != NULL && (replace_node == t->nil || replace_node->color == RBTREE_BLACK)) {
      rbtree_erase_fixup(t, removed_node_parent, is_left);
    }
  }
  return 0;
}

// RB Tree를 오름차순 배열로 변환하는 함수
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // 가장 작은 노드부터 시작 (중위 순회의 시작점)
  node_t *curr = rbtree_min(t);

  // 첫 번째 원소 삽입
  if (curr == t->nil) return 0;
  arr[0] = curr->key;

  // 나머지 원소 삽입
  for (int i = 1; i < n; i++)
  {
    // 현재 노드가 nil이면 중단 (범위 벗어남)
    if (curr == t->nil)
      break;

    // 후속 노드로 이동
    curr = get_successor(t, curr);

    // 후속 노드가 nil이면 중단
    if (curr == t->nil)
      break;

    // 배열에 삽입
    arr[i] = curr->key;
  }
  return 0;
}