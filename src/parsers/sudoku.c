/*
 * Copyright (C) 2021  h3nnn4n, aka Renan S. Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_utils.h"
#include "generators/sudoku.h"
#include "parsers/sudoku.h"
#include "strtok.h"

void sudoku_parse(char *input) {
    const char separator[2]     = " ";
    uint16_t   loaded_from_file = 0;
    char *     token_data       = NULL;
    char *     data             = NULL;
    size_t     data_len         = 0;

    char sudoku_solution[ROW_LENGTH][COLUMN_LENGTH] = {0};

    if (file_exists(input)) {
        FILE *f = fopen(input, "rt");
        readall(f, &data, &data_len);
        fclose(f);
        loaded_from_file = 1;
    } else {
        data     = input;
        data_len = strlen(data);
    }

    char *token = strtok_r(data, separator, &token_data);

    while (token != NULL) {
        uint16_t values[4];

        for (int i = 0; i < 4; i++) {
            do {
                values[i] = atoi(token);
                token     = strtok_r(NULL, separator, &token_data);
            } while (token != NULL && values[i] == 0);

            if (token == NULL) {
                break;
            }
        }

        if (token == NULL) {
            break;
        }

        uint16_t position = values[0] - 1;
        uint16_t row      = position % ROW_LENGTH;
        uint16_t column   = position / ROW_LENGTH;
        uint16_t value    = (values[1] - 1) % ROW_LENGTH + 1;

        assert(row < ROW_LENGTH);
        assert(column < COLUMN_LENGTH);

        assert(value > 0);
        assert(value <= MAX_VALUE);

        sudoku_solution[row][column] = value;
    }

    for (int row = 0; row < ROW_LENGTH; row++) {
        for (int col = 0; col < COLUMN_LENGTH; col++) {
            printf("%d ", sudoku_solution[row][col]);
            if ((col + 1) % GRID_SIZE == 0 && col > 0) {
                printf(" ");
            }
        }
        printf("\n");

        if ((row + 1) % GRID_SIZE == 0 && row > 0) {
            printf("\n");
        }
    }

    sudoku_check(sudoku_solution);

    if (loaded_from_file) {
        free(data);
    }
}

void sudoku_check(char sudoku_solution[ROW_LENGTH][COLUMN_LENGTH]) {
    int row_check[ROW_LENGTH][COLUMN_LENGTH] = {0};
    int col_check[ROW_LENGTH][COLUMN_LENGTH] = {0};
    int value_count_check[MAX_VALUE]         = {0};

    for (int row = 0; row < ROW_LENGTH; row++) {
        for (int col = 0; col < COLUMN_LENGTH; col++) {
            int value = sudoku_solution[row][col];
            value_count_check[value - 1] += 1;

            if (row_check[row][value - 1] != 0) {
                fprintf(stderr, "Sudoku row check failed on %d %d %d\n", row, col, value);
                abort();
            } else {
                row_check[row][value - 1] = value;
            }

            if (col_check[col][value - 1] != 0) {
                fprintf(stderr, "Sudoku column check failed on %d %d %d\n", row, col, value);
                abort();
            } else {
                col_check[col][value - 1] = value;
            }
        }
    }

    // I think this might be redundant :thinking:
    for (int value_i = 0; value_i < MAX_VALUE; value_i++) {
        if (value_count_check[value_i] != MAX_VALUE) {
            fprintf(stderr, "Sudoku value count check failed on %d\n", value_i + 1);
            abort();
        }
    }
}