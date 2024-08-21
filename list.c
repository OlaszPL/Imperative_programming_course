#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

struct List;
typedef void (*DataFp)(void*);
typedef void (*ConstDataFp)(const void*);
typedef  int (*CompareDataFp)(const void*, const void*);

typedef struct ListElement {
	struct ListElement *next;
	void *data;
} ListElement;

typedef struct {
	ListElement *head;
	ListElement *tail;
	ConstDataFp dump_data;
	DataFp free_data;
	CompareDataFp compare_data;
	DataFp modify_data;
} List;

void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if(ptr) return ptr;
	printf("malloc error\n");
	exit(EXIT_FAILURE);
}

void *safe_strdup(const char *string) {
	void *ptr = strdup(string);
	if(ptr) return ptr;
	printf("strdup error\n");
	exit(EXIT_FAILURE);
}

// --- generic functions --- for any data type

void init_list(List *p_list, ConstDataFp dump_data, DataFp free_data, CompareDataFp compare_data, DataFp modify_data) {
	p_list->head = NULL;
	p_list->tail = NULL;
	p_list->dump_data = dump_data;
	p_list->compare_data = compare_data;
	p_list->free_data = free_data;
	p_list->modify_data = modify_data;
}

// Print elements of the list
void dump_list(const List* p_list) {
	ListElement *p = p_list->head;

	while (p != NULL){
		p_list->dump_data(p->data);
		p = p->next;
	}
}

// Print elements of the list if comparable to data
void dump_list_if(List *p_list, void *data) {
	ListElement *p = p_list->head;

    while (p != NULL) {
        if (!p_list->compare_data(p->data, data)) {
            p_list->dump_data(p->data);
        }
        p = p->next;
    }
}

// Free element pointed by data using free_data() function
void free_element(DataFp free_data, ListElement *to_delete) {
	if (to_delete != NULL){
		if (free_data != NULL){
			free_data(to_delete->data); // free the data of the element
			}
		free(to_delete); // free the element itself
	}
}

// Free all elements of the list
void free_list(List* p_list) {
	ListElement *curr = p_list->head, *next;

	while (curr != NULL){
		next = curr->next;
		free_element(p_list->free_data, curr);
		curr = next;
	}
	p_list->head = NULL;
	p_list->tail = NULL;
}

// Push element at the beginning of the list
void push_front(List *p_list, void *data) {
	ListElement *p = safe_malloc(sizeof(ListElement));
	p->data = data;
	p->next = p_list->head;

	if (p_list->head == NULL) p_list->tail = p;

	p_list->head = p;
}

// Push element at the end of the list
void push_back(List *p_list, void *data) {
	ListElement *p = safe_malloc(sizeof(ListElement));
	p->data = data;
	p->next = NULL;

	if (p_list->head == NULL){
		p_list->head = p;
		p_list->tail = p;
	}
	else{
		p_list->tail->next = p;
		p_list->tail = p;
	}
}

// Remove the first element
void pop_front(List *p_list) {
	if (p_list->head != NULL){
		ListElement *a = p_list->head->next;
		free_element(p_list->free_data, p_list->head);
		p_list->head = a;
	}
}

// Reverse the list
void reverse(List *p_list) {
	if (p_list->head == NULL || p_list->head->next == NULL) return;

	ListElement *curr = p_list->head->next, *next;
	p_list->head->next = NULL;
	p_list->tail = p_list->head;

	while (curr != NULL){
		next = curr->next;
		curr->next = p_list->head;
		p_list->head = curr;
		curr = next;
	}
}

// find element in sorted list after which to insert given element
ListElement* find_insertion_point(const List *p_list, ListElement *p_element) {
	ListElement *p = p_list->head;

	while (p->next != NULL && p_list->compare_data(p->next->data, p_element->data) <= 0){
		p = p->next;
	}
	return p;
}

// Insert element after 'previous'
void push_after(List *p_list, void *data, ListElement *previous) {
	ListElement *el = safe_malloc(sizeof(ListElement));
	el->data = data;
	el->next = previous->next;
	previous->next = el;

	if (previous == p_list->tail) p_list->tail = el;
}

// Insert element preserving order
void insert_in_order(List *p_list, void *p_data) {
	CompareDataFp cmp = p_list->compare_data;

	if (p_list->head == NULL){
		push_front(p_list, p_data);
		return;
	}

	ListElement *el = safe_malloc(sizeof(ListElement));
	el->data = p_data;

	ListElement *ins_point = find_insertion_point(p_list, el);
	int cmpres = cmp(p_data, ins_point->data);
	if (!cmpres){
		if (p_list->modify_data != NULL) p_list->modify_data(ins_point->data);
		p_list->free_data(p_data);
	}
	else if (cmpres > 0) push_after(p_list, p_data, ins_point);
	else push_front(p_list, p_data);

	free(el);
}

// -----------------------------------------------------------
// --- type-specific definitions

// int element

void dump_int(const void *d) {
	printf("%d ", *((int*)d));
}

void free_int(void *d) {
	free((int*)d);
}

int cmp_int(const void *_a, const void *_b) {
	int a = *((int*)_a);
	int b = *((int*)_b);
	return a - b;
}

int *create_data_int(int v) {
	int *data = safe_malloc(sizeof(int));
	*data = v;
	return data;
}

// Word element

typedef struct DataWord {
	char *word;
	int counter;
} DataWord;

void dump_word (const void *d) {
	printf("%s ", ((DataWord*)d)->word);
}

void to_lowercase(char *str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

void dump_word_lowercase (const void *d) {
	char *tmp = ((const DataWord*)d)->word;
	to_lowercase(tmp);
	printf("%s ", tmp);
}

void free_word(void *d) {
	DataWord *word = (DataWord*)d;
	free(word->word);
	free(word);
}

int cmp_word_alphabet(const void *_a, const void *_b) {
	char *a = ((const DataWord*)_a)->word;
	char *b = ((const DataWord*)_b)->word;

	to_lowercase(a);
	to_lowercase(b);

	return strcmp(a, b);
}

int cmp_word_counter(const void *_a, const void *_b) {
	const DataWord *a = (const DataWord*)_a;
    const DataWord *b = (const DataWord*)_b;
	
    return a->counter - b->counter;
}

void modify_word(void *p) {
	((DataWord*)p)->counter++;
}

void *create_data_word(const char *string, int counter) {
	DataWord *data = safe_malloc(sizeof(DataWord));
	data->word = safe_strdup(string);
	data->counter = counter;

	return data;
}

// read text, parse it to words, and insert those words to the list.
// Order of insertions is given by the last parameter of type CompareDataFp.
// (comparator function address). If this address is not NULL the element is
// inserted according to the comparator. Otherwise, read order is preserved.
void stream_to_list(List *p_list, FILE *stream, CompareDataFp cmp) {
	const char delimits[] = " \r\t\n.,?!:;-";
	char c_buf[BUFFER_SIZE];
	char *token;
	void *data;

	if (cmp) p_list->compare_data = cmp;

	while (fgets(c_buf, BUFFER_SIZE, stream) != NULL){
		token = strtok(c_buf, delimits);
		while (token != NULL){
			data = create_data_word(token, 1);
			if (cmp != NULL) insert_in_order(p_list, data);
			else push_back(p_list, data);

			token = strtok(NULL, delimits);
		}
	}
}

// test integer list
void list_test(List *p_list, int n) {
	char op;
	int v;
	for (int i = 0; i < n; ++i) {
		scanf(" %c", &op);
		switch (op) {
			case 'f':
				scanf("%d", &v);
				push_front(p_list, create_data_int(v));
				break;
			case 'b':
				scanf("%d", &v);
				push_back(p_list, create_data_int(v));
				break;
			case 'd':
				pop_front(p_list);
				break;
			case 'r':
				reverse(p_list);
				break;
			case 'i':
				scanf("%d", &v);
				insert_in_order(p_list, create_data_int(v));
				break;
			default:
				printf("No such operation: %c\n", op);
				break;
		}
	}
}

int main(void) {
	int to_do, n;
	List list;

	scanf ("%d", &to_do);
	switch (to_do) {
		case 1: // test integer list
			scanf("%d",&n);
			init_list(&list, dump_int, free_int, cmp_int, NULL);
			list_test(&list, n);
			dump_list(&list);
			free_list(&list);
			break;
		case 2: // read words from text, insert into list, and print
			init_list(&list, dump_word, free_word, NULL, NULL);
			stream_to_list(&list, stdin, NULL);
			dump_list(&list);
			free_list(&list);
			break;
		case 3: // read words, insert into list alphabetically, print words encountered n times
			scanf("%d",&n);
			init_list(&list, dump_word_lowercase, free_word, NULL, modify_word);
			stream_to_list(&list, stdin, cmp_word_alphabet);
			list.compare_data = cmp_word_counter;
			DataWord data = { NULL, n };
			dump_list_if(&list, &data);
			free_list(&list);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}