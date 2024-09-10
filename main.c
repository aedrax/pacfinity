#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>



#define WIDTH 19
#define HEIGHT 18
#define TICK_RATE_USEC 100000
#define WALLCHAR '#'
#define DOTVALUE 10

char gameBoard[HEIGHT][WIDTH] = {
    "###################",
    "#........#........#",
    "#.##.###.#.###.##.#",
    "#.................#",
    "#.##.#.#####.#.##.#",
    "#....#...#...#....#",
    "####.###.#.###.####",
    "####.#.......#.####",
    ".......#####.......",
    "####.#       #.####",
    "#........#........#",
    "#.##.###.#.###.##.#",
    "#..#...........#..#",
    "##.#.#.#####.#.#.##",
    "#....#...#...#....#",
    "#.######.#.######.#",
    "#.................#",
    "###################",
};

typedef enum COLOR {
    YELLOW = 1,
    RED = 2,
    PINK = 3,
    CYAN = 4,
    GREEN = 5,
    BLUE = 6,
} COLOR;

typedef struct {
    int x;
    int y;
    char symbol;
} Entity;

Entity pacman = {
    .x=9,
    .y=9,
    .symbol='P',
};

Entity ghosts[] = {
    {.x=9, .y=7, .symbol='G'},
    {.x=9, .y=7, .symbol='G'},
    {.x=9, .y=7, .symbol='G'},
    {.x=9, .y=7, .symbol='G'},
};

void displayBoard() {
    clear(); // Clear the screen
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            mvaddch(i, j, gameBoard[i][j]);
        }
    }
    mvaddch(pacman.y, pacman.x, pacman.symbol); // Draw Pacman
    for (int i = 0; i < sizeof(ghosts) / sizeof(ghosts[0]); i++) {
        mvaddch(ghosts[i].y, ghosts[i].x, ghosts[i].symbol); // Draw ghosts
    }
    refresh(); // Refresh the screen to show changes
}

void movePacman(int ch) {
    int newX = pacman.x;
    int newY = pacman.y;

    switch (ch) {
        case KEY_UP: newY--; break;
        case KEY_DOWN: newY++; break;
        case KEY_LEFT: newX--; break;
        case KEY_RIGHT: newX++; break;
    }

    // if new position is out of bounds, wrap around
    newX = (newX + WIDTH) % WIDTH; // Wrap around horizontally
    newY = (newY + HEIGHT) % HEIGHT; // Wrap around vertically

    if (gameBoard[newY][newX] != WALLCHAR) {
        pacman.x = newX;
        pacman.y = newY;
    }
    
    if (gameBoard[newY][newX] == '.') {
        gameBoard[newY][newX] = ' ';
        score += DOTVALUE;
    }
}

void moveGhosts() {

    for (int i = 0; i < sizeof(ghosts) / sizeof(ghosts[0]); i++) {
        int newX = ghosts[i].x;
        int newY = ghosts[i].y;

        // Move ghosts randomly
        switch (rand() % 4) {
            case 0: newY--; break;
            case 1: newY++; break;
            case 2: newX--; break;
            case 3: newX++; break;
        }

        // if new position is out of bounds, wrap around
        newX = (newX + WIDTH) % WIDTH; // Wrap around horizontally
        newY = (newY + HEIGHT) % HEIGHT; // Wrap around vertically

        if (gameBoard[newY][newX] != WALLCHAR) {
            ghosts[i].x = newX;
            ghosts[i].y = newY;
        }
    }
}

bool checkForCollision() {
    for (int i = 0; i < sizeof(ghosts) / sizeof(ghosts[0]); i++) {
        if (ghosts[i].x == pacman.x && ghosts[i].y == pacman.y) {
            return true;
        }
    }
    return false;
}

void initializeColors() {
    if (!has_colors()) {
        return;
    }

    start_color();
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(RED, COLOR_BLACK, COLOR_RED);
    init_pair(PINK, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(CYAN, COLOR_BLACK, COLOR_CYAN);
    init_pair(GREEN, COLOR_BLACK, COLOR_GREEN);
    init_pair(BLUE, COLOR_WHITE, COLOR_BLUE);

}

int main() {
    initscr(); // Initialize the ncurses screen
    keypad(stdscr, TRUE); // Enable keyboard input for the window
    noecho(); // Don't echo pressed keys to the screen
    curs_set(FALSE); // Hide the default screen cursor
    nodelay(stdscr, TRUE); // Set getch to non-blocking mode
    initializeColors();


    displayBoard();
    while (1) {
        int ch = ERR;
        // Get the most recent key press
        int new_ch = getch();
        while (new_ch != ERR) {
            ch = new_ch;
            new_ch = getch();
        }

        if (ch == 'q') break; // Exit the loop if 'q' is pressed
        if (ch != ERR) {
            movePacman(ch);
        }
        bool collisionDetected = checkForCollision();
        moveGhosts();
        collisionDetected |= checkForCollision();
        if (collisionDetected) {
            break;
        }
        displayBoard();
        usleep(TICK_RATE_USEC); // Sleep for a short period to pace the game
    }

    endwin(); // End ncurses mode
    // print the game over message
    printf("Game Over!\n");
    printf("Score: %d\n", score);
    return 0;
}
