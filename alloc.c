#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>

#define TAB_SIZE  1000
#define BUF_SIZE  1000

// 1
////////////////////////////////////////////////////////////

int get(int cols, int row, int col, const int* A) {
	return A[(row * cols) + col];
}

void set(int cols, int row, int col, int* A, int value) {
	A[(row * cols) + col] = value;
}

void prod_mat(int rowsA, int colsA, int colsB, int* A, int* B, int* AB) {
	int a, b, c;
	for (int i = 0; i < rowsA; i++){
		for (int j = 0; j < colsB; j++){
			for (int k = 0; k < colsA; k++){
				a = get(colsA, i, k, A);
				b = get(colsB, k, j, B);
				c = get(colsB, i, j, AB);
				set(colsB, i, j, AB, c + a * b);
			}
		}
	}
}

void read_mat(int rows, int cols, int* t) {
	int rd;
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			scanf("%d", &rd);
			set(cols, i, j, t, rd);
		}
	}
}

void print_mat(int rows, int cols, int* t) {
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			printf("%d ", get(cols, i, j, t));
		}
		printf("\n");
	}
}

// 2
/////////////////////////////////////////////////////////////

int read_ints_from_line(char* c_buf, int *int_buf) {
	int i = 0;
	char delimiters[] = " ,\n";
	char* token = strtok(c_buf, delimiters);

	while (token != NULL){
		int_buf[i] = atoi(token);
		token = strtok(NULL, delimiters);
		i++;
	}
	return i;
}

int read_int_lines_cont(int* ptr_array[]) {
	int* continous_array = ptr_array[0];
	char c_buf[BUF_SIZE];
	int rows = 0, cols;

	while (fgets(c_buf, BUF_SIZE, stdin) != NULL){
		cols = read_ints_from_line(c_buf, continous_array);
		ptr_array[rows] = continous_array;
		continous_array += cols;
		*continous_array = INT_MAX;
		continous_array++;
		rows++;
	}

	return rows;
}
	

void write_int_line_cont(int *ptr_array[], int n) {
	int i = 0, a;
	while ((a = *(ptr_array[n] + i)) != INT_MAX) printf("%d ", a), i++;
}

// 3
///////////////////////////////////////////////////////////

int read_char_lines(char *array[]) {
	char c_buff[BUF_SIZE];
	char a = getchar();
	int n = 0, k = 0;

	while (a != EOF){
		k = 0;
		while (a != '\n'){
			c_buff[k] = a;
			k++;
			a = getchar();
		}
		c_buff[k] = '\0';
		k++;
		char *tmp =  malloc(k * sizeof(char));
		memmove(tmp, c_buff, k * sizeof(char));
		array[n] = tmp;
		n++;
		a = getchar();
	}
	
	return n;
}

void write_char_line(char *array[], int n) {
	printf("%s", array[n]);
}

void delete_lines(char *array[]) {
	int i = 0;
	while(array[i] != NULL) free(array[i]), i++;
}

// 4
////////////////////////////////////////////////

typedef struct {
	int *values;
	int len;
	double average;
} line_type;

int read_int_lines(line_type lines_array[]) {
	char c_buf[BUF_SIZE];
	char delimiters[] = " ,\n";
	int i, int_buf[BUF_SIZE], n = 0;
	double avg;

	while (fgets(c_buf, BUF_SIZE, stdin) != NULL){
		char* token = strtok(c_buf, delimiters);

		i = 0;
		while (token != NULL){
			int_buf[i] = atoi(token);
			token = strtok(NULL, delimiters);
			i++;
		}

		int *values = malloc(i * sizeof(int));
		memmove(values, int_buf, i * sizeof(int));

		lines_array[n].values = values;
		lines_array[n].len = i;

		avg = 0;
		for (int j = 0; j < i; j++){
			avg += values[j];
		}
		avg /= i;

		lines_array[n].average = avg;

		n++;
	}

	return n;
}

void write_int_line(line_type lines_array[], int n) {
	for (int i = 0; i < lines_array[n].len; i++){
		printf("%d ", lines_array[n].values[i]);
	}
	printf("\n%0.2f", lines_array[n].average);
}

void delete_int_lines(line_type array[], int line_count) {
	for (int i = 0; i < line_count; i++){
		free(array[i].values);
	}
}

int cmp (const void *_a, const void *_b) {
	const line_type a = *((const line_type*)_a);
	const line_type b = *((const line_type*)_b);

	return a.average - b.average;
}

void sort_by_average(line_type lines_array[], int line_count) {
	qsort(lines_array, line_count, sizeof(lines_array[0]), cmp);
}

// 5
///////////////////////////////////////////////////////////

typedef struct {
	int r, c, v;
} triplet;

int read_sparse(triplet* triplet_array, int n_triplets) {
	for (int i = 0; i < n_triplets; i++){
		scanf("%d %d %d", &triplet_array[i].r, &triplet_array[i].c, &triplet_array[i].v);
	}

	return n_triplets;
}

int cmp_triplets(const void* _t1, const void* _t2) {
	const triplet t1 = *((const triplet*)_t1);
	const triplet t2 = *((const triplet*)_t2);

	if (t1.r < t2.r) return -1;
	if (t1.r > t2.r) return 1;

	return t1.c - t2.c;
}

void make_CSR(triplet* triplet_array, int n_triplets, int rows, int* V, int* C, int* R) {
	qsort(triplet_array, n_triplets, sizeof(triplet_array[0]), cmp_triplets);

	for (int i = 0; i < n_triplets; i++){
		V[i] = triplet_array[i].v;
		C[i] = triplet_array[i].c;
		for(int j = triplet_array[i].r + 1; j <= rows; j++){
			R[j]++;
		}
	}
}

void multiply_by_vector(int rows, const int* V, const int* C, const int* R,
						const int* x, int* y) {
	for (int i = 0; i < rows; i++){
		y[i] = 0;
		for (int j = R[i]; j < R[i + 1]; j++){
			y[i] += V[j] * x[C[j]];
		}
	}
}

void read_vector(int* v, int n) {
	for (int i = 0; i < n;i++){
		scanf("%d", &v[i]);
	}
}

void write_vector(int* v, int n) {
	for (int i = 0; i < n; i++) printf("%d ", v[i]);
	printf("\n");
}

// auxiliary
////////////////////////////////////////////////////////////

int read_int(void) {
	char c_buf[BUF_SIZE];
	fgets(c_buf, BUF_SIZE, stdin);
	return (int)strtol(c_buf, NULL, 10);
}

int main(void) {
	int to_do = read_int();

	int A[TAB_SIZE], B[TAB_SIZE], AB[TAB_SIZE];
	int n, lines_counter, rowsA, colsA, rowsB, colsB;
	int rows, cols, n_triplets;
	char* char_lines_array[TAB_SIZE] = { NULL };
	int continuous_array[TAB_SIZE] = {0};
	int* ptr_array[TAB_SIZE];
	triplet triplet_array[TAB_SIZE];
	int V[TAB_SIZE], C[TAB_SIZE], R[TAB_SIZE];
	int x[TAB_SIZE], y[TAB_SIZE];
	line_type int_lines_array[TAB_SIZE];

	switch (to_do) {
		case 1:
			scanf("%d %d", &rowsA, &colsA);
			read_mat(rowsA, colsA, A);
			scanf("%d %d", &rowsB, &colsB);
			read_mat(rowsB, colsB, B);
			prod_mat(rowsA, colsA, colsB, A, B, AB);
			print_mat(rowsA, colsB, AB);
			break;
		case 2:
			n = read_int() - 1; // we count from 1 :)
			ptr_array[0] = continuous_array;
			read_int_lines_cont(ptr_array);
			write_int_line_cont(ptr_array, n);
			break;
		case 3:
			n = read_int() - 1;
			read_char_lines(char_lines_array);
			write_char_line(char_lines_array, n);
			delete_lines(char_lines_array);
			break;
		case 4:
			n = read_int() - 1;
			lines_counter = read_int_lines(int_lines_array);
			sort_by_average(int_lines_array, lines_counter);
			write_int_line(int_lines_array, n);
			delete_int_lines(int_lines_array, lines_counter);
			break;
		case 5:
			scanf("%d %d %d", &rows, &cols, &n_triplets);
			n_triplets = read_sparse(triplet_array, n_triplets);
			read_vector(x, cols);
			make_CSR(triplet_array, n_triplets, rows, V, C, R);
			multiply_by_vector(rows, V, C, R, x, y);
			write_vector(V, n_triplets);
			write_vector(C, n_triplets);
			write_vector(R, rows + 1);
			write_vector(y, rows);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}