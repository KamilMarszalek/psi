#include "binary_tree.h"
#include "buffer.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>


static uint32_t node_size_serialized(const node_t* node);
static void node_serialize(const node_t* node, buffer_t* buf);
node_t* node_deserialize(buffer_t* buf);
static void generate_random_string(size_t n, char* str);


node_t* node_new(int16_t number16, int32_t number32, const char* text) {
  node_t* node = malloc(sizeof(node_t));
  node->left = node->right = nullptr;
  node->number16 = number16;
  node->number32 = number32;
  node->text_length = strlen(text) + 1;
  node->text = strdup(text);
  return node;
}

node_t* tree_create_random_inorder(int i, int n) {
  node_t* root = NULL;
  if (i < n) {
    size_t text_size = rand() % 64 + 1;
    char text[text_size];
    generate_random_string(text_size, text);
    root = node_new((short) (rand() % 32), rand() % 64, text);
    root->left = tree_create_random_inorder(i * 2 + 1, n);
    root->right = tree_create_random_inorder(i * 2 + 2, n);
  }
  return root;
}

uint32_t tree_calc_serialized_size(const node_t* root) {
  uint32_t root_size = node_size_serialized(root);
  if (root == NULL) { return root_size; }
  uint32_t l = tree_calc_serialized_size(root->left);
  uint32_t r = tree_calc_serialized_size(root->right);
  return root_size + l + r;
}

void tree_serialize_preorder(const node_t* root, buffer_t* buf) {
  node_serialize(root, buf);
  if (root == NULL) { return; }
  tree_serialize_preorder(root->left, buf);
  tree_serialize_preorder(root->right, buf);
}

node_t* tree_deserialize_preorder(buffer_t* buf) {
  node_t* root = node_deserialize(buf);
  if (root == NULL) { return root; }
  root->left = tree_deserialize_preorder(buf);
  root->right = tree_deserialize_preorder(buf);
  return root;
}

void tree_free(node_t* root) {
  if (root == NULL) { return; }
  tree_free(root->left);
  tree_free(root->right);
  free(root->text);
  free(root);
}

uint32_t node_size_serialized(const node_t* node) {
  if (node == NULL) { return 1; }
  return 1 + sizeof(node->text_length) + sizeof(node->number32) + sizeof(node->number16) + node->text_length;
}

void node_serialize(const node_t* node, buffer_t* buf) {
  uint8_t null_flag = 0;
  if (node == NULL) {
    buffer_write_bytes(buf, &null_flag, sizeof(null_flag));
    return;
  }
  null_flag = 1;
  buffer_write_bytes(buf, &null_flag, sizeof(null_flag));

  uint32_t text_length_n = htonl(node->text_length);
  uint32_t number32_n = htonl(node->number32);
  uint16_t number16_n = htons(node->number16);

  buffer_write_bytes(buf, &text_length_n, sizeof(text_length_n));
  buffer_write_bytes(buf, &number32_n, sizeof(number32_n));
  buffer_write_bytes(buf, &number16_n, sizeof(number16_n));
  buffer_write_bytes(buf, node->text, node->text_length);
}

node_t* node_deserialize(buffer_t* buf) {
  uint8_t header;
  buffer_read_bytes(buf, &header, sizeof(header));
  if (header == 0) { return NULL; }

  uint32_t text_length_n;
  uint32_t number32_n;
  uint16_t number16_n;

  buffer_read_bytes(buf, &text_length_n, sizeof(text_length_n));
  buffer_read_bytes(buf, &number32_n, sizeof(number32_n));
  buffer_read_bytes(buf, &number16_n, sizeof(number16_n));

  uint32_t text_length = ntohl(text_length_n);
  int32_t number32 = (int32_t) ntohl(number32_n);
  int16_t number16 = (int16_t) ntohs(number16_n);

  char* text = malloc(text_length);
  buffer_read_bytes(buf, text, text_length);
  node_t* root = node_new(number16, number32, text);
  free(text);
  return root;
}

void generate_random_string(size_t n, char* str) {
  char charset[] = "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "0123456789";

  for (size_t i = 0; i < n - 1; i++) { str[i] = charset[rand() % (sizeof(charset) - 1)]; }
  str[n - 1] = '\0';
}
