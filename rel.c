#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100

typedef struct {
	int first;
	int second;
} pair;

// Add pair to existing relation if not already there
int add_relation (pair *tab, int n, pair new_pair) { 
	for (int i = 0; i < n; i++){
		if (tab[i].first == new_pair.first && tab[i].second == new_pair.second) return n;
	}

	tab[n] = new_pair;

	return n + 1;
}

// Read number of pairs, n, and then n pairs of ints
int read_relation(pair *relation) { 
	int readn, i, x, y;
	int n = 0;

	scanf("%d", &readn);

	for (int i = 0; i < readn; i++){
		scanf("%d %d", &x, &y);
		pair new_pair;
		new_pair.first = x;
		new_pair.second = y;

		n = add_relation(relation, n, new_pair);
	}

	return n;
}

void print_int_array(int *array, int n) {
	printf("%d\n", n);
	for (int i = 0; i < n; ++i) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

//------------------------------------------------

// Case 1:

// The relation R is reflexive if xRx for every x in X
int is_reflexive(pair *tab, int n) {
	int x, flag;

	for (int i = 0; i < n; i++){
		flag = 0;
		x = tab[i].first;
		for (int j = 0; j < n; j++){
			if (x == tab[j].first && x == tab[j].second){
				flag = 1;
				break;
			}
		}
		if (!flag) return 0;
	}
	return 1;
}

// The relation R on the set X is called irreflexive
// if xRx is false for every x in X
int is_irreflexive(pair *tab, int n) {
	for (int i = 0; i < n; i++){
		if (tab[i].first == tab[i].second) return 0;
	}
	return 1;
}

// A binary relation R over a set X is symmetric if:
// for all x, y in X xRy <=> yRx
int is_symmetric(pair *tab, int n) {
	int x, flag = 0;

	for (int i = 0; i < n; i++){
		flag = 0;
		for (int j = 0; j < n; j++){
			if (tab[i].first == tab[j].second && tab[j].first == tab[i].second){
				flag = 1;
				break;
			}
		}
		if (!flag) return 0;
	}
	return 1;
 }

// A binary relation R over a set X is antisymmetric if:
// for all x,y in X if xRy and yRx then x=y
int is_antisymmetric(pair *tab, int n) {
	int x;

	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			if (tab[i].first == tab[j].second && tab[j].first == tab[i].second){
				if (!(tab[i].first == tab[j].first)) return 0;
			}
		}
	}
	return 1;
}

// A binary relation R over a set X is asymmetric if:
// for all x,y in X if at least one of xRy and yRx is false
int is_asymmetric(pair *tab, int n) {
	int x, flag = 0;

	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			if (tab[i].first == tab[j].second && tab[j].first == tab[i].second) return 0;
		}
	}
	return 1;
}

// A homogeneous relation R on the set X is a transitive relation if:
// for all x, y, z in X, if xRy and yRz, then xRz
int is_transitive(pair *tab, int n) {
	int flag = 0;

	for (int i = 0; i < n; i++){
		flag = 0;
		for (int j = 0; j < n; j++){
			if (tab[i].first == tab[j].second && tab[j].first == tab[i].second){
				for (int k = 0; k < n; k++){
					if (tab[i].first == tab[k].first && tab[j].second == tab[k].second){
						flag = 1;
						break;
					}
				}
				if (!flag) return 0;
			}
		}
	}
	return 1;
}

//------------------------------------------------

// Case 2:

// A partial order relation is a homogeneous relation that is
// reflexive, transitive, and antisymmetric
int is_partial_order(pair *tab, int n) {
	return (is_reflexive(tab, n) && is_transitive(tab, n) && is_antisymmetric(tab, n));
}

// Relation R is connected if for each x, y in X:
// xRy or yRx (or both)
int is_connected(pair *tab, int n) {
	// Assuming 'tab' contains all possible pairs within the set
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j) {
                int x = tab[i].first;
                int y = tab[j].first;
                int connected = 0;

                // Check if xRy or yRx
                for (int k = 0; k < n; k++) {
                    if ((tab[k].first == x && tab[k].second == y) || (tab[k].first == y && tab[k].second == x)) {
                        connected = 1;
                        break;
                    }
                }

                // If neither xRy nor yRx, the relation is not connected
                if (!connected) {
                    return 0;
                }
            }
        }
    }
    return 1; // The relation is connected
}

// A total order relation is a partial order relation that is connected
int is_total_order(pair *tab, int n) {
	return (is_partial_order(tab, n) && is_connected(tab, n));
}

int find_max_elements(pair *tab, int n, int *max_elements) { // tab - partial order
	int flag, flag2, cnt = 0;

	for (int i = 0; i < n; i++){
		flag = 0;
		for (int j = 0; j < n; j++){
			if (tab[i].second != tab[j].second && tab[i].second == tab[j].first){
				flag = 1;
				break;
			}
		}
		if (!flag){
			flag2 = 0;
			for (int k = 0; k < cnt + 1; k++){
				if (max_elements[k] == tab[i].second){
					flag2 = 1;
					break;
				}
			}
			if (!flag2) max_elements[cnt] = tab[i].second, cnt++;
		}
	}
	return cnt;
}

int find_min_elements(pair *tab, int n, int *min_elements) { // tab - strong partial order
	int flag, flag2, cnt = 0;

	for (int i = 0; i < n; i++){
		flag = 0;
		for (int j = 0; j < n; j++){
			if (tab[i].first != tab[j].first && tab[i].first == tab[j].second){
				flag = 1;
				break;
			}
		}
		if (!flag){
			flag2 = 0;
			for (int k = 0; k < cnt; k++){
				if (min_elements[k] == tab[i].first){
					flag2 = 1;
					break;
				}
			}
			if (!flag2) min_elements[cnt] = tab[i].first, cnt++;
		}
	}
	return cnt;
}

int compare(const void *a, const void *b) {
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a == int_b) return 0;
    return (int_a < int_b) ? -1 : 1;
}

int get_domain(pair *tab, int n, int *domain) {
	int cnt = 0, flag;

	for (int i = 0; i < n; i++){
		flag = 0;
		for (int j = 0; j < cnt; j++){
			if (tab[i].first == domain[j]){
				flag = 1;
				break;
			}
		}
		if (!flag) domain[cnt] = tab[i].first, cnt++;

		flag = 0;
		for (int j = 0; j < cnt; j++){
			if (tab[i].second == domain[j]){
				flag = 1;
				break;
			}
		}
		if (!flag) domain[cnt] = tab[i].second, cnt++;
	}

	qsort(domain, cnt, sizeof(domain[0]), compare);

	return cnt;
}

//------------------------------------------------

// Case 3:

// x(S o R)z iff exists y: xRy and ySz
int composition (pair *rel_1, int n1, pair *rel_2, int n2, pair *rel_3) {
	int cnt = 0, flag;

	for (int i = 0; i < n1; i++){
		for (int j = 0; j < n2; j++){
			if (rel_1[i].second == rel_2[j].first){
				flag = 0;
				for (int k = 0; k < cnt; k++){
					if (rel_1[i].first == rel_3[k].first && rel_2[j].second == rel_3[k].second){
						flag = 1;
						break;
					}
				}
				if (!flag){
					pair new;
					new.first = rel_1[i].first;
					new.second = rel_2[j].second;
					rel_3[cnt] = new;
					cnt++;
				}
			}
		}
	}
	return cnt;
}

int main(void) {
	int to_do;
	pair relation[MAX_REL_SIZE];
	pair relation_2[MAX_REL_SIZE];
	pair comp_relation[MAX_REL_SIZE];
	int domain[MAX_REL_SIZE];
	int max_elements[MAX_REL_SIZE];
	int min_elements[MAX_REL_SIZE];

	scanf("%d",&to_do);
	int size = read_relation(relation);
	int ordered, size_2, n_domain;

	switch (to_do) {
		case 1:
			printf("%d ", is_reflexive(relation, size));
			printf("%d ", is_irreflexive(relation, size));
			printf("%d ", is_symmetric(relation, size));
			printf("%d ", is_antisymmetric(relation, size));
			printf("%d ", is_asymmetric(relation, size));
			printf("%d\n", is_transitive(relation, size));
			break;
		case 2:
			ordered = is_partial_order(relation, size);
			n_domain = get_domain(relation, size, domain);
			printf("%d %d\n", ordered, is_total_order(relation, size));
			print_int_array(domain, n_domain);
			if (!ordered) break;
			int no_max_elements = find_max_elements(relation, size, max_elements);
			int no_min_elements = find_min_elements(relation, size, min_elements);
			print_int_array(max_elements, no_max_elements);
			print_int_array(min_elements, no_min_elements);
			break;
		case 3:
			size_2 = read_relation(relation_2);
			printf("%d\n", composition(relation, size, relation_2, size_2, comp_relation));
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	
	return 0;
}

