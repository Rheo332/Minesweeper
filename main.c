#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

char **createEmptyBoard(int n);

void fillBoard(char **board, int n, int mineCount);

void showBoard(char **board, const int *revealed, const int *marked, int n);

int isDuplicate(int value, const int *arr, int size);

int revealField(char **board, int *revealed, int *marked, int n, int row, int col);

int markField(const int *revealed, int *marked, int n, int row, int col);

void printLogo();

void printWinner();

void printLoser();

int boardSizeInput();

int mineCountInput(int n);

int main() {
    printLogo();

    int n = boardSizeInput();
    int mineCount = mineCountInput(n);

    int *revealed = calloc(n * n, sizeof(int)); // 0 = not revealed, 1 = revealed
    int *marked = calloc(n * n, sizeof(int));   // 0 = not marked, 1 = marked
    char **board = createEmptyBoard(n);
    fillBoard(board, n, mineCount);
    showBoard(board, revealed, marked, n);

    int row;
    int col;
    int playing = 1;
    int revealStatus;
    int markStatus;
    while (playing) {
        do {
            char input[10];
            char command;
            printf("\nPlease enter the field you want to reveal(r) or mark/unmark(m) (command row column) e.g: 'r 2 5':\n");
            fgets(input, sizeof(input), stdin);
            int scanResult = sscanf(input, "%c %d %d", &command, &row, &col);
            revealStatus = 0;
            markStatus = 0;
            if (scanResult != 3 || (command != 'r' && command != 'm')) {
                printf("Invalid input. Please follow the format 'r/m row column'.\n");
            } else {
                if (command == 'r')
                    revealStatus = revealField(board, revealed, marked, n, row - 1, col - 1);
                else
                    markStatus = markField(revealed, marked, n, row - 1, col - 1);
            }
        } while (!revealStatus && !markStatus);

        showBoard(board, revealed, marked, n);

        // check win/lose condition
        int hiddenFieldsCount = n * n;
        for (int i = 0; i < n * n; ++i) {
            hiddenFieldsCount -= revealed[i];
        }
        if (revealStatus == 2 || hiddenFieldsCount <= mineCount)
            playing = 0;
    }

    if (revealStatus == 2) {
        printf("(%d, %d) was a bomb!\n", row, col);
        printLoser();
    } else {
        printWinner();
    }

    for (int i = 0; i < n * n; ++i) {
        revealed[i] = 1;
    }
    showBoard(board, revealed, marked, n);

    free(revealed);
    free(board);

    printf("\nPress x to exit...\n");
    while (getchar() != 'x');

    return 0;
}

int markField(const int *revealed, int *marked, int n, int row, int col) {
    if (row >= n || col >= n || row < 0 || col < 0) {
        printf("(%d, %d) is not on the Board!\n", row + 1, col + 1);
        return 0;
    }
    if (revealed[row * n + col]) {
        printf("(%d, %d) is revealed. You can't mark a revealed Field!\n", row + 1, col + 1);
        return 0;
    }
    marked[row * n + col] = !marked[row * n + col];
    return 1;
}

int revealField(char **board, int *revealed, int *marked, int n, int row, int col) {
    if (marked[row * n + col]) {
        printf("You can't reveal (%d, %d), because it is marked.\n", row + 1, col + 1);
        return 0;
    }

    if (row < n && col < n && row >= 0 && col >= 0 && !revealed[row * n + col]) {
        switch (board[row][col]) {
            case '*':
                revealed[row * n + col] = 1;
                return 2;
            case ' ':
                revealed[row * n + col] = 1;
                // recursively reveal all neighbours if field is empty
                for (int r = max(0, row - 1); r <= min(n - 1, row + 1); ++r) {
                    for (int c = max(0, col - 1); c <= min(n - 1, col + 1); ++c) {
                        if (r == row && c == col) continue;
                        if (!revealed[r * n + c])
                            revealField(board, revealed, marked, n, r, c);
                    }
                }
                return 1;
            default:
                revealed[row * n + col] = 1;
                return 1;
        }
    }

    if (row < n && col < n && row >= 0 && col >= 0)
        printf("You can't reveal (%d, %d), because it is already revealed!\n", row + 1, col + 1);
    else
        printf("You can't reveal (%d, %d), because it is not on the board!\n", row + 1, col + 1);
    return 0;
}

void showBoard(char **board, const int *revealed, const int *marked, int n) {
    for (int i = 0; i < n * 2 + 4; ++i) {
        printf("-");
    }
    printf("\n");
    printf("   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9)
            printf("  ");
        else
            printf(" 1");
    }
    printf("\n   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9)
            printf(" %d", i + 1);
        else
            printf(" %d", i - 9);
    }
    printf("\n");
    for (int i = 0; i < n; ++i) {
        if (i < 9) printf(" ");
        printf("%d:|", i + 1);
        for (int j = 0; j < n; ++j) {
            if (revealed[i * n + j])
                printf("%c|", board[i][j]);
            else if (marked[i * n + j])
                printf("#|");
            else
                printf("-|");
        }
        printf("\n");
    }
    printf("   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9)
            printf(" %d", i + 1);
        else
            printf(" %d", i - 9);
    }
    printf("\n   ");
    for (int i = 0; i < n; ++i) {
        if (i < 9)
            printf("  ");
        else
            printf(" 1");
    }
    printf("\n");
    for (int i = 0; i < n * 2 + 4; ++i) {
        printf("-");
    }
    printf("\n");
}

void fillBoard(char **board, int n, int mineCount) {
    // place bombs at random positions
    srand(time(NULL));
    int mines[mineCount];
    for (int i = 0; i < mineCount; ++i) {
        int randomNumber;
        do {
            randomNumber = rand() % (n * n) + 1;
        } while (isDuplicate(randomNumber, mines, mineCount));
        mines[i] = randomNumber;
        int row = (int) ceil((double) randomNumber / (double) n) - 1;
        int col = (randomNumber - 1) % n;
        board[row][col] = '*';
    }

    // calculate neighbour bomb count for non-bomb fields
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int bombCount = 0;
            if (board[i][j] != '*') {
                // calculate valid neighbour positions
                int rowMin = max(0, i - 1);
                int rowMax = min(n - 1, i + 1);
                int colMin = max(0, j - 1);
                int colMax = min(n - 1, j + 1);
                for (int r = rowMin; r <= rowMax; ++r) {
                    for (int c = colMin; c <= colMax; ++c) {
                        if (r == i && c == j)
                            continue;
                        if (board[r][c] == '*')
                            bombCount++;
                    }
                }
                if (bombCount > 0)
                    board[i][j] = '0' + bombCount;
                else
                    board[i][j] = ' ';
            }
        }
    }
}

char **createEmptyBoard(int n) {
    char *values = calloc(n * n, sizeof(char));
    char **rows = malloc(n * sizeof(char *));
    for (int i = 0; i < n; ++i) {
        rows[i] = values + i * n;
    }
    return rows;
}

int isDuplicate(int value, const int *arr, int size) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == value) return 1;
    }
    return 0;
}

int mineCountInput(int n) {
    int mineCount;
    int mineLimit = ceil((double) (n * n) * 0.25);

    do {
        char input[10];
        printf("Please enter the number of mines! (min: 5, max: %d)\n", mineLimit);
        fgets(input, sizeof(input), stdin);
        char *endptr;
        mineCount = strtol(input, &endptr, 10);
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Invalid input. Please input a valid number.\n");
        } else {
            if (mineCount < 5 || mineCount > mineLimit) printf("%d is not a valid mine count!\n", mineCount);
        }
    } while (mineCount < 5 || mineCount > mineLimit);

    return mineCount;
}

int boardSizeInput() {
    int n;

    do {
        char input[10];
        printf("Please enter the size of your board! (min: 5, max: 15)\n");
        fgets(input, sizeof(input), stdin);
        char *endptr;
        n = strtol(input, &endptr, 10);
        if (*endptr != '\0' && *endptr != '\n') {
            printf("Invalid input. Please input a valid number.\n");
        } else {
            if (n < 5 || n > 15) printf("%d is not a valid boardsize!\n", n);
        }
    } while (n < 5 || n > 15);

    return n;
}

void printLogo() {
    printf(" ____________________________________________________________\n");
    printf(" |  \\/  (_)                                                   \n");
    printf(" | .  . |_ _ __   ___  _____      _____  ___ _ __   ___ _ __  \n");
    printf(" | |\\/| | | '_ \\ / _ \\/ __\\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__| \n");
    printf(" | |  | | | | | |  __/\\__ \\\\ V  V /  __/  __/ |_) |  __/ |    \n");
    printf(" \\_|  |_/_|_| |_|\\___||___/ \\_/\\_/ \\___|\\___| .__/ \\___|_|    \n");
    printf("                                            | |               \n");
    printf("____________________________________________|_|______________\n\n");
}

void printWinner() {
    printf("\n__   __                                  _  \n");
    printf("\\ \\ / /                                 | | \n");
    printf(" \\ V /___  _   _  __      _____  _ __   | | \n");
    printf("  \\ // _ \\| | | | \\ \\ /\\ / / _ \\| '_ \\  | | \n");
    printf("  | | (_) | |_| |  \\ V  V / (_) | | | | |_| \n");
    printf("  \\_/\\___/ \\__,_|   \\_/\\_/ \\___/|_| |_| (_) \n\n");
}

void printLoser() {
    printf("\n__   __             _           _     _  \n");
    printf("\\ \\ / /            | |         | |   | | \n");
    printf(" \\ V /___  _   _   | | ___  ___| |_  | | \n");
    printf("  \\ // _ \\| | | |  | |/ _ \\/ __| __| | | \n");
    printf("  | | (_) | |_| |  | | (_) \\__ \\ |_  |_| \n");
    printf("  \\_/\\___/ \\__,_|  |_|\\___/|___/\\__| (_) \n\n");
}
