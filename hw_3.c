#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tree {
    char data;
    struct Tree *left, *right;
} Tree;

typedef struct {
    Tree *root;
} BinaryTree;

BinaryTree *create_binarytree()
{
    BinaryTree *tree = malloc(sizeof(BinaryTree));
    tree->root = NULL;
    return tree;
}

Tree *find_node(Tree *root, char *path)
{
    Tree *cur = root;
    int i = 1;

    if (!root || path[0] != '/' || path[1] != root->data)
        return NULL;

    while (path[i + 1]) {
        i++;

        if (cur->left && cur->left->data == path[i])
            cur = cur->left;
        else if (cur->right && cur->right->data == path[i])
            cur = cur->right;
        else
            return NULL;

        i++;
    }

    return cur;
}

Tree *find_parent(Tree *root, char *path)
{
    Tree *cur = root;
    int i = 2;

    if (!root || path[0] != '/' || path[1] != root->data)
        return NULL;

    while (path[i + 1]) {
        i++;

        if (!path[i + 1])
            return cur;

        if (cur->left && cur->left->data == path[i])
            cur = cur->left;
        else if (cur->right && cur->right->data == path[i])
            cur = cur->right;
        else
            return NULL;

        i++;
    }

    return cur;
}

int insert_root(BinaryTree *tree, char data)
{
    Tree *node;

    if (tree->root)
        return 0;

    node = malloc(sizeof(Tree));
    node->data = data;
    node->left = node->right = NULL;
    tree->root = node;

    return 1;
}

int insert_child(BinaryTree *tree, char *path, char child, char data)
{
    Tree *parent = find_node(tree->root, path);
    Tree *node;

    if (!parent)
        return 0;

    if (child == 'L') {
        if (parent->left ||
            (parent->right && parent->right->data == data))
            return 0;

        parent->left = malloc(sizeof(Tree));
        node = parent->left;
    }
    else if (child == 'R') {
        if (parent->right ||
            (parent->left && parent->left->data == data))
            return 0;

        parent->right = malloc(sizeof(Tree));
        node = parent->right;
    }
    else
        return 0;

    node->data = data;
    node->left = node->right = NULL;

    return 1;
}

int delete_node(BinaryTree *tree, char *path)
{
    Tree *node = find_node(tree->root, path);
    Tree *parent;

    if (!node || node->left || node->right)
        return 0;

    if (node == tree->root) {
        free(node);
        tree->root = NULL;
        return 1;
    }

    parent = find_parent(tree->root, path);

    if (parent->left == node)
        parent->left = NULL;
    else
        parent->right = NULL;

    free(node);
    return 1;
}

int update_node(BinaryTree *tree, char *path, char data)
{
    Tree *node = find_node(tree->root, path);
    Tree *parent;

    if (!node)
        return 0;

    if (node == tree->root) {
        node->data = data;
        return 1;
    }

    parent = find_parent(tree->root, path);

    if (parent->left == node && parent->right &&
        parent->right->data == data)
        return 0;

    if (parent->right == node && parent->left &&
        parent->left->data == data)
        return 0;

    node->data = data;
    return 1;
}

void read_node(BinaryTree *tree, char *path)
{
    Tree *node = find_node(tree->root, path);

    if (!node) {
        printf("ERROR\n");
        return;
    }

    if (!node->left && !node->right)
        printf("자식 노드가 없습니다.");
    else {
        if (node->left)
            printf("%c(L)", node->left->data);

        if (node->left && node->right)
            printf(", ");

        if (node->right)
            printf("%c(R)", node->right->data);
    }

    printf("\n");
}

void print_tree(Tree *node, int depth)
{
    int i;

    if (!node)
        return;

    print_tree(node->right, depth + 1);

    for (i = 0; i < depth; i++)
        printf("        ");

    printf("+---%c\n", node->data);

    print_tree(node->left, depth + 1);
}

void destroy_tree(Tree *node)
{
    if (!node)
        return;

    destroy_tree(node->left);
    destroy_tree(node->right);
    free(node);
}

int main()
{
    BinaryTree *tree = create_binarytree();
    char line[100], *arg[4], *token;
    int count;

    while (1) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin))
            break;

        line[strcspn(line, "\n")] = '\0';

        count = 0;
        token = strtok(line, " ");

        while (token && count < 4) {
            arg[count++] = token;
            token = strtok(NULL, " ");
        }

        if (count == 0)
            continue;

        if (arg[0][0] == 'I') {
            if (count == 3 && strcmp(arg[1], "/") == 0 &&
                arg[2][0] >= 'A' && arg[2][0] <= 'Z' &&
                arg[2][1] == '\0') {
                if (insert_root(tree, arg[2][0]) == 0)
                    printf("ERROR\n");
            }
            else if (count == 4 &&
                     arg[3][0] >= 'A' && arg[3][0] <= 'Z' &&
                     arg[3][1] == '\0') {
                if (insert_child(tree, arg[1], arg[2][0], arg[3][0]) == 0)
                    printf("ERROR\n");
            }
            else
                printf("ERROR\n");
        }
        else if (arg[0][0] == 'D') {
            if (count != 2 || delete_node(tree, arg[1]) == 0)
                printf("ERROR\n");
        }
        else if (arg[0][0] == 'U') {
            if (count != 3 ||
                arg[2][0] < 'A' || arg[2][0] > 'Z' ||
                arg[2][1] != '\0' ||
                update_node(tree, arg[1], arg[2][0]) == 0)
                printf("ERROR\n");
        }
        else if (arg[0][0] == 'R') {
            if (count == 2)
                read_node(tree, arg[1]);
            else
                printf("ERROR\n");
        }
        else if (arg[0][0] == 'P') {
            if (count != 1)
                printf("ERROR\n");
            else if (tree->root)
                print_tree(tree->root, 0);
            else
                printf("트리가 비어 있습니다.\n");
        }
        else
            printf("ERROR\n");
    }

    destroy_tree(tree->root);
    free(tree);

    return 0;
}
