#include <OpenDoor.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <ctype.h>
#include "portvictoria.h"

extern struct playerdata_t *player;

void twoup(void) {
	int wager;
	int coin1;
	int coin2;
	int done = 0;
	char c;
	char buffer[16];
	
	srand(time(NULL));
	
	od_printf("How much do you wager? (You have %d, Type 0 to back out) : ", player->dollars);
	od_input_str(buffer, 15, '0', '9');
	wager = atoi(buffer);
	
	if (wager == 0)
		return;
	
	if (wager > player->dollars) {
		od_printf("You can't afford that!\r\n");
		return;
	}
	
	do {
		od_printf("\r\nTo flip the coins, press ENTER...\r\n");
		od_get_key(TRUE);
		
		coin1 = rand() % 2;
		coin2 = rand() % 2;
		
		if (coin1 != coin2) {
			od_printf("You flipped a %s and a %s! Flip again!\r\n", (coin1 == 1 ? "Heads" : "Tails"), (coin2 == 1 ? "Heads" : "Tails"));
		} else {
			if (coin1 == 0) {
				od_printf("Two tails. You lose.\r\n");
				player->dollars -= wager;
				done = 1;
			} else if (coin1 == 1) {
				od_printf("Two heads! You Win!\r\n");
				player->dollars += wager;
				done = 1;
			} else {
				od_printf("Something strange happened... coin1 = %d and coin2 = %d\r\n", coin1, coin2);
			}
		}
	} while(!done);
	
	od_printf("\r\nGame over, Press ENTER...\r\n");
	od_get_key(TRUE);
		
}

int tavern(void) {
	FILE *fptr;
	char c;
	
	while (1) {
		od_send_file("tavern_header.ans");
		
		od_printf("\r\n`bright yellow`The tavern is just like any other tavern.\r\n\r\n");
		od_printf("        `bright white`(`bright yellow`N`bright white`) Read a copy of the Seafarer's Daily\r\n");
		od_printf("        `bright white`(`bright yellow`T`bright white`) Play a game of two-up.\r\n");
		od_printf("        `bright white`(`bright yellow`S`bright white`) Find out what the captains of the realm are worth\r\n");
		od_printf("\r\n        `bright white`(`bright yellow`R`bright white`) Return to the docks\r\n");
		od_printf("        `bright white`(`bright red`Q`bright white`) Quit to BBS`white`\r\n");
	
		c = od_get_key(TRUE);
	
		switch (tolower(c)) {
			case 'n':
				od_send_file("todays_news.ans");
				od_get_key(TRUE);
				break;
			case 't':
				twoup();
				break;
			case 's':
				od_send_file("scores.ans");
				od_get_key(TRUE);
				break;
			case 'r':
				return 1;
			case 'q':
				return 0;
		}
	}
}
