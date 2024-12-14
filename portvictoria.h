#ifndef __PORTVICTORIA_H
#define __PORTVICTORIA_H

#define MONTHS_PER_DAY 12



struct portdata_t {
	int id;
	char name[64];
	
	double food;
	double sugar;
	double tobacco;
	double cotton;
	double hides;
	double lumber;
	double iron;
	double silver;
	double horses;
	double grog;
	double clothes;
	double muskets;
	
	int location1;
	int location1cost;
	int location2;
	int location2cost;
	int location3;
	int location3cost;
	int location4;
	int location4cost;
};

struct shipdata_t {
	int id;
	char name[32];
	int cargo_units;
	int value;
};

struct playerdata_t {
	int id;
	char captain_name[64];
	time_t last_played;
	int months_used;
	struct portdata_t *current_port;
	struct shipdata_t *ship;
	int food;
	int sugar;
	int tobacco;
	int cotton;
	int hides;
	int lumber;
	int iron;
	int silver;
	int horses;
	int grog;
	int clothes;
	int muskets;
	int dollars;
};

extern int visit_market(void);
extern int display_current_location(void);
extern int select_destination(void);
extern int shipyard(void);
extern int total_cargo_spaces_used(void);
extern int tavern(void);
#endif
