#include "binary_tree.h"
#include "buffer.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static size_t node_size_serialized(const node_t* node);
static void node_serialize(const node_t* node, buffer_t* buf);


node_t* node_new(int16_t number16, int32_t number32, const char* text) {
  node_t* node = malloc(sizeof(node_t));
  node->left = node->right = nullptr;
  node->number16 = number16;
  node->number32 = number32;
  node->text_length = strlen(text) + 1;
  node->text = strdup(text);
  return node;
}

size_t tree_calc_serialized_size(const node_t* root) {
  size_t header_size = sizeof(uint8_t);
  if (root == NULL) { return header_size; }
  size_t l, r;
  l = tree_calc_serialized_size(root->left);
  r = tree_calc_serialized_size(root->right);
  return header_size + node_size_serialized(root) + l + r;
}

void tree_serialize_preorder(const node_t* root, buffer_t* buf) {
  if (root == NULL) {
    uint8_t null_header = 0;
    buffer_write_bytes(buf, &null_header, sizeof(uint8_t));
    return;
  }
  node_serialize(root, buf);
  tree_serialize_preorder(root->left, buf);
  tree_serialize_preorder(root->right, buf);
}

void tree_free(node_t* root) {
  if (root == NULL) { return; }
  tree_free(root->left);
  tree_free(root->right);
  free(root->text);
  free(root);
}

void node_serialize(const node_t* node, buffer_t* buf) {
  uint32_t text_length_n = htonl(node->text_length);
  uint32_t number32_n = htonl(node->number32);
  uint16_t number16_n = htons(node->number16);
  buffer_write_bytes(buf, &text_length_n, sizeof(text_length_n));
  buffer_write_bytes(buf, &number32_n, sizeof(number32_n));
  buffer_write_bytes(buf, &number16_n, sizeof(number16_n));
  buffer_write_bytes(buf, node->text, sizeof(char) * node->text_length);
}

size_t node_size_serialized(const node_t* node) {
  return sizeof(node->text_length) + sizeof(node->number32) + sizeof(node->number16) + sizeof(char) * node->text_length;
}
