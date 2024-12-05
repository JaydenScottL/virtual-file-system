#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "file.h"
#include "externals.h"

/*
Name: Jayden LeCorps
Date: 11/30/2024
*/

typedef enum OperationType OperationType;

typedef struct OperationStack OperationStack;

typedef struct UndoOperation UndoOperation;

enum OperationType{ 
    DELETE_OPERATION,
    RENAME_OPERATION
};

struct UndoOperation{
    OperationType operation;
    Node *node;
    char oldName[100];
    char oldContent[1000];
    char oldDateTime[100];
    Node *parent;
    UndoOperation *children;
    int numChildren = 0;
};

UndoOperation createUndoOperation(OperationType operation, Node *node){
    UndoOperation u;
    u.operation = operation;
    u.node = node;
    strcpy(u.oldName, node->name);
    strcpy(u.oldContent, node->content);
    strcpy(u.oldDateTime, node->creation_time);
    u.parent = node->parent;
    return u;
}

struct OperationStack{
    UndoOperation operations[100];
    int top;
};

void pushUndo(OperationStack* stack, UndoOperation operation){
    if (stack->top < 99) {
        stack->operations[++stack->top] = operation;
    }
}

UndoOperation * popUndo(OperationStack* stack){
    if (stack->top >= 0) {
        return &stack->operations[stack->top--];
    }
}

void printTree(Node* node, int depth) {
    for (int i = 0; i < depth; i++) {
        if(!node->isFile){
            printf("   ");
        }else{
            printf("   ");
        }
    }
    printf("%s | Created: %s\n", node->name,node->creation_time);

    for (int i = 0; i < node->numChildren; i++) {
        printTree(node->children[i], depth + 1);
    }
}

void printPath(Node* node, char* path, const char* targetDirectory) {
    
    if(strcmp(node->name,targetDirectory) == 0){
        printf("%s",node->name);
        return;
    }
    
    if (node->isFile) {
        return; 
    }

    for (int i = 0; i < node->numChildren; i++) {
        char newPath[200];
        strcpy(newPath, path);
        strcat(newPath, "\\");
        strcat(newPath, node->children[i]->name);

        if (strcmp(node->children[i]->name, targetDirectory) == 0) {
            printf("%s", newPath);
            return; 
        }

        printPath(node->children[i], newPath, targetDirectory);
    }
}

void printFileContent(Node* node){
    if(node->isFile){
        printf("Content of %s:\n%s\n",node->name,node->content);
    }
}

void printFileDetails(Node* node, Node *root){
    printf("Name: %s\n", node->name);
    printf("Created: %s\n",node->creation_time);
    printf("Path: ");
    printPath(root,"Root",node->name);
    
    printf("\nType: ");
    if(node->isFile){
        printf("File\n");
        if(node->content != NULL){
            printf("Size: %d\n",(int)strlen(node->content));
        }
    }else{
        printf("Directory\n");
    }
}

int main() {

    // Create root directory
    Node* root = createNode("Root", 0);
    Node *currentDirectory = root;
    OperationStack undoStack;
    undoStack.top = -1;

    while(true){
        
        printPath(root,"Root",currentDirectory->name);
        printf(">");
        
        char command[100];
        fgets(command,100,stdin);
        
        command[strcspn(command, "\n")] = '\0';
        
        char l_command[100];
        for (int i = 0; command[i] != '\0'; i++) {
            l_command[i] = tolower(command[i]);
        }

        if(strstr((l_command),"cd") != NULL){
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            bool flag = true;

            if(argument != NULL){
                if(strcmp("..",argument) == 0){
                    if(currentDirectory->parent!=NULL){
                        currentDirectory = currentDirectory->parent;
                        flag = false;
                    }
                }

                for(int i = 0;i < currentDirectory->numChildren && flag;i++){
                    
                    if(strcmp(currentDirectory->children[i]->name,argument) == 0){
                        if(!currentDirectory->children[i]->isFile){
                            printf("Entering %s directory\n",argument);
                            currentDirectory = currentDirectory->children[i];
                            flag = false;
                            break;
                        }else{
                            printf("%s is not a directory!\n",argument);
                            flag = false;
                            break;
                        }
                    }
                }

                if(flag){
                    printf("%s was not found in the directory\n",argument);
                }
            }else{
                printf("Invalid cd notation. Make sure you are including a space between \"cd\" and [directory / ..] \n");
            }
        }

        else if(strstr(l_command,"mkdir") != NULL){
            
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            if(argument != NULL){
                Node * newFolder = createNode(argument,0);
                addChild(currentDirectory,newFolder);
                printf("Created directory %s\n",newFolder->name);
            }else{
                printf("Invalid directory name\n");
            }
        }

        else if(strstr(l_command,"dir") != NULL){

            for(int i = 0;i < currentDirectory->numChildren;i++){
                
                if(!currentDirectory->children[i]->isFile){
                    printf("\\");
                }

                printf("%s %s\n",currentDirectory->children[i]->name,currentDirectory->children[i]->creation_time);
            }
        }

        else if(strstr(l_command,"touch") != NULL){
            
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            bool flag = true;

            if(argument != NULL){

                for(int i = 0;i < currentDirectory->numChildren;i++){
                    
                    if(strcmp(currentDirectory->children[i]->name,argument) == 0){
                        printf("%s already exists in the directory.",argument);
                        flag = false;
                        break;
                    }
                }
                
                if(flag){
                    Node * newFile = createNode(argument,1);
                    addChild(currentDirectory,newFile);
                    printf("Created file %s\n",newFile->name);
                }

            }else{
                printf("Invalid file name\n");
            }
        }

        else if(strstr(l_command,"rename") != NULL){
            
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 
            bool flag = true;

            if(argument != NULL){
                for(int i = 0;i < currentDirectory->numChildren;i++){
                    
                    if(strcmp(currentDirectory->children[i]->name,argument) == 0){
                        
                            printf("Enter name for %s:\n",argument);
                            char name[100];
                            fgets(name,100,stdin);
                            
                            command[strcspn(name, "\n")] = '\0';

                            char *ptr = name;

                            while (*ptr) {
                                if (*ptr == '\n') {
                                    *ptr = '\0';
                                }
                                ptr++;
                            }

                            UndoOperation u = createUndoOperation(RENAME_OPERATION,currentDirectory->children[i]);
                            pushUndo(&undoStack, u);

                            strcpy(currentDirectory->children[i]->name,name);
                            flag = false;
                    }
                }
            }

            if(flag){
                printf("%s was not found in the directory\n",argument);
            }
        }

        else if(strstr(l_command,"write") != NULL){
            
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 
            bool flag = true;

            if(argument != NULL){
                for(int i = 0;i < currentDirectory->numChildren;i++){
                    
                    if(strcmp(currentDirectory->children[i]->name,argument) == 0){
                        if(currentDirectory->children[i]->isFile){
                                printf("Enter file contents for %s:\n",argument);
                                char content[100];
                                fgets(content,100,stdin);
                                
                                command[strcspn(content, "\n")] = '\0';

                                char *ptr = content;

                                while (*ptr) {
                                    if (*ptr == '\n') {
                                        *ptr = '\0';
                                    }
                                    ptr++;
                                }

                                strcpy(currentDirectory->children[i]->content,content);
                                flag = false;
                        }
                    }
                }
            }

            if(flag){
                printf("%s was not found in the directory\n",argument);
            }
        }

        else if(strstr(l_command,"read") != NULL){
            
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            bool flag = true;

            if(argument != NULL){
                for(int i = 0;i < currentDirectory->numChildren;i++){
                    
                    if(strcmp(currentDirectory->children[i]->name,argument) == 0){
                        if(currentDirectory->children[i]->isFile){
                            printFileContent(currentDirectory->children[i]);
                            flag = false;
                        }
                    }
                }
            }

            if(flag){
                printf("%s was not found in the directory\n",argument);
            }
        }

        else if(strstr(l_command,"details") != NULL){
            
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            bool flag = true;

            if(argument != NULL){
                for(int i = 0;i < currentDirectory->numChildren;i++){
                    
                    if(strcmp(currentDirectory->children[i]->name,argument) == 0){
                        
                        printFileDetails(currentDirectory->children[i],root);

                        flag = false;
                    }
                }
            }

            if(flag){
                printFileDetails(currentDirectory,root);
                //printf("%s was not found in the directory\n",argument);
            }
        } 

        else if(strstr(l_command,"rm") != NULL){
            
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            bool flag = true;

            if(argument != NULL){
                for(int i = 0;i < currentDirectory->numChildren;i++){
                    
                    if(strcmp(currentDirectory->children[i]->name,argument) == 0){
                        
                        UndoOperation u = createUndoOperation(DELETE_OPERATION,currentDirectory->children[i]);
                        pushUndo(&undoStack, u);
                        
                        if(!currentDirectory->children[i]->isFile){
                            for(int j = 0;j < currentDirectory->children[i]->numChildren;j++){
                                
                                UndoOperation u2 = createUndoOperation(DELETE_OPERATION,currentDirectory->children[i]->children[j]);
                                (u.children) = (UndoOperation*)realloc(u.children, (u.numChildren + 1) * sizeof(UndoOperation*));
                                u.children[j] = u2;
                                
                                deleteNode(currentDirectory->children[i]->children[j]);
                            }
                        }

                        deleteNode(currentDirectory->children[i]);

                        printf("Deleted %s\n", argument);
                        flag = false;
                    }
                }
            }

            if(flag){
                printf("%s was not found in the directory\n",argument);
            }
        }  

        else if(strstr(l_command,"undo") != NULL){

            if(undoStack.top >= 0){
                UndoOperation *u = popUndo(&undoStack);
                

                if(u->operation == RENAME_OPERATION){
                    
                    printf("Undid rename %s->%s\n",u->node->name,u->oldName);
                    strcpy(u->node->name, u->oldName);

                }else if(u->operation == DELETE_OPERATION){
                    printf("Undid delete %s\n",u->oldName);
                    Node *newNode = createNode(u->oldName,u->node->isFile);
                    strcpy(newNode->content,u->oldContent);
                    strcpy(newNode->creation_time,u->oldDateTime);
                    addChild(u->parent,u->node);
                }
            }else{
                printf("No operations to undo\n");
            }
        }

        else if(strstr(l_command,"listfs") != NULL){ 
            printTree(root,0);
        }

        else if(strstr(l_command,"export") != NULL){ 
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            bool flag = true;

            if(argument != NULL){

                FILE *fp = fopen(argument, "w");
                if (fp == NULL) {
                    perror("Error creating file");
                    return 1;

                }
                
                exportFileSystem(fp,root);
                fclose(fp);
                flag = false;
            }

            if(flag){
                printf("Unable to export to %s\n",argument);
            }
        }

        else if(strstr(l_command,"import") != NULL){ 
            char * argument = strtok(command, " "); 
            argument = strtok(NULL, ""); 

            bool flag = true;

            if(argument != NULL){
                FILE * fp = fopen(argument, "r");
                if (fp == NULL) {
                    perror("Error opening file");
                    return 1;
                }
                deleteNode(root);
                root = importFileSystem(fp);
                currentDirectory = root;
                flag = false;

                printf("Imported the %s file system. \n", argument);
            }

            if(flag){
                printf("Unable to import %s\n",argument);
            }
        }

        else if(strstr(l_command,"help") != NULL){ 
            printf("List of commands: \n");
            printf("cd [childdir / ..] - Switches directory\n");
            printf("dir - Lists files and directories contained in this directory\n");
            printf("touch [filename] - Creates new file\n");
            printf("rm [filename] - Deletes file or directory\n");
            printf("write [filename] - Writes to file\n");
            printf("read [filename] - Reads from file\n");
            printf("rename [filename] - Renames file or directory\n");
            printf("details [filename/empty] - Prints details of file or directory\n");
            printf("export [filename] - Exports file system to disk\n");
            printf("import [filename] - Imports file system to disk\n");
            printf("listfs - Prints entire file system\n");
        }else{
            printf("\"%s\" is not a valid command. Run \"Help\" for a list of commands.\n",command);
        }
        
    }
    

    return 0;
}