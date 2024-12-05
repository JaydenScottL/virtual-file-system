#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

typedef struct Node Node;

struct Node {
    char name[1000];
    char creation_time[100];
    int isFile; // 1 for file, 0 for directory
    Node *parent;
    Node **children;
    int numChildren;
    char content[1000];
};

struct Node* createNode(const char* name, int isFile) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    strcpy(node->name, name);
    node->isFile = isFile;
    node->children = NULL;
    node->parent = NULL;
    node->numChildren = 0;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    strftime(node->creation_time, sizeof(node->creation_time), "%Y-%m-%d %H:%M:%S", tm);

    return node;
}

void addChild(struct Node* parent, struct Node* child) {
    (parent->children) = (Node**)realloc(parent->children, (parent->numChildren + 1) * sizeof(Node*));
    parent->children[parent->numChildren] = child;
    child->parent = parent;
    parent->numChildren++;
}

void addFileContent(Node* file, const char* content) {
    //file->content = (char*)malloc(strlen(content) + 1);
    strcpy(file->content, content);
}

void deleteNode(Node* node) {

    
    Node * parent;

    if(node->parent != NULL){
        parent = node->parent;
    

        for (int i = 0; i < parent->numChildren; i++) {
            if (parent->children[i] == node) {
                // Shift elements to the left to remove the deleted node
                for (int j = i; j < parent->numChildren - 1; j++) {
                    parent->children[j] = parent->children[j + 1];
                }
                parent->numChildren--;
                break;
            }
        }
    }

    for (int i = 0; i < node->numChildren; i++) {
        deleteNode(node->children[i]);
    }

    if (node->isFile) {
        //free(node->content);
    }

    free(node);
}