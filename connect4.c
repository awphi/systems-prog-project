//You can include any of headers defined in the C11 standard here. They are:
//assert.h, complex.h, ctype.h, errno.h, fenv.h, float.h, inttypes.h, iso646.h, limits.h, locale.h, math.h, setjmp.h, signal.h, stdalign.h, stdarg.h, stdatomic.h, stdbool.h, stddef.h, stdint.h, stdio.h, stdlib.h, stdnoreturn.h, string.h, tgmath.h, threads.h, time.h, uchar.h, wchar.h or wctype.h
//You may not include any other headers.
#include"connect4.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<ctype.h>

#define EUCMOD(a, b) (a < 0 ? (((a % b) + b) % b) : (a % b))
#define SIGN(x) ((x > 0) - (x < 0))

struct board_structure {
    int rows;
    int cols;
    char* data;
};

char* cell_wrapc(board u, int c, int r, bool wrapc) {
    if(wrapc && (c > 8 || c < 1)) {
        c = EUCMOD((c - 1), u->cols) + 1;
    }

    return &u->data[(r - 1) * u->cols + (c - 1)];
}

char* cell(board u, int c, int r) {
    return cell_wrapc(u, c, r, false);
}

board setup_board(){
    board b = malloc(sizeof(struct board_structure));
    return b;
}

void cleanup_board(board u) {
    free(u->data);
    free(u);
}

void read_in_file(FILE *infile, board u) {
    char buff;
    int c = 0, r = 0;

    while(fscanf(infile, "%c", &buff) != EOF) {
        if(buff == '\n') {
            u->cols = c;
            c = 0;
            r ++;
        } else {
            c ++;
        }
    }

    u->rows = r;
    u->data = calloc(u->cols * u->rows, sizeof(char));

    r = u->rows;
    c = 1;

    rewind(infile);

    while(fscanf(infile, "%c", &buff) != EOF) {
        if(buff == '\n') {
            r --;
            c = 1;
        } else {
            *cell(u, c, r) = buff;
            c ++;
        }
    }
}

void write_out_file(FILE *outfile, board u) {
    current_winner(u);
    for(int i = u->rows; i >= 1; i --) {
        for(int j = 1; j <= u->cols; j ++) {
            fprintf(outfile,"%c", *cell(u, j, i));
        }
        fprintf(outfile, "\n");
    }
}

char next_player(board u) {
    int x = 0, o = 0;

    for(int i = 1; i <= u->rows; i ++) {
        for(int j = 1; j <= u->cols; j ++) {
            char c = *cell(u, j, i);
            if(c == 'x') {
                x ++;
            } else if(c == 'o') {
                o ++;
            }
        }
    }

    return x == o ? 'x' : 'o';
}

char current_winner(board u) {
    int directions[4][2] = {
            {1, -1},
            {1, 0},
            {1, 1},
            {0, 1}
    };

    char* line[4];

    for(int d = 0; d < 4; d ++) {
        int dx = directions[d][0];
        int dy = directions[d][1];

        for(int i = 1; i <= u->cols; i ++) {
            for(int j = u->rows; j >= 1; j --) {
                int maxj = j + 3 * dy;
                if(maxj > u-> rows || maxj < 1) {
                    continue;
                }

                line[0] = cell(u, i, j);
                bool flag = false;

                if(*line[0] == '.') {
                    continue;
                }

                for(int k = 1; k < 4; k ++) {
                    line[k] = cell_wrapc(u, i + k * dx, j + k * dy, true);
                    if(*line[k] != *line[0]) {
                        flag = true;
                        break;
                    }
                }

                if(!flag) {
                    for(int k = 0; k < 4; k ++) {
                        *line[k] = toupper(*line[k]);
                    }

                    return *line[0];
                }
            }
        }
    }

    return '.';
}

struct move read_in_move(board u) {
    struct move m;
    printf("Player %c enter column to place your token: ", next_player(u)); //Do not edit this line
    scanf("%d", &m.column);
    printf("Player %c enter row to rotate: ", next_player(u)); // Do not edit this line
    scanf("%d", &m.row);

    return m;
}

int is_valid_move(struct move m, board u) {
    int oob = m.column > u->cols || m.column < 1 || abs(m.row) > u->rows;
    int col_full = *cell(u, m.column, u->rows) != '.';
    return !oob && !col_full;
}

char is_winning_move(struct move m, board u) {
    // Copy the board into a new struct, play the move, cache, free, return
    struct board_structure c = *u;
    play_move(m, &c);
    char w = current_winner(&c);
    free(c.data);
    free(&c);
    return w;
}

void rotate_row(board u, int r) {
    int abs_r = abs(r);
    int sign = SIGN(r);
    int c = sign == -1 ? 1 : u->cols;
    char temp = *cell(u, c, abs_r);

    while(c >= 1 && c <= u-> cols) {
        *cell(u, c, abs_r) = *cell(u, c - sign, abs_r);
        c -= sign;
    }

    *cell(u, c + sign, abs_r) = temp;
}

void drop(board u, int col, int row, char ch) {
    for(int i = row; i <= u->rows; i --) {
        if(i - 1 == 0 || *cell(u, col, i - 1) != '.') {
            *cell(u, col, i) = ch;
            break;
        }
    }
}

void play_move(struct move m, board u) {
    drop(u, m.column, u->rows, next_player(u));

    if(m.row != 0) {
        rotate_row(u, m.row);
        for(int i = 1; i <= u->cols; i ++) {
            for(int j = 1; j < u->rows; j ++) {
                char *cell_ptr = cell(u, i, j + 1);
                char cache = *cell_ptr;
                if(*cell(u, i, j) == '.' && cache != '.') {
                    *cell_ptr = '.';
                    drop(u, i, j, cache);
                }
            }
        }
    }
}
