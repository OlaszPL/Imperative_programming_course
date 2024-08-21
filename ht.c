#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_RATE 4
#define MEMORY_ALLOCATION_ERROR  (-1)

typedef union {
	int int_data;
	char char_data;
	// ... other primitive types used
	void *ptr_data;
} data_union;

typedef struct ht_element {
	struct ht_element *next;
	data_union data;
} ht_element;

typedef void (*DataFp)(data_union);
typedef void (*DataPFp)(data_union*);
typedef int (*CompareDataFp)(data_union, data_union);
typedef size_t (*HashFp)(data_union, size_t);
typedef data_union (*CreateDataFp)(void*);

typedef struct {
	size_t size;
	size_t no_elements;
	ht_element *ht;
	DataFp dump_data;
	CreateDataFp create_data;
	DataFp free_data;
	CompareDataFp compare_data;
	HashFp hash_function;
	DataPFp modify_data;
} hash_table;

// ---------------------- functions to implement

// initialize table fields
void init_ht(hash_table *p_table, size_t size, DataFp dump_data, CreateDataFp create_data,
		 DataFp free_data, CompareDataFp compare_data, HashFp hash_function, DataPFp modify_data) {
	p_table->ht = malloc(size * sizeof(ht_element)); // array of sentinels
	for (size_t i = 0; i < size; i++) p_table->ht[i].next = NULL;
	p_table->compare_data = compare_data;
	p_table->create_data = create_data;
	p_table->dump_data = dump_data;
	p_table->free_data = free_data;
	p_table->hash_function = hash_function;
	p_table->modify_data = modify_data;
	p_table->no_elements = 0;
	p_table->size = size;
}

// remember about sentinels

// print elements of the list with hash n
void dump_list(const hash_table* p_table, size_t n) {
	ht_element* p = &p_table->ht[n];
	while (p->next != NULL){
		p_table->dump_data(p->next->data);
		p = p->next;
	}
}

// Free element pointed by data_union using free_data() function
void free_element(DataFp free_data, ht_element *to_delete) {
	if (free_data != NULL) free_data(to_delete->data);
	free(to_delete);
}

// free all elements from the table (and the table itself)
void free_table(hash_table* p_table) {
	ht_element * next;
	for (size_t i = 0; i < p_table->size; i++){
		ht_element* p = &p_table->ht[i];
		p = p->next;
		while (p != NULL){
			next = p->next;
			free_element(p_table->free_data, p);
			p = next;
		}
	}
	free(p_table->ht);
	p_table->no_elements = 0;
}

// calculate hash function for integer k
size_t hash_base(int k, size_t size) {
	static const double c = 0.618033988; // (sqrt(5.) – 1) / 2.;
	double tmp = k * c;
	return (size_t)floor((double)size * (tmp - floor(tmp)));
}

void rehash(hash_table *p_table) {
	size_t new_size = 2 * p_table->size;
	ht_element* new_array = malloc(new_size * sizeof(ht_element));
	ht_element* to_move;

	for (size_t i = 0; i < new_size; i++) {
    	new_array[i].next = NULL;
	}

	for (size_t i = 0; i < p_table->size; i++){
		ht_element* p = &p_table->ht[i];
		p = p->next;
		while (p != NULL){
			to_move = p;
			p = p->next;

			size_t idx = p_table->hash_function(to_move->data, new_size);
			to_move->next = new_array[idx].next;

			new_array[idx].next = to_move; // to keep the sentinel
		}
	}
	free(p_table->ht);
	p_table->ht = new_array;
	p_table->size = new_size;
}

// find element; return pointer to previous
ht_element *find_previous(hash_table *p_table, data_union data) {
	size_t idx = p_table->hash_function(data, p_table->size);
	ht_element* p = &p_table->ht[idx];

	while (p->next != NULL && p_table->compare_data(data, p->next->data) != 0) p = p->next;

	if (p->next == NULL) return NULL;

	return p;
}

// return pointer to element with given value
ht_element *get_element(hash_table *p_table, data_union *data) {
	ht_element* prev = find_previous(p_table, *data);
	if (prev == NULL) return NULL;
    return prev->next;
}

// insert element
void insert_element(hash_table *p_table, data_union *data) {
	size_t idx = p_table->hash_function(*data, p_table->size);
	ht_element* p = &p_table->ht[idx];
	ht_element* start = p;
	ht_element* new = malloc(sizeof(ht_element));
	new->data = *data;
	
	
	while (p->next != NULL && p_table->compare_data(*data, p->next->data) != 0) p = p->next;

	if (p->next != NULL){
		if (p_table->modify_data != NULL) p_table->modify_data(&p->next->data);
	}
	else{
		new->next = start->next;
		start->next = new;
		p_table->no_elements++;
	}

	if ((p_table->no_elements / p_table->size) > MAX_RATE) rehash(p_table);
}

// remove element
void remove_element(hash_table *p_table, data_union data) {
	ht_element* prev = find_previous(p_table, data);
	ht_element* new_next;
	
	if (prev != NULL && prev->next != NULL){
		new_next = prev->next->next;
		free_element(p_table->free_data, prev->next);
		prev->next = new_next;
		p_table->no_elements--;
	}
}

// type-specific definitions

// int element

size_t hash_int(data_union data, size_t size) {
	return hash_base(data.int_data, size);
}

void dump_int(data_union data) {
	printf("%d ", data.int_data);
}

int cmp_int(data_union a, data_union b) {
	return a.int_data - b.int_data;
}

data_union create_int(void* value) {
	int a;
	scanf("%d", &a);

	if (value){
		((data_union*)value)->int_data = a;
		return *((data_union*)value);
	}
	else{
		data_union new;
		new.int_data = a;
		return new;
	}
}

// char element

size_t hash_char(data_union data, size_t size) {
	return hash_base((int)data.char_data, size);
}

void dump_char(data_union data) {
	printf("%c ", data.char_data);
}

int cmp_char(data_union a, data_union b) {
	return (int)a.char_data - (int)b.char_data;
}

data_union create_char(void* value) {
	char a;
	scanf(" %c", &a);

	if (value){
		((data_union*)value)->char_data = a;
		return *((data_union*)value);
	}
	else{
		data_union new;;
		new.char_data = a;
		return new;
	}
}

// Word element

typedef struct DataWord {
	char *word;
	int counter;
} DataWord;

void dump_word(data_union data) {
	DataWord* d = (DataWord*)data.ptr_data;
	printf("%s ", d->word);
	printf("%d", d->counter);
}

void free_word(data_union data) {
	DataWord* d = (DataWord*)data.ptr_data;
	free(d->word);
	free(d);
}

int cmp_word(data_union _a, data_union _b) {
	DataWord* a = (DataWord*)_a.ptr_data;
	DataWord* b = (DataWord*)_b.ptr_data;

	return strcmp(a->word, b->word);
}

size_t hash_word(data_union data, size_t size) {
	int s = 0;
	DataWord *dw = (DataWord*)data.ptr_data;
	char *p = dw->word;
	while (*p) {
		s += *p++;
	}
	return hash_base(s, size);
}

void modify_word(data_union *data) {
	((DataWord*)data->ptr_data)->counter++;
}

void *safe_strdup(const char *string) {
	void *ptr = strdup(string);
	if(ptr) return ptr;
	printf("strdup error\n");
	exit(EXIT_FAILURE);
}

void to_lowercase(char *str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

data_union create_data_word(void *value) {
	data_union data;
	DataWord* ptr = malloc(sizeof(DataWord));
	to_lowercase((char*)value);
	ptr->word = safe_strdup((char*)value);
	ptr->counter = 1;
	data.ptr_data = ptr;

	return data;
}

// read text, parse it to words, and insert these words to the hashtable
void stream_to_ht(hash_table *p_table, FILE *stream) {
	const char delimits[] = " \r\t\n.,?!:;-";
	char c_buf[BUFFER_SIZE];
	char *token;
	data_union data;

	while (fgets(c_buf, BUFFER_SIZE, stream) != NULL){
		token = strtok(c_buf, delimits);
		while (token != NULL){
			data = create_data_word(token);

			insert_element(p_table, &data);

			token = strtok(NULL, delimits);
		}
	}
}

// test primitive type list
void test_ht(hash_table *p_table, int n) {
	char op;
	data_union data;
	for (int i = 0; i < n; ++i) {
		scanf(" %c", &op);
//		p_table->create_data(&data);
		data = p_table->create_data(NULL); // should give the same result as the line above
		switch (op) {
			case 'r':
				remove_element(p_table, data);
				break;
			case 'i':
				insert_element(p_table, &data);
				break;
			default:
				printf("No such operation: %c\n", op);
				break;
		}
	}
}

int main(void) {
	int to_do, n;
	size_t index;
	hash_table table;
	char buffer[BUFFER_SIZE];
	data_union data;

	scanf ("%d", &to_do);
	switch (to_do) {
		case 1: // test integer hash table
			scanf("%d %zu", &n, &index);
			init_ht(&table, 4, dump_int, create_int, NULL, cmp_int, hash_int, NULL);
			test_ht(&table, n);
			printf ("%zu\n", table.size);
			dump_list(&table, index);
			break;
		case 2: // test char hash table
			scanf("%d %zu", &n, &index);
			init_ht(&table, 4, dump_char, create_char, NULL, cmp_char, hash_char, NULL);
			test_ht(&table, n);
			printf ("%zu\n", table.size);
			dump_list(&table, index);
			break;
		case 3: // read words from text, insert into hash table, and print
			scanf("%s", buffer);
			init_ht(&table, 8, dump_word, create_data_word, free_word, cmp_word, hash_word, modify_word);
			stream_to_ht(&table, stdin);
			printf ("%zu\n", table.size);
			data = table.create_data(buffer);
			ht_element *e = get_element(&table, &data);
			if (e) table.dump_data(e->data);
			if (table.free_data) table.free_data(data);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	free_table(&table);

	return 0;
}