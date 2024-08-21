#include <stdio.h>
#include <math.h>

#define SIZE 40

// Macierz jednostkowa po prawej stronie się zmieści

void read_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		scanf("%lf", x++);
	}
}

void print_vector(double x[], int n) {
	for(int i = 0; i < n; ++i) {
		printf("%.4f ", x[i]);
	}
	printf("\n");
}

void read_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			scanf("%lf", &A[i][j]);
		}
	}
}

void print_mat(double A[][SIZE], int m, int n) {
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			printf("%.4f ", A[i][j]);
		}
		printf("\n");
	}
}

void print_mat_fake_idx(double A[][SIZE], int idx[], int m, int n) {
	printf("\n");
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			printf("%.4f ", A[idx[i]][j]);
		}
		printf("\n");
	}
	printf("\n");
}

// 1. Calculate matrix product, AB = A X B
// A[m][p], B[p][n], AB[m][n]
void mat_product(double A[][SIZE], double B[][SIZE], double AB[][SIZE], int m, int p, int n) {

	for (int r = 0; r < m; r++){
		for (int c = 0; c < p; c++){
			AB[r][c] = 0;
			for (int i = 0; i < p; i++){
				AB[r][c] += (A[r][i] * B[i][c]);
			}
		}
	}
}


// 2. Matrix triangulation and determinant calculation - simplified version
// (no rows' swaps). If A[i][i] == 0, function returns NAN.
// Function may change A matrix elements.
// By using arrays of indexes.
double gauss_simplified(double A[][SIZE], int n) {
	int idx[n];
	int flag = 0, tmp, swaps = 0;
	double ratio, res = 1;
	for (int i = 0; i < n; i++) idx[i] = i;

	for (int c = 0; c < n - 1; c++){ // because c + 1 is below
		flag = 0;
		for (int r = c; r < n; r++){
			if (A[idx[r]][c] != 0){
				flag = 1;
				tmp = idx[c];
				idx[c] = idx[r];
				idx[r] = tmp;

				if (tmp != idx[c]) swaps++;

				for (int i = c + 1; i < n; i++){
					ratio = A[idx[i]][c] / A[idx[c]][c];

					for (int k = 0; k < n; k++){
						A[idx[i]][k] -= A[idx[c]][k] * ratio;
					}
				}
			}
		}
		if (!flag) return NAN;
	}

	for (int i = 0; i < n; i++){
		res *= A[idx[i]][i];
	}

	if (swaps % 2 != 0) res = -res; // jezeli nieparzysta liczba zamian to zmieniam znak wyznacznika

	return res;
}


void backward_substitution_index(double A[][SIZE], const int indices[], double x[], int n) {
}

// 3. Matrix triangulation, determinant calculation, and Ax = b solving - extended version
// (Swap the rows so that the row with the largest, leftmost nonzero entry is on top. While
// swapping the rows use index vector - do not copy entire rows.)
// If max A[i][i] < eps, function returns 0.
// If det != 0 && b != NULL && x != NULL then vector x should contain solution of Ax = b.

// Można dopisać macierz b
double gauss(double A[][SIZE], const double b[], double x[], const int n, const double eps) {
	int idx[n];
	int tmp, tmp2, tmp_i, swaps = 0;
	double ratio, res = 1, max;

	for (int i = 0; i < n; i++) {
		idx[i] = i;
		A[i][n] = b[i]; // stworzenie macierzy uzupelnionej (na n-tym indeksie nic nie bylo)
	}

	for (int c = 0; c < n - 1; c++){
		max = 0;
		tmp_i = 0;
		for (int r = c; r < n; r++){
			tmp2 = fabs(A[idx[r]][c]);
			if (tmp2 > max){
				max = tmp2;
				tmp_i = r;
			}
		}
		tmp = idx[c];
		idx[c] = idx[tmp_i];
		idx[tmp_i] = tmp;

		if (tmp != idx[c]) swaps++;

		if (max < eps) return 0;

		for (int i = c + 1; i < n; i++){
			ratio = A[idx[i]][c] / A[idx[c]][c];

			for (int k = 0; k < n + 1; k++){ // zmiana wraz z kolumną uzupełnioną
				A[idx[i]][k] -= A[idx[c]][k] * ratio;
			}
		}
	}
	
	for (int i = 0; i < n; i++){
		res *= A[idx[i]][i];
	}

	if (res != 0 && b != NULL && x != NULL){
		for (int i = n - 1; i > -1; i--){
			x[i] = A[idx[i]][n];

			for (int j = n - 1; j > i; j--) x[i] -= A[idx[i]][j] * x[j];

			x[i] /= A[idx[i]][i];
		}
	}
	// print_mat_fake_idx(A, idx, n, n+1);
	if (swaps % 2 != 0) res = -res;

	return res;
}

// 4. Returns the determinant; B contains the inverse of A (if det(A) != 0)
// If max A[i][i] < eps, function returns 0.

// Można dopisać macierz jednostkową.
// To samo co wyżej, bo najpierw potrzebny wyznacznik, a potem biorę zawsze pierwszy element z przekątnej i dzielę cały wiersz tak by ten element był równy 1 i odejmuje jak trzeba od innych
double matrix_inv(double A[][SIZE], double B[][SIZE], int n, double eps) {
	int idx[n];
	int tmp, tmp2, tmp_i, swaps = 0;
	double ratio, res = 1, max;

	for (int i = 0; i < n; i++) idx[i] = i;

	for (int i = n; i < 2 * n; i++) A[i - n][i] = 1; // dopisanie macierzy jednostkowej

	for (int c = 0; c < n - 1; c++){
		max = 0;
		tmp_i = 0;
		for (int r = c; r < n; r++){
			tmp2 = fabs(A[idx[r]][c]);
			if (tmp2 > max){
				max = tmp2;
				tmp_i = r;
			}
		}
		tmp = idx[c];
		idx[c] = idx[tmp_i];
		idx[tmp_i] = tmp;

		if (tmp != idx[c]) swaps++;

		if (max < eps) return 0;

		for (int i = c + 1; i < n; i++){
			ratio = A[idx[i]][c] / A[idx[c]][c];

			for (int k = 0; k < 2 * n; k++){ // zmiana wraz z kolumną uzupełnioną
				A[idx[i]][k] -= A[idx[c]][k] * ratio;
			}
		}

	}
	for (int i = 0; i < n; i++){
		res *= A[idx[i]][i];
	}

	if (swaps % 2 != 0) res = -res;

	ratio = A[idx[n - 1]][n - 1];
	for (int c = n - 1; c < 2 * n; c++) A[idx[n - 1]][c] /= ratio;

	for (int r = n - 1; r > 0; r--){
		for (int rem = r - 1; rem > -1; rem--){
			ratio = A[idx[rem]][r] / A[idx[r]][r];

			for (int c = r; c < 2 * n; c++) A[idx[rem]][c] -= ratio * A[idx[r]][c];
		}

		ratio = A[idx[r - 1]][r - 1];
		for (int c = r - 1; c < 2 * n; c++) A[idx[r - 1]][c] /= ratio;
	}

	for (int i = 0; i < n; i++){
		for (int j = n; j < 2 * n; j++) B[i][j - n] = A[idx[i]][j];
	}

	return res;
}

int main(void) {

	double A[SIZE][SIZE] = {{0}}, B[SIZE][SIZE] = {{0}}, C[SIZE][SIZE] = {{0}};
	double b[SIZE] = {0}, x[SIZE] = {0}, det, eps = 1.e-13;

	int to_do;
	int m, n, p;

	scanf ("%d", &to_do);

	switch (to_do) {
		case 1:
			scanf("%d %d %d", &m, &p, &n);
			read_mat(A, m, p);
			read_mat(B, p, n);
			mat_product(A, B, C, m, p, n);
			print_mat(C, m, n);
			break;
		case 2:
			scanf("%d", &n);
			read_mat(A, n, n);
			printf("%.4f\n", gauss_simplified(A, n));
			break;
		case 3:
			scanf("%d", &n);
			read_mat(A,n, n);
			read_vector(b, n);
			det = gauss(A, b, x, n, eps);
			printf("%.4f\n", det);
			if (det) print_vector(x, n);
			break;
		case 4:
			scanf("%d", &n);
			read_mat(A, n, n);
			det = matrix_inv(A, B, n, eps);
			printf("%.4f\n", det);
			if (det) print_mat(B, n, n);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}