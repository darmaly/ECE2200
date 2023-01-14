/* enslist.c
 * Deeb Armaly
 * darmaly
 * ECE 2230 Fall 2021
 * MP1
 *
 * See ECE 2230 programming guide for required comments!
 *
 * You must have comments at the top of the file.
 *
 * EVERY function must have comments 
 * 
 * This program contains all the functions that manipulate the list of structures
 * containing the epid records.
 */

// The ONLY members you can access in a epid_record_t block is
// ep_id, rec_num, or time_received.  All other members in this structure cannot be used
// in this file.

#include <stdlib.h>

#include "enslist.h"


/*
  This function sets the size for the enslist using
  integer input and outputting a pointer to the list
*/
struct enslist_t *enslist_construct(int size)
{

  struct enslist_t *enslist_ptr;
  int i;

  enslist_ptr = (struct enslist_t *) malloc(sizeof(struct enslist_t));
  enslist_ptr->ens_size_list = size;

  enslist_ptr->epids_ptr = (struct epid_record_t **) malloc(size * sizeof(struct epid_record_t *));

  for(i = 0; i < size; i++) {
	enslist_ptr->epids_ptr[i] = NULL;
  }
  enslist_ptr->num_epids = 0;

  return enslist_ptr;
}

/*
  This function frees the list memory using a pointer to the
  list as input and has no return value
*/
void enslist_destruct(struct enslist_t *ptr)
{

  //check to see if list has been created
  if(ptr == NULL) {
	  return;
  }

  int i;

  for(i = 0; i < ptr->ens_size_list; i++) {
	free(ptr->epids_ptr[i]);
	ptr->epids_ptr[i] = NULL;
  }

  free(ptr->epids_ptr);
  free(ptr);

}

/*
  This function adds an ID to the list in sequential order
  Input is a pointer to enslist_t and a pointer to the ID we would like to add
  return values are as follows
  1: inserted new record into list
  2: replaced record in list
  3: List is full, removed smallest rec_num and added new record
  0: Error, list does not exist
*/
int enslist_add(struct enslist_t *list_ptr, struct epid_record_t *rec_ptr)
{

  int i, temp, loc;
  struct epid_record_t *temp_ptr;

  //first we check to see if list exists
  if(list_ptr == NULL)
	return 0;

  //check to see if the new item is a duplicate
  for(i = 0; i < list_ptr->num_epids; i++) {
	if(list_ptr->epids_ptr[i]->ep_id == rec_ptr->ep_id){
	  free(list_ptr->epids_ptr[i]);
	  list_ptr->epids_ptr[i] = rec_ptr;
	  return 2;
	}
  }

  //check to see if list is full
  if(list_ptr->ens_size_list == list_ptr->num_epids) {
	temp = list_ptr->epids_ptr[0]->rec_num;
	loc = 0;
	
	if(list_ptr->num_epids > 1) {
	  for(i = 1; i < list_ptr->num_epids; i++) {
	    if(list_ptr->epids_ptr[i]->rec_num < temp) {
		temp = list_ptr->epids_ptr[i]->rec_num;
		loc = i;
	    }
	  }
	}
        /*
	//loop to close gap in record
	for(i = loc; i < list_ptr->num_epids-1; i++) {
	  list_ptr->epids_ptr[i] = list_ptr->epids_ptr[i+1];
	}
	*/
	free(list_ptr->epids_ptr[loc]);
	list_ptr->epids_ptr[loc] = rec_ptr; //inserts new record at top of list
	
	temp = i;
	//insert new record in correct place
	for(i = list_ptr->num_epids-1; i >= 1; i--) {
	  if(list_ptr->epids_ptr[i]->ep_id < list_ptr->epids_ptr[i-1]->ep_id) {
		temp_ptr = list_ptr->epids_ptr[i];
		list_ptr->epids_ptr[i] = list_ptr->epids_ptr[i-1];
		list_ptr->epids_ptr[i-1] = temp_ptr;
	  }
	}
	
	
	return 3;
  }

  //if none of the above, just add entry to list in correct location
  loc = 0;
  for(i = 0; i < list_ptr->num_epids;i++) {
	  if(list_ptr->epids_ptr[i]->ep_id < rec_ptr->ep_id) loc = i + 1;
  }

  list_ptr->num_epids++;
 
  //make room for the new record in list
  for(i = list_ptr->num_epids-1; i > loc; i--) {
	  list_ptr->epids_ptr[i] = list_ptr->epids_ptr[i-1];
  }

  list_ptr->epids_ptr[loc] = rec_ptr;

  return 1;
}

/*
  This function finds a record with a specific ep_id with a
  pointer to the list and the desired id number to find as inputs
  and outputs a pointer to the record with that ep_id or NULL if
  the ep_id is not found in the list
*/
struct epid_record_t *enslist_lookup(struct enslist_t *list_ptr, int id)
{

  //first we check to see if list exists
  if(list_ptr == NULL)
        return NULL;

  int i;
  for(i = 0; i < list_ptr->num_epids; i++) {
	if(list_ptr->epids_ptr[i]->ep_id == id) {
	  return list_ptr->epids_ptr[i];
	}
  }

  return NULL;
}

/*
  This function returns a pointer to the record at the specified index.
  The inputs are a pointer to the list and the desired index.
  The output is a pointer to the record at that index or if it does not exist
  NULL is returned
*/
struct epid_record_t *enslist_access(struct enslist_t *list_ptr, int index)
{

  
  if(index < 0 || index > list_ptr->num_epids) {
	return NULL;
  }
  return list_ptr->epids_ptr[index];

}

/*
  This function removes a record with the specified ep_id
  It takes a pinter to the list structer and the desired ep_id to be removed
  as inputs and returns the removed record
*/
struct epid_record_t *enslist_remove_epid(struct enslist_t *list_ptr, int id)
{

  //check to see if list is created
  if(list_ptr == NULL) return NULL;

  int loc = 0;
  int i, check = 0;
  struct epid_record_t *temp_ptr;
  //search to see if there is a matching id in list
  for(i = 0; i < list_ptr->num_epids; i++) {
	  if(list_ptr->epids_ptr[i]->ep_id == id) {
            check++;
	    loc = i;
	    temp_ptr = list_ptr->epids_ptr[i];
	  }
  }

  //if there is no match return null
  if(check == 0) {
	  return NULL;
  }

  //free(list_ptr->epids_ptr[loc]);
  //shift down
  for(i = loc; i < list_ptr->num_epids; i++) {
	  list_ptr->epids_ptr[i] = list_ptr->epids_ptr[i+1];
  }
  list_ptr->num_epids--;
  list_ptr->epids_ptr[i] = NULL;
  return temp_ptr;
}

/*
  This function removes all the records with a time value less than the one specified
  The inputs consist of a pointer to the list and the time desired
  The return value is the number of records removed from the list 
*/
int enslist_remove_old(struct enslist_t *list_ptr, int time)
{

  int i, j, loc, count = 0;

  //check to see if list has been created
  if(list_ptr == NULL) return 0;

  //find all values with time less than time specified and then shift list down
  for(i = list_ptr->num_epids - 1; i >= 0; i--) {
	  if(list_ptr->epids_ptr[i]->time_received < time) {
	    loc = i;
	    count++;
	    free(list_ptr->epids_ptr[loc]);
	    //list_ptr->epids_ptr[loc] = NULL;
	    //close gap on element with time less than that specified 
	    for(j = loc; j < list_ptr->num_epids - 1; j++) {
		    //free(list_ptr->epids_ptr[j]);
		    list_ptr->epids_ptr[j] = list_ptr->epids_ptr[j+1];
	    }
	
	    list_ptr->epids_ptr[j] = NULL;
	    list_ptr->num_epids--;
	  }
  }

  return count;

}

/*
  This function removes the record with the smallest rec_num
  The input is a pointer to the list
  The output is the ep_id of the removed record
*/
int enslist_remove_smallest(struct enslist_t *list_ptr)
{
  //check if list has been created
  if(list_ptr == NULL || list_ptr->epids_ptr == NULL){ 
	  return -1;
  }
  
  int i, loc = 0;
  unsigned int num = list_ptr->epids_ptr[0]->rec_num;
  //find location of smallest rec_num
  if(list_ptr->num_epids > 1) {
    for(i = 1; i < list_ptr->num_epids; i++) {
	if(list_ptr->epids_ptr[i]->rec_num < num) {
	  loc = i;
	  num = list_ptr->epids_ptr[i]->rec_num;
	}
  }
  }

  int id = list_ptr->epids_ptr[loc]->ep_id;
  free(list_ptr->epids_ptr[loc]);  
  //close gap in record
  for(i = loc; i < list_ptr->num_epids - 1; i++) {
	list_ptr->epids_ptr[i] = list_ptr->epids_ptr[i+1];
  }
  list_ptr->epids_ptr[i] = NULL;
  list_ptr->num_epids--;
  return id;
}

/*
  This function takes a pointer to the list as input
  The output of the function is the size of the list
*/
int enslist_list_size(struct enslist_t *list_ptr)
{
  //check to see if list has been created
  if(list_ptr == NULL) return 0;

  return list_ptr->ens_size_list;
}

/*
  This function receives a pointer to the list as input
  The output is the number of id's currently in the list
*/
int enslist_id_count(struct enslist_t *list_ptr)
{
  //check to see if list has been created
  if(list_ptr == NULL) return 0;

  return list_ptr->num_epids;
   
}

