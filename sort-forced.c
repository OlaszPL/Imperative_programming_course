#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	int day;
	int month;
	int year;
} Date;

// 1 bsearch2

#define NAME_MAX  20   // max. length of name + 1
#define FOOD_MAX  30   // max. number of goods
#define RECORD_MAX 64  // max. line length

typedef struct {
	char name[NAME_MAX];
	float price;
	int amount;
	Date valid_date;
} Food;

typedef int (*ComparFp)(const void *, const void *);

int cmp_date(const void *d1, const void *d2) {
    const Date date1 = *((Date *)d1);
    const Date date2 = *((Date *)d2);
    if (date1.year==date2.year){
        if (date1.month==date2.month){
            return date2.day-date1.day;
        }
        return date2.month-date1.month;
    }
    return date2.year-date1.year;
}


int cmp(const void *a, const void *b) {
    const Food f1 = *((Food *)a);
    const Food f2 = *((Food *)b);
    if (strcmp(f1.name,f2.name)!=0){
        return strcmp(f1.name,f2.name);
    }
    if (f2.price>f1.price) {
        return -1;
    }
    if (f2.price<f1.price) {
            return 1;
    }
    if (cmp_date(&f1.valid_date,&f2.valid_date)!=0){
        return -1*cmp_date(&f1.valid_date,&f2.valid_date);
    }
    return 0;

}

void* bsearch2 (const void *key, const void *base, const size_t nmemb,
		const size_t size, const ComparFp compar, char *result) {
    if (nmemb == 0) {
        *result = 0;
        return base;
    }
    size_t middle = nmemb / 2;
    void* middle_ptr = (void*)base + middle * size;
    int cmp = compar(key, middle_ptr);
    if (cmp == 0) {
        *result = 1;
        return middle_ptr;
    }
    else if (cmp < 0) {
        return bsearch2(key, base, middle, size, compar, result);
    }
    else {
        return bsearch2(key, middle_ptr + size, nmemb - middle - 1, size, compar, result);
    }

}

void print_art(Food *p, const int n, const char *art) {
    for (int i=0;i<n;i++){
        if (strcmp(p[i].name,art)==0){
            printf("%.2f %d %.2d.%.2d.%.2d \n", p[i].price, p[i].amount, p[i].valid_date.day, p[i].valid_date.month, p[i].valid_date.year);
        }
    }
}

Food* add_record(Food *tab, int *np, const ComparFp compar, const Food *new, const int sorted) {
    char num=0;
    Food *found = bsearch2(new, tab,*np, sizeof(Food), compar,&num);
    if (num!=0){
        found -> amount += new -> amount;
    }
    else{
        int i = ((*np)++);
        int ind = (int)(found-tab);
        while(i>ind){
            tab[i] = tab[i-1];
            i--;
        }
        tab[i] = *new;
    }
    return found;

}

int read_goods(Food *tab, const int no, FILE *stream, const int sorted) {
    Food new_product;
    int size = 0;
    for (int i=0; i<no;i++){
        fscanf(stream, "%s %f %d %d.%d.%d", new_product.name, &new_product.price, &new_product.amount, &new_product.valid_date.day, &new_product.valid_date.month, &new_product.valid_date.year);
        add_record(tab, &size, cmp, &new_product,sorted);
    }
    return size;
}

int cmp_qs(const void *a, const void *b) {
	const Food *fa = (Food*)a, *fb = (Food*)b;
	return cmp_date(&fa->valid_date, &fb->valid_date);
}

int cmp_bs(const void *a, const void *b) {
	const Date *pd = (Date*)a;
	const Food *fb = (Food*)b;
	return cmp_date(pd, &fb->valid_date);
}
int cmp_by_date(const void *a, const void *b){
    Food *f1 = (Food*)a;
    Food *f2 = (Food*)b;
    if (f1->valid_date.year<f2->valid_date.year) return -1;
    if (f2->valid_date.year<f1->valid_date.year) return 1;
    if (f1->valid_date.month<f2->valid_date.month) return -1;
    if (f2->valid_date.month<f1->valid_date.month) return 1;
    if (f1->valid_date.day<f2->valid_date.day) return -1;
    if (f2->valid_date.day<f1->valid_date.day) return 1;
    if (cmp(f1->name,f2->name)==0){
        if (f1->price>f2->price) return 1;
        if (f1->price<f2->price) return -1;
        return 0;
    }
    return cmp(f1->name,f2->name);
}
float value(Food *food_tab, const size_t n, const Date curr_date, const int days) {
    qsort(food_tab,n,sizeof(Food),cmp_by_date);
    struct tm cdate = {0};
    cdate.tm_year = curr_date.year - 1900;
    cdate.tm_mon = curr_date.month - 1;
    cdate.tm_mday = curr_date.day;
    time_t ctime = mktime(&cdate);
    float result = 0.0;
    for(int i=0; i<n; i++){
        struct tm fdate = {0};
        fdate.tm_year = food_tab[i].valid_date.year - 1900;
        fdate.tm_mon = food_tab[i].valid_date.month - 1;
        fdate.tm_mday = food_tab[i].valid_date.day;
        time_t ftime = mktime(&fdate);

        if(ctime+((days)*24*3600) == ftime){
            result+=((food_tab[i].amount)*(food_tab[i].price));
        }
    }
    return result;
}

/////////////////////////////////////////////////////////////////
// 3 Succession

#define CHILD_MAX 20

enum Sex {F, M};
enum BOOL {no, yes};

struct Bit_data {
	enum Sex sex:1;
	enum BOOL in_line:1;
};

typedef struct {
	char *name;
	struct Bit_data bits;
	Date born;
	char *parent;
} Person;

typedef struct {
	char *par_name;
	int index;
	int children;
} Parent;

const Date primo_date = { 28, 10, 2011 }; // new succession act

int cmp_primo(const Person *person1, const Person *person2) {
	if (person1->bits.sex == person2->bits.sex) return 0;
	if (person1->bits.sex == F && cmp_date(&person1->born, &primo_date) > 0) return 0;
	if (person2->bits.sex == F && cmp_date(&person2->born, &primo_date) > 0) return 0;
	return person2->bits.sex - person1->bits.sex;
}

int comp_date(const void* a, const void* b){
    Date d1 = *((Date*)a);
    Date d2 = *((Date*)b);
    if(d1.year < d2.year) return -1;
    if(d1.year > d2.year) return 1;
    if(d1.month < d2.month) return -1;
    if(d1.month > d2.month) return 1;
    if(d1.day < d2.day) return -1;
    if(d1.day > d2.day) return 1;
    return 0;
}

int cmp_person(const void *a, const void *b) {
    Person first = *((Person*)a);
    Person second = *((Person*)b);
    if(first.parent==NULL) return -1;
    if (second.parent==NULL) return 1;
    if(strcmp(first.parent,second.parent)<0) return -1;
    if (strcmp(first.parent,second.parent)>0) return 1;
    if (comp_date(&first.born,&primo_date)<0 && comp_date(&second.born,&primo_date)<0){
        if(first.bits.sex == F && second.bits.sex == M) return 1;
        if(first.bits.sex == M && second.bits.sex == F) return -1;
    }
    if (comp_date(&first.born,&second.born)>0) return 1;
    if (comp_date(&first.born,&second.born)<0) return -1;
    if (first.bits.sex > second.bits.sex) return 1;
    if (first.bits.sex < second.bits.sex) return -1;
    return 0;
}

int cmp_parent(const void *a, const void *b) {
    Parent first = *((Parent*)a);
    Parent second = *((Parent*)b);
    return strcmp(first.par_name,second.par_name);
}

int fill_indices_tab(Parent *idx_tab, const Person *pers_tab, const int size) {
    int numb=0;
    for(int i=0; i<size; i++){
        idx_tab[i].par_name = pers_tab[i].name;
    }
    for(int i=0; i<size; i++){
        idx_tab[i].index = 0;
    }
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(pers_tab[j].parent == idx_tab[i].par_name){
                idx_tab[i].index = j;
                break;
            }
        }
    }
    for(int i=0; i<size; i++){
        int cnt = 0, start = idx_tab[i].index;
        for(int j=start; j<size; j++){
            if(pers_tab[j].parent == pers_tab[i].name) {cnt++;}
            else {break;}
        }
        idx_tab[i].children = cnt;
    }
    for(int i=0; i<size; i++){
        if(idx_tab[i].children >0) numb+=1;
    }
    return numb;
}

void persons_shifts(Person *person_tab, const int size, Parent *idx_tab, int no_parents) {
    for(int i=0; i<size; i++){
        if(idx_tab[i].children >0){
            Person *shifted_arr = malloc((idx_tab[i].children)*sizeof(Person));
            memmove(shifted_arr, &person_tab[idx_tab[i].index], (idx_tab[i].children)*sizeof(Person));
            int ile = idx_tab[i].index - i - 1;
            int gdzie = idx_tab[i].index - ile + idx_tab[i].children;
            memmove(&person_tab[gdzie],&person_tab[i+1],ile*sizeof(Person));
            memmove(&person_tab[i+1], shifted_arr, idx_tab[i].children*sizeof(Person));
            free(shifted_arr);
            no_parents = fill_indices_tab(idx_tab, person_tab, size);
        }
    }
}

int cleaning(Person *person_tab, int n) {
    for(int i=0; i<n; i++){
        if(person_tab[i].bits.in_line == no){
            memmove(&person_tab[i], &person_tab[i + 1], (n - i - 1)*sizeof(Person));
            i--;
            n--;
        }
    }
    return n;
}

void print_person(const Person *p) {
	printf("%s\n", p->name);
}

int create_list(Person *person_tab, int n) {
    int no_par, no_per;
    qsort(person_tab, n, sizeof(Person), cmp_person);
    Parent id_tab[33];
    no_par = fill_indices_tab(id_tab, person_tab, n);
    persons_shifts(person_tab, n, id_tab, no_par);
    return cleaning(person_tab,n);
}

int main(void) {
	Person person_tab[] = {
		{"Charles III", {M, no}, {14, 11, 1948},"Elizabeth II"},
		{"Anne", {F,yes}, {15, 8, 1950},"Elizabeth II"},
		{"Andrew", {M,yes}, {19, 2, 1960},"Elizabeth II"},
		{"Edward", {M,yes}, {10, 3, 1964} ,"Elizabeth II"},
		{"David", {M,yes}, {3, 11, 1961} ,"Margaret"},
		{"Sarah", {F,yes}, {1, 5, 1964}, "Margaret"},
		{"William", {M,yes}, {21, 6, 1982}, "Charles III"},
		{"Henry", {M,yes}, {15, 9, 1984}, "Charles III"},
		{"Peter", {M,yes}, {15, 11, 1977}, "Anne"},
		{"Zara", {F,yes}, {15, 5, 1981}, "Anne"},
		{"Beatrice", {F,yes}, {8, 8, 1988}, "Andrew"},
		{"Eugenie", {F,yes}, {23, 3, 1990}, "Andrew"},
		{"James", {M,yes}, {17, 12, 2007}, "Edward"},
		{"Louise", {F,yes}, {8, 11, 2003}, "Edward"},
		{"Charles", {M,yes}, {1, 7, 1999}, "David"},
		{"Margarita", {F,yes}, {14, 5, 2002}, "David"},
		{"Samuel", {M,yes}, {28, 7, 1996}, "Sarah"},
		{"Arthur", {M,yes}, {6, 5, 2019}, "Sarah"},
		{"George", {M,yes}, {22, 7, 2013}, "William"},
		{"George VI", {M,no}, {14, 12, 1895}, NULL},
		{"Charlotte", {F,yes}, {2, 5, 2015}, "William"},
		{"Louis", {M,yes}, {23, 4, 2018}, "William"},
		{"Archie", {M,yes}, {6, 5, 2019}, "Henry"},
		{"Lilibet", {F,yes}, {4, 6, 2021}, "Henry"},
		{"Savannah", {F,yes}, {29, 12, 2010}, "Peter"},
		{"Isla", {F,yes}, {29, 3, 2012}, "Peter"},
		{"Mia", {F,yes}, {17, 1, 2014}, "Zara"},
		{"Lena", {F,yes}, {18, 6, 2018}, "Zara"},
		{"Elizabeth II", {F,no}, {21, 4, 1926}, "George VI"},
		{"Margaret", {F,no}, {21, 8, 1930}, "George VI"},
		{"Lucas", {M,yes}, {21, 3, 2021}, "Zara"},
		{"Sienna", {F,yes}, {18, 9, 2021}, "Beatrice"},
		{"August", {M,yes}, {9, 2, 2021}, "Eugenie"},
		{"Ernest", {M,yes}, {30, 5, 2023}, "Eugenie"}
	};

	int to_do, no;
	int n;
	Food food_tab[FOOD_MAX];
	char buff[RECORD_MAX];
	fgets(buff, RECORD_MAX, stdin);
	sscanf(buff, "%d", &to_do);

	switch (to_do) {
		case 1:  // bsearch2
			fgets(buff, RECORD_MAX, stdin);
			sscanf(buff, "%d", &no);
			n = read_goods(food_tab, no, stdin, 1);
			scanf("%s",buff);
			print_art(food_tab, n, buff);
			break;
		case 2: // qsort
			fgets(buff, RECORD_MAX, stdin);
			sscanf(buff, "%d", &no);
			n = read_goods(food_tab, no, stdin, 0);
			Date curr_date;
			int days;
			scanf("%d %d %d", &curr_date.day, &curr_date.month, &curr_date.year);
			scanf("%d", &days);
			printf("%.2f\n", value(food_tab, (size_t)n, curr_date, days));
			break;
		case 3: // succession
			scanf("%d", &no);
			int no_persons = sizeof(person_tab) / sizeof(Person);
			no_persons = create_list(person_tab,no_persons);
			print_person(person_tab + no - 1);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
	}
	return 0;
}

