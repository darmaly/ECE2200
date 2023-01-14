/* tree.h
 * Lab 5: Binary Search Trees
 * ECE 2230 Fall 2021
 *
 * This file contains all the key definitions of the data structures as well as
 * function prototypes to be used in tree.c file. 
 */


enum balanceoptions {AVL, BST, TWOTHREET};

typedef int KeyType;
typedef void *DataType;

typedef struct NodeTag {
	KeyType NodeItem;
	DataType DataPtr;
	struct NodeTag *LeftLink;
	struct NodeTag *RightLink;
} TreeNode;

typedef struct TreeTag {
	TreeNode *Root;
	int BalanceOption;
	int ItemCount;
	int RecentComparisonCount;
} Tree;


/* Creates the header block for the tree and saves the BalanceOption in the header block
 * Initializes root pointer to NULL 
 */
Tree *tree_construct (int BalanceOption);


/* Frees all items stored in the tree including the memory block with the data and the TreeNode
 * structure. Also Frees the header block
 */
void tree_destruct(Tree *);


/* Inserts block pointed to by DataPointer into the the tree with the assosciated Key
 * The function returns 0 if the key is already in the tree (memory block is replaced)
 * Function returns a 1 if the key was not already in the tree and was added to it
 */
int tree_insert (Tree *, KeyType Key, DataType DataPointer);


/* Removes an item from the tree with matching key
 * Returns pointer to the data memory block and frees the TreeNode memory block.
 * If key is not found in tree then it returns NULL
 */
DataType tree_remove(Tree *, KeyType Key);

/* Finds the tree element wiht the matching key 
 * Returns NULL pointer if key is not found in tree
 * Returns a pointer to the data block that is stored in this node in tree if found
 */
DataType tree_search (Tree *, KeyType Key);


/* Returns number of keys in the tree
 * Takes a pointer to the tree header
 */
int tree_count (Tree *);


/* This function returns RecentComparisonCount, which is the number of
 * key comparisons for the most recent call to 
 * tree_insert, tree_remove, or tree_search
 * Takes a pointer to the header block of the tree
 */
int tree_stats (Tree *);


/* Returns the path length of a tree
 * takes a pointer to the header block of the tree
 */
int tree_internal_path_len (Tree *);


/* Debugging Functions */
void tree_debug_print_tree (Tree *);
void tree_debug_validate (Tree *);

