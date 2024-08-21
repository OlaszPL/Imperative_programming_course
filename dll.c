#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

// list node
typedef struct Node {
	int *data;
	size_t size;
	struct Node* next;
	struct Node* prev;
} Node;

// doubly linked list
typedef struct List {
	Node *head;
	Node *tail;
} List;

// iterator
typedef struct iterator {
	struct Node* node_ptr; // pointer to the node
	size_t position; // position of the element within the node
} iterator;

// forward initialization
iterator begin(Node* head) {
	iterator it = { head, 0 };
	return it;
}

// backward initialization
iterator end(Node* tail) {
	iterator it = { tail, tail->size }; // points to the element after the last one
	return it;
}

void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr) return ptr;
	exit(EXIT_FAILURE);
}

void *safe_realloc(void *old_ptr, size_t size) {
	void *ptr = realloc(old_ptr, size);
	if (ptr) return ptr;
	free(old_ptr);
	exit(EXIT_FAILURE);
}

Node *create_node(int *data, size_t size, Node *next, Node *prev) {
	Node *node = safe_malloc(sizeof(Node));
	node->data = data;
	node->size = size;
	node->next = next;
	node->prev = prev;
	return node;
}

// initialize list
void init(List *list) {
	list->head = NULL;
	list->tail = NULL;
}

void free_node(Node *node) {
	if (node == NULL) return;
	if ((node)->data != NULL) free((node)->data);
	free(node);
	node = NULL;
}

// to implement ...

// push front
void push_front(List *list, int *data, size_t size) {
	Node *new = create_node(data, size, list->head, NULL);
	if (list->head == NULL) list->tail = new;
	else list->head->prev = new;
	list->head = new;
}

// append element to the list
void push_back(List *list, int *data, size_t size) {
	Node *new = create_node(data, size, NULL, list->tail);
	if (list->head == NULL) list->head = new;
	else list->tail->next = new;
	list->tail = new;
}

int pop_front(List *list) {
	if (list->head == NULL) return INT_MIN;

	Node *p = list->head;

	if (list->head == list->tail) list->tail = NULL;
	else list->head->next->prev = NULL;
	list->head = list->head->next;

	int res = *(p->data); // technically res is an array... (but this isn't used anyway)
	free_node(p);

	return res;
}

int pop_back(List *list) {
	if (list->head == NULL) return INT_MIN;

	Node *p = list->tail;

	if (list->head == list->tail) list->head = NULL;
	else list->tail->prev->next = NULL;
	list->tail = list->tail->prev;

	int res = *(p->data);
	free_node(p);

	return res;
}

// set iterator to move n elements forward from its current position
void skip_forward(iterator* itr, size_t n) {
	while (itr->node_ptr && n > itr->node_ptr->size){
		n -= itr->node_ptr->size;
		itr->node_ptr = itr->node_ptr->next;
	}
	itr->position = n - 1;
}

// forward iteration - get n-th element in the list
int get_forward(List *list, size_t n) {
	iterator itr = begin(list->head);

	skip_forward(&itr, n);

	return itr.node_ptr->data[itr.position];
}

// set iterator to move n elements backward from its current position
void skip_backward(iterator* itr, size_t n) {
	while (itr->node_ptr && n > itr->node_ptr->size){
		n -= itr->node_ptr->size;
		itr->node_ptr = itr->node_ptr->prev;
	}
	itr->position = itr->node_ptr->size - n;
}

// backward iteration - get n-th element from the end of the list
int get_backward(List *list, size_t n) {
	iterator itr = end(list->tail);

	skip_backward(&itr, n);

	return itr.node_ptr->data[itr.position];
}

void remove_node(List *list, Node *node_ptr) {
	if (list->head == node_ptr) pop_front(list);
	else if (list->tail == node_ptr) pop_back(list);
	else{
		node_ptr->prev->next = node_ptr->next;
		node_ptr->next->prev = node_ptr->prev;
		free_node(node_ptr);
	}
}

// remove n-th element; if array empty remove node
void remove_at(List *list, size_t n) {
	iterator itr = begin(list->head);

	skip_forward(&itr, n);

	if (itr.node_ptr->size == 1) remove_node(list, itr.node_ptr);
	else{
		memmove((void*)(&itr.node_ptr->data[itr.position]), (const void*)(&itr.node_ptr->data[itr.position + 1]), (itr.node_ptr->size - itr.position - 1) * sizeof(itr.node_ptr->data[0]));
		itr.node_ptr->size--;
		itr.node_ptr->data = safe_realloc(itr.node_ptr->data, itr.node_ptr->size * sizeof(itr.node_ptr->data[0]));
	}
}

size_t digits(int n) {
    if (n == 0) return 1;
    return (size_t)(log10((double)abs(n)) + 1);
}

void insert_in_order(List *list, int value) {
	if (list->head == NULL){
		int *data = safe_malloc(sizeof(int));
		data[0] = value;
		push_back(list, data, 1);
	}
	else{
		Node *p = list->head;
		size_t value_digits = digits(value);
		while (p != NULL && digits(p->data[0]) < value_digits) p = p->next;

		if (p == NULL){
			int *data = safe_malloc(sizeof(int));
			data[0] = value;
			push_back(list, data, 1);
			return;
		}

		int cmp = (int)(digits(value) - digits(p->data[0]));

		if (!cmp){
			p->data = safe_realloc(p->data, (p->size + 1) * sizeof(p->data[0]));
			if (p->data[p->size - 1] < value) p->data[p->size] = value;
			else{
				size_t i = 0;
				while (i < p->size && p->data[i] < value) i++;
				memmove((void*)&p->data[i + 1], (void*)&p->data[i], (p->size - i) * sizeof(p->data[0]));
				p->data[i] = value;
			}
			p->size++;
			return;
		}

		int *data = safe_malloc(sizeof(int));
		data[0] = value;

		if (cmp < 0){
			if (list->head == p) push_front(list, data, 1);
			else{
				Node *new  = create_node(data, 1, p, p->prev);
				p->prev->next = new;
				p->prev = new;
			}
		}
		else if (cmp > 0){
			if (list->tail == p) push_back(list, data, 1);
			else{
				Node *new = create_node(data, 1, p->next, p);
				p->next->prev = new;
				p->next = new;
			}
		}
	}
}

// -------------------------------------------------------------
// helper functions

// print list
void dumpList(const List *list) {
	for(Node *node = list->head; node != NULL; node = node->next) {
		printf("-> ");
		for (size_t k = 0; k < node->size; k++) {
			printf("%d ", node->data[k]);
		}
		printf("\n");
	}
}

// free list
void freeList(List *list) {
	while(pop_front(list) != INT_MIN);
}

// read int vector
void read_vector(int array[], size_t n) {
	for (size_t i = 0; i < n; ++i) {
		scanf("%d", array + i);
	}
}

// initialize the list and push data
void read_list(List *list) {
	int n;
	size_t size;
	scanf("%d", &n);
	for (int i = 0; i < n; i++) {
		scanf("%zu", &size);
		int *array = (int*) safe_malloc(size * sizeof(int));
		read_vector(array, size);
		push_back(list, array, size);
	}
}

int main(void) {
	int to_do, value;
	size_t size, offset;
	List list;
	init(&list);

	scanf("%d", &to_do);
	switch (to_do) {
		case 1:
			read_list(&list);
			dumpList(&list);
			break;
		case 2:
			read_list(&list);
			scanf("%zu", &size);
			for (size_t i = 0; i < size; i++) {
				scanf("%zu", &offset);
				printf("%d ", get_forward(&list, offset));
			}
			printf("\n");
			break;
		case 3:
			read_list(&list);
			scanf("%zu", &size);
			for (size_t i = 0; i < size; i++) {
				scanf("%zu", &offset);
				printf("%d ", get_backward(&list, offset));
			}
			printf("\n");
			break;
		case 4:
			read_list(&list);
			scanf("%zu", &size);
			for (size_t i = 0; i < size; i++) {
				scanf("%zu", &offset);
				remove_at(&list, offset);
			}
			dumpList(&list);
			break;
		case 5:
			scanf("%zu", &size);
			for (size_t i = 0; i < size; i++) {
				scanf("%d", &value);
				insert_in_order(&list, value);
			}
			dumpList(&list);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	freeList(&list);

	return 0;
}