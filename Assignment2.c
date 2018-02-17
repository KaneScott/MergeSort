#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int listSizeCount = 0;

typedef struct JobList JOB;
struct JobList {
	int id;
	int burstLength;
	int waitTime;
	JOB *next;
};

/**
 * Creates a job for use within a JobList. A job represents a program in memory.
 */
JOB *create_JobList(int id, int burstLength, int waitTime) {
	JOB *j = (JOB *) malloc(sizeof(JOB));
	j->id = id;
	j->burstLength = burstLength;
	j->waitTime = waitTime;
	j->next = NULL;
	return j;
}

/**
 * Helper method for the merge_sort function.
 */
JOB *merge(JOB *first, JOB *second) {
	JOB *dummyHead, *curr;
	dummyHead = create_JobList(0, 0, 0);
	curr = dummyHead;
	while (first != NULL && second != NULL) {
		if (first->burstLength <= second->burstLength) {
			curr->next = first;
			first = first->next;
		} else {
			curr->next = second;
			second = second->next;
		}
		curr = curr->next;
	}
	curr->next = (first == NULL) ? second : first;
	return dummyHead->next;
}

/**
 * Secondary helper method for the merge_sort function.
 */
JOB *getMid(JOB *head) {
	if (head == NULL)
		return head;
	JOB *slow, *fast;
	slow = fast = head;
	while (fast->next != NULL && fast->next->next != NULL) {
		slow = slow->next;
		fast = fast->next->next;
	}
	return slow;
}
/**
 * Sorts the specified linkedlist via mergesort. The mergesort algorithm sorts the list via burstLength size, jobs with lower burstLength coming first.
 */
JOB *merge_sort(JOB *list) {
	if (list == NULL || list->next == NULL)
		return list;
	JOB *mid = getMid(list);
	JOB *sHalf = mid->next;
	mid->next = NULL;

	return merge(merge_sort(list), merge_sort(sHalf));
}

/**
 * Inserts a job within the linked list. This method inserts jobs first-come-first-served (e.g. unsorted list).
 */
JOB *insert(JOB *list, JOB *job) {
	if (list == NULL)
		return job;
	list->next = insert(list->next, job);
	return list;
}

/**
 * Inserts a job within the linked list. The linked list is sorted via the Job ID field, with the smallest ID coming first within the linked list. NO LONGER USED DUE TO ASSIGNMENT SPECIFICATION.
 * *list - JobList to insert Job into.
 * *job - Job to be inserted into JobList.
 */
JOB *insertSorted(JOB *list, JOB *job) {
	if (list == NULL)
		return job;
	if (job->burstLength < list->burstLength) {
		job->next = list;
		return job;
	}
	list->next = insertSorted(list->next, job);
	printf("\n");
	return list;
}

/**
 * Prints information of all jobs held within the linked list.
 */
void traverse(JOB *list) {
	if (list == NULL) {
		return;
	}
	printf("ID: %d\tBurstLength: %d\tWaitTime: %d\n", list->id,
			list->burstLength, list->waitTime);
	traverse(list->next);
}

/**
 * Returns a job at specified index within the linked list.
 */
JOB * searchIndex(JOB *list, int index) {
	JOB *tmp = list;
	int i;
	for (i = 0; i <= index; i++) {
		if (i == index)
			return tmp;
		else
			tmp = tmp->next;
	}
	return NULL;
}

/**
 * Returns a job with specified id from within the linked list.
 */
JOB * search(JOB *list, int id) {
	if (list == NULL)
		return NULL;
	if (list->id == id)
		return list;
	return search(list->next, id);
}

/**
 * Deletes a job at specified index within the linked list, returns a copy of the deleted list.
 */
JOB * deleteIndex(JOB *list, int index) {
	JOB *tmp = list;
	if (list == NULL)
		return NULL;
	if (index == 0) {
		listSizeCount--;
		free(list);
		list = tmp->next;
		return list;
	}
	int i;
	for (i = 1; i <= index; i++) {
		if (i == index) {
			JOB *remove = tmp->next;
			listSizeCount--;
			free(remove);
			tmp->next = remove->next;
			return remove;
		} else {
			tmp = list->next;
		}
	}
	return NULL;
}

/**
 * Deletes a job with specified id from within the linked list, returns a copy of the deleted list.
 */
JOB * delete(JOB *list, int id) {
	if (list == NULL)
		return NULL;
	if (list->id > id)
		return NULL;
	if (list->id == id) {
		JOB *tmp = list->next;
		free(list);
		return tmp;
	}
	list->next = delete(list->next, id);
	return list;
}

/**
 * Deletes all jobs within the linkedlist, freeing used memory.
 */
void deleteALL(JOB *list) {
	while (listSizeCount > 0) {
		delete(list, listSizeCount);
		listSizeCount--;
	}
}

int main(int argc, char **argv) {
	JOB *list = NULL;
	if (argc != 3) {
		perror(
				"Incorrect commandline arguments.\nFirst argument specifies file to be read.\nSecond argument is either F or S, representing FCFS and SJF respectively.");
		exit(0);
	}
	//Read specified file
	FILE *fp = fopen(argv[1], "r");
	//Char that splits token
	const char s[2] = ", ";
	char *token;
	if (fp != NULL) {
		char line[20];
		while (fgets(line, sizeof line, fp) != NULL) {
			//Get first token on line
			token = strtok(line, s);
			int id;
			//Check if token is placeholder i value
			if (strcmp(token, "i") == 0)
				id = -1;
			else
				id = atoi(token);
			//Get second token on line
			token = strtok(NULL, s);
			int burstLength;
			//Check if token is placeholder b value
			if (strcmp(token, "b") == 0)
				burstLength = -1;
			else
				burstLength = atoi(token);
			//If placeholder values exist on line, ignore line. Otherwise, create a new job and add it to the linkedlist.
			if (id > -1 && burstLength > -1) {
				JOB *newJob = create_JobList(id, burstLength, 0);
				listSizeCount++;
				list = insert(list, newJob);
			}

		}
		fclose(fp);
	} else {
		//Print error and close program if no file is specified.
		perror("No file specified");
		exit(-1);
	}
	//If s is specified in commandline, use SJF, else use FCFS.
	if (strcmp(argv[2], "s") == 0)
		list = merge_sort(list);
	JOB *tmp = list;
	float waitTime = 0;
	float totalWaitTime = 0;
	float count = 0;
	//Calculate appropriate waitTimes.
	while (tmp != NULL) {
		tmp->waitTime = waitTime;
		totalWaitTime += waitTime;
		waitTime += tmp->burstLength;
		count++;
		tmp = tmp->next;
	}
	//Display linkedlist values for program checking on small linkedlists.
	if (listSizeCount < 20) {
		traverse(list);
		printf("\n\n");
	}
	//Display average wait time.
	printf("Average Wait Time: %0.2f\n", totalWaitTime / count);
	//Remove all values in linkedlist to avoid memory leakage.
	deleteALL(list);
}

