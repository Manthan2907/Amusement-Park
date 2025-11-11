#ifndef BST_H
#define BST_H

#include "config.h"
#include "ride_manager.h"

/* BST Node Structure */
typedef struct BSTNode {
    Ride* ride;
    struct BSTNode* left;
    struct BSTNode* right;
    int height;  // For AVL balancing (optional)
} BSTNode;

/* BST Structure */
typedef struct BST {
    BSTNode* root;
    int size;
} BST;

/* Function Prototypes */

// BST Creation and Management
BST* createBST();
BSTNode* createBSTNode(Ride* ride);
void freeBST(BST* bst);
void freeBSTNode(BSTNode* node);

// BST Operations
BSTNode* insertRide(BSTNode* root, Ride* ride);
BSTNode* deleteRide(BSTNode* root, int ride_id);
BSTNode* searchByRideId(BSTNode* root, int ride_id);
BSTNode* findMin(BSTNode* root);
BSTNode* findMax(BSTNode* root);

// Traversal Operations
void inorderTraversal(BSTNode* root);
void preorderTraversal(BSTNode* root);
void postorderTraversal(BSTNode* root);

// Query Functions
Ride** findRidesWithMaxWait(BSTNode* root, int max_minutes, int* count);
Ride* findShortestWaitRide(BSTNode* root);
Ride** findRideInWaitRange(BSTNode* root, int min_wait, int max_wait, int* count);

// Update Operations
void updateRideWaitTimeInBST(BST* bst, int ride_id, int new_wait_time);
void rebuildBST(BST* bst, RideList* rides);

// AVL Balancing (Optional)
int getHeight(BSTNode* node);
int getBalanceFactor(BSTNode* node);
BSTNode* rotateLeft(BSTNode* node);
BSTNode* rotateRight(BSTNode* node);
BSTNode* balance(BSTNode* node);

// Display Functions
void displayBSTInorder(BSTNode* root);
void displayRidesByWaitTime(BST* bst);

// Helper Functions
int countNodes(BSTNode* root);
void collectRidesInRange(BSTNode* root, int min_wait, int max_wait, Ride** result, int* index);

#endif /* BST_H */
