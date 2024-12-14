#include <OpenDoor.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <ctype.h>
#include "portvictoria.h"

extern struct shipdata_t **ships;
extern int ship_count;
extern struct playerdata_t *player;

int shipyard(void) {
	int i;
	char c;
	int j;
	
	od_clr_scr();
	
	od_printf("`bright yellow`You have %d gold coins.\r\n\r\n", player->dollars);
	
	for (i=0;i<ship_count;i++) {
		od_printf("        `bright white`(`bright yellow`%d`bright white`) %15s Price: %6d Trade-in: %6d %s\r\n", i+1, ships[i]->name, ships[i]->value, ships[i]->value / 2, (ships[i]->id == player->ship->id ? "(You own this)" : "" ));
	}
	od_printf("\r\n        `bright white`(`bright yellow`R`bright white`) Return to the docks\r\n");
	od_printf("        `bright white`(`bright red`Q`bright white`) Quit to BBS\r\n`white`");
	
	c = od_get_key(TRUE);
	
	switch(tolower(c)) {
			case 'r':
				return 0;
			case 'q':
				return 1;
			default:
				j = c - '0';
				if (j > 0 && j <= i) {
					j--;
					if (player->dollars + (player->ship->value / 2) < ships[j]->value) {
						od_printf("\r\nYou can't afford that!\r\n\r\nPress ENTER to continue...\r\n");
						od_get_key(TRUE);
						return 0;
					}
					od_printf("\r\nAre you sure you want to trade in your %s and buy a %s?\r\n", player->ship->name, ships[j]->name);
					if (tolower(od_get_answer("YyNn")) == 'y') {
						player->dollars = player->dollars - (ships[j]->value - (player->ship->value / 2));
						player->ship = ships[j];
						
						od_printf("Congratulations! You're now the proud owner of a shiny new %s.\r\n\r\nPress ENTER to continue...\r\n", player->ship->name);
						
						od_get_key(TRUE);
					}
					return 0;
				}
				return 0;
	}
}
