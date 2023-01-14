/* ens_support.c
 * Prof. Russell          <<-- many updates required including your name
 * harlanr
 * ECE 2230 Fall 2021
 * MP2
 *
 * Propose: A template for MP2 
 *
 * Assumptions: suggested structure definitions and public functions 
 *              as defined for assignmnet.  
 *
 *              You must change this file for your design.
 *
 *              I have provided prototypes of some of the functions
 *              and you CANNOT change these prototypes
 *
 *              However, you can add additional functions to handle
 *              operations on the lists.
 *
 * Bugs:
 *
 * Correction:
 *
 * You must change this file for your design.
 *
 * (You CANNOT modify any of the details of the linkedlist.h interface, or use any 
 *  of the private variables outside of linkedlist.c.)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "linkedlist.h"
#include "ens_support.h"

#define MAXLINE 256

// private functions for use in ens_support.c only
epid_rec_t *private_make_rec(int id);   // make record and collect input from user
void private_print_rec(epid_rec_t *rec);  // print one record

/* ens_compare_epid is required by the list ADT for sorted lists. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the front than rec_b,
 *    -1 if rec_b is to be considered closer to the front, and 
 *     0 if the records are equal for sorting.
 *
 * For the alert data we want to sort from lowest ID up, so
 * closer to the front means a smaller ID.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int ens_compare_epid(const epid_rec_t *rec_a, const epid_rec_t *rec_b)
{
    assert(rec_a != NULL && rec_b != NULL);
    if (rec_a->ep_id < rec_b->ep_id)
        return 1;
    else if (rec_a->ep_id > rec_b->ep_id)
        return -1;
    else
        return 0;
}

/* ens_compare_recnum can be used with linked_elem_remove_extremum. 
 *
 * This function returns 
 *     1 if rec_a should be considered an extremum compared to rec_b,
 *     0 if the comparison fields of the two records are equal.
 *    -1 if rec_b is the relative extremum 
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 */
int ens_compare_recnum(const epid_rec_t *rec_a, const epid_rec_t *rec_b)
{
    assert(rec_a != NULL && rec_b != NULL);
    if (rec_a->rec_num < rec_b->rec_num)
        return 1;
    else if (rec_a->rec_num > rec_b->rec_num)
        return -1;
    else
        return 0;
}
/* print one of the EP ID record lists
 *
 * inputs: 
 *    list_ptr: a pointer to either sorted or unsorted list
 *
 *    type_of_list: a charter string that must be either "Contact" or "Exposure"
 *
 * output: prints the list in the format required for grading.
 *         Do not change any of these prints 
 */
void ens_print(linked_t *list_ptr, const char *list_type)
{

    //check if list exists
    if(list_ptr == NULL) return;
    assert(strcmp(list_type, "Contact")==0 || strcmp(list_type, "Exposure")==0);
    int num_in_list = list_ptr->current_list_size;  
    int index;
    if (num_in_list == 0) {
        printf("%s list is empty\n", list_type);
    } else {
        printf("%s list contains %d record%s\n", list_type, num_in_list,
                num_in_list==1 ? "." : "s.");
    }
    for (index = 0; index < num_in_list; index++) {
        printf("%d: ", index+1);
        // you must use the ens_print_rec function to format output 
        epid_rec_t *rec_ptr = linked_access(list_ptr, index);   // fixed
        private_print_rec(rec_ptr);
    }
    printf("\n");
}

/* This function removes all EP IDs from the unsorted exposure list.
 *
 * Do not print each record.  Print the number of records removed
 *
 * list_ptr: exposure list
 */
void ens_remove_exposures(linked_t *list_ptr)
{
    //check if list exists
    if(list_ptr == NULL) return;
    int num_recs = 0;
    int i;
    data_t *dptr = NULL;

    //loop through list deleting all records and freeing the data
    for(i = 0; i < list_ptr->current_list_size; i++) {
	    dptr = linked_remove(list_ptr, i);
	    free(dptr);
	    dptr = NULL;
	    num_recs++;
	    i--;
    }

    if (num_recs > 0)
        printf("Deleted %d confirmed exposures\n", num_recs); 
    else
        printf("Exposure queue empty, did not remove\n");
}

/* This creates a list and it can be either a sorted or unsorted list.
 * If the list is unsorted, a comparison function is not needed.  
 *
 * inputs: 
 *    list_type: a character string that must be either "Contact" or "Exposure"
 *
 * This function is provided to you as an example of how to use a
 * function pointer as a parameter in a function call.  
 *
 */
linked_t *ens_create(const char *list_type)
{
    assert(strcmp(list_type, "Contact")==0 || strcmp(list_type, "Exposure")==0);
    if (strcmp(list_type, "Contact") == 0) {
        return linked_construct(ens_compare_epid);
    } else if (strcmp(list_type, "Exposure") == 0) {
        return linked_construct(NULL);
    } else {
        printf("ERROR, invalid list type %s\n", list_type);
        exit(1);
    }
}

/* This function adds an ephemeral ID record to the contact list.  The list is
 * sorted based on ep_id.
 *
 * Inputs:
 *      list_ptr      : pointer to header block of sorted list of ep_id's
 *      id            : ephemeral ID from ADDIN
 *      max_list_size : The maximum number of records that are permitted
 *                      to be stored in the contact list.  
 *
 * If the new EP ID has the same ep_id as a record already in the contact list,
 * then the old record is removed and discarded and the new record is inserted
 * in its place
 *
 * If the list is full, and the record is NOT a duplicate, then remove the
 * record in the list with the smallest rec_num but do NOT print the record. 
 * Then add the new record in its correct sorted position.
 *
 * If this function is called but the list does not yet exist, then gather all
 * the details of the EP ID record from the input.  However, discard the record
 * because the list does not exist and print the corresponding message
 *
 * There is no return value since the operation required for this function 
 * must always be successful, except in the catastrophic condition that the 
 * program has run out of memory.
 *
 * Hint: you must use linked_insert_sorted.  If the list is full and the 
 *       smallest rec_num is to be removed use linked_elem_remove_extremum.
 */
void ens_add(linked_t *list_ptr, const int id, const int max_list_size)
{
    int add_result = -2;

    //check if list exists
    if(list_ptr == NULL) return;

    epid_rec_t *new_ptr = private_make_rec(id);

    // after the IP id record is created you must determine the type of 
    // insertion is required and print the associated message.
    
    data_t *data_ptr = NULL;
    int pos;

    //declare function pointers to point to ens compare functions
    int (*comp_ptr)(const epid_rec_t *, const epid_rec_t *) = &ens_compare_epid;
    int (*rec_comp)(const epid_rec_t *, const epid_rec_t *) = &ens_compare_recnum;

    //see if list exists
    if(list_ptr == NULL) {
	    free(new_ptr);
	    new_ptr = NULL;
	    add_result = 0;
    }

    //do the following if list exists
    if(list_ptr != NULL) {
	    data_ptr = linked_elem_find(list_ptr, new_ptr, &pos, (*comp_ptr));
    	//handle case of match
    	if(data_ptr != NULL) {
	    linked_remove(list_ptr, pos);
	    linked_insert_sorted(list_ptr, new_ptr);
	    free(data_ptr);
	    data_ptr = NULL;
	    add_result = 2;
    	}
    	// check to see if list is full
    	else if(data_ptr == NULL && list_ptr->current_list_size == max_list_size) {
	    data_ptr = linked_elem_remove_extremum(list_ptr, (*rec_comp));
	    free(data_ptr);
	    linked_insert_sorted(list_ptr, new_ptr);
	    add_result = 3;
    	}
    	//if neither of the special cases just insert normally
    	else {
	    linked_insert_sorted(list_ptr, new_ptr);
	    add_result = 1;
    	}
    }
	

    if (add_result == 1) {
        printf("Inserted: %d\n", new_ptr->ep_id);
    } else if (add_result == 2) {
        printf("Updated: %d\n", new_ptr->ep_id);
    } else if (add_result == 3) {
        printf("Removed old record and added: %d\n", new_ptr->ep_id);
    } else if (add_result == 0) {
        printf("The list does not exist.  Record rejected: %d\n", new_ptr->ep_id);
    } else {
        printf("Error with return value! Fix your code.\n");
    }
}

/* This function searches the contact list for a matching record for the given EP ID.
 *
 * Inputs:
 *    list_ptr: pointer to header block of a list
 *    id: the IP ID to search for
 *
 *    Hint: linked_elem_find should be used
 */
void ens_print_epid(linked_t *list_ptr, int id)
{
    //check if list exists
    if(list_ptr == NULL) return;

    int num_in_list = list_ptr->current_list_size;   // fixed
    int pos = 0;

    int (*comp_ptr)(const epid_rec_t *, const epid_rec_t *) = &ens_compare_epid;
    epid_rec_t *temp_elem_ptr = (epid_rec_t *)malloc(sizeof(epid_rec_t));
    temp_elem_ptr->ep_id = id;

    if (num_in_list == 0) {
        printf("List empty. Did not find ephemeral ID %d\n", id);
    } else {
        printf("List has %d records.  Looking for EPID %d\n", num_in_list, id);
        // lookup EPID and print if found
        epid_rec_t *rec_ptr = linked_elem_find(list_ptr,temp_elem_ptr, &pos,(*comp_ptr));
        if (rec_ptr != NULL) {
            private_print_rec(rec_ptr);
        } else {
            printf("Did not find ephemeral ID %d\n", id);
        }
    }
    free(temp_elem_ptr);
    temp_elem_ptr = NULL;
}

/* This function prints all recent exposure records 
 *
 * list_ptr: the unsorted list of EP ID records that matched for exposure
 * time:     only print records with time_recevied value >= time
 */
void ens_list_exposures(linked_t *list_ptr, int time)
{

    //check if list exists
    if(list_ptr == NULL) return;

    int count = 0;
    // First, you must print record that is recent with time_received >= time
    int i;
    data_t *dptr;

    for(i = 0; i < list_ptr->current_list_size; i++) {
	    dptr = linked_access(list_ptr, i);
	    if(dptr->time_received >= time) {
		    private_print_rec(dptr);
		    count++;
	    }
    }

    // after printing each matching record, print one of these summaries
    if (count > 0)
        printf("Found %d exposures from time %d or later\n", count, time);
    else
        printf("Did not find any recent exposures: %d\n", time);
}

/* This function removes all IP ID records from the contact list that have
 * a time_received value that is smaller than cut_off_time 
 * Do not print the records that are removed.  
 *
 * Inputs:
 *     contact_list: sorted list of contact EP ID records
 *     cut_off_time: records with a time_received < cut_off_time are moved
 */
void ens_remove_old(linked_t *contact_list, int cut_off_time)
{

    //check if list exists
    if(contact_list == NULL) return;
    int num_removed = 0;
    int i;
    data_t *dptr = NULL;

    // remove all records from contact list that are older than cut_off_time
    for(i = 0; i < contact_list->current_list_size; i++) {
	    dptr = linked_access(contact_list, i);
	    if(dptr->time_received < cut_off_time) {
		    dptr = linked_remove(contact_list, i);
		    free(dptr);
		    num_removed++;
		    i--;
	    }
    }

    if (num_removed == 0) {
        printf("No old records found: %d\n", cut_off_time);
    } else {
        printf("Removed %d records older than %d\n", num_removed, cut_off_time);
    }
}

/* This function removes the EP ID record with the smallest rec_num value.  
 * The record is discarded and not put on the confirmed exposure list.  
 *
 * The function prints the EP ID of the record that was removed
 *
 * Hint: use the linked_elem_remove_extremum function
 */
void ens_trim(linked_t *contact_list)
{

    //check if list exists
    if(contact_list == NULL) return;

    int ep_id_removed = -1;
    int (*comp_ptr)(const epid_rec_t *, const epid_rec_t *) = &ens_compare_recnum;
    data_t *dptr = NULL;

    dptr = linked_elem_remove_extremum(contact_list, (*comp_ptr));

    // remove record with the smalled rec_num.
    if(dptr != NULL) {
	    ep_id_removed = dptr->ep_id;
	    free(dptr);
	    dptr = NULL;
    }

  
    if (ep_id_removed == -1) {
        printf("The list is empty and no ID removed\n");
    } else {
        printf("Removed oldest EPID %d\n", ep_id_removed);
    }
}

/* This function removes one EP ID record from the contact list that
 * matches the provided EP ID. 
 *
 * If a record is removed for the sorted contact list, then it  should be 
 * appended to the tail of the unsorted exposure list.
 *
 * Inputs:
 *     contact_list: sorted list of contact EP ID records
 *     exposure_list: unsorted exposure list 
 *     match_id: EP ID with a report of an exposure 
 *
 * Hint: linked_elem_find should be used
 */
int ens_match(linked_t *contact_list, linked_t *exposure_list, int match_id)
{

    //check if list exists
    if(contact_list == NULL || exposure_list == NULL) return 0;
	
    int found = 0;
    int pos = 0;
    int (*comp_ptr)(const epid_rec_t *, const epid_rec_t *) = &ens_compare_epid;

    epid_rec_t *rec_ptr = NULL;  // look for a match
    data_t *dptr = (data_t *) malloc(sizeof(data_t));
    dptr->ep_id = match_id;

    //see if there is a match
    rec_ptr = linked_elem_find(contact_list, dptr, &pos, (*comp_ptr));


    if (rec_ptr != NULL) {
        found = 1;
        private_print_rec(rec_ptr);
	//remove reocrd from contact list
	rec_ptr = linked_remove(contact_list, pos);
	//insert record in exposure list
	linked_insert(exposure_list, rec_ptr, LLPOSITION_BACK);
    }
    free(dptr);

    return found;
}

/* the function takes a pointer to each list and prints the
 * number of items in each list
 */
void ens_stats(linked_t *sorted, linked_t *unsorted)
{
    //see if list exists
    if(sorted == NULL || unsorted == NULL) return;
    // get the number in list and size of the list
    int num_in_sorted_list = sorted->current_list_size;
    int num_in_unsorted_list = unsorted->current_list_size;
    printf("Number contacts: %d, exposures: %d\n", 
            num_in_sorted_list, num_in_unsorted_list);
}

/* this function frees the memory for either a sorted or unsorted list.
 */
void ens_cleanup(linked_t *list_ptr)
{
    // saftey: if the list does not exist do nothing
    if (list_ptr == NULL) 
        return;

    linked_destruct(list_ptr);
}

/* Prompts user for EP ID record input.
 * The input is not checked for errors but will default to an acceptable value
 * if the input is incorrect or missing.
 *
 * A static variable is used to assign a sequence number to each record that
 * is created.  That implies this function cannot be used to generate 
 * temporary records.
 *
 * The input to the function is the EP ID.
 *
 * There output is the dynamic memory block with the data filled in.
 *
 * Do not change the sequence of prompts as grading requires this exact
 * format
 */
epid_rec_t *private_make_rec(int id)
{
    char line[MAXLINE];
    char str[MAXLINE];
    epid_rec_t *new; 
    static int seq_num = 0;   // automatically track each record created

    new = (epid_rec_t *) calloc(1, sizeof(epid_rec_t));
    assert(new != NULL);
    seq_num++;

    new->ep_id = id;
    new->rec_num = seq_num;
    printf("time received:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->time_received);
    printf("source type:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->source_type);
    printf("MAC address:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->mac_addr);

    printf("Authenticated (T/F):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "T")==0 || strcmp(str, "t")==0)
        new->authenticated = 1;
    else
        new->authenticated = 0;

    printf("Privacy (none|standard|strong|NSA):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%s", str);
    if (strcmp(str, "standard")==0)
        new->privacy = 1;
    else if (strcmp(str, "strong")==0)
        new->privacy = 2;
    else if (strcmp(str, "NSA")==0)
        new->privacy = 3;
    else
        new->privacy = 0;

    printf("Channel:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%d", &new->channel);

    printf("RSSI:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->rssi);

    printf("Band (2.4|3.5|5.0):");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%f", &new->band);
    printf("\n");
    return new;
}

/* print the information for a particular alert record 
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * Do not change any of these lines and grading depends on this
 * format.
 */
void private_print_rec(epid_rec_t *rec)
{
    const char *pri_str[] = {"none", "standard", "strong", "NSA"};
    assert(rec != NULL);
    printf("EPID: %d, Rec# %d, Time: %d", rec->ep_id, rec->rec_num, rec->time_received);
    printf(" Src: %d, MAC: %d,", rec->source_type, rec->mac_addr);
    printf(" Auth: %s,", rec->authenticated ? "T" : "F"); 
    printf(" Pri: %s, C: %d, RSSI: %g, B: %g\n", pri_str[rec->privacy], 
            rec->channel, rec->rssi, rec->band);
}

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
