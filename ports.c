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
extern int port_count;
extern struct portdata_t **ports;

int display_current_location(void) {
	int i;
	
	od_send_file("docks.ans");
	
	
	od_printf("`bright yellow`You're standing on the docks of `bright white`%s`bright yellow`.\r\n\r\n", player->current_port->name);
	od_printf("\r\n\r\n");
	od_printf("Your %s has %d of %d cargo spaces free. You have %d gold coins.\r\n\r\n", player->ship->name, player->ship->cargo_units - total_cargo_spaces_used(), player->ship->cargo_units, player->dollars);
	od_printf("        `bright white`(`bright yellow`M`bright white`) Enter the Marketplace\r\n");
	od_printf("        `bright white`(`bright yellow`T`bright white`) Visit the Tavern\r\n");
	od_printf("        `bright white`(`bright yellow`S`bright white`) Visit the Shipyard\r\n");
	od_printf("        `bright white`(`bright yellow`C`bright white`) Continue voyage\r\n\r\n");
	od_printf("        `bright white`(`bright red`Q`bright white`) Quit to BBS\r\n");
	     
	while (1) {
		switch(tolower(od_get_key(TRUE))) {
			case 'm':
				return 1;
			case 't':
				return 4;
			case 'c':
				return 2;
			case 'q':
				return 3;
			case 's':
				return 5;
		}
	}
}

int select_destination(void) {
	struct portdata_t *dest1 = NULL;
	struct portdata_t *dest2 = NULL;
	struct portdata_t *dest3 = NULL;
	struct portdata_t *dest4 = NULL;
	
	int i;
	char c;
	
	for (i=0;i<port_count;i++) {
		if (ports[i]->id == player->current_port->location1) {
			dest1 = ports[i];
		}
		if (ports[i]->id == player->current_port->location2) {
			dest2 = ports[i];
		}
		if (ports[i]->id == player->current_port->location3) {
			dest3 = ports[i];
		}
		if (ports[i]->id == player->current_port->location4) {
			dest4 = ports[i];
		}
	}
	
	while (1) {
		od_clr_scr();
		od_printf("`bright yellow`Studying your map, you see four nearby destinations.\r\nYou have %d months of travel left this turn.\r\n\r\n", MONTHS_PER_DAY - player->months_used);
	
		if (dest1 != NULL) {
			od_printf("        `bright white`(`bright yellow`1`bright white`) %s (%d months travel time)\r\n", dest1->name, player->current_port->location1cost);
		}
		if (dest2 != NULL) {
			od_printf("        `bright white`(`bright yellow`2`bright white`) %s (%d months travel time)\r\n", dest2->name, player->current_port->location2cost);
		}
		if (dest3 != NULL) {
			od_printf("        `bright white`(`bright yellow`3`bright white`) %s (%d months travel time)\r\n", dest3->name, player->current_port->location3cost);
		}
		if (dest4 != NULL) {
			od_printf("        `bright white`(`bright yellow`4`bright white`) %s (%d months travel time)\r\n", dest4->name, player->current_port->location4cost);
		}
	
		od_printf("\r\n        `bright white`(`bright yellow`R`bright white`) Return to the docks\r\n");
		od_printf("        `bright white`(`bright red`Q`bright white`) Quit to BBS\r\n");
	
		c = od_get_key(TRUE);
	
		switch (c) {
			case '1':
				if (dest1 != NULL) {
					if (MONTHS_PER_DAY - player->months_used >= player->current_port->location1cost) {
						player->months_used += player->current_port->location1cost;
						player->current_port = dest1;
						return 1;
					} else {
						od_printf("Not enough months left.\r\n");
					}
				}
				break;
			case '2':
				if (dest2 != NULL) {
					if (MONTHS_PER_DAY - player->months_used >= player->current_port->location2cost) {
						player->months_used += player->current_port->location2cost;
						player->current_port = dest2;
						return 1;
					} else {
						od_printf("Not enough months left.\r\n");
					}
				}
				break;
			case '3':
				if (dest3 != NULL) {
					if (MONTHS_PER_DAY - player->months_used >= player->current_port->location3cost) {
						player->months_used += player->current_port->location3cost;
						player->current_port = dest3;
						return 1;
					} else {
						od_printf("Not enough months left.\r\n");
					}
				}
				break;
			case '4':
				if (dest4 != NULL) {
					if (MONTHS_PER_DAY - player->months_used >= player->current_port->location4cost) {
						player->months_used += player->current_port->location4cost;
						player->current_port = dest4;
						return 1;
					} else {
						od_printf("Not enough months left.\r\n");
					}
				}
				break;
			case 'q':
			case 'Q':
				return 0;
			case 'r':
			case 'R':
				return 1;
		}
	}
}
