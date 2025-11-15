#include "binary_tree.h"
#include "buffer.h"
#include <arpa/inet.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void node_serialize(const node_t* node, buffer_t* buf);
static size_t node_size_serialized(const node_t* node);

node_t* node_new(int16_t number16, int32_t number32, const char* text) {
  node_t* node = malloc(sizeof(node_t));
  node->left = node->right = nullptr;
  node->number16 = number16;
  node->number32 = number32;
  node->text_length = strlen(text) + 1;
  node->text = strdup(text);
  return node;
}

int tree_send_preorder(int sock, const node_t* root) {
  size_t node_size = node_size_serialized(root);
  uint8_t data[node_size];
  buffer_t buf = {.data = data, .length = 0};
  node_serialize(root, &buf);

  if (write(sock, buf.data, buf.length) == -1) { return -1; };
  if (root == NULL) { return 0; }
  if (tree_send_preorder(sock, root->left) == -1) { return -1; }
  if (tree_send_preorder(sock, root->right) == -1) { return -1; }
  return 0;
}

void tree_free(node_t* root) {
  if (root == NULL) { return; }
  tree_free(root->left);
  tree_free(root->right);
  free(root->text);
  free(root);
}

void node_serialize(const node_t* node, buffer_t* buf) {
  uint8_t exists = (node != NULL);
  buffer_write_bytes(buf, &exists, sizeof(exists));
  if (exists) {
    uint32_t text_length_n = htonl(node->text_length);
    uint32_t number32_n = htonl(node->number32);
    uint16_t number16_n = htons(node->number16);
    buffer_write_bytes(buf, &text_length_n, sizeof(text_length_n));
    buffer_write_bytes(buf, &number32_n, sizeof(number32_n));
    buffer_write_bytes(buf, &number16_n, sizeof(number16_n));
    buffer_write_bytes(buf, node->text, sizeof(char) * node->text_length);
  }
}

size_t node_size_serialized(const node_t* node) {
  uint8_t header_size = sizeof(uint8_t);
  if (node == NULL) { return header_size; }
  return header_size + sizeof(node->text_length) + sizeof(node->number32) + sizeof(node->number16) +
         sizeof(char) * node->text_length;
}
