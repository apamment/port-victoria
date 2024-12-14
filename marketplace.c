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

char *types[] = {"Food", "Sugar", "Tobacco" , "Cotton", "Hides", "Lumber", "Iron", "Silver", "Horses", "Grog", "Clothes", "Muskets"};

int total_cargo_spaces_used(void) {
	return player->food + player->sugar + player->tobacco + player->cotton + player->hides + player->lumber + player->iron + player->silver + player->horses + player->grog + player->clothes + player->muskets;
}

void buysell(int type) {
	int quantity_owned = 0;
	int sell_price = 0;
	int buy_price = 0;
	char buffer[16];
	int quantity = 0;
	char c;

	switch (type) {
		case 1:
			quantity_owned = player->food;
			buy_price = (int)(5.0f / 100 * player->current_port->food);
			sell_price = (int)(10.0f / 100 * player->current_port->food);
			break;
		case 2:
			quantity_owned = player->sugar;
			buy_price = (int)(5.0f / 100 * player->current_port->sugar);
			sell_price = (int)(10.0f / 100 * player->current_port->sugar);
			break;
		case 3:
			quantity_owned = player->tobacco;
			buy_price = (int)(5.0f / 100 * player->current_port->tobacco);
			sell_price = (int)(10.0f / 100 * player->current_port->tobacco);
			break;
		case 4:
			quantity_owned = player->cotton;
			buy_price = (int)(5.0f / 100 * player->current_port->cotton);
			sell_price = (int)(10.0f / 100 * player->current_port->cotton);
			break;
		case 5:
			quantity_owned = player->hides;
			buy_price = (int)(5.0f / 100 * player->current_port->hides);
			sell_price = (int)(10.0f / 100 * player->current_port->hides);
			break;
		case 6:
			quantity_owned = player->lumber;
			buy_price = (int)(5.0f / 100 * player->current_port->lumber);
			sell_price = (int)(10.0f / 100 * player->current_port->lumber);
			break;
		case 7:
			quantity_owned = player->iron;
			buy_price = (int)(5.0f / 100 * player->current_port->iron);
			sell_price = (int)(10.0f / 100 * player->current_port->iron);
			break;
		case 8:
			quantity_owned = player->silver;
			buy_price = (int)(25.0f / 100 * player->current_port->silver);
			sell_price = (int)(50.0f / 100 * player->current_port->silver);
			break;
		case 9:
			quantity_owned = player->horses;
			buy_price = (int)(25.0f / 100 * player->current_port->horses);
			sell_price = (int)(50.0f / 100 * player->current_port->horses);
			break;
		case 10:
			quantity_owned = player->grog;
			buy_price = (int)(25.0f / 100 * player->current_port->grog);
			sell_price = (int)(50.0f / 100 * player->current_port->grog);
			break;
		case 11:
			quantity_owned = player->clothes;
			buy_price = (int)(25.0f / 100 * player->current_port->clothes);
			sell_price = (int)(50.0f / 100 * player->current_port->clothes);
			break;
		case 12:
			quantity_owned = player->muskets;
			buy_price = (int)(50.0f / 100 * player->current_port->muskets);
			sell_price = (int)(100.0f / 100 * player->current_port->muskets);
			break;

	}

	od_printf("Would you like to [B]uy or [S]ell?\r\n");
	while (1) {
		c = od_get_key(TRUE);
		if (tolower(c) == 'b') {
			// buying
			if (player->ship->cargo_units - total_cargo_spaces_used() > 0) {
				while (1) {
					od_printf("How many crates would you like to buy? (Max. %d)\r\n", (player->dollars / sell_price < player->ship->cargo_units - total_cargo_spaces_used() ? player->dollars / sell_price : player->ship->cargo_units - total_cargo_spaces_used()));
					od_input_str(buffer, 15, '0', '9');
					quantity = atoi(buffer);
					if (quantity == 0) {
						break;
					}
					if (quantity <= player->ship->cargo_units - total_cargo_spaces_used()) {
						od_printf("\r\nBuy %d crates of %s for %d? (Y/N)\r\n", quantity, types[type-1], sell_price * quantity);
						if (tolower(od_get_answer("YyNn")) == 'y') {
							break;
						} else {
							return;
						}
					} else {
						od_printf("\r\nYou don't have enough cargo spaces!\r\n");
					}
				}
			} else {
				od_printf("You have no free cargo spaces!\r\n");
			}
			break;
		} else if (tolower(c) == 's') {
			if (quantity_owned > 0) {
				while (1) {
					od_printf("How many would you like to sell? (Max. %d)\r\n", quantity_owned);
					od_input_str(buffer, 15, '0', '9');
					quantity = atoi(buffer);
					if (quantity == 0) {
						break;
					}
					if (quantity <= quantity_owned) {
						od_printf("\r\nSell %d crates of %s for %d? (Y/N)\r\n", quantity, types[type-1], buy_price * quantity);
						if (tolower(od_get_answer("YyNn")) == 'y') {
							quantity = -quantity;
							break;
						} else {
							return;
						}
					} else {
						od_printf("\r\nYou don't have that many to sell!\r\n");
					}
				}
			} else {
				od_printf("You don't have any crates of%s!\r\n");
				return;
			}
			// selling
			break;
		}
	}
	if (quantity == 0) {
		return;
	}
	if (quantity > 0) {
		if (sell_price * quantity <= player->dollars) {
			player->dollars -= sell_price * quantity;
		} else {
			od_printf("You can't afford that!\r\n");
			return;
		}
	}
	if (quantity < 0) {
		player->dollars += buy_price * (-quantity);

	}

	switch (type) {
		case 1:
			player->food += quantity;
			break;
		case 2:
			player->sugar += quantity;
			break;
		case 3:
			player->tobacco += quantity;
			break;
		case 4:
			player->cotton += quantity;
			break;
		case 5:
			player->hides += quantity;
			break;
		case 6:
			player->lumber += quantity;
			break;
		case 7:
			player->iron += quantity;
			break;
		case 8:
			player->silver += quantity;
			break;
		case 9:
			player->horses += quantity;
			break;
		case 10:
			player->grog += quantity;
			break;
		case 11:
			player->clothes += quantity;
			break;
		case 12:
			player->muskets += quantity;
			break;
	}
}

void gossip_prices(struct portdata_t *port) {
	od_printf("\r\n\r\nYou manage to find a fellow seafarer who has\r\nrecently arrived from %s...\r\n\r\n", port->name);
	if (port->food > 0) {
		od_printf("`bright white`            Food: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(5.0f / 100 * port->food), (int)(10.0f / 100 * port->food));
		if (player->current_port->food > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->food > player->current_port->food ? "`bright green`\x18" : "`bright red`\x19"), abs(port->food - player->current_port->food));
		} else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->sugar > 0) {
		od_printf("`bright white`           Sugar: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(5.0f / 100 * port->sugar), (int)(10.0f / 100 * port->sugar));
		if (player->current_port->sugar > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->sugar > player->current_port->sugar ? "`bright green`\x18" : "`bright red`\x19"), abs(port->sugar - player->current_port->sugar));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}

	if (port->tobacco > 0) {
		od_printf("`bright white`         Tobacco: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(5.0f / 100 * port->tobacco), (int)(10.0f / 100 * port->tobacco));
		if (player->current_port->tobacco > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->tobacco > player->current_port->tobacco ? "`bright green`\x18" : "`bright red`\x19"), abs(port->tobacco - player->current_port->tobacco));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->cotton > 0) {
		od_printf("`bright white`          Cotton: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(5.0f / 100 * port->cotton), (int)(10.0f / 100 * port->cotton));
		if (player->current_port->cotton > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->cotton > player->current_port->cotton ? "`bright green`\x18" : "`bright red`\x19"), abs(port->cotton - player->current_port->cotton));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->hides > 0) {
		od_printf("`bright white`           Hides: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(5.0f / 100 * port->hides), (int)(10.0f / 100 * port->hides));
		if (player->current_port->hides > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->hides > player->current_port->hides ? "`bright green`\x18" : "`bright red`\x19"), abs(port->hides - player->current_port->hides));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->lumber > 0) {
		od_printf("`bright white`          Lumber: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(5.0f / 100 * port->lumber), (int)(10.0f / 100 * port->lumber));
		if (player->current_port->lumber > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->lumber > player->current_port->lumber ? "`bright green`\x18" : "`bright red`\x19"), abs(port->lumber - player->current_port->lumber));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->iron > 0) {
		od_printf("`bright white`            Iron: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(5.0f / 100 * port->iron), (int)(10.0f / 100 * port->iron));
		if (player->current_port->iron > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->iron > player->current_port->iron ? "`bright green`\x18" : "`bright red`\x19"), abs(port->iron - player->current_port->iron));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->silver > 0) {
		od_printf("`bright white`          Silver: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(25.0f / 100 * port->silver), (int)(50.0f / 100 * port->silver));
		if (player->current_port->silver > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->silver > player->current_port->silver ? "`bright green`\x18" : "`bright red`\x19"), abs(port->silver - player->current_port->silver));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->horses > 0) {
		od_printf("`bright white`          Horses: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(25.0f / 100 * port->horses), (int)(50.0f / 100 * port->horses));
		if (player->current_port->horses > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->horses > player->current_port->horses ? "`bright green`\x18" : "`bright red`\x19"), abs(port->horses - player->current_port->horses));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->grog > 0) {
		od_printf("`bright white`            Grog: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(25.0f / 100 * port->grog), (int)(50.0f / 100 * port->grog));
		if (player->current_port->grog > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->grog > player->current_port->grog ? "`bright green`\x18" : "`bright red`\x19"), abs(port->grog - player->current_port->grog));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->clothes > 0) {
		od_printf("`bright white`         Clothes: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(25.0f / 100 * port->clothes), (int)(50.0f / 100 * port->clothes));
		if (player->current_port->clothes > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->clothes > player->current_port->clothes ? "`bright green`\x18" : "`bright red`\x19"), abs(port->clothes - player->current_port->clothes));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}
	if (port->muskets > 0) {
		od_printf("`bright white`         Muskets: `bright green`Buying @ %3d, `bright red`Selling @ %3d ", (int)(50.0f / 100 * port->muskets), (int)(100.0f / 100 * port->muskets));
		if (player->current_port->muskets > 0) {
			od_printf("`bright white`(%s%d%%%`bright white`)\r\n", (port->muskets > player->current_port->muskets ? "`bright green`\x18" : "`bright red`\x19"), abs(port->muskets - player->current_port->muskets));
		}
		else {
			od_printf("`bright white`(N/A)\r\n");
		}
	}

	od_printf("`bright white`\r\nPress ENTER to continue...\r\n");
	od_get_key(TRUE);
}

int visit_market(void) {
	char c;
	struct portdata_t *nextport = ports[0];

	int i;

	for (i=0;i<port_count;i++) {
		if (ports[i]->id == player->current_port->location1) {
			nextport = ports[i];
			break;
		}
	}


	while(1) {
		od_clr_scr();
		od_printf("`bright yellow`Welcome to the %s marketplace!\r\n", player->current_port->name);
		od_printf("\r\nYou have %d cargo units free and %d gold coins\r\n\r\n", (player->ship->cargo_units - total_cargo_spaces_used()), player->dollars);
		if (player->current_port->food > 0) {
			od_printf("        `bright white`(`bright yellow`1`bright white`)    Food: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(5.0f / 100 * player->current_port->food), (int)(10.0f / 100 * player->current_port->food), player->food);
		}

		if (player->current_port->sugar > 0) {
			od_printf("        `bright white`(`bright yellow`2`bright white`)   Sugar: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(5.0f / 100 * player->current_port->sugar), (int)(10.0f / 100 * player->current_port->sugar), player->sugar);
		}

		if (player->current_port->tobacco > 0) {
			od_printf("        `bright white`(`bright yellow`3`bright white`) Tobacco: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(5.0f / 100 * player->current_port->tobacco), (int)(10.0f / 100 * player->current_port->tobacco), player->tobacco);
		}
		if (player->current_port->cotton > 0) {
			od_printf("        `bright white`(`bright yellow`4`bright white`)  Cotton: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(5.0f / 100 * player->current_port->cotton), (int)(10.0f / 100 * player->current_port->cotton), player->cotton);
		}
		if (player->current_port->hides > 0) {
			od_printf("        `bright white`(`bright yellow`5`bright white`)   Hides: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(5.0f / 100 * player->current_port->hides), (int)(10.0f / 100 * player->current_port->hides), player->hides);
		}
		if (player->current_port->lumber > 0) {
			od_printf("        `bright white`(`bright yellow`6`bright white`)  Lumber: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(5.0f / 100 * player->current_port->lumber), (int)(10.0f / 100 * player->current_port->lumber), player->lumber);
		}
		if (player->current_port->iron > 0) {
			od_printf("        `bright white`(`bright yellow`7`bright white`)    Iron: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(5.0f / 100 * player->current_port->iron), (int)(10.0f / 100 * player->current_port->iron), player->iron);
		}
		if (player->current_port->silver > 0) {
			od_printf("        `bright white`(`bright yellow`8`bright white`)  Silver: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(25.0f / 100 * player->current_port->silver), (int)(50.0f / 100 * player->current_port->silver), player->silver);
		}
		if (player->current_port->horses > 0) {
			od_printf("        `bright white`(`bright yellow`9`bright white`)  Horses: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(25.0f / 100 * player->current_port->horses), (int)(50.0f / 100 * player->current_port->horses), player->horses);
		}
		if (player->current_port->grog > 0) {
			od_printf("        `bright white`(`bright yellow`A`bright white`)    Grog: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(25.0f / 100 * player->current_port->grog), (int)(50.0f / 100 * player->current_port->grog), player->grog);
		}
		if (player->current_port->clothes > 0) {
			od_printf("        `bright white`(`bright yellow`B`bright white`) Clothes: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(25.0f / 100 * player->current_port->clothes), (int)(50.0f / 100 * player->current_port->clothes), player->clothes);
		}
		if (player->current_port->muskets > 0) {
			od_printf("        `bright white`(`bright yellow`C`bright white`) Muskets: `bright green`Buying @ %3d, `bright red`Selling @ %3d `bright white`(You have %d)\r\n", (int)(50.0f / 100 * player->current_port->muskets), (int)(100.0f / 100 * player->current_port->muskets), player->muskets);
		}

		od_printf("\r\n        `bright white`(`bright yellow`P`bright white`) Ask around about prices at %s\r\n", nextport->name);
		od_printf("\r\n        `bright white`(`bright yellow`R`bright white`) Return to the docks\r\n");
		od_printf("        `bright white`(`bright red`Q`bright white`) Quit to BBS\r\n`white`");

		c = od_get_answer("123456789AaBbCcPpRrQq");

		switch(tolower(c)) {
			case '1':
				if (player->current_port->food > 0) {
					buysell(1);
				}
				break;
			case '2':
				if (player->current_port->sugar > 0) {
					buysell(2);
				}
				break;
			case '3':
				if (player->current_port->tobacco > 0) {
					buysell(3);
				}
				break;
			case '4':
				if (player->current_port->cotton > 0) {
					buysell(4);
				}
				break;
			case '5':
				if (player->current_port->hides > 0) {
					buysell(5);
				}
				break;
			case '6':
				if (player->current_port->lumber > 0) {
					buysell(6);
				}
				break;
			case '7':
				if (player->current_port->iron > 0) {
					buysell(7);
				}
				break;
			case '8':
				if (player->current_port->silver > 0) {
					buysell(8);
				}
				break;
			case '9':
				if (player->current_port->horses > 0) {
					buysell(9);
				}
				break;
			case 'a':
				if (player->current_port->grog > 0) {
					buysell(10);
				}
				break;
			case 'b':
				if (player->current_port->clothes > 0) {
					buysell(11);
				}
				break;
			case 'c':
				if (player->current_port->muskets > 0) {
					buysell(12);
				}
				break;
			case 'p':
				gossip_prices(nextport);
				break;
			case 'r':
				return 1;
			case 'q':
				return 0;

		}
	}
}
