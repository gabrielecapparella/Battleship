#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char *shipName[] = {"Carrier", "Battleship", "Cruiser", "Submarine", "Destroyer"};
const int shipShots[] = {5, 4, 3, 3, 2};

typedef struct {
	int squares[5][3]; // 0=x, 1=y, 2=hit
} ship;

typedef struct {
	ship ships[5];
	// ~=water, s=ship, x=hit, o=miss
	char grid[10][10];
	char shots[10][10];
	int sunken;
} player;


void playerInit(player *p);
int playerTurn(player *u, player *a);
int aiTurn(player *a, player *b);
void placeShips(char grid[10][10], ship ships[5]);
int isValid(int x, int y);
int isClear(char grid[10][10], int x, int y);
void printGrids(player *p);
void initialize(char arr[10][10]);
int shot(int x, int y, player *from, player *to);
int c2i(char c);
char i2c(int i);


int main() {

	srand( time(NULL) );
	
	player user, ai;
	
	playerInit(&user);
	playerInit(&ai);
	
	printf("Welcome! You already know how it works.\nCoordinates should be in the format [a-j][0-9], es: a2\n");

	while(1) {
		printGrids(&user);
		//printGrids(&ai); just for debugging
		printf("\n\n");
		if(playerTurn(&user, &ai)) {
			printGrids(&user);
			printf("\n\nYAY! YOU WON!\n");
			break;
		}
		if(aiTurn(&ai, &user)) {
			printGrids(&user);
			printf("\n\nWHOOPS, YOU LOST!\n");
			break;
		}
	}


	return 0;
}

void playerInit(player *p) {
	initialize(p->grid);
	
	initialize(p->shots);
	
	placeShips(p->grid, p->ships);
	
	p->sunken = 0;
	
}

int playerTurn(player *u, player *a) {
	char x;
	int y;
	
	printf("Insert coordinates: ");
	int s = scanf(" %c%d", &x, &y);
	if(s==2 && x>='a' && x<='j' && y>=0 && y<=9) {
		if(shot(c2i(x), y, u, a)) return 1;
	} else {
		printf("Nope\n");
		playerTurn(u, a);
	}
	return 0;
}

int aiTurn(player *a, player *u) {
	int i, j, x, y, k;
	
	//look for a hit and shoot a randomly chosen water cell around it
	for(i=0; i<10; i++) {
		for(j=0; j<10; j++) {
		
			if(a->shots[i][j]=='x') { 
				for(k=0; k<3; k++) { //make 3 attempts to shoot around it
					x = i + rand() % 3 - 1;
					y = j + rand() % 3 - 1;
					if(isValid(x, y) && a->shots[x][y]=='~') {
						printf("AI shot %c%d. ", i2c(x), y);
						if(shot(x, y, a, u)) return 1;
						return 0;
					}
				}
			}
		}
	}
	
	//shoot randomly
	do {
		x = rand() % 10;
		y = rand() % 10;
	} while(a->shots[x][y]!='~');
	
	printf("AI shot %c%d. ", i2c(x), y);
	if(shot(x, y, a, u)) return 1;
	return 0;
}

void placeShips(char grid[10][10], ship ships[5]) {
	int s, i, d, c, x, y, j, cx, cy;	

	for(i=0; i<5; i++) {
		d = rand() % 2; //direction, 0 is vertical
		c = 0; //check, 1 when the ship is placed
		while(c==0) { // I pick one square randomly and check if I can put the current ship there
			c = 1;
			x = rand() % 10;
			y = rand() % 10;
			cx = x;
			cy = y;
			c = 1;
			for(j=0; j<shipShots[i]; j++) {
				if(isClear(grid, cx, cy)) {
					if(d==0) {
						cy++;
					} else {
						cx++;
					}
				} else {
					c = 0;
					break;
				}
			}
			
			if(c==0) continue; 
			
			cx = x;
			cy = y;
			
			for(j=0; j<shipShots[i]; j++) {
				grid[cx][cy] = 's';//i + '0';
				ships[i].squares[j][0] = cx;
				ships[i].squares[j][1] = cy;
				ships[i].squares[j][2] = 0;
				if(d==0) {
					cy++;
				} else {
					cx++;
				}
			}
		}
	}
}

int isValid(int x, int y) {
	if(x<0 || y<0) return 0;
	if(x>9 || y>9) return 0;
	return 1;
}

int isClear(char grid[10][10], int x, int y) {
	if(isValid(x, y) && grid[x][y]=='~') return 1;
	return 0;
}

void printGrids(player *p) {
	int x, y;
	
	printf("\n  0 1 2 3 4 5 6 7 8 9       0 1 2 3 4 5 6 7 8 9\n");
	for(x=0; x<10; x++) {
		printf("%c ", 'a'+x);
		for(y=0; y<10; y++) {
			printf("%c ", p->shots[x][y]);
		}
		printf("    ");
		printf("%c ", 'a'+x);
		for(y=0; y<10; y++) {
			printf("%c ", p->grid[x][y]);
		}
		
		
		printf("\n");
	}
	printf("\n");
}

void initialize(char arr[10][10]) {
	int x, y;
	for(x=0; x<10; x++) {
		for(y=0; y<10; y++) {
			arr[x][y] = '~';
		}
	}	
}

int shot(int x, int y, player *from, player *to) { //it returns 1 if player from wins
	if(to->grid[x][y]=='~' || to->grid[x][y]=='o') { //miss
		to->grid[x][y] = 'o';
		from->shots[x][y] = 'o';
		printf("Miss!\n");
	} else if(to->grid[x][y]=='s') { //hit
		to->grid[x][y] = 'x';
		from->shots[x][y] = 'x';	
		//updating ship's state
		int i, j, k;
		for(i=0; i<5; i++) {
			for(j=0; j<shipShots[i]; j++) {
				if(to->ships[i].squares[j][0]==x && to->ships[i].squares[j][1]==y) {
					to->ships[i].squares[j][2] = 1;
					int isSunk = 1;
					for(k=0; k<shipShots[i]; k++) {
						if(to->ships[i].squares[k][2]==0) {
							isSunk = 0;
							break;
						}
					}
					if(isSunk) {
						printf("%s hit and sunk!\n", shipName[i]);
						from->sunken++;
						if(from->sunken == 5) return 1;
						return 0;
					}
				}
			}
		}	
		printf("Hit!\n");		
	} else { //already hit
		printf("It was already hit there.\n");
	}
	
	return 0;
}

int c2i(char c) {
	return c - 'a';
}

char i2c(int i) {
	return i + 'a';
}
