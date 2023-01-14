/*  Deeb Armaly
 *  ECE 2230
 *  Fall 2021
 *  MP6
 *
 *  This file contains the implementations of the funcitons in table.h (function definitions given in table.h).
 *  These functions are to set up a table ADT using hashing with open addressing
 *  and three approaches to collision resolution policies. The design is modular like
 *  in MP5.
 *  For this table implementation, 0 is the key value for an empty block, and 1 is a key value
 *  for a block that has been deleted. 
 */

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "table.h"

//local functions

/*  This function determines which number is larger
 *  Inputs: two integers 
 *  Output: returns the larger of the two integers
 */
int max(int a, int b) {
	if (a > b) return a;
       	return b;
}



/*  Function to allocate memory for the table
 *  Inputs: 
 *  	-table_size: defines how many table elements the table can hold
 *  	-probe_type: defines the probing that will be used to handle collisions (linear|double|quad)
 *  Return: returns a pointer to the newly allocated table 
 */ 
table_t *table_construct(int table_size, int probe_type) {

	if(table_size < 2) return NULL; //table must fit table_size - 1 entries
	table_t *T;
	

	T = (table_t *)malloc(sizeof(table_t));
	T->table_ptr = (table_entry_t *)calloc(table_size, sizeof(table_entry_t)); //allocate table and initialize to zero
	T->size = table_size;
	T->probe_type = probe_type;
	T->num_keys = 0;
	T->num_probes = 0;
	
	int i;
	for(i = 0; i < table_size; i++) {
		T->table_ptr[i].key = 0;
	}

	return T;

}


/*  This function takes all keys from the old table and inserts them into a new table
 *  with size of new_table_size. All delete keys are removed when transferring to new table.
 *  Old tables memory is free'd upon completion of this function.
 *  Inputs:
 *  	- *T: pointer to the table 
 *  	- new_table_size: size of the new table 
 *  Return: returns the new table with all the keys from the old table transferred over
 */
table_t *table_rehash(table_t *T, int new_table_size) {

	if(T == NULL) return NULL;

	table_t *T_new;
	T_new = table_construct(new_table_size, T->probe_type);

	int i;
	for(i = 0; i < T->size; i++) { //insert all entries of old table into new table
		if(T->table_ptr[i].key == 1 || T->table_ptr[i].key == 0) {
			T_new->table_ptr[i].key = T_new->table_ptr[i].key;
		}
		else {
			table_insert(T_new, T->table_ptr[i].key, T->table_ptr[i].data_ptr);
		}
	}

	T_new->num_keys = T->num_keys;

	free(T->table_ptr);
	free(T);
	return T_new;
}


/*  This function returns the number of keys in a table
 *  Input:
 * 	- *T: pointer to the table
 *  Return: number of entries in table pointed to be *T.
 */
int table_entries(table_t *T) {
	if (T == NULL) return -1;

	return T->num_keys;
}


/*  This function determines if the table is full
 *  Input: 
 *  	- *T: pointer to the table
 *  Return:
 *  	- (-1): if table does not exist
 *  	- 1: if table is full
 *  	- 0: if table is not full
 */
int table_full(table_t *T) {

	if (T == NULL) return -1;

	if(T->num_keys == T->size-1)  return 1;

       	return 0;
}


/*  This function returns the amount of table locations marked as having
 *  been deleted.
 *  Input:
 *  	- *T: pointer to the table
 *  Return:
 *  	- (-1): if table does not exist yet
 *  	- delete_count: number of entries marked with the delete bit (1) as the key
 */
int table_deletekeys(table_t *T) {

	if(T == NULL) return -1;

	int delete_count = 0;
	int i;

	for(i = 0; i < T->size; i++) {
		if(T->table_ptr[i].key == 1) delete_count++;
	}

	return delete_count;
}


/*  This function inserts a new data block I into table T with key of K.
 *  Input:
 *  	- *T: pointer to the table
 *  	- K: hashkey used to compute the hash location as well as key identifier for info I
 *  	- I: pointer to data to be inserted in table
 *  Return: 
 *  	- (-1): if (K, I) cannot be inserted
 *  	- 0: if (K, I) is inserted
 *  	- 1: if an older (K, I) is already in the table (update with the new I)
 */
int table_insert(table_t *T, hashkey_t K, data_t I) {

	if(T == NULL) return -1;

	int full = table_full(T);

	int M = T->size;
        int i = K % M;
	int ProbeDecrement;

	if(T->probe_type == LINEAR) ProbeDecrement = 1;
	else if(T->probe_type == DOUBLE) ProbeDecrement = max(1, (K/M) % M);

	int num_probes = 1;
	int j = 0;
	int del_loc = -1; //save the location of a delete in case it needs to be returned to

	hashkey_t ProbeKey = T->table_ptr[i].key;
	while(ProbeKey != 0 && ProbeKey != K && num_probes < T->size) { // probe to find spot
		num_probes++;

		if(del_loc == -1 && ProbeKey == 1) del_loc = i;//save first deleted location

		if(T->probe_type == QUAD) {
			j++;
			i -= j;
			if(i < 0) i += M;
		}
		else {
			i -= ProbeDecrement;
			if(i < 0) i += M;
		}
		ProbeKey = T->table_ptr[i].key;
	}

	//if it has probed T->size amount of times then probe value is incompatible with the table size 
	if(ProbeKey != 0 && ProbeKey != K && del_loc == -1) { 
		printf("Table of size %d is incompatible with the probe sequence\n", M);
		exit(1);
	}

	T->num_probes = num_probes;

	if(ProbeKey == K) { //if key exists update it with new key
		free(T->table_ptr[i].data_ptr);
		T->table_ptr[i].data_ptr = I;
		return 1;
	}

	//if search results in no match but there was a previous open deleted location
	if(del_loc != -1 && (T->table_ptr[i].key == 0 || num_probes == T->size)) i = del_loc;

	if(full == 1) return -1; //if table is full and replacement is not an option, can not insert.

	T->num_keys++;
	T->table_ptr[i].key = K;
	T->table_ptr[i].data_ptr = I;
	return 0;

}


/*  Function to delete entry (K, I) from table T
 *  Input:
 *  	- *T: pointer to the table
 *  	- K: hashkey to be removed
 *  Return:
 *  	- NULL: if (K, I) not found in table
 *  	- I: pointer to data in table
 */
data_t table_delete(table_t *T, hashkey_t K) {

	int M = T->size;
        int i = K % M;
        int ProbeDecrement;

        if(T->probe_type == LINEAR) ProbeDecrement = 1;
        else if(T->probe_type == DOUBLE) ProbeDecrement = max(1, (K/M) % M);

        int num_probes = 1;
        int j = 0;

        hashkey_t ProbeKey = T->table_ptr[i].key;
        while(ProbeKey != 0 && ProbeKey != K && num_probes < T->size) { // probe to find spot
		num_probes++;

                if(T->probe_type == QUAD) {
                        j++;
                        i -= j;
                        if(i < 0) i += M;
                }
                else {
                        i -= ProbeDecrement;
                        if(i < 0) i += M;
                }
                ProbeKey = T->table_ptr[i].key;
        }

        T->num_probes = num_probes;

        if(T->table_ptr[i].key != K) return NULL;

	data_t temp = T->table_ptr[i].data_ptr;
	T->table_ptr[i].data_ptr = NULL;
	T->table_ptr[i].key = 1;
	T->num_keys--;
	return temp;

}


/*  Function to retrieve I from table T
 *  Input:
 *  	- *T: pointer to table
 *  	- K: haskey to retrieve data from
 *  Return:
 *  	- NULL: if information is not in table
 *  	- I: data for (K, I)
 */
data_t table_retrieve(table_t *T, hashkey_t K) {
	if(T == NULL) return NULL;

	int M = T->size;
        int i = K % M;
        int ProbeDecrement;

        if(T->probe_type == LINEAR) ProbeDecrement = 1;
        else if(T->probe_type == DOUBLE) ProbeDecrement = max(1, (K/M) % M);

        int num_probes = 1;
        int j = 0;

	hashkey_t ProbeKey = T->table_ptr[i].key;
        while(ProbeKey != 0 && ProbeKey != K && num_probes < T->size) { // probe to find spot
                num_probes++;

                if(T->probe_type == QUAD) {
                        j++;
			i -= j;
			if(i < 0) i += M;
                }
                else {
                        i -= ProbeDecrement;
                        if(i < 0) i += M;
                }
		ProbeKey = T->table_ptr[i].key;
        }

	T->num_probes = num_probes;

	if(T->table_ptr[i].key != K) return NULL;

	return T->table_ptr[i].data_ptr;
}


/*  Function that frees all allocated memory included in table T
 *  Input: 
 *  	- *T: pointer to the table
 *  Return:
 *  	- void
 */
void table_destruct(table_t *T) {
	if(T == NULL) return;

	int i;
	for(i = 0; i < T->size; i++) {
		if(T->table_ptr[i].key > 1) free(T->table_ptr[i].data_ptr);
	}
	free(T->table_ptr);
	free(T);
	T = NULL;
}


/*  Function to return the number of probes from the most recent call to 
 *  table_insert, table_delete, or table_retreive
 *  Input:
 *  	- *T: pointer to the table
 *  Return: 
 *  	- 0: if table does not exist
 *  	- T->num_probes: number of probes from most recent call
 */
int table_stats(table_t *T) {
	if(T == NULL) return 0;

	return T->num_probes;
}


/*  Function that returns value of the key if there is data at index location
 *  Input: 
 *  	- *T: pointer to table
 *  	- index: index value in table to check for data
 *  Return:
 *  	- 0: if there is no data stored at that index location
 *  	- data_ptr: the pointer to the data stored at T->table_ptr[index].data_ptr
 */
hashkey_t table_peek(table_t *T, int index) {
	assert(0 <= index && index < T->size);

	if(T->table_ptr[index].data_ptr == NULL) return 0;

	return T->table_ptr[index].key;
}


/*  Function that prints the contents of the table showing index and key
 *  in a (index, key) format
 *  Input: 
 *  	- *T: pointer to the table
 *  Return:
 *  	- void function
 */
void table_debug_print(table_t *T) {
	if(T == NULL) return;

	printf("Printing table keys and positions\n");
	int i;
	for(i = 0; i < T->size; i++) {
		if(T->table_ptr[i].key == 1) printf("(%d, DEL)\n", i);
		else if(T->table_ptr[i].key == 0) printf("(%d, EMPTY)\n", i);
		else printf("(%d, %d)\n", i, T->table_ptr[i].key);
	}
}
