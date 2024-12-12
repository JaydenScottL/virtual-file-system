#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "file.h"

typedef enum OperationType OperationType;

typedef struct OperationStack OperationStack;

typedef struct UndoOperation UndoOperation;

typedef struct PathLinkedList PathLinkedList;

struct OperationStack{
    UndoOperation *operations[100];
    int top;
};

enum OperationType{ 
    DELETE_OPERATION,
    RENAME_OPERATION,
    MOVE_OPERATION
};

struct UndoOperation{
    OperationType operation;
    Node *node;
    char oldName[100];
    char oldContent[1000];
    char oldDateTime[100];
    Node *parent;
    char **parentPath;
    int numParentPath;
    int isFile;
    UndoOperation **children;
    int numChildren;
};

struct PathLinkedList{
    char *path;
    PathLinkedList *next;
};

UndoOperation * createUndoOperation(OperationType operation, Node *node){
    
    UndoOperation *u = (UndoOperation*)malloc(sizeof(UndoOperation));
    u->operation = operation;
    u->node = node;
    u->isFile = node->isFile;
    strcpy(u->oldName, node->name);
    if(node->content != NULL){
        strcpy(u->oldContent, node->content);
    }
    strcpy(u->oldDateTime, node->creation_time);
    u->parent = node->parent;
    u->parentPath = NULL;
    u->numParentPath = 0;

    Node *parent = node->parent;
    int numParentPath = 0;

    while(parent != NULL){
        u->parentPath = (char**)realloc(u->parentPath, (numParentPath + 1) * sizeof(char*));
        u->parentPath[numParentPath] = (char*)malloc(100 * sizeof(char));
        strcpy(u->parentPath[numParentPath],parent->name);
        numParentPath++;
        parent = parent->parent;
        
    }
    
    u->numParentPath = numParentPath;
    u->numChildren = node->numChildren;
    return u;
}

void pushUndo(OperationStack* stack, UndoOperation *operation){
    if (stack->top < 99) {
        stack->operations[++stack->top] = operation;
    }
}

UndoOperation * popUndo(OperationStack* stack){
    if (stack->top >= 0) {
        return stack->operations[stack->top--];
    }
}

Node* scoutRoot(Node *root, char **parentPath,int numParentPath){
    Node *temp = root;

    int i = 2; // changed from 1 to 2
    
    while(i <= numParentPath){   

        bool flag = false;
        for(int j = 0; j < temp->numChildren; j++){
            if(strcmp(temp->children[j]->name, parentPath[numParentPath-i]) == 0){
                temp = temp->children[j];      
                flag = true;
            }
        }
        if(!flag){
            return NULL; // added with change from i = 1 to i = 2
        }
        i++;
    }
    return temp;
}

void processUndo(UndoOperation *currentOperation, Node *root){

    if(currentOperation == NULL) {
        return;
    }

    if(currentOperation->operation == RENAME_OPERATION){
        if(currentOperation->node != NULL) {
            strcpy(currentOperation->node->name, currentOperation->oldName);
        }
    }

    else if(currentOperation->operation == MOVE_OPERATION){
        if(currentOperation->parent != NULL) {
            Node *newNode = currentOperation->node;

            if(newNode->parent != NULL){
                Node *parent = newNode->parent;
            

                for (int i = 0; i < parent->numChildren; i++) {
                    if (parent->children[i] == newNode) {
                        // Shift elements to the left to remove the deleted node
                        for (int j = i; j < parent->numChildren - 1; j++) {
                            parent->children[j] = parent->children[j + 1];
                        }
                        parent->numChildren--;
                        break;
                    }
                }
            }

            addChild(scoutRoot(root, currentOperation->parentPath, (currentOperation->numParentPath)), newNode);  
            printf("Restored %s\n", newNode->name);
            
        }
    }
    
    else if(currentOperation->operation == DELETE_OPERATION){
        if(currentOperation->parent != NULL) {
            Node *newNode = createNode(currentOperation->oldName, currentOperation->isFile);
            if(currentOperation->oldContent != NULL){
                newNode->content = (char*)malloc(strlen(currentOperation->oldContent) + 1);
                strcpy(newNode->content, currentOperation->oldContent);
            }

            if(currentOperation->oldDateTime != NULL){
                strcpy(newNode->creation_time, currentOperation->oldDateTime);
            }

            addChild(scoutRoot(root, currentOperation->parentPath, (currentOperation->numParentPath)), newNode);  

            printf("Restored %s\n", newNode->name);

            if(currentOperation->children != NULL) {
                for(int i = 0; i < currentOperation->numChildren; i++) {
                    if(currentOperation->children[i] != NULL) {
                        processUndo(currentOperation->children[i], root);
                    }
                }
            }
        }
    }
}