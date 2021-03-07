#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#define AVE_PRICE 33625
#define PROPERTY_ONE_THIRD 4

typedef enum {noone, cap, car}  player_type;
typedef enum {start, property, tax, punish, fortune} block_type;
typedef enum {free_house=1, time_travel, garnishment, generosity, treasure_hunter} fortune_type;

typedef struct {
	player_type type;
	int current_block_id; /* Player location as block id */
	int owned_block_ids[12];
	int account;
	int turn_to_wait; /* Number of turns to wait if the player got a punishment */
	char *name;
} player;

typedef struct block{
	int block_id;
	char *name;
	int price; /* price, tax fee or the number of turns to wait */
	int rent; /* with no house */
	int rent_1; /* with one house */
	int rent_2; /* with two house */
	int rent_3; /* with three house */
	int house_price;
	int house_count; /* The number of the houses */
	player owner;
	block_type type;
	struct block *next;
} block;

typedef struct fortune_card{
	fortune_type type;
} fortune_card;

void alloc_n_units(block *ll, int n); /* It allocates given units for given linked list */

block *nth_indice(block *ll, int nth);/* It returns given indice of given linked list. ll must be allocated to the given indice. */

int the_most_expensive_property(block *board, player current_player); /* Returns the most expensive property's id of the given player */

void init_the_board(block *board); /* Fills the board */

void init_the_players(player *p1, player *p2);

void show_board(block *board, player player_one, player player_two); /* Prints the board */

int intlen(int num); /* Returns the length of given number */

void show_properties(block *board); /* Prints all properties and asks user to select a property, then prints the details of the selected property */

void print_property_details(block current_block); /* Auxiliary function of show_properties */

void buy_property(block *current_block, player *current_player);

void sell_property(block *board, player *current_player);

void sell_property_ai(block *board, player *ai_player);

void print_my_properties(block *board, player current_player); /* Prints all properties of given player */

int num_of_my_properties(player current_player);

int if_sell_all_properties(block *board, player p); /* Returns the price if all properties of the given player is sold */

int is_bankrupt(block *board, block current_block, player p); /* Returns 1 if the given player is bankrupt. If not, returns 0 */

int is_bankrupt_debt(block *board, int debt, player p); /* Returns 1 if the given player is bankrupt. If not, returns 0 */

void gameplay_menu(block *board, player *current_player, player *other_player); /* Prints gameplay menu and rolls dice */

void gameplay_menu_ai(block *board, player *ai_player, player *other_player);

void turn(block *board, player *current_player, player *other_player);

void turn_ai(block *board, player *ai_player, player *other_player);

void gameplay(block *board, player *player_one, player *player_two);

int main(){
	block *board;
	player player_one, player_two;
	srand(time(NULL));

	board = (block *) malloc(sizeof(block));
	alloc_n_units(board, 24);
	init_the_board(board);
	init_the_players(&player_one, &player_two);
	gameplay(board, &player_one, &player_two);
	return 0;
}

void alloc_n_units(block *ll, int n){
	int i;
	block *temp;

	temp = ll;
	temp->next = NULL;
	for(i=0; i<n-1; i++){
		temp->next = (block *) malloc(sizeof(block));
		temp = temp->next;
		temp->next = NULL;
	}
}

block *nth_indice(block *ll, int nth){
	int i;
	block *temp;
	temp = ll;
	for(i=0; i<nth; i++){
		temp = temp->next;
	}
	return temp;
}

int the_most_expensive_property(block *board, player current_player){
	int i;
	int most_exp_id = 0;

	for(i=0; i<12; i++){
		if(current_player.owned_block_ids[i] != -1){
			if(nth_indice(board, current_player.owned_block_ids[i])->price > nth_indice(board, most_exp_id)->price){
				most_exp_id = current_player.owned_block_ids[i];
			}
		}
	}
	return most_exp_id;
}

void init_the_players(player *p1, player *p2){
	int i;

	p1->type = car;
	p1->current_block_id = 0;
	p1->account = 100000;	
	p1->turn_to_wait = 0;
	p1->name = "car";

	p2->type = cap;
	p2->current_block_id = 0;
	p2->account = 100000;
	p2->turn_to_wait = 0;
	p2->name = "cap";

	for(i=0; i<12; i++){
		p1->owned_block_ids[i] = -1;
		p2->owned_block_ids[i] = -1;
	}
}

void show_board(block *board, player player_one, player player_two){
	int i, j;

	for(i=0; i<29; i++){ /* The board has 29 rows */
		switch(i % 4){
			case 0:	/* Borders */
				if(i == 0 || i == 4 || i == 24 || i == 28){
					for(j=0; j<169; j++){
						printf("-");
					}
				}else{
					for(j=0; j<25; j++){
						printf("-");
					}
					for(j=0; j<15; j++){
						printf("\t");
					}
					for(j=0; j<25; j++){
						printf("-");
					}
				}
				break;
			case 1: /* First line of a block */
				if(i == 1){
					for(j=0; j<7; j++){
						printf("|\t");
						printf("%s", nth_indice(board, j)->name);
						if(strlen(nth_indice(board, j)->name) < 8) printf("\t\t");
						else if(strlen(nth_indice(board, j)->name) < 16) printf("\t");
					}
					printf("|");
				}else if(i == 25){
					for(j=18; j>11; j--){
						printf("|\t");
						printf("%s", nth_indice(board, j)->name);
						if(strlen(nth_indice(board, j)->name) < 8) printf("\t\t");
						else if(strlen(nth_indice(board, j)->name) < 16) printf("\t");
					}
					printf("|");
				}else{
					printf("|\t");
					printf("%s", nth_indice(board, 24 - (i - 1) / 4)->name);
					if(strlen(nth_indice(board, 24 - (i - 1) / 4)->name) < 8) printf("\t\t|");
					else if(strlen(nth_indice(board, 24 - (i - 1) / 4)->name) < 16) printf("\t|");

					for(j=0; j<15; j++) printf("\t");

					printf("|\t");
					printf("%s", nth_indice(board, 6 + (i - 1) / 4)->name);
					if(strlen(nth_indice(board, 6 + (i - 1) / 4)->name) < 8) printf("\t\t|");
					else if(strlen(nth_indice(board, 6 + (i - 1) / 4)->name) < 16) printf("\t|");
				}
				break;
			case 2: /* Second line of a block */
				if(i == 2){
					for(j=0; j<7; j++){
						if(nth_indice(board, j)->type == property || nth_indice(board, j)->type == tax){
							printf("|\t");
							printf("%d$", nth_indice(board, j)->price);
							if(intlen(nth_indice(board, j)->price) + 1 < 8) printf("\t\t");
							else if(intlen(nth_indice(board, j)->price) + 1 < 16) printf("\t");
						}else printf("|\t\t\t");
					}
					printf("|");
				}else if(i == 26){
					for(j=18; j>11; j--){
						if(nth_indice(board, j)->type == property || nth_indice(board, j)->type == tax){
							printf("|\t");
							printf("%d$", nth_indice(board, j)->price);
							if(intlen(nth_indice(board, j)->price) + 1 < 8) printf("\t\t");
							else if(intlen(nth_indice(board, j)->price) + 1 < 16) printf("\t");
						}else printf("|\t\t\t");
					}
					printf("|");
				}else{
					if(nth_indice(board, 24 - (i - 2) / 4)->type == property || nth_indice(board, 24 - (i - 2) / 4)->type == tax){
						printf("|\t");
						printf("%d$", nth_indice(board, 24 - (i - 2) / 4)->price);
						if(intlen(nth_indice(board, 24 - (i - 2) / 4)->price) + 1 < 8) printf("\t\t|");
						else if(intlen(nth_indice(board, 24 - (i - 2) / 4)->price) + 1 < 16) printf("\t|");
					}else printf("|\t\t\t|");

					for(j=0; j<15; j++) printf("\t");

					if(nth_indice(board, 6 + (i - 2) / 4)->type == property || nth_indice(board, 6 + (i - 2) / 4)->type == tax){
						printf("|\t");
						printf("%d$", nth_indice(board, 6 + (i - 2) / 4)->price);
						if(intlen(nth_indice(board, 6 + (i - 2) / 4)->price) + 1 < 8) printf("\t\t|");
						else if(intlen(nth_indice(board, 6 + (i - 2) / 4)->price) + 1 < 16) printf("\t|");
					}else printf("|\t\t\t|");
				}
				break;
			case 3: /* Third line of a block */
				if(i == 3){
					for(j=0; j<7; j++){
						printf("|\t");
						if(player_one.current_block_id == player_two.current_block_id && player_one.current_block_id == j){
							printf("%s %s", player_one.name, player_two.name);
							if(strlen(player_one.name) + strlen(player_two.name) + 1 < 8) printf("\t\t");
							else if(strlen(player_one.name) + strlen(player_two.name) + 1 < 16) printf("\t");
						}else if(player_one.current_block_id == j){
							printf("%s", player_one.name);
							if(strlen(player_one.name) < 8) printf("\t\t");
							else if(strlen(player_one.name) < 16) printf("\t");
						}else if(player_two.current_block_id == j){
							printf("%s", player_two.name);
							if(strlen(player_two.name) < 8) printf("\t\t");
							else if(strlen(player_two.name) < 16) printf("\t");
						}else printf("\t\t");
					}
					printf("|");
				}else if(i == 27){
					for(j=18; j>11; j--){
						printf("|\t");
						if(player_one.current_block_id == player_two.current_block_id && player_one.current_block_id == j){
							printf("%s %s", player_one.name, player_two.name);
							if(strlen(player_one.name) + strlen(player_two.name) + 1 < 8) printf("\t\t");
							else if(strlen(player_one.name) + strlen(player_two.name) + 1 < 16) printf("\t");
						}else if(player_one.current_block_id == j){
							printf("%s", player_one.name);
							if(strlen(player_one.name) < 8) printf("\t\t");
							else if(strlen(player_one.name) < 16) printf("\t");
						}else if(player_two.current_block_id == j){
							printf("%s", player_two.name);
							if(strlen(player_two.name) < 8) printf("\t\t");
							else if(strlen(player_two.name) < 16) printf("\t");
						}else printf("\t\t");
					}
					printf("|");
				}else{
					printf("|\t");
					if(player_one.current_block_id == player_two.current_block_id && player_one.current_block_id == 24 - (i - 3) / 4){
						printf("%s %s", player_one.name, player_two.name);
						if(strlen(player_one.name) + strlen(player_two.name) + 1 < 8) printf("\t\t|");
						else if(strlen(player_one.name) + strlen(player_two.name) + 1 < 16) printf("\t|");
					}else if(player_one.current_block_id == 24 - (i - 3) / 4){
						printf("%s", player_one.name);
						if(strlen(player_one.name) < 8) printf("\t\t|");
						else if(strlen(player_one.name) < 16) printf("\t|");
					}else if(player_two.current_block_id == 24 - (i - 3) / 4){
						printf("%s", player_two.name);
						if(strlen(player_two.name) < 8) printf("\t\t|");
						else if(strlen(player_two.name) < 16) printf("\t|");
					}else printf("\t\t|");

					for(j=0; j<15; j++) printf("\t");

					printf("|\t");
					if(player_one.current_block_id == player_two.current_block_id && player_one.current_block_id == 6 + (i - 3) / 4){
						printf("%s %s", player_one.name, player_two.name);
						if(strlen(player_one.name) + strlen(player_two.name) + 1 < 8) printf("\t\t|");
						else if(strlen(player_one.name) + strlen(player_two.name) + 1 < 16) printf("\t|");
					}else if(player_one.current_block_id == 6 + (i - 3) / 4){
						printf("%s", player_one.name);
						if(strlen(player_one.name) < 8) printf("\t\t|");
						else if(strlen(player_one.name) < 16) printf("\t|");
					}else if(player_two.current_block_id == 6 + (i - 3) / 4){
						printf("%s", player_two.name);
						if(strlen(player_two.name) < 8) printf("\t\t|");
						else if(strlen(player_two.name) < 16) printf("\t|");
					}else printf("\t\t|");
				}
				break;
		}
		printf("\n");
	}
}

int intlen(int number){
	int len = 0;
	while(number != 0){
		number /= 10;
		len++;
	}
	return len;
}

void show_properties(block *board){
	int selection, i;

	while(1){
		do{
			printf("Please select a property to see details:\n");
			for(i=0; i<20; i++){
				if(nth_indice(board, i)->type == property) printf("%d -\t%s\n", nth_indice(board, i)->block_id, nth_indice(board, i)->name);
			}
			printf("0 -\tExit\n");
			scanf("%d", &selection);
		}while(!(selection == 0 || nth_indice(board, selection)->type == property));

		if(selection == 0) return;
		print_property_details(*nth_indice(board, selection));
	}
}

void print_property_details(block current_block){
	int i;

	for(i=0; i<33; i++) printf("-");
	printf("\n");
	
	printf("|\t");
	printf("%s", current_block.name);
	if(strlen(current_block.name) < 8) printf("\t\t\t|");
	else if(strlen(current_block.name) < 16) printf("\t\t|");
	printf("\n");

	for(i=0; i<33; i++) printf("-");
	printf("\n");

	printf("|\tRent\t\t");
	printf("%d$\t|", current_block.rent);
	printf("\n");

	printf("|\tRent 1 H\t");
	printf("%d$\t|", current_block.rent_1);
	printf("\n");

	printf("|\tRent 2 H\t");
	printf("%d$\t|", current_block.rent_2);
	printf("\n");

	printf("|\tRent 3 H\t");
	printf("%d$\t|", current_block.rent_3);
	printf("\n");

	for(i=0; i<33; i++) printf("-");
	printf("\n");

	printf("|\tHouse Price\t");
	printf("%d$\t|", current_block.house_price);
	printf("\n");

	for(i=0; i<33; i++) printf("-");
	printf("\n");
}

void buy_property(block *current_block, player *current_player){
	int i, decision;

	if(current_block->type == property){
		printf("Do you want to buy %s (1: Yes - 0: No)\n", current_block->name);
		do{
			scanf("%d", &decision);
		}while(!(decision == 0 || decision == 1));

		if(decision == 1){
			if(current_block->owner.type == noone && current_player->account >= current_block->price){
				for(i=0; current_player->owned_block_ids[i] != -1; i++);
				current_player->owned_block_ids[i] = current_block->block_id;
				current_block->owner = *current_player;
				current_player->account -= current_block->price;
				
				while(current_block->house_count < 3 && current_player->account >= current_block->house_price){
					printf("Do you want to build a house? (1: Yes - 0: No)\n");
					do{
						scanf("%d", &decision);
					}while(!(decision == 0 || decision == 1));

					if(decision == 1){
						current_block->house_count++;
						current_player->account -= current_block->house_price;
					}else return;	
				}
			}else printf("You cannot buy this property\n");
		}else return;
	}
}

void sell_property(block *board, player *current_player){
	int decision, yesno, i;

	if(num_of_my_properties(*current_player) == 0){
		printf("There is no any properties to sell\n");
		return;
	}

	do{
		printf("Which property do you want to sell?\n");
		print_my_properties(board, *current_player);
		printf("0 - Exit\n");
		scanf("%d", &decision);
		if(decision == 0) return;
		for(i=0; i<12; i++){
			if(current_player->owned_block_ids[i] == decision) current_player->owned_block_ids[i] = -1;
		}
		nth_indice(board, decision)->owner.type = noone;
		current_player->account += (nth_indice(board, decision)->price / 2) + (nth_indice(board, decision)->house_count * nth_indice(board, decision)->house_price / 2);
		nth_indice(board, decision)->house_count = 0;

		printf("Do you want to sell another property? (1: Yes - 0: No)\n");
		do{
			scanf("%d", &yesno);
		}while(!(yesno == 0 || yesno == 1));
	}while(yesno == 1 && num_of_my_properties(*current_player) > 0);

	if(yesno == 1 && num_of_my_properties(*current_player) == 0){
		printf("There is no any properties to sell\n");
		return;
	}else return;
}

void sell_property_ai(block *board, player *ai_player){
	int i;
	int most_exp_id = the_most_expensive_property(board, *ai_player);

	for(i=0; i<12; i++){
		if(ai_player->owned_block_ids[i] == most_exp_id) ai_player->owned_block_ids[i] = -1;
	}
	nth_indice(board, most_exp_id)->owner.type = noone;
	ai_player->account += (nth_indice(board, most_exp_id)->price / 2) + (nth_indice(board, most_exp_id)->house_count * nth_indice(board, most_exp_id)->house_price / 2);
	nth_indice(board, most_exp_id)->house_count = 0;
	printf("AI has sold %s\n", nth_indice(board, most_exp_id)->name);
}

void print_my_properties(block *board, player current_player){
	int i;
	if(num_of_my_properties(current_player) != 0){
		for(i=0; i<12; i++){
			if(current_player.owned_block_ids[i] != -1){
				printf("%d - %s (%d H)\t", current_player.owned_block_ids[i], nth_indice(board, current_player.owned_block_ids[i])->name, nth_indice(board, current_player.owned_block_ids[i])->house_count);
				printf("Sell Price: %d$\n", (nth_indice(board, current_player.owned_block_ids[i])->price / 2) + (nth_indice(board, current_player.owned_block_ids[i])->house_count * nth_indice(board, current_player.owned_block_ids[i])->house_price / 2));
			}
		}
	}else printf("You have no any properties\n");
}

int num_of_my_properties(player current_player){
	int i;
	int counter = 0;
	for(i=0; i<12; i++){
		if(current_player.owned_block_ids[i] != -1) counter++;
	}
	return counter;
}

int if_sell_all_properties(block *board, player p){
	int money = 0;
	int i;

	for(i=0; i<12; i++){
		if(p.owned_block_ids[i] != -1){
			money += (nth_indice(board, p.owned_block_ids[i])->price / 2) + (nth_indice(board, p.owned_block_ids[i])->house_count * nth_indice(board, p.owned_block_ids[i])->house_price / 2);
		}
	}
	return money;
}

int is_bankrupt(block *board, block current_block, player p){
	if(current_block.owner.type != noone && current_block.owner.type != p.type){
		switch(current_block.house_count){
			case 0:
				if(p.account + if_sell_all_properties(board, p) < current_block.rent) return 1;
				else return 0;
				break;
			case 1:
				if(p.account + if_sell_all_properties(board, p) < current_block.rent_1) return 1;
				else return 0;
				break;
			case 2:
				if(p.account + if_sell_all_properties(board, p) < current_block.rent_2) return 1;
				else return 0;
				break;
			case 3:
				if(p.account + if_sell_all_properties(board, p) < current_block.rent_3) return 1;
				else return 0;
				break;
		}
	}else if(current_block.type == tax){
		if(p.account + if_sell_all_properties(board, p) < current_block.price) return 1;
		else return 0;
	}
}

int is_bankrupt_debt(block *board, int debt, player p){
	if(p.account + if_sell_all_properties(board, p) < debt) return 1;
	else return 0;
}

void gameplay_menu(block *board, player *current_player, player *other_player){
	int selection, property_selection, decision, next_block;

	while(1){
        printf("1 - Roll the dice\n");
        printf("2 - Show my account\n");
        printf("3 - Show my properties\n");
        printf("4 - Show property deeds\n");
        printf("5 - Buy Property\n");
        printf("6 - Buy house \n");
        printf("7 - Sell property\n");
        printf("Please select an option to continue (%s):\n",current_player->name);
        scanf("%d",&selection);

		switch (selection){
			case 1:
				if(current_player->turn_to_wait == 0){
					next_block = current_player->current_block_id + (rand() % 6 + 1);
					if(next_block >= 24) current_player->account += 10000;
					current_player->current_block_id = next_block % 24;
					turn(board, current_player, other_player);
					return;
				}else{
					printf("You need to wait %d turn to continue.\n", current_player->turn_to_wait);
					current_player->turn_to_wait--;
					return;
				}
				break;
			case 2:
				printf("%s: %d$\n", current_player->name, current_player->account);
				break;
			case 3:
				print_my_properties(board, *current_player);
				break;
			case 4:
				show_properties(board);
				break;
			case 5:
				if(nth_indice(board, current_player->current_block_id)->type == property && nth_indice(board, current_player->current_block_id)->owner.type == noone){
					buy_property(nth_indice(board, current_player->current_block_id), current_player);
				}else if(nth_indice(board, current_player->current_block_id)->type == property && nth_indice(board, current_player->current_block_id)->owner.type == current_player->type){
					printf("You have already bought this property\n");
				}else printf("You cannot buy this property\n");
				break;
			case 6:
				if(num_of_my_properties(*current_player) != 0){
					printf("Choose a property\n");
					print_my_properties(board, *current_player);
					scanf("%d", &property_selection);

					while(nth_indice(board, property_selection)->house_count < 3 && current_player->account >= nth_indice(board, property_selection)->house_price){
						printf("Do you want to build a house? (1: Yes - 0: No)\n");
						do{
							scanf("%d", &decision);
						}while(!(decision == 0 || decision == 1));

						if(decision == 1){
							nth_indice(board, property_selection)->house_count++;
							current_player->account -= nth_indice(board, property_selection)->house_price;
						}else break;
					}
				}else printf("You have no any properties\n");
				break;
			case 7:
				sell_property(board, current_player);
				break;
		
			default:
				printf("Wrong selection!\n");
				break;
		}
	}
}

void gameplay_menu_ai(block *board, player *ai_player, player *other_player){
	int next_block, dice;
	if(ai_player->turn_to_wait == 0){
		dice = rand() % 6 + 1;
		next_block = ai_player->current_block_id + dice;
		if(next_block >= 24) ai_player->account += 10000;
		ai_player->current_block_id = next_block % 24;
		printf("Computer rolled the dice: %d\n", dice);;
		turn_ai(board, ai_player, other_player);
		return;
	}else{
		printf("AI needs to wait %d turn to continue.\n", ai_player->turn_to_wait);
		ai_player->turn_to_wait--;
	}
}

void turn(block *board, player *current_player, player *other_player){
	fortune_card card;
	int property_selection, i, tt_dice, next_block;
	int counter = 0;

	if(nth_indice(board, current_player->current_block_id)->owner.type == other_player->type){
		printf("%s has arrived %s\n", current_player->name, nth_indice(board, current_player->current_block_id)->name);
		switch (nth_indice(board, current_player->current_block_id)->house_count){
			case 0:
				if(is_bankrupt(board, *nth_indice(board, current_player->current_block_id), *current_player)){
					printf("You went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(current_player->account < nth_indice(board, current_player->current_block_id)->rent){
						printf("You don't have enough money to pay. You need %d$.\n", nth_indice(board, current_player->current_block_id)->rent - current_player->account);
						sell_property(board, current_player);
					}
					current_player->account -= nth_indice(board, current_player->current_block_id)->rent;
					other_player->account += nth_indice(board, current_player->current_block_id)->rent;
				}
				break;
			case 1:
				if(is_bankrupt(board, *nth_indice(board, current_player->current_block_id), *current_player)){
					printf("You went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(current_player->account < nth_indice(board, current_player->current_block_id)->rent_1){
						printf("You don't have enough money to pay. You need %d$.\n", nth_indice(board, current_player->current_block_id)->rent_1 - current_player->account);
						sell_property(board, current_player);
					}
					current_player->account -= nth_indice(board, current_player->current_block_id)->rent_1;
					other_player->account += nth_indice(board, current_player->current_block_id)->rent_1;
				}
				break;
			case 2:
				if(is_bankrupt(board, *nth_indice(board, current_player->current_block_id), *current_player)){
					printf("You went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(current_player->account < nth_indice(board, current_player->current_block_id)->rent_2){
						printf("You don't have enough money to pay. You need %d$.\n", nth_indice(board, current_player->current_block_id)->rent_2 - current_player->account);
						sell_property(board, current_player);
					}
					current_player->account -= nth_indice(board, current_player->current_block_id)->rent_2;
					other_player->account += nth_indice(board, current_player->current_block_id)->rent_2;
				}
				break;
			case 3:
				if(is_bankrupt(board, *nth_indice(board, current_player->current_block_id), *current_player)){
					printf("You went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(current_player->account < nth_indice(board, current_player->current_block_id)->rent_3){
						printf("You don't have enough money to pay. You need %d$.\n", nth_indice(board, current_player->current_block_id)->rent_3 - current_player->account);
						sell_property(board, current_player);
					}
					current_player->account -= nth_indice(board, current_player->current_block_id)->rent_3;
					other_player->account += nth_indice(board, current_player->current_block_id)->rent_3;
				}
				break;
		}
	}else if(nth_indice(board, current_player->current_block_id)->type == property && nth_indice(board, current_player->current_block_id)->owner.type == noone){
		printf("%s has arrived %s\n", current_player->name, nth_indice(board, current_player->current_block_id)->name);
		buy_property(nth_indice(board, current_player->current_block_id), current_player);
	}else if(nth_indice(board, current_player->current_block_id)->type == tax){
		printf("%s has arrived %s (tax)\n", current_player->name, nth_indice(board, current_player->current_block_id)->name);
		if(is_bankrupt(board, *nth_indice(board, current_player->current_block_id), *current_player)){
			printf("You went bankrupt, %s won\n", other_player->name);
			exit(0);
		}else{
			while(current_player->account < nth_indice(board, current_player->current_block_id)->price){
				printf("You don't have enough money to pay. You need %d$.\n", nth_indice(board, current_player->current_block_id)->price - current_player->account);
				sell_property(board, current_player);
			}
			current_player->account -= nth_indice(board, current_player->current_block_id)->price;
		}
	}else if(nth_indice(board, current_player->current_block_id)->type == punish){
		printf("%s has arrived %s\n", current_player->name, nth_indice(board, current_player->current_block_id)->name);
		printf("%s has punished for %d turns\n", current_player->name, nth_indice(board, current_player->current_block_id)->price);
		current_player->turn_to_wait = nth_indice(board, current_player->current_block_id)->price;
	}else if(nth_indice(board, current_player->current_block_id)->type == fortune){
		printf("%s has arrived %s\n", current_player->name, nth_indice(board, current_player->current_block_id)->name);
		card.type = (rand() % 5) + 1;
		switch(card.type){
			case free_house:
				printf("You can build a house for free\n");
				if(num_of_my_properties(*current_player) != 0){
					for(i=0; i<12; i++){
						if(current_player->owned_block_ids[i] != -1){
							if(nth_indice(board, current_player->owned_block_ids[i])->house_count != 3) counter++;
						}
					}
					if(counter == 0){
						printf("Your all properties are full\n");
						break;
					}

					do{
						printf("Choose a property\n");
						print_my_properties(board, *current_player);
						scanf("%d", &property_selection);
					}while(nth_indice(board, property_selection)->house_count == 3);
					nth_indice(board, property_selection)->house_count++;
				}else printf("You have no any properties\n");
				break;
			case time_travel:
				tt_dice = (rand() % 6) + 1;
				if(tt_dice >= 1 && tt_dice <= 3){
					printf("Time Travel: 2 blocks forward\n");
					next_block = current_player->current_block_id + 2;
					if(next_block >= 24) current_player->account += 10000;
					current_player->current_block_id = next_block % 24;
					turn(board, current_player, other_player);
				}else if(tt_dice >= 4 && tt_dice <= 6){
					printf("Time Travel: 2 blocks back\n");
					next_block = current_player->current_block_id - 2;
					current_player->current_block_id = next_block % 24;
					turn(board, current_player, other_player);
				}
				break;
			case garnishment:
				printf("Garnishment: You have to pay 5000$ to the bank\n");
				if(is_bankrupt_debt(board, 5000, *current_player)){
					printf("You went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(current_player->account < 5000){
						printf("You don't have enough money to pay. You need %d$.\n", 5000 - current_player->account);
						sell_property(board, current_player);
					}
					current_player->account -= 5000;
				}
				break;
			case generosity:
				printf("Generosity: You have to pay 10000$ to the opponent\n");
				if(is_bankrupt_debt(board, 10000, *current_player)){
					printf("You went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(current_player->account < 10000){
						printf("You don't have enough money to pay. You need %d$.\n", 10000 - current_player->account);
						sell_property(board, current_player);
					}
					current_player->account -= 10000;
					other_player->account += 10000;
				}
				break;
			case treasure_hunter:
				printf("Treasure Hunter: You earned 20000$\n");
				current_player->account += 20000;
				break;
		}
	}
}

void turn_ai(block *board, player *ai_player, player *other_player){
	int next_block, i, dice, property_selection;
	fortune_card card;
	int counter = 0;

	if(nth_indice(board, ai_player->current_block_id)->owner.type == other_player->type){
		printf("AI has arrived %s\n", nth_indice(board, ai_player->current_block_id)->name);
		switch(nth_indice(board, ai_player->current_block_id)->house_count){
			case 0:
				if(is_bankrupt(board, *nth_indice(board, ai_player->current_block_id), *ai_player)){
					printf("AI went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(ai_player->account < nth_indice(board, ai_player->current_block_id)->rent){
						sell_property_ai(board, ai_player);
					}
					ai_player->account -= nth_indice(board, ai_player->current_block_id)->rent;
					other_player->account += nth_indice(board, ai_player->current_block_id)->rent;
				}
				break;
			case 1:
				if(is_bankrupt(board, *nth_indice(board, ai_player->current_block_id), *ai_player)){
					printf("AI went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(ai_player->account < nth_indice(board, ai_player->current_block_id)->rent_1){
						sell_property_ai(board, ai_player);
					}
					ai_player->account -= nth_indice(board, ai_player->current_block_id)->rent_1;
					other_player->account += nth_indice(board, ai_player->current_block_id)->rent_1;
				}
				break;
			case 2:
				if(is_bankrupt(board, *nth_indice(board, ai_player->current_block_id), *ai_player)){
					printf("AI went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(ai_player->account < nth_indice(board, ai_player->current_block_id)->rent_2){
						sell_property_ai(board, ai_player);
					}
					ai_player->account -= nth_indice(board, ai_player->current_block_id)->rent_2;
					other_player->account += nth_indice(board, ai_player->current_block_id)->rent_2;
				}
				break;
			case 3:
				if(is_bankrupt(board, *nth_indice(board, ai_player->current_block_id), *ai_player)){
					printf("AI went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(ai_player->account < nth_indice(board, ai_player->current_block_id)->rent_3){
						sell_property_ai(board, ai_player);
					}
					ai_player->account -= nth_indice(board, ai_player->current_block_id)->rent_3;
					other_player->account += nth_indice(board, ai_player->current_block_id)->rent_3;
				}
				break;
		}
	}else if(nth_indice(board, ai_player->current_block_id)->type == property && nth_indice(board, ai_player->current_block_id)->owner.type == noone){
		printf("AI has arrived %s\n", nth_indice(board, ai_player->current_block_id)->name);
		if(ai_player->account >= nth_indice(board, ai_player->current_block_id)->price){
			if(nth_indice(board, ai_player->current_block_id)->price < AVE_PRICE){
				for(i=0; ai_player->owned_block_ids[i] != -1; i++);
				ai_player->owned_block_ids[i] = ai_player->current_block_id;
				nth_indice(board, ai_player->current_block_id)->owner = *ai_player;
				ai_player->account -= nth_indice(board, ai_player->current_block_id)->price;
				printf("AI has bought %s\n", nth_indice(board, ai_player->current_block_id)->name);
			}else{
				dice = (rand() % 6) + 1;
				if(dice >= 1 && dice <= 3){
					for(i=0; ai_player->owned_block_ids[i] != -1; i++);
					ai_player->owned_block_ids[i] = ai_player->current_block_id;
					nth_indice(board, ai_player->current_block_id)->owner = *ai_player;
					ai_player->account -= nth_indice(board, ai_player->current_block_id)->price;
					printf("AI has bought %s\n", nth_indice(board, ai_player->current_block_id)->name);
				}else if(dice >= 4 && dice <= 6) return;
			}
		}
	}else if(nth_indice(board, ai_player->current_block_id)->owner.type == ai_player->type){
		printf("AI has arrived %s\n", nth_indice(board, ai_player->current_block_id)->name);
		if(ai_player->account >= nth_indice(board, ai_player->current_block_id)->house_price && nth_indice(board, ai_player->current_block_id)->house_count < 3){
			if(num_of_my_properties(*ai_player) >= PROPERTY_ONE_THIRD){
				dice = (rand() % 6) + 1;
				if(dice >= 1 && dice <= 3){
					nth_indice(board, ai_player->current_block_id)->house_count++;
					ai_player->account -= nth_indice(board, ai_player->current_block_id)->house_price;
					printf("AI has built a house on %s\n", nth_indice(board, ai_player->current_block_id)->name);
				}else if(dice >= 4 && dice <= 6) return;
			}else return;
		}else return;
	}else if(nth_indice(board, ai_player->current_block_id)->type == tax){
		printf("AI has arrived %s (tax)\n", nth_indice(board, ai_player->current_block_id)->name);
		if(is_bankrupt(board, *nth_indice(board, ai_player->current_block_id), *ai_player)){
			printf("AI went bankrupt, %s won\n", other_player->name);
			exit(0);
		}else{
			while(ai_player->account < nth_indice(board, ai_player->current_block_id)->price){
				sell_property_ai(board, ai_player);
			}
			ai_player->account -= nth_indice(board, ai_player->current_block_id)->price;
		}
	}else if(nth_indice(board, ai_player->current_block_id)->type == punish){
		printf("AI has arrived %s\n", nth_indice(board, ai_player->current_block_id)->name);
		printf("AI has punished for %d turns\n", nth_indice(board, ai_player->current_block_id)->price);
		ai_player->turn_to_wait = nth_indice(board, ai_player->current_block_id)->price;
	}else if(nth_indice(board, ai_player->current_block_id)->type == fortune){
		printf("AI has arrived %s\n", nth_indice(board, ai_player->current_block_id)->name);
		card.type = (rand() % 5) + 1;
		switch(card.type){
			case free_house:
				if(num_of_my_properties(*ai_player) != 0){
					for(i=0; i<12; i++){
						if(ai_player->owned_block_ids[i] != -1){
							if(nth_indice(board, ai_player->owned_block_ids[i])->house_count != 3){
								counter++;
								property_selection = ai_player->owned_block_ids[i];
							}
						}
					}
					if(counter == 0) break;
					nth_indice(board, property_selection)->house_count++;
					printf("AI has built a house in %s\n", nth_indice(board, property_selection)->name);
				}
				break;
			case time_travel:
				dice = (rand() % 6) + 1;
				if(dice >= 1 && dice <= 3){
					printf("Time Travel: 2 blocks forward\n");
					next_block = ai_player->current_block_id + 2;
					if(next_block >= 24) ai_player->account += 10000;
					ai_player->current_block_id = next_block % 24;
					turn_ai(board, ai_player, other_player);
				}else if(dice >= 4 && dice <= 6){
					printf("Time Travel: 2 blocks back\n");
					next_block = ai_player->current_block_id - 2;
					ai_player->current_block_id = next_block % 24;
					turn_ai(board, ai_player, other_player);
				}
				break;
			case garnishment:
				printf("Garnishment: AI have to pay 5000$ to the bank\n");
				if(is_bankrupt_debt(board, 5000, *ai_player)){
					printf("AI went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(ai_player->account < 5000){
						sell_property_ai(board, ai_player);
					}
					ai_player->account -= 5000;
				}
				break;
			case generosity:
				printf("Generosity: AI have to pay 10000$ to the opponent\n");
				if(is_bankrupt_debt(board, 10000, *ai_player)){
					printf("AI went bankrupt, %s won\n", other_player->name);
					exit(0);
				}else{
					while(ai_player->account < 10000){
						sell_property_ai(board, ai_player);
					}
					ai_player->account -= 10000;
					other_player->account += 10000;
				}
				break;
			case treasure_hunter:
				printf("Treasure Hunter: AI earned 20000$\n");
				ai_player->account += 20000;
				break;
		}
	}
}

void gameplay(block *board, player *player_one, player *player_two){
	show_board(board, *player_one, *player_two);
	printf("Welcome to Monopoly game. %s will start first\n", player_one->name);
    printf("%s :%d$ , %s:%d$\n", player_one->name, player_one->account, player_two->name, player_two->account);
	gameplay_menu(board, player_one, player_two);
	gameplay_menu_ai(board, player_two, player_one);

	while(1){
		show_board(board, *player_one, *player_two);
    	printf("%s :%d$ , %s:%d$\n", player_one->name, player_one->account, player_two->name, player_two->account);
		gameplay_menu(board, player_one, player_two);
		gameplay_menu_ai(board, player_two, player_one);
	}
}

void init_the_board(block *board){
	nth_indice(board, 0)->block_id = 0;
	nth_indice(board, 0)->name = "Start";
	nth_indice(board, 0)->price = 0;
	nth_indice(board, 0)->rent = 0;
	nth_indice(board, 0)->rent_1 = 0;
	nth_indice(board, 0)->rent_2 = 0;
	nth_indice(board, 0)->rent_3 = 0;
	nth_indice(board, 0)->house_price = 0;
	nth_indice(board, 0)->house_count = 0;
	nth_indice(board, 0)->owner.type = noone;
	nth_indice(board, 0)->type = start;

	nth_indice(board, 1)->block_id = 1;
	nth_indice(board, 1)->name = "Esenyurt";
	nth_indice(board, 1)->price = 16000;
	nth_indice(board, 1)->rent = 800;
	nth_indice(board, 1)->rent_1 = 4000;
	nth_indice(board, 1)->rent_2 = 9000;
	nth_indice(board, 1)->rent_3 = 25000;
	nth_indice(board, 1)->house_price = 10000;
	nth_indice(board, 1)->house_count = 0;
	nth_indice(board, 1)->owner.type = noone;
	nth_indice(board, 1)->type = property;

	nth_indice(board, 2)->block_id = 2;
	nth_indice(board, 2)->name = "Car Park";
	nth_indice(board, 2)->price = 1500;
	nth_indice(board, 2)->rent = 0;
	nth_indice(board, 2)->rent_1 = 0;
	nth_indice(board, 2)->rent_2 = 0;
	nth_indice(board, 2)->rent_3 = 0;
	nth_indice(board, 2)->house_price = 0;
	nth_indice(board, 2)->house_count = 0;
	nth_indice(board, 2)->owner.type = noone;
	nth_indice(board, 2)->type = tax;

	nth_indice(board, 3)->block_id = 3;
	nth_indice(board, 3)->name = "Fortune Card";
	nth_indice(board, 3)->price = 0;
	nth_indice(board, 3)->rent = 0;
	nth_indice(board, 3)->rent_1 = 0;
	nth_indice(board, 3)->rent_2 = 0;
	nth_indice(board, 3)->rent_3 = 0;
	nth_indice(board, 3)->house_price = 0;
	nth_indice(board, 3)->house_count = 0;
	nth_indice(board, 3)->owner.type = noone;
	nth_indice(board, 3)->type = fortune;

	nth_indice(board, 4)->block_id = 4;
	nth_indice(board, 4)->name = "Tuzla";
	nth_indice(board, 4)->price = 16500;
	nth_indice(board, 4)->rent = 850;
	nth_indice(board, 4)->rent_1 = 4250;
	nth_indice(board, 4)->rent_2 = 9500;
	nth_indice(board, 4)->rent_3 = 26000;
	nth_indice(board, 4)->house_price = 12000;
	nth_indice(board, 4)->house_count = 0;
	nth_indice(board, 4)->owner.type = noone;
	nth_indice(board, 4)->type = property;

	nth_indice(board, 5)->block_id = 5;
	nth_indice(board, 5)->name = "Arnavutkoy";
	nth_indice(board, 5)->price = 17000;
	nth_indice(board, 5)->rent = 875;
	nth_indice(board, 5)->rent_1 = 4500;
	nth_indice(board, 5)->rent_2 = 10000;
	nth_indice(board, 5)->rent_3 = 28000;
	nth_indice(board, 5)->house_price = 12000;
	nth_indice(board, 5)->house_count = 0;
	nth_indice(board, 5)->owner.type = noone;
	nth_indice(board, 5)->type = property;

	nth_indice(board, 6)->block_id = 6;
	nth_indice(board, 6)->name = "Wait 2 Turn";
	nth_indice(board, 6)->price = 2;
	nth_indice(board, 6)->rent = 0;
	nth_indice(board, 6)->rent_1 = 0;
	nth_indice(board, 6)->rent_2 = 0;
	nth_indice(board, 6)->rent_3 = 0;
	nth_indice(board, 6)->house_price = 0;
	nth_indice(board, 6)->house_count = 0;
	nth_indice(board, 6)->owner.type = noone;
	nth_indice(board, 6)->type = punish;

	nth_indice(board, 7)->block_id = 7;
	nth_indice(board, 7)->name = "Catalca";
	nth_indice(board, 7)->price = 20000;
	nth_indice(board, 7)->rent = 1000;
	nth_indice(board, 7)->rent_1 = 5000;
	nth_indice(board, 7)->rent_2 = 12000;
	nth_indice(board, 7)->rent_3 = 30000;
	nth_indice(board, 7)->house_price = 13000;
	nth_indice(board, 7)->house_count = 0;
	nth_indice(board, 7)->owner.type = noone;
	nth_indice(board, 7)->type = property;

	nth_indice(board, 8)->block_id = 8;
	nth_indice(board, 8)->name = "Beykoz";
	nth_indice(board, 8)->price = 23000;
	nth_indice(board, 8)->rent = 1100;
	nth_indice(board, 8)->rent_1 = 5500;
	nth_indice(board, 8)->rent_2 = 12500;
	nth_indice(board, 8)->rent_3 = 33000;
	nth_indice(board, 8)->house_price = 13000;
	nth_indice(board, 8)->house_count = 0;
	nth_indice(board, 8)->owner.type = noone;
	nth_indice(board, 8)->type = property;

	nth_indice(board, 9)->block_id = 9;
	nth_indice(board, 9)->name = "Fortune Card";
	nth_indice(board, 9)->price = 0;
	nth_indice(board, 9)->rent = 0;
	nth_indice(board, 9)->rent_1 = 0;
	nth_indice(board, 9)->rent_2 = 0;
	nth_indice(board, 9)->rent_3 = 0;
	nth_indice(board, 9)->house_price = 0;
	nth_indice(board, 9)->house_count = 0;
	nth_indice(board, 9)->owner.type = noone;
	nth_indice(board, 9)->type = fortune;

	nth_indice(board, 10)->block_id = 10;
	nth_indice(board, 10)->name = "Car Fix";
	nth_indice(board, 10)->price = 1750;
	nth_indice(board, 10)->rent = 0;
	nth_indice(board, 10)->rent_1 = 0;
	nth_indice(board, 10)->rent_2 = 0;
	nth_indice(board, 10)->rent_3 = 0;
	nth_indice(board, 10)->house_price = 0;
	nth_indice(board, 10)->house_count = 0;
	nth_indice(board, 10)->owner.type = noone;
	nth_indice(board, 10)->type = tax;

	nth_indice(board, 11)->block_id = 11;
	nth_indice(board, 11)->name = "Maltepe";
	nth_indice(board, 11)->price = 30000;
	nth_indice(board, 11)->rent = 1350;
	nth_indice(board, 11)->rent_1 = 7000;
	nth_indice(board, 11)->rent_2 = 15000;
	nth_indice(board, 11)->rent_3 = 40000;
	nth_indice(board, 11)->house_price = 15000;
	nth_indice(board, 11)->house_count = 0;
	nth_indice(board, 11)->owner.type = noone;
	nth_indice(board, 11)->type = property;

	nth_indice(board, 12)->block_id = 12;
	nth_indice(board, 12)->name = "Bills";
	nth_indice(board, 12)->price = 2000;
	nth_indice(board, 12)->rent = 0;
	nth_indice(board, 12)->rent_1 = 0;
	nth_indice(board, 12)->rent_2 = 0;
	nth_indice(board, 12)->rent_3 = 0;
	nth_indice(board, 12)->house_price = 0;
	nth_indice(board, 12)->house_count = 0;
	nth_indice(board, 12)->owner.type = noone;
	nth_indice(board, 12)->type = tax;

	nth_indice(board, 13)->block_id = 13;
	nth_indice(board, 13)->name = "Sisli";
	nth_indice(board, 13)->price = 33000;
	nth_indice(board, 13)->rent = 1500;
	nth_indice(board, 13)->rent_1 = 8000;
	nth_indice(board, 13)->rent_2 = 16000;
	nth_indice(board, 13)->rent_3 = 42000;
	nth_indice(board, 13)->house_price = 16000;
	nth_indice(board, 13)->house_count = 0;
	nth_indice(board, 13)->owner.type = noone;
	nth_indice(board, 13)->type = property;

	nth_indice(board, 14)->block_id = 14;
	nth_indice(board, 14)->name = "Oil";
	nth_indice(board, 14)->price = 2250;
	nth_indice(board, 14)->rent = 0;
	nth_indice(board, 14)->rent_1 = 0;
	nth_indice(board, 14)->rent_2 = 0;
	nth_indice(board, 14)->rent_3 = 0;
	nth_indice(board, 14)->house_price = 0;
	nth_indice(board, 14)->house_count = 0;
	nth_indice(board, 14)->owner.type = noone;
	nth_indice(board, 14)->type = tax;

	nth_indice(board, 15)->block_id = 15;
	nth_indice(board, 15)->name = "Fortune Card";
	nth_indice(board, 15)->price = 0;
	nth_indice(board, 15)->rent = 0;
	nth_indice(board, 15)->rent_1 = 0;
	nth_indice(board, 15)->rent_2 = 0;
	nth_indice(board, 15)->rent_3 = 0;
	nth_indice(board, 15)->house_price = 0;
	nth_indice(board, 15)->house_count = 0;
	nth_indice(board, 15)->owner.type = noone;
	nth_indice(board, 15)->type = fortune;

	nth_indice(board, 16)->block_id = 16;
	nth_indice(board, 16)->name = "Atasehir";
	nth_indice(board, 16)->price = 35000;
	nth_indice(board, 16)->rent = 1600;
	nth_indice(board, 16)->rent_1 = 8500;
	nth_indice(board, 16)->rent_2 = 17000;
	nth_indice(board, 16)->rent_3 = 45000;
	nth_indice(board, 16)->house_price = 17000;
	nth_indice(board, 16)->house_count = 0;
	nth_indice(board, 16)->owner.type = noone;
	nth_indice(board, 16)->type = property;

	nth_indice(board, 17)->block_id = 17;
	nth_indice(board, 17)->name = "Sariyer";
	nth_indice(board, 17)->price = 40000;
	nth_indice(board, 17)->rent = 1750;
	nth_indice(board, 17)->rent_1 = 9500;
	nth_indice(board, 17)->rent_2 = 19000;
	nth_indice(board, 17)->rent_3 = 48000;
	nth_indice(board, 17)->house_price = 19000;
	nth_indice(board, 17)->house_count = 0;
	nth_indice(board, 17)->owner.type = noone;
	nth_indice(board, 17)->type = property;

	nth_indice(board, 18)->block_id = 18;
	nth_indice(board, 18)->name = "Wait 1 Turn";
	nth_indice(board, 18)->price = 1;
	nth_indice(board, 18)->rent = 0;
	nth_indice(board, 18)->rent_1 = 0;
	nth_indice(board, 18)->rent_2 = 0;
	nth_indice(board, 18)->rent_3 = 0;
	nth_indice(board, 18)->house_price = 0;
	nth_indice(board, 18)->house_count = 0;
	nth_indice(board, 18)->owner.type = noone;
	nth_indice(board, 18)->type = punish;

	nth_indice(board, 19)->block_id = 19;
	nth_indice(board, 19)->name = "Kadikoy";
	nth_indice(board, 19)->price = 43000;
	nth_indice(board, 19)->rent = 1900;
	nth_indice(board, 19)->rent_1 = 11000;
	nth_indice(board, 19)->rent_2 = 21500;
	nth_indice(board, 19)->rent_3 = 55000;
	nth_indice(board, 19)->house_price = 23000;
	nth_indice(board, 19)->house_count = 0;
	nth_indice(board, 19)->owner.type = noone;
	nth_indice(board, 19)->type = property;

	nth_indice(board, 20)->block_id = 20;
	nth_indice(board, 20)->name = "Besiktas";
	nth_indice(board, 20)->price = 60000;
	nth_indice(board, 20)->rent = 2500;
	nth_indice(board, 20)->rent_1 = 15000;
	nth_indice(board, 20)->rent_2 = 28000;
	nth_indice(board, 20)->rent_3 = 60000;
	nth_indice(board, 20)->house_price = 30000;
	nth_indice(board, 20)->house_count = 0;
	nth_indice(board, 20)->owner.type = noone;
	nth_indice(board, 20)->type = property;

	nth_indice(board, 21)->block_id = 21;
	nth_indice(board, 21)->name = "Fortune Card";
	nth_indice(board, 21)->price = 0;
	nth_indice(board, 21)->rent = 0;
	nth_indice(board, 21)->rent_1 = 0;
	nth_indice(board, 21)->rent_2 = 0;
	nth_indice(board, 21)->rent_3 = 0;
	nth_indice(board, 21)->house_price = 0;
	nth_indice(board, 21)->house_count = 0;
	nth_indice(board, 21)->owner.type = noone;
	nth_indice(board, 21)->type = fortune;

	nth_indice(board, 22)->block_id = 22;
	nth_indice(board, 22)->name = "Vocation";
	nth_indice(board, 22)->price = 5000;
	nth_indice(board, 22)->rent = 0;
	nth_indice(board, 22)->rent_1 = 0;
	nth_indice(board, 22)->rent_2 = 0;
	nth_indice(board, 22)->rent_3 = 0;
	nth_indice(board, 22)->house_price = 0;
	nth_indice(board, 22)->house_count = 0;
	nth_indice(board, 22)->owner.type = noone;
	nth_indice(board, 22)->type = tax;

	nth_indice(board, 23)->block_id = 23;
	nth_indice(board, 23)->name = "Bebek";
	nth_indice(board, 23)->price = 70000;
	nth_indice(board, 23)->rent = 3500;
	nth_indice(board, 23)->rent_1 = 20000;
	nth_indice(board, 23)->rent_2 = 35500;
	nth_indice(board, 23)->rent_3 = 65000;
	nth_indice(board, 23)->house_price = 35000;
	nth_indice(board, 23)->house_count = 0;
	nth_indice(board, 23)->owner.type = noone;
	nth_indice(board, 23)->type = property;
}