#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX(a, b)               ((a) > (b) ? (a) : (b))
#define MIN(a, b)               ((a) < (b) ? (a) : (b))
#define COORD_POS(row, col, n)  ((row) * (n) + (col))

char **create_empty_board(int n);

void fill_board(char **board, int n, int mine_count);

void show_board(char **board, const int *revealed, const int *marked, int n);

int is_duplicate(int value, const int *arr, int size);

int reveal_field(char **board, int *revealed, int *marked, int n, int row, int col);

int mark_field(const int *revealed, int *marked, int n, int row, int col);

int board_size_input();

int mine_count_input(int n);

void print_logo();

void print_you_won();

void print_you_lost();

int main() {
    int n, mine_count, row, col;
    int *revealed, *marked;
    int playing, reveal_status, mark_status; // bools
    char **board;

    print_logo();

    n = board_size_input();
    mine_count = mine_count_input(n);

    revealed = (int *) calloc(n * n, sizeof(int)); // 0 = not revealed, 1 = revealed
    marked = (int *) calloc(n * n, sizeof(int));   // 0 = not marked, 1 = marked

    board = create_empty_board(n);

    fill_board(board, n, mine_count);
    show_board(board, revealed, marked, n);

    playing = 1;
    while (playing) {
        do {
            char input[10];
            char command;
            int scan_result;
            printf("\nPlease enter the field you want to reveal(r) or mark/unmark(m) (command row column) e.g: 'r 2 5':\n");
            fgets(input, sizeof(input), stdin);
            scan_result = sscanf(input, "%c %d %d", &command, &row, &col);
            reveal_status = 0;
            mark_status = 0;
            if (scan_result != 3 || (command != 'r' && command != 'm')) {
                printf("Invalid input. Please follow the format 'r/m row column'.\n");
            } else {
                if (command == 'r') {
                    reveal_status = reveal_field(board, revealed, marked, n, row - 1, col - 1);
                } else {
                    mark_status = mark_field(revealed, marked, n, row - 1, col - 1);
                }
            }
        } while (!reveal_status && !mark_status);

        show_board(board, revealed, marked, n);

        // check win/lose condition
        int hidden_fields_count = n * n;
        for (int i = 0; i < n * n; ++i) {
            hidden_fields_count -= revealed[i];
        }
        if (reveal_status == 2 || hidden_fields_count == mine_count) {
            playing = 0;
        }
    }

    if (reveal_status == 2) {
        printf("(%d, %d) was a bomb!\n", row, col);
        print_you_lost();
    } else {
        print_you_won();
    }

    for (int i = 0; i < n * n; ++i) {
        revealed[i] = 1;
    }
    show_board(board, revealed, marked, n);

    free(revealed);
    free(marked);
    free(board);

    printf("\nPress x to exit...\n");
    while (getchar() != 'x') {}

    return 0;
}

int mark_field(const int *revealed, int *marked, int n, int row, int col) {
    int pos = COORD_POS(row, col, n);
    if (row >= n || col >= n || row < 0 || col < 0) {
        printf("(%d, %d) is not on the Board!\n", row + 1, col + 1);
        return 0;
    }
    if (revealed[pos]) {
        printf("(%d, %d) is revealed. You can't mark a revealed Field!\n", row + 1, col + 1);
        return 0;
    }
    marked[pos] = !marked[pos];
    return 1;
}

int reveal_field(char **board, int *revealed, int *marked, int n, int row, int col) {
    int pos = COORD_POS(row, col, n);
    if (marked[pos]) {
        printf("You can't reveal (%d, %d), because it is marked.\n", row + 1, col + 1);
        return 0;
    }

    if (row < n && col < n && row >= 0 && col >= 0 && !revealed[pos]) {
        switch (board[row][col]) {
            case '*':
                revealed[pos] = 1;
                return 2;
            case ' ':
                revealed[pos] = 1;
                // recursively reveal all neighbours if field is empty
                for (int r = MAX(0, row - 1); r <= MIN(n - 1, row + 1); ++r) {
                    for (int c = MAX(0, col - 1); c <= MIN(n - 1, col + 1); ++c) {
                        if ((r != row || c != col) && !revealed[r * n + c]) {
                            reveal_field(board, revealed, marked, n, r, c);
                        }
                    }
                }
                return 1;
            default:
                revealed[pos] = 1;
                return 1;
        }
    }

    if (row < n && col < n && row >= 0 && col >= 0) {
        printf("You can't reveal (%d, %d), because it is already revealed!\n", row + 1, col + 1);
    } else {
        printf("You can't reveal (%d, %d), because it is not on the board!\n", row + 1, col + 1);
    }
    return 0;
}

void show_board(char **board, const int *revealed, const int *marked, int n) {
    for (int i = 0; i < n * 2 + 4; ++i) {
        printf("-");
    }
    printf("\n");
    printf("   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9) {
            printf("  ");
        } else {
            printf(" 1");
        }
    }
    printf("\n   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9) {
            printf(" %d", i + 1);
        } else {
            printf(" %d", i - 9);
        }
    }
    printf("\n");
    for (int i = 0; i < n; ++i) {
        if (i < 9) {
            printf(" ");
        }
        printf("%d:|", i + 1);
        for (int j = 0; j < n; ++j) {
            if (revealed[i * n + j]) {
                printf("%c|", board[i][j]);
            } else if (marked[i * n + j]) {
                printf("#|");
            } else {
                printf("-|");
            }
        }
        printf("\n");
    }
    printf("   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9) {
            printf(" %d", i + 1);
        } else {
            printf(" %d", i - 9);
        }
    }
    printf("\n   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9) {
            printf("  ");
        } else {
            printf(" 1");
        }
    }
    printf("\n");
    for (int i = 0; i < n * 2 + 4; ++i) {
        printf("-");
    }
    printf("\n");
}

void fill_board(char **board, int n, int mine_count) {
    // place bombs at random positions
    srand(time(NULL));
    int mines[mine_count];
    for (int i = 0; i < mine_count; ++i) {
        int random_number;
        do {
            random_number = rand() % (n * n) + 1;
        } while (is_duplicate(random_number, mines, mine_count));
        mines[i] = random_number;
        int row = (int) ceil((double) random_number / (double) n) - 1;
        int col = (random_number - 1) % n;
        board[row][col] = '*';
    }

    // calculate neighbour bomb count for non-bomb fields
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int bomb_count = 0;
            if (board[i][j] != '*') {
                // calculate valid neighbour positions
                int row_min = MAX(0, i - 1);
                int row_max = MIN(n - 1, i + 1);
                int col_min = MAX(0, j - 1);
                int col_max = MIN(n - 1, j + 1);
                for (int r = row_min; r <= row_max; ++r) {
                    for (int c = col_min; c <= col_max; ++c) {
                        if ((r != i || c != j) && board[r][c] == '*') {
                            bomb_count++;
                        }
                    }
                }
                if (bomb_count > 0) {
                    board[i][j] = '0' + bomb_count;
                } else {
                    board[i][j] = ' ';
                }
            }
        }
    }
}

char **create_empty_board(int n) {
    char *values = calloc(n * n, sizeof(char));
    char **rows = malloc(n * sizeof(char *));
    for (int i = 0; i < n; ++i) {
        rows[i] = values + i * n;
    }
    return rows;
}

int is_duplicate(int value, const int *arr, int size) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == value) {
            return 1;
        }
    }
    return 0;
}

int mine_count_input(int n) {
    int mine_count;
    int mine_limit = ceil((double) (n * n) * 0.25);
    do {
        char input[10];
        printf("Please enter the number of mines! (MIN: 5, MAX: %d)\n", mine_limit);
        fgets(input, sizeof(input), stdin);
        char *endptr;
        mine_count = strtol(input, &endptr, 10);
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Invalid input. Please input a valid number.\n");
        } else {
            if (mine_count < 5 || mine_count > mine_limit) {
                printf("%d is not a valid mine count!\n", mine_count);
            }
        }
    } while (mine_count < 5 || mine_count > mine_limit);
    return mine_count;
}

int board_size_input() {
    int n;
    do {
        char input[10];
        printf("Please enter the size of your board! (MIN: 5, MAX: 15)\n");
        fgets(input, sizeof(input), stdin);
        char *endptr;
        n = strtol(input, &endptr, 10);
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Invalid input. Please input a valid number.\n");
        } else {
            if (n < 5 || n > 15) {
                printf("%d is not a valid boardsize!\n", n);
            }
        }
    } while (n < 5 || n > 15);
    return n;
}

void print_logo() {
    printf(" ____________________________________________________________\n");
    printf(" |  \\/  (_)                                                   \n");
    printf(" | .  . |_ _ __   ___  _____      _____  ___ _ __   ___ _ __  \n");
    printf(" | |\\/| | | '_ \\ / _ \\/ __\\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__| \n");
    printf(" | |  | | | | | |  __/\\__ \\\\ V  V /  __/  __/ |_) |  __/ |    \n");
    printf(" \\_|  |_/_|_| |_|\\___||___/ \\_/\\_/ \\___|\\___| .__/ \\___|_|    \n");
    printf("                                            | |               \n");
    printf("____________________________________________|_|______________\n\n");
}

void print_you_won() {
    printf("\n__   __                                  _  \n");
    printf("\\ \\ / /                                 | | \n");
    printf(" \\ V /___  _   _  __      _____  _ __   | | \n");
    printf("  \\ // _ \\| | | | \\ \\ /\\ / / _ \\| '_ \\  | | \n");
    printf("  | | (_) | |_| |  \\ V  V / (_) | | | | |_| \n");
    printf("  \\_/\\___/ \\__,_|   \\_/\\_/ \\___/|_| |_| (_) \n\n");
}

void print_you_lost() {
    printf("\n__   __             _           _     _  \n");
    printf("\\ \\ / /            | |         | |   | | \n");
    printf(" \\ V /___  _   _   | | ___  ___| |_  | | \n");
    printf("  \\ // _ \\| | | |  | |/ _ \\/ __| __| | | \n");
    printf("  | | (_) | |_| |  | | (_) \\__ \\ |_  |_| \n");
    printf("  \\_/\\___/ \\__,_|  |_|\\___/|___/\\__| (_) \n\n");
}
