#ifndef V_FILE
#define V_FILE

typedef struct Node Node;
typedef struct OperationStack OperationStack;
typedef struct UndoOperation UndoOperation;

struct Node {
    char name[1000];
    char creation_time[100];
    int isFile; // 1 for file, 0 for directory
    Node *parent;
    Node **children;
    int numChildren;
    char * content;
};

struct Node* createNode(const char* name, int isFile);

void addChild(struct Node* parent, struct Node* child);
void addFileContent(Node* file, const char* content);

UndoOperation* deleteNode(Node* node);

#endif