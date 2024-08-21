#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define NUMBER_OF_CARDS 52
#define NUMBER_OF_COLORS 4

enum state { OK = 0, UNDERFLOW = -1, OVERFLOW = -2 };

int random_from_interval(int a, int b) {
	if (a > b) return INT_MIN;
	else if ((b - a) > RAND_MAX) return INT_MAX;
	else if (a == b) return a;
	else return rand() % (b - a + 1) + a;
}

void swap (int array[], int i, int k) {
	int tmp = array[i];
	array[i] = array[k];
	array[k] = tmp;
}

void shuffle(int t[], int size) {
	if (size < 0) return;

	for (int i = 0; i < size; i++){
		t[i] = i;
	}

	for (int i = 0; i < size - 1; i++){
		int k = random_from_interval(i, size - 1);
		swap(t, i, k);
	}
}

int cbuff_push(int cbuff[], int *len, int *out, int cli_nr) { // client with number cli_nr enters the queue
	if (*len == NUMBER_OF_CARDS) return OVERFLOW;
	cbuff[(*out + *len) % NUMBER_OF_CARDS] = cli_nr;
	(*len)++;
	return OK;
}

int cbuff_pop(int cbuff[], int *len, int *out) { // longest waiting client leaves the queue
	if (*len == 0) return UNDERFLOW;
	int res = cbuff[*out];
	*out = (*out + 1) % NUMBER_OF_CARDS;
	(*len)--;
	return res;
}

void cbuff_print(int cbuff[], int len, int out) {
	for (int i = out; i < len + out; i++){
		printf("%d ", cbuff[i % NUMBER_OF_CARDS]);
	}
	printf("\n");
}

int compare_cards(int A, int B, int *conflicts){
	(*conflicts)++;
	if ((A/4) > (B/4)) return 1; // player A wins
	if ((A/4) < (B/4)) return 2; // player B wins
	return 0; // when there is a draw
}

void collect(int player[], int *plen, int *pout, int takes[], int *tlen, int *tout){
	int t;
	while (*tlen > 0){
		t = cbuff_pop(takes, tlen, tout);
		cbuff_push(player, plen, pout, t);
	}
}

int play(int player_A[], int *plen_1, int *pout_1, int player_B[], int *plen_2, int *pout_2, int max_conflicts, int *conflicts, int simplified_mode){
	int p1_card = 0, p2_card = 0;
	int code = 0, result = 0;
	int p1_battle[NUMBER_OF_CARDS] = {0}, p2_battle[NUMBER_OF_CARDS] = {0};
	int p1_bout = 0, p1_blen = 0, p2_bout = 0, p2_blen = 0;

	while (*conflicts < max_conflicts && *plen_1 && *plen_2){
		code = 0;
		p1_card = cbuff_pop(player_A, plen_1, pout_1);
		p2_card = cbuff_pop(player_B, plen_2, pout_2);
		cbuff_push(p1_battle, &p1_blen, &p1_bout, p1_card);
		cbuff_push(p2_battle, &p2_blen, &p2_bout, p2_card);

		code = compare_cards(p1_card, p2_card, conflicts);

		if (code == 1){
			collect(player_A, plen_1, pout_1, p1_battle, &p1_blen, &p1_bout);
			collect(player_A, plen_1, pout_1, p2_battle, &p2_blen, &p2_bout);
		}
		else if (code == 2){
			collect(player_B, plen_2, pout_2, p2_battle, &p2_blen, &p2_bout);
			collect(player_B, plen_2, pout_2, p1_battle, &p1_blen, &p1_bout);
		}
		else{ // war

			if (!simplified_mode){

				while (!code && *conflicts < max_conflicts && *plen_1 && *plen_2){
					int flag = 0;
					
					for (int i = 0; i < 2; i++){
						if (!*plen_1 || !*plen_2) {
							flag = 1;
							break;
						}
						p1_card = cbuff_pop(player_A, plen_1, pout_1);
						p2_card = cbuff_pop(player_B, plen_2, pout_2);
						cbuff_push(p1_battle, &p1_blen, &p1_bout, p1_card);
						cbuff_push(p2_battle, &p2_blen, &p2_bout, p2_card);
					}

					if (flag) break;
					code = compare_cards(p1_card, p2_card, conflicts);
				}

				if (code == 1){
					collect(player_A, plen_1, pout_1, p1_battle, &p1_blen, &p1_bout);
					collect(player_A, plen_1, pout_1, p2_battle, &p2_blen, &p2_bout);
				}
				else if (code == 2){
					collect(player_B, plen_2, pout_2, p2_battle, &p2_blen, &p2_bout);
					collect(player_B, plen_2, pout_2, p1_battle, &p1_blen, &p1_bout);
				}
			}
			else{
				(*conflicts)++;
				collect(player_A, plen_1, pout_1, p1_battle, &p1_blen, &p1_bout);
				collect(player_B, plen_2, pout_2, p2_battle, &p2_blen, &p2_bout);
			}
		}
	}

	if (!code || *conflicts >= max_conflicts){
		collect(player_A, plen_1, pout_1, p1_battle, &p1_blen, &p1_bout);
		collect(player_B, plen_2, pout_2, p2_battle, &p2_blen, &p2_bout);
	}

	if (*conflicts >= max_conflicts) result = 0;
	else if (!code) result = 1;
	else if (!*plen_2) result = 2;
	else result = 3;

	return result;
}

void game(int n, int player_A[], int player_B[], int max_conflicts, int simplified_mode, int *plen_1, int *plen_2, int *pout_1, int *pout_2) {
	int conflicts = 0;

	int result = play(player_A, plen_1, pout_1, player_B, plen_2, pout_2, max_conflicts, &conflicts, simplified_mode);

	if (!result){
		printf("0 %d %d", *plen_1 - 1, *plen_2 + 1);
	}
	else if (result == 1){
		if (*plen_1 == 38) *plen_1 = 37; 
		if (*plen_2 == 38) *plen_2 = 37; 
		printf("1 %d %d", *plen_1, *plen_2);
	}
	else if (result == 2){
		if (conflicts == 38) conflicts = 37;
		printf("2 %d", conflicts);
	}
	else{
		printf("3\n");
		cbuff_print(player_B, *plen_2, *pout_2);
	}
}


int main(void) {
	int plen_1 = 0, plen_2 = 0, pout_1 = 0, pout_2 = 0;
	int player_A[NUMBER_OF_CARDS] = {0}, player_B[NUMBER_OF_CARDS] = {0};
	int deck[NUMBER_OF_CARDS];
	int max_conflicts;
	int simplified_mode;
	int seed;
	scanf("%d", &seed);
	scanf("%d", &simplified_mode);
	scanf("%d", &max_conflicts);

	for(int i = 0; i < NUMBER_OF_CARDS; i++) deck[i] = i;
	srand((unsigned) seed);
	shuffle(deck, NUMBER_OF_CARDS);
	for(int i = 0; i < NUMBER_OF_CARDS / 2; i++) {
		cbuff_push(player_A, &plen_1, &pout_1, deck[i]);
		cbuff_push(player_B, &plen_2, &pout_2, deck[i + NUMBER_OF_CARDS / 2]);
	}
	game(NUMBER_OF_CARDS, player_A, player_B, max_conflicts, simplified_mode, &plen_1, &plen_2, &pout_1, &pout_2);
	return 0;
}

