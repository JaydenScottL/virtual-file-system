#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "file.h"

Node* importFileSystem(FILE* fp) {
    char line[1000];
    Node* root = NULL;
    Node* current = NULL;

    while (fgets(line, sizeof(line), fp) != NULL) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        if(!(strcmp(line, "#end_folder#") == 0)){
            // Check if it's a directory or file
            if (!(line[0] == '/')) {
                char name[100];
                char content[100];
                char dt[100];

                bool dt_set = false;
                bool content_set = false;

                char *token;

                char * str = malloc(strlen(line) + 1);

                strcpy(str,line);

                token = strtok(str, "#");
                
                while ((token != NULL)) {
                    
                    if (strcmp(token, "name") == 0) {
                        token = strtok(NULL, "#");
                        strcpy(name,token);
                    } else if (strcmp(token, "file_content") == 0) {
                        token = strtok(NULL, "#");
                        strcpy(content,token);
                        content_set = true;
                    } else if (strcmp(token, "date_time") == 0) {
                        token = strtok(NULL, "#");
                        strcpy(dt,token);
                        dt_set = true;
                    }

                    token = strtok(NULL, "#");
                }

                // Create a new file node
                Node* newNode = createNode(name, 1);

                if(content_set){
                    addFileContent(newNode,content);
                }

                if(dt_set){
                    strcpy(newNode->creation_time,dt);
                }
                newNode->parent = current;
                addChild(current, newNode);

            } else {
                char name[100];
                char dt[100];

                bool dt_set = false;

                char *token;

                char * str = malloc(strlen(line) + 1);
                strcpy(str,line);
                token = strtok(str, "#");

                while ((token != NULL)) {
                    if (strcmp(token, "folder_name") == 0) {
                        token = strtok(NULL, "#");
                        strcpy(name,token);
                    }
                    else if (strcmp(token, "date_time") == 0) {
                        token = strtok(NULL, "#");
                        strcpy(dt,token);
                        dt_set = true;
                    }

                    token = strtok(NULL, "#");
                }

                // Create a new directory node
                Node* newNode = createNode(name, 0);

                if(dt_set){
                    strcpy(newNode->creation_time,dt);
                }

                if(root == NULL){
                    root = newNode;
                }else{
                    addChild(current, newNode);
                    newNode->parent = current;
                     // Move to the new directory
                }

                current = newNode;
            }
        }
        // Check for the end of a directory
        else{
            current = current->parent; // Move up to the parent directory
            continue;
        }
        
    }

    fclose(fp);
    return root;
}

void exportFileSystem(FILE* fp,Node * node){
    
    if(!node->isFile){
        //printf("%s",node->name);
        fprintf(fp,"/#folder_name#%s",node->name);
        fprintf(fp,"#date_time#%s",node->creation_time);
        fprintf(fp,"\n");
    }else{
        //printf("/%s",node->name);
        fprintf(fp,"#name#%s",node->name);
        if(strlen(node->content) > 0){
            fprintf(fp,"#file_content#%s",node->content);
        }

        fprintf(fp,"#date_time#%s",node->creation_time);
        fprintf(fp,"\n");
    }

    if(!node->isFile){

        if(node->numChildren < 1){
            fprintf(fp,"#end_folder#\n");
        }else{

            for(int i = 0;i < node->numChildren;i++){
                exportFileSystem(fp,node->children[i]);

                if(i == node->numChildren-1){
                    fprintf(fp,"#end_folder#\n");
                }
            }
        }
    }
}