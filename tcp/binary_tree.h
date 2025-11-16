#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include "buffer.h"
#include <stdint.h>

typedef struct Node {
  struct Node* left;
  struct Node* right;
  uint32_t text_length;
  int32_t number32;
  int16_t number16;
  char* text;
} node_t;


node_t* node_new(int16_t number16, int32_t number32, const char* text);
node_t* tree_create_random_inorder(int i, int n);
uint32_t tree_calc_serialized_size(const node_t* root);
void tree_serialize_preorder(const node_t* root, buffer_t* buf);
node_t* tree_deserialize_preorder(buffer_t* buf);
void tree_free(node_t* root);

#endif
