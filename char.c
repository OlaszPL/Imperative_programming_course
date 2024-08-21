#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_BIGRAMS ((LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR))

#define NEWLINE '\n'
#define IN_WORD 1

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2

int count[MAX_BIGRAMS] = { 0 };

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	if (count[va] == count[vb]) return va - vb;	return count[vb] - count[va];
}

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp_di (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	// sort according to second char if counts and the first char equal
	if (count[va] == count[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
	// sort according to first char if counts equal
	if (count[va] == count[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
	return count[vb] - count[va];
}

char is_ending_char(char x){
	return x == '\n' || x == ' ' || x == '\t';
}

// count lines, words & chars in a given text file
void wc(int *nl, int *nw, int *nc) {
	int a, lastchar = '\n';
	*nl = 0, *nw = 0, *nc = 0;

	while((a = getchar()) != EOF){
		if (a == '\0') continue;

		if (a == '\n') (*nl)++;

		if (!is_ending_char(lastchar) && is_ending_char(a)) (*nw)++;

		(*nc)++;
		lastchar = a;
	}
}

void char_count(int char_no, int *n_char, int *cnt) {
	int a;

	while ((a = getchar()) != EOF ){
		if (a >= FIRST_CHAR && a <= LAST_CHAR - 1){
			count[a - FIRST_CHAR]++;
		}
	}

	int idx[MAX_CHARS];

	for (int i = 0; i < MAX_CHARS; i++) idx[i] = i;

	qsort(idx, MAX_CHARS, sizeof(idx[0]), cmp);

	(*n_char) = idx[char_no - 1] + FIRST_CHAR; // -1 because array index start with 0
	(*cnt) = count[idx[char_no - 1]];
}

void bigram_count(int bigram_no, int bigram[]) {
	int a, prev;

	prev = getchar();
	if (prev == EOF) return;

	while ((a = getchar()) != EOF){
		if (!is_ending_char(prev) && !is_ending_char(a) && a != '\0' && prev != '\0' && prev >= FIRST_CHAR && prev <= LAST_CHAR - 1 && a >= FIRST_CHAR && a <= LAST_CHAR - 1){
			count[(prev - FIRST_CHAR) * MAX_CHARS + (a - FIRST_CHAR)]++;
		}
		prev = a;
	}

	int idx[MAX_BIGRAMS];
	for (int i = 0; i < MAX_BIGRAMS; i++) idx[i] = i;

	qsort(idx, MAX_BIGRAMS, sizeof(idx[0]), cmp_di);

	bigram[0] = (idx[bigram_no - 1] / MAX_CHARS) + FIRST_CHAR;
    bigram[1] = (idx[bigram_no - 1] % MAX_CHARS) + FIRST_CHAR;
    bigram[2] = count[idx[bigram_no - 1]];
}

void find_comments(int *line_comment_counter, int *block_comment_counter) {
    int a, prev_a = 0;
    *line_comment_counter = 0, *block_comment_counter = 0;

    int flag = 0; // 0 - nothing, 1 - line comment, 2 - block comment
    
    while ((a = getchar()) != EOF){
        if (flag == 0){
            if (a == '/'){
                a = getchar();
                if (a == '/') {
                    (*line_comment_counter)++;
                    flag = 1;
                }
				else if (a == '*') {
                    (*block_comment_counter)++;
                    flag = 2;
                }
            }
        }
        else if (flag == 1 && a == '\n') flag = 0;

        else if (flag == 2){
            if (prev_a == '*' && a == '/') flag = 0;
        }

        prev_a = a; // remember the last character
    }
}

#define MAX_LINE 128

int read_int() {
	char line[MAX_LINE];
	fgets(line, MAX_LINE, stdin); // to get the whole line
	return (int)strtol(line, NULL, 10);
}

int main(void) {
	int to_do;
	int nl, nw, nc, char_no, n_char, cnt;
	int line_comment_counter, block_comment_counter;
	int bigram[3];

	to_do = read_int();
	switch (to_do) {
		case 1: // wc()
			wc (&nl, &nw, &nc);
			printf("%d %d %d\n", nl, nw, nc);
			break;
		case 2: // char_count()
			char_no = read_int();
			char_count(char_no, &n_char, &cnt);
			printf("%c %d\n", n_char, cnt);
			break;
		case 3: // bigram_count()
			char_no = read_int();
			bigram_count(char_no, bigram);
			printf("%c%c %d\n", bigram[0], bigram[1], bigram[2]);
			break;
		case 4:
			find_comments(&line_comment_counter, &block_comment_counter);
			printf("%d %d\n", block_comment_counter, line_comment_counter);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}

