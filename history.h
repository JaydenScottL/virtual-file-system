#ifndef HISTORY
#define HISTORY

typedef struct Node Node;

typedef enum OperationType OperationType;

typedef struct OperationStack OperationStack;

typedef struct UndoOperation UndoOperation;

struct OperationStack{
    UndoOperation *operations[100];
    int top;
};

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
    char **parentPath;
    int numParentPath;
    int isFile;
    UndoOperation **children;
    int numChildren;
};

UndoOperation * createUndoOperation(OperationType operation, Node *node);



void pushUndo(OperationStack* stack, UndoOperation *operation);

UndoOperation * popUndo(OperationStack* stack);
void processUndo(UndoOperation *u, Node *root);

#endif