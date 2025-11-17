#include "binary_tree.h"
#include "buffer.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAND_MAX_NUMBER32 (1 << 10)
#define RAND_MAX_NUMBER16 (1 << 9)
#define RAND_MAX_TEXT_SIZE (1 << 6)


static int tree_height(const node_t* root);
static void tree_print_level(const node_t* root, int level, int* n_node);
static uint32_t node_size_serialized(const node_t* node);
static void node_serialize(const node_t* node, buffer_t* buf);
static node_t* node_deserialize(buffer_t* buf);
static void generate_random_string(size_t n, char* str);

node_t* node_new(node_data_t data) {
  node_t* node = malloc(sizeof(node_t));
  node->left = node->right = NULL;
  node->text_length = strlen(data.text) + 1;
  node->text = strdup(data.text);
  node->number32 = data.number32;
  node->number16 = data.number16;
  return node;
}

node_t* tree_create_random(int idx, int n_nodes) {
  node_t* root = NULL;
  if (idx < n_nodes) {
    size_t text_size = (rand() % RAND_MAX_TEXT_SIZE) + 1;
    char text[text_size];
    generate_random_string(text_size, text);
    root = node_new((node_data_t) {.text = text,
                                   .number32 = rand() % RAND_MAX_NUMBER32,
                                   .number16 = (short) (rand() % RAND_MAX_NUMBER16)});
    root->left = tree_create_random((idx * 2) + 1, n_nodes);
    root->right = tree_create_random((idx * 2) + 2, n_nodes);
  }
  return root;
}

uint32_t tree_calc_serialized_size(const node_t* root) {
  uint32_t root_size = node_size_serialized(root);
  if (root == NULL) {
    return root_size;
  }
  uint32_t left_size = tree_calc_serialized_size(root->left);
  uint32_t right_size = tree_calc_serialized_size(root->right);
  return root_size + left_size + right_size;
}

void tree_serialize_preorder(const node_t* root, buffer_t* buf) {
  node_serialize(root, buf);
  if (root == NULL) {
    return;
  }
  tree_serialize_preorder(root->left, buf);
  tree_serialize_preorder(root->right, buf);
}

node_t* tree_deserialize_preorder(buffer_t* buf) {
  node_t* root = node_deserialize(buf);
  if (root == NULL) {
    return root;
  }
  root->left = tree_deserialize_preorder(buf);
  root->right = tree_deserialize_preorder(buf);
  return root;
}

void tree_print_level_order(const node_t* root, int n_levels) {
  int n_node = 0;
  int height = tree_height(root);
  for (int i = 0; i < (height > n_levels ? n_levels : height); ++i) {
    tree_print_level(root, i, &n_node);
  }
}

void tree_free(node_t* root) {
  if (root == NULL) {
    return;
  }
  tree_free(root->left);
  tree_free(root->right);
  free(root->text);
  free(root);
}

// -------------------------------

int tree_height(const node_t* root) {
  if (root == NULL) {
    return 0;
  }
  int left_height = tree_height(root->left);
  int right_height = tree_height(root->right);
  return 1 + (left_height > right_height ? left_height : right_height);
}

void tree_print_level(const node_t* root, int level, int* n_node) {
  if (root == NULL) {
    return;
  }
  if (level == 0) {
    printf("N%d: {int_num: %d, short_num: %d, text_length: %d, text: %s}\n", *n_node, root->number32, root->number16,
           root->text_length, root->text);
    *n_node += 1;
  } else {
    tree_print_level(root->left, level - 1, n_node);
    tree_print_level(root->right, level - 1, n_node);
  }
}


uint32_t node_size_serialized(const node_t* node) {
  if (node == NULL) {
    return 1;
  }
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
  uint8_t header = 0;
  buffer_read_bytes(buf, &header, sizeof(header));
  if (header == 0) {
    return NULL;
  }

  uint32_t text_length_n = 0;
  uint32_t number32_n = 0;
  uint16_t number16_n = 0;

  buffer_read_bytes(buf, &text_length_n, sizeof(text_length_n));
  buffer_read_bytes(buf, &number32_n, sizeof(number32_n));
  buffer_read_bytes(buf, &number16_n, sizeof(number16_n));

  uint32_t text_length = ntohl(text_length_n);
  int32_t number32 = (int32_t) ntohl(number32_n);
  int16_t number16 = (int16_t) ntohs(number16_n);

  char* text = malloc(text_length);
  buffer_read_bytes(buf, text, text_length);
  node_t* root = node_new((node_data_t) {.text = text, .number32 = number32, .number16 = number16});
  free(text);
  return root;
}

void generate_random_string(size_t n, char* str) {
  char charset[] = "abcdefghijklmnopqrstuvwxyz"
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                   "0123456789";

  for (size_t i = 0; i < n - 1; i++) {
    str[i] = charset[rand() % (sizeof(charset) - 1)];
  }
  str[n - 1] = '\0';
}
