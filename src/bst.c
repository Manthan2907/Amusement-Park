#include <stdio.h>
#include <stdlib.h>
#include "../include/bst.h"

/* Create BST */
BST* createBST() {
    BST* bst = (BST*)malloc(sizeof(BST));
    if (!bst) {
        fprintf(stderr, "Error: Memory allocation failed for BST\n");
        return NULL;
    }
    
    bst->root = NULL;
    bst->size = 0;
    
    return bst;
}

/* Create BST node */
BSTNode* createBSTNode(Ride* ride) {
    if (!ride) return NULL;
    
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for BST node\n");
        return NULL;
    }
    
    node->ride = ride;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    
    return node;
}

/* Insert ride into BST */
BSTNode* insertRide(BSTNode* root, Ride* ride) {
    if (!ride) return root;
    
    if (root == NULL) {
        return createBSTNode(ride);
    }
    
    if (ride->current_wait_time < root->ride->current_wait_time) {
        root->left = insertRide(root->left, ride);
    } else if (ride->current_wait_time > root->ride->current_wait_time) {
        root->right = insertRide(root->right, ride);
    }
    
    return root;
}

/* Find minimum node */
BSTNode* findMin(BSTNode* root) {
    while (root && root->left) {
        root = root->left;
    }
    return root;
}

/* Find maximum node */
BSTNode* findMax(BSTNode* root) {
    while (root && root->right) {
        root = root->right;
    }
    return root;
}

/* Delete ride from BST */
BSTNode* deleteRide(BSTNode* root, int ride_id) {
    if (!root) return NULL;
    
    if (ride_id < root->ride->id) {
        root->left = deleteRide(root->left, ride_id);
    } else if (ride_id > root->ride->id) {
        root->right = deleteRide(root->right, ride_id);
    } else {
        if (!root->left) {
            BSTNode* temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            BSTNode* temp = root->left;
            free(root);
            return temp;
        }
        
        BSTNode* temp = findMin(root->right);
        root->ride = temp->ride;
        root->right = deleteRide(root->right, temp->ride->id);
    }
    
    return root;
}

/* Search by ride ID */
BSTNode* searchByRideId(BSTNode* root, int ride_id) {
    if (!root || root->ride->id == ride_id) {
        return root;
    }
    
    if (ride_id < root->ride->id) {
        return searchByRideId(root->left, ride_id);
    }
    
    return searchByRideId(root->right, ride_id);
}

/* Inorder traversal */
void inorderTraversal(BSTNode* root) {
    if (root) {
        inorderTraversal(root->left);
        printf("[%d] %s - Wait: %d min\n",
               root->ride->id, root->ride->name, root->ride->current_wait_time);
        inorderTraversal(root->right);
    }
}

/* Preorder traversal */
void preorderTraversal(BSTNode* root) {
    if (root) {
        printf("[%d] %s\n", root->ride->id, root->ride->name);
        preorderTraversal(root->left);
        preorderTraversal(root->right);
    }
}

/* Postorder traversal */
void postorderTraversal(BSTNode* root) {
    if (root) {
        postorderTraversal(root->left);
        postorderTraversal(root->right);
        printf("[%d] %s\n", root->ride->id, root->ride->name);
    }
}

/* Find shortest wait ride */
Ride* findShortestWaitRide(BSTNode* root) {
    BSTNode* min_node = findMin(root);
    return min_node ? min_node->ride : NULL;
}

/* Collect rides in range */
void collectRidesInRange(BSTNode* root, int min_wait, int max_wait, Ride** result, int* index) {
    if (!root) return;
    
    if (root->ride->current_wait_time > min_wait) {
        collectRidesInRange(root->left, min_wait, max_wait, result, index);
    }
    
    if (root->ride->current_wait_time >= min_wait && root->ride->current_wait_time <= max_wait) {
        result[(*index)++] = root->ride;
    }
    
    if (root->ride->current_wait_time < max_wait) {
        collectRidesInRange(root->right, min_wait, max_wait, result, index);
    }
}

/* Find rides with max wait */
Ride** findRidesWithMaxWait(BSTNode* root, int max_minutes, int* count) {
    Ride** result = (Ride**)malloc(sizeof(Ride*) * MAX_RIDES);
    *count = 0;
    
    if (result) {
        collectRidesInRange(root, 0, max_minutes, result, count);
    }
    
    return result;
}

/* Find rides in wait range */
Ride** findRideInWaitRange(BSTNode* root, int min_wait, int max_wait, int* count) {
    Ride** result = (Ride**)malloc(sizeof(Ride*) * MAX_RIDES);
    *count = 0;
    
    if (result) {
        collectRidesInRange(root, min_wait, max_wait, result, count);
    }
    
    return result;
}

/* Display BST inorder */
void displayBSTInorder(BSTNode* root) {
    printf("\n--- Rides by Wait Time ---\n");
    inorderTraversal(root);
    printf("--------------------------\n");
}

/* Display rides by wait time */
void displayRidesByWaitTime(BST* bst) {
    if (!bst || !bst->root) {
        printf("No rides in BST.\n");
        return;
    }
    
    displayBSTInorder(bst->root);
}

/* Count nodes */
int countNodes(BSTNode* root) {
    if (!root) return 0;
    return 1 + countNodes(root->left) + countNodes(root->right);
}

/* Free BST node */
void freeBSTNode(BSTNode* node) {
    if (node) {
        freeBSTNode(node->left);
        freeBSTNode(node->right);
        free(node);
    }
}

/* Free BST */
void freeBST(BST* bst) {
    if (bst) {
        freeBSTNode(bst->root);
        free(bst);
    }
}

/* Update ride wait time in BST */
void updateRideWaitTimeInBST(BST* bst, int ride_id, int new_wait_time) {
    if (!bst) return;
    
    bst->root = deleteRide(bst->root, ride_id);
    // Note: Need to reinsert with updated wait time
}

/* Rebuild BST */
void rebuildBST(BST* bst, RideList* rides) {
    if (!bst || !rides) return;
    
    freeBSTNode(bst->root);
    bst->root = NULL;
    bst->size = 0;
    
    RideNode* current = rides->head;
    while (current) {
        bst->root = insertRide(bst->root, current->ride);
        bst->size++;
        current = current->next;
    }
}

/* Get height */
int getHeight(BSTNode* node) {
    return node ? node->height : 0;
}

/* Get balance factor */
int getBalanceFactor(BSTNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

/* Rotate left */
BSTNode* rotateLeft(BSTNode* node) {
    if (!node || !node->right) return node;
    
    BSTNode* new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;
    
    return new_root;
}

/* Rotate right */
BSTNode* rotateRight(BSTNode* node) {
    if (!node || !node->left) return node;
    
    BSTNode* new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    
    return new_root;
}

/* Balance node */
BSTNode* balance(BSTNode* node) {
    if (!node) return NULL;
    
    int bf = getBalanceFactor(node);
    
    if (bf > 1) {
        if (getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }
    
    if (bf < -1) {
        if (getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }
    
    return node;
}
