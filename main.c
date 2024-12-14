#if WIN32
#   define _MSC_VER 1
#	define _CRT_SECURE_NO_WARNINGS
#   include <windows.h>
#else

#endif

#include <OpenDoor.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <ctype.h>
#include "portvictoria.h"


struct playerdata_t *player;

struct portdata_t **ports;
int port_count;

struct shipdata_t **ships;
int ship_count;

void create_scorefile(void) {
	FILE *fptr, *fptr2;
	sqlite3 *db;
	char sqlbuffer[256];
	int rc;
	sqlite3_stmt *stmt;
	int score;
	char c;

	fptr = fopen("scores.ans", "w");

	if (fptr) {
		fptr2 = fopen("scores_header.ans", "r");
		c = fgetc(fptr2);
		while(!feof(fptr2)) {
			fputc(c, fptr);
			c = fgetc(fptr2);
		}

		fclose(fptr2);

		rc = sqlite3_open("users.db3", &db);
		if (rc) {
			// Error opening the database
			od_printf("Error opening users database: %s\r\n", sqlite3_errmsg(db));
			od_exit(-1, FALSE);
		}
		sqlite3_busy_timeout(db, 5000);
		snprintf(sqlbuffer, 256, "SELECT * FROM users;");
		sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			score = sqlite3_column_int(stmt, 19);

			score += ships[sqlite3_column_int(stmt, 6)]->value;

			score /= 100;

			fprintf(fptr, " \033[1;37m%-64s \033[1;33m%13d\033[0m\r\n", sqlite3_column_text(stmt, 1), score);
		}

		sqlite3_finalize(stmt);
		sqlite3_close(db);
	}
}

void save_portdata(void) {
	int i;
	sqlite3 *db;
	char sqlbuffer[1024];
	int rc;
	sqlite3_stmt *stmt;

	rc = sqlite3_open("locations.db3", &db);
	if (rc) {
		// Error opening the database
		od_printf("Error opening locations database: %s\r\n", sqlite3_errmsg(db));
		od_exit(-1, FALSE);
	}
	sqlite3_busy_timeout(db, 5000);
	for (i=0;i<port_count;i++) {
		snprintf(sqlbuffer, 1024, "UPDATE locations SET food=?,"
													   "sugar=?,"
													   "tobacco=?,"
													   "cotton=?, "
													   "hides=?, "
													   "lumber=?, "
													   "iron=?,"
													   "silver=?,"
													   "horses=?,"
													   "grog=?,"
													   "clothes=?,"
													   "muskets=? WHERE id=?;");
		sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
		sqlite3_bind_int(stmt, 1, ports[i]->food);
		sqlite3_bind_int(stmt, 2, ports[i]->sugar);
		sqlite3_bind_int(stmt, 3, ports[i]->tobacco);
		sqlite3_bind_int(stmt, 4, ports[i]->cotton);
		sqlite3_bind_int(stmt, 5, ports[i]->hides);
		sqlite3_bind_int(stmt, 6, ports[i]->lumber);
		sqlite3_bind_int(stmt, 7, ports[i]->iron);
		sqlite3_bind_int(stmt, 8, ports[i]->silver);
		sqlite3_bind_int(stmt, 9, ports[i]->horses);
		sqlite3_bind_int(stmt, 10, ports[i]->grog);
		sqlite3_bind_int(stmt, 11, ports[i]->clothes);
		sqlite3_bind_int(stmt, 12, ports[i]->muskets);
		sqlite3_bind_int(stmt, 13, ports[i]->id);

		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}

	sqlite3_close(db);
}

void door_quit(void) {
	// save data.
	sqlite3 *db;
	char sqlbuffer[1024];
	int rc;
	sqlite3_stmt *stmt;

	char bbsname[256];

	save_portdata();

	create_scorefile();

	snprintf(bbsname, 256, "%s!%s", od_control_get()->user_name, od_control_get()->user_handle);

	if (player != NULL) {
		// save data

		rc = sqlite3_open("users.db3", &db);
		if (rc) {
			// Error opening the database
			od_printf("Error opening users database: %s\r\n", sqlite3_errmsg(db));

			od_exit(-1, FALSE);
		}
		sqlite3_busy_timeout(db, 5000);		
		if (player->id == -1) {
			snprintf(sqlbuffer, 1024, "INSERT INTO users (name, last_played, months_used, current_location,"
													"ship, food, sugar,tobacco, cotton, hides, lumber, iron,"
													"silver, horses, grog, clothes, muskets, dollars, bbsname) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?, ?);");
			sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, player->captain_name, strlen(player->captain_name) + 1, SQLITE_STATIC);
			sqlite3_bind_int(stmt, 2, player->last_played);
			sqlite3_bind_int(stmt, 3, player->months_used);
			sqlite3_bind_int(stmt, 4, player->current_port->id);
			sqlite3_bind_int(stmt, 5, player->ship->id);
			sqlite3_bind_int(stmt, 6, player->food);
			sqlite3_bind_int(stmt, 7, player->sugar);
			sqlite3_bind_int(stmt, 8, player->tobacco);
			sqlite3_bind_int(stmt, 9, player->cotton);
			sqlite3_bind_int(stmt, 10, player->hides);
			sqlite3_bind_int(stmt, 11, player->lumber);
			sqlite3_bind_int(stmt, 12, player->iron);
			sqlite3_bind_int(stmt, 13, player->silver);
			sqlite3_bind_int(stmt, 14, player->horses);
			sqlite3_bind_int(stmt, 15, player->grog);
			sqlite3_bind_int(stmt, 16, player->clothes);
			sqlite3_bind_int(stmt, 17, player->muskets);
			sqlite3_bind_int(stmt, 18, player->dollars);
			sqlite3_bind_text(stmt, 19, bbsname, strlen(bbsname) + 1, SQLITE_STATIC);
		} else {
			snprintf(sqlbuffer, 1024, "UPDATE users SET name=?,"
													   "last_played=?,"
													   "months_used=?,"
													   "current_location=?,"
													   "ship=?,"
													   "food=?,"
													   "sugar=?,"
													   "tobacco=?,"
													   "cotton=?, "
													   "hides=?, "
													   "lumber=?, "
													   "iron=?,"
													   "silver=?,"
													   "horses=?,"
													   "grog=?,"
													   "clothes=?,"
													   "muskets=?,"
													   "dollars=? WHERE id=?;");
			sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, player->captain_name, strlen(player->captain_name) + 1, SQLITE_STATIC);
			sqlite3_bind_int(stmt, 2, player->last_played);
			sqlite3_bind_int(stmt, 3, player->months_used);
			sqlite3_bind_int(stmt, 4, player->current_port->id);
			sqlite3_bind_int(stmt, 5, player->ship->id);
			sqlite3_bind_int(stmt, 6, player->food);
			sqlite3_bind_int(stmt, 7, player->sugar);
			sqlite3_bind_int(stmt, 8, player->tobacco);
			sqlite3_bind_int(stmt, 9, player->cotton);
			sqlite3_bind_int(stmt, 10, player->hides);
			sqlite3_bind_int(stmt, 11, player->lumber);
			sqlite3_bind_int(stmt, 12, player->iron);
			sqlite3_bind_int(stmt, 13, player->silver);
			sqlite3_bind_int(stmt, 14, player->horses);
			sqlite3_bind_int(stmt, 15, player->grog);
			sqlite3_bind_int(stmt, 16, player->clothes);
			sqlite3_bind_int(stmt, 17, player->muskets);
			sqlite3_bind_int(stmt, 18, player->dollars);
		    sqlite3_bind_int(stmt, 19, player->id);
		}

		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			// Error opening the database
			od_printf("Error saving to users database: %s\r\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			od_exit(-1, FALSE);
		}
		sqlite3_finalize(stmt);
		sqlite3_close(db);

		free(player);
	}
}

void game_loop(void) {
	int ret;
	while (1) {

		ret = display_current_location();

		switch (ret) {
			case 1: // visit market
				if (!visit_market()) {
					od_exit(0, FALSE);
				}
				break;
			case 2: // select destination
				if (!select_destination()) {
					od_exit(0, FALSE);
				}
				break;
			case 3:
				// quit game;
				// print a good bye message
				od_exit(0, FALSE);
			case 4:
				// tavern
				tavern();
				break;
			case 5:
				// Shipyard
				shipyard();
				break;
		}
	}
}

void load_data(void) {
	sqlite3 *db;
	char sqlbuffer[256];
	int rc;
	sqlite3_stmt *stmt;

	struct portdata_t *m_port;
	struct shipdata_t *m_ship;
	port_count = 0;
	ports = NULL;

	rc = sqlite3_open("locations.db3", &db);
	if (rc) {
		// Error opening the database
        od_printf("Error opening locations database: %s\r\n", sqlite3_errmsg(db));
        od_exit(-1, FALSE);
    }
	sqlite3_busy_timeout(db, 5000);
	snprintf(sqlbuffer, 256, "SELECT * FROM locations;");
	sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
	while (1) {
		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			m_port = (struct portdata_t *)malloc(sizeof(struct portdata_t));
			if (!m_port) {
				od_printf("Out of memory!\r\n");
				od_exit(-1, FALSE);
			}

			m_port->id = sqlite3_column_int(stmt, 0);
			strncpy(m_port->name, sqlite3_column_text(stmt, 1), 64);
			m_port->food = sqlite3_column_double(stmt, 2);
			m_port->sugar = sqlite3_column_double(stmt, 3);
			m_port->tobacco = sqlite3_column_double(stmt, 4);
			m_port->cotton = sqlite3_column_double(stmt, 5);
			m_port->hides = sqlite3_column_double(stmt, 6);
			m_port->lumber = sqlite3_column_double(stmt, 7);
			m_port->iron = sqlite3_column_double(stmt, 8);
			m_port->silver = sqlite3_column_double(stmt, 9);
			m_port->horses = sqlite3_column_double(stmt, 10);
			m_port->grog = sqlite3_column_double(stmt, 11);
			m_port->clothes = sqlite3_column_double(stmt, 12);
			m_port->muskets = sqlite3_column_double(stmt, 13);

			m_port->location1 = sqlite3_column_int(stmt, 14);
			m_port->location1cost = sqlite3_column_int(stmt, 15);
			m_port->location2 = sqlite3_column_int(stmt, 16);
			m_port->location2cost = sqlite3_column_int(stmt, 17);
			m_port->location3 = sqlite3_column_int(stmt, 18);
			m_port->location3cost = sqlite3_column_int(stmt, 19);
			m_port->location4 = sqlite3_column_int(stmt, 20);
			m_port->location4cost = sqlite3_column_int(stmt, 21);

			if (port_count == 0) {
				ports = (struct portdata_t **)malloc(sizeof(struct portdata_t *));
			} else {
				ports = (struct portdata_t **)realloc(ports, sizeof(struct portdata_t *) * (port_count + 1));
			}
			ports[port_count] = m_port;
			port_count++;
		} else {
			break;
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	// TODO: Load ship types
	rc = sqlite3_open("ships.db3", &db);
	if (rc) {
		// Error opening the database
        od_printf("Error opening ships database: %s\r\n", sqlite3_errmsg(db));
        od_exit(-1, FALSE);
    }
	sqlite3_busy_timeout(db, 5000);
    ship_count = 0;

    snprintf(sqlbuffer, 256, "SELECT * FROM ships;");
	sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
	while (1) {
		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			m_ship = (struct shipdata_t *)malloc(sizeof(struct shipdata_t));
			if (!m_ship) {
				od_printf("Out of Memory!\n");
				od_exit(-1, FALSE);
			}
			m_ship->id = sqlite3_column_int(stmt, 0);
			strncpy(m_ship->name, sqlite3_column_text(stmt, 1), 32);
			m_ship->cargo_units = sqlite3_column_int(stmt, 2);
			m_ship->value = sqlite3_column_int(stmt, 3);

			if (ship_count == 0) {
				ships = (struct shipdata_t **)malloc(sizeof(struct shipdata_t *));
			} else {
				ships = (struct shipdata_t **)realloc(ships, sizeof(struct shipdata_t *) * (ship_count + 1));
			}
			ships[ship_count] = m_ship;
			ship_count++;
		} else {
			break;
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}



void new_user(void) {
	char buffer[64];

	od_clr_scr();
	od_printf("It looks like you're new here...\r\n");
	do {
		od_printf("\r\nWhat's your name captain? : ");

		od_input_str(buffer, 63, 32, 127);

		if (strlen(buffer) == 0) {
			od_printf("\r\nQuit ? (Y/N) ");
			if (tolower(od_get_answer("YyNn")) == 'y') {
				od_exit(0, FALSE);
			} else {
				continue;
			}
		}


		od_printf("\r\nCaptain %s sounds OK ? (Y/N) ", buffer);
	} while(tolower(od_get_answer("YyNn")) == 'n');
	player = (struct playerdata_t *)malloc(sizeof(struct playerdata_t));
	if (!player) {
		od_printf("`bright red`Out of memory!`white`\n");
		od_exit(-1, FALSE);
	}
	strncpy(player->captain_name, buffer, 64);
	player->id = -1;
	player->last_played = time(NULL);
	player->months_used = 0;
	player->current_port = ports[0];
	player->ship = ships[0];

	player->food = 0;
	player->sugar = 0;
	player->tobacco = 0;
	player->cotton = 0;
	player->hides = 0;
	player->lumber = 0;
	player->iron = 0;
	player->silver = 0;
	player->horses = 0;
	player->grog = 0;
	player->clothes = 0;
	player->muskets = 0;
	player->dollars = 100;
}



void run_maintenance() {
	char sqlbuffer[256];
	int rc;
	sqlite3_stmt *stmt;
	int i;
	char c;
	time_t last_run;
	time_t now;
	struct tm today;
	struct tm lastday;
	sqlite3 *db;
	int lid;
	int type;
	int diff;
	struct portdata_t *location;
	FILE *fptr;
	FILE *fptr2;
	char ev_buff[1024];
	int do_maintenance = 0;

	// open location database
	rc = sqlite3_open("locations.db3", &db);
	if (rc) {
		// Error opening the database
        printf("Error opening locations database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
	sqlite3_busy_timeout(db, 5000);
	// check maintenance table for last run
	snprintf(sqlbuffer, 256, "SELECT * FROM maintenance;");
	sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
	rc = sqlite3_step(stmt);

	if (rc == SQLITE_ROW) {
		last_run = sqlite3_column_int(stmt, 0);
		memcpy(&lastday, localtime(&last_run), sizeof(struct tm));
		now = time(NULL);
		memcpy(&today, localtime(&now), sizeof(struct tm));

		if (today.tm_mday != lastday.tm_mday || today.tm_mon != lastday.tm_mon || today.tm_year != lastday.tm_year) {
			sqlite3_finalize(stmt);

			snprintf(sqlbuffer, 256, "UPDATE maintenance SET last_check=?;");
			sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
			sqlite3_bind_int(stmt, 1, time(NULL));
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			do_maintenance = 1;

		} else {
			sqlite3_finalize(stmt);
		}
	} else {
		sqlite3_finalize(stmt);

		snprintf(sqlbuffer, 256, "INSERT INTO maintenance (last_check) VALUES(?);");
		sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
		sqlite3_bind_int(stmt, 1, time(NULL));
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		do_maintenance = 1;
	}

	// if last run yesterday, run now

	if (do_maintenance) {
		// ----> end events
		snprintf(sqlbuffer, 256, "SELECT * FROM events;");
		sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
		rc = sqlite3_step(stmt);

		while (rc == SQLITE_ROW) {
			lid = sqlite3_column_int(stmt, 1);
			type = sqlite3_column_int(stmt, 2);
			diff = sqlite3_column_int(stmt, 3);
			location = ports[0];
			for (i=0;i<port_count;i++) {
				if (ports[i]->id == lid) {
					location = ports[i];
					break;
				}
			}

			switch (type) {
				case 1: // food
					location->food -= diff;
					break;
				case 2: // sugar
					location->sugar -= diff;
					break;
				case 3: // tobacco;
					location->tobacco -= diff;
					break;
				case 4: // cotton;
					location->cotton -= diff;
					break;
				case 5: // hides;
					location->hides -= diff;
					break;
				case 6: // lumber;
					location->lumber -= diff;
					break;
				case 7: // iron;
					location->iron -= diff;
					break;
				case 8: // silver;
					location->silver -= diff;
					break;
				case 9:  // horses;
					location->horses -= diff;
					break;
				case 10: // grog;
					location->grog -= diff;
					break;
				case 11: // clothes;
					location->clothes -= diff;
					break;
				case 12: // muskets;
					location->muskets -= diff;
					break;
			}
			rc = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);

		snprintf(sqlbuffer, 256, "DELETE * FROM events;");
		sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		// ----> start events && create news ansi


		srand(time(NULL));
		fptr = fopen("todays_news.ans", "w");
		if (fptr) {
			fptr2 = fopen("news_header.ans", "r");
			c = fgetc(fptr2);
			while (!feof(fptr2)) {
				fputc(c, fptr);
				c = fgetc(fptr2);
			}
		}
		for (i=0;i<3;i++) {
			if ((rand() % 10 + 1) > 5) {
				// do event
				snprintf(sqlbuffer, 256, "SELECT * FROM events_def ORDER BY RANDOM() LIMIT 1;");
				sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
				rc = sqlite3_step(stmt);
				if (rc == SQLITE_ROW) {
					lid = rand() % port_count + 1;
					location = ports[lid];
					strncpy(ev_buff, sqlite3_column_text(stmt, 1), 1023);

					type = sqlite3_column_int(stmt, 2);
					diff = sqlite3_column_int(stmt, 3);

					sqlite3_finalize(stmt);

					switch (type) {
						case 1: // food
							if (location->food + diff < 0 || location->food == 0) {
								diff = 0;
							}
							else {
								location->food += diff;
							}
							break;
						case 2: // sugar
							if (location->sugar + diff < 0 || location->sugar == 0) {
								diff = 0;
							}
							else {
								location->sugar += diff;
							}
							break;
						case 3: // tobacco;
							if (location->tobacco + diff < 0 || location->tobacco == 0) {
								diff = 0;
							}
							else {
								location->tobacco += diff;
							}
							break;
						case 4: // cotton;
							if (location->cotton + diff < 0 || location->cotton == 0) {
								diff = 0;
							}
							else {
								location->cotton += diff;
							}
							break;
						case 5: // hides;
							if (location->hides + diff < 0 || location->hides == 0) {
								diff = 0;
							}
							else {
								location->hides += diff;
							}
							break;
						case 6: // lumber;
							if (location->lumber + diff < 0 || location->lumber == 0) {
								diff = 0;
							}
							else {
								location->lumber += diff;
							}
							break;
						case 7: // iron;
							if (location->iron + diff < 0 || location->iron == 0) {
								diff = 0;
							}
							else {
								location->iron += diff;
							}
							break;
						case 8: // silver;
							if (location->silver + diff < 0 || location->silver == 0) {
								diff = 0;
							}
							else {
								location->silver += diff;
							}
							break;
						case 9:  // horses;
							if (location->horses + diff < 0 || location->horses == 0) {
								diff = 0;
							}
							else {
								location->horses += diff;
							}
							break;
						case 10: // grog;
							if (location->grog + diff < 0 || location->grog == 0) {
								diff = 0;
							}
							else {
								location->grog += diff;
							}
							break;
						case 11: // clothes;
							if (location->clothes + diff < 0 || location->clothes == 0) {
								diff = 0;
							}
							else {
								location->clothes += diff;
							}
							break;
						case 12: // muskets;
							if (location->muskets + diff < 0 || location->muskets == 0) {
								diff = 0;
							}
							else {
								location->muskets += diff;
							}
							break;
					}
					if (diff) {
						if (fptr) {
							fprintf(fptr, "          ");
							fprintf(fptr, ev_buff, location->name);
							fprintf(fptr, "\r\n\r\n");
						}
						snprintf(sqlbuffer, 256, "INSERT INTO events (lid, type, diff) VALUES(?, ?, ?);");
						sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
						sqlite3_bind_int(stmt, 1, location->id);
						sqlite3_bind_int(stmt, 2, type);
						sqlite3_bind_int(stmt, 3, diff);

						sqlite3_step(stmt);

						sqlite3_finalize(stmt);
					}
				}
			}
		}
		fclose(fptr);
	}
	sqlite3_close(db);
}
#if _MSC_VER
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
#else
int main(int argc, char **argv) {
#endif
	char sqlbuffer[256];
	char bbsname[256];
	int rc;
	sqlite3_stmt *stmt;
	int i;

	sqlite3 *db;
	time_t timenow;
	struct tm today_tm, last_tm;
	struct stat s;

	if (stat("users.db3", &s) != 0 || stat("locations.db3", &s) != 0 || stat("ships.db3", &s) != 0) {
#if _MSC_VER
		MessageBox(0, "Database(s) Missing, please run reset.bat.", "Error", 0);
#else
		fprintf(stderr, "Error: Database(s) Missing, please run reset.sh.\n");
#endif
		exit(-1);
	}

	player = NULL;
#if _MSC_VER
	od_parse_cmd_line(lpszCmdLine);
#else
	od_parse_cmd_line(argc, argv);
#endif
	od_init();

	od_control_get()->od_page_pausing = FALSE;
	od_send_file("portvictoria.ans");
	od_control_get()->od_page_pausing = TRUE;
	od_printf("Press ENTER to continue...");
	od_get_key(TRUE);

	// 0.5 Load Port data
	load_data();

	// 0. Daily Maintenance
	run_maintenance();

	// 1. check user
	rc = sqlite3_open("users.db3", &db);
	if (rc) {
		// Error opening the database
        	od_printf("Error opening user database: %s\n", sqlite3_errmsg(db));
        	od_exit(-1, FALSE);
    	}
	sqlite3_busy_timeout(db, 5000);
	snprintf(bbsname, 256, "%s!%s", od_control_get()->user_name, od_control_get()->user_handle);
	snprintf(sqlbuffer, 256, "SELECT * FROM users WHERE bbsname LIKE ?;");

	sqlite3_prepare_v2(db, sqlbuffer, strlen(sqlbuffer) + 1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, bbsname, strlen(bbsname) + 1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW) {
		// load user
		player = (struct playerdata_t *)malloc(sizeof(struct playerdata_t));

		if (!player) {
			od_printf("Out of memory!\n");
			od_exit(-1, FALSE);
		}

		player->id = sqlite3_column_int(stmt, 0);
		strncpy(player->captain_name, sqlite3_column_text(stmt, 1), 64);

		for (i=0;i<port_count;i++) {
			if (ports[i]->id == sqlite3_column_int(stmt, 2)) {
				player->current_port = ports[i];
				break;
			}
		}


		player->last_played = sqlite3_column_int(stmt, 3);
		player->months_used = sqlite3_column_int(stmt, 4);

		memcpy(&last_tm, localtime(&player->last_played), sizeof(struct tm));
		timenow = time(NULL);
		memcpy(&today_tm, localtime(&timenow), sizeof(struct tm));
		if (today_tm.tm_mday != last_tm.tm_mday || today_tm.tm_mon != last_tm.tm_mon || today_tm.tm_year != last_tm.tm_year) {
			player->months_used = 0;
		}
		player->last_played = timenow;
		for (i=0;i<ship_count;i++) {
			if (ships[i]->id == sqlite3_column_int(stmt, 6)) {
				player->ship = ships[i];
				break;
			}
		}

		player->food = sqlite3_column_int(stmt, 7);
		player->sugar = sqlite3_column_int(stmt, 8);
		player->tobacco = sqlite3_column_int(stmt, 9);
		player->cotton = sqlite3_column_int(stmt, 10);
		player->hides = sqlite3_column_int(stmt, 11);
		player->lumber = sqlite3_column_int(stmt, 12);
		player->iron = sqlite3_column_int(stmt, 13);
		player->silver = sqlite3_column_int(stmt, 14);
		player->horses = sqlite3_column_int(stmt, 15);
		player->grog = sqlite3_column_int(stmt, 16);
		player->clothes = sqlite3_column_int(stmt, 17);
		player->muskets = sqlite3_column_int(stmt, 18);
		player->dollars = sqlite3_column_int(stmt, 19);

		sqlite3_finalize(stmt);
		sqlite3_close(db);
	} else {
		// new user
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		new_user();
	}

	od_control_get()->od_before_exit = door_quit;
	// 1a. 	-> load user
	// 1b.	-> new user

	// show news
	od_send_file("todays_news.ans");

	od_printf("Press ENTER to continue...");
	od_get_key(TRUE);

	// 2. -> Game Loop
	game_loop();

	// 3. -> Save data
	od_exit(0, FALSE);
}
