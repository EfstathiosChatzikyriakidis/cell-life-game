/*
 *  Cell Life Game is a classic Artificial Zero-Player Life Game.
 *
 *  Copyright (C) 2008 Efstathios Chatzikyriakidis (contact@efxa.org)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* headers. */

/* standard headers. */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

/* defines. */

/* default colony's rows, cols and gens value. */
#define DEFAULT_VALUE 10

/* the random numbers generator filename. */
#define RANDOM_FILE "/dev/urandom"

/* special characters. */
#define NEW_LINE "\n"
#define TAB_CHAR "  "

/* typedefs. */

/* boolean data type by using enumeration. */
typedef enum bool { false, true } bool;

/* const pointer to const char. */
typedef const char *const unique;

/* const integer. */
typedef const int cint;

/* const boolean. */
typedef const bool cbool;

/* global variables. */

/* program messages. */
static unique package_and_vers_txt = "clg (Cell life game) 1.0";
static unique input_cols_value_txt = "Number of columns (c>0): ";
static unique input_rows_value_txt = "Number of rows (r>0): ";
static unique input_gens_value_txt = "Number of generations (g>0): ";
static unique init_rand_colony_txt = "The initial colony:";
static unique evolu_gen_number_txt = "Generation: ";

/* colony dimensions, and generations. */
int MAX_COLS, MAX_ROWS, MAX_GENS;

/* function prototypes. */

/* non-void arguments functions. */
static bool copy_colony_values (bool * a_f[], bool * a_t[]);

static bool cell_lives_or_not (bool * a[], cint x, cint y);

static bool cell_is_limited (cint x, cint y);
static bool cell_is_corner (cint x, cint y);

static int colony_evolution (bool * a[], cint max_gen);
static int set_colony_value (bool * a[], cbool init_value);

static int display_colony (bool * a[]);
static int set_colony_random (bool * a[]);

static bool check_conditions (bool * a[],
                              cint x,
                              cint y,
                              bool (*cons_func) (bool * colony[], cint x, cint y));

static bool cons_lim_cells (bool * a[], cint x, cint y);
static bool cons_sim_cells (bool * a[], cint x, cint y);

static bool top_left_place (cint x, cint y);
static bool top_right_place (cint x, cint y);
static bool bottom_left_place (cint x, cint y);
static bool bottom_right_place (cint x, cint y);

static bool top_center_place (cint x, cint y);
static bool right_center_place (cint x, cint y);
static bool left_center_place (cint x, cint y);
static bool bottom_center_place (cint x, cint y);

/* void arguments functions. */
static bool random_bool (void);

/* functions. */

/* main function. */
int
main (void)
{
  /* is used as a loop counter. */
  int i;

  /* the colony (two-dimensional array). */
  bool **colony;

  /* print the program name and version. */
  fprintf (stderr, "%s", package_and_vers_txt);

  /* leave two blank lines. */
  fprintf (stderr, "%s%s", NEW_LINE, NEW_LINE);

  /* read columns from the input. */
  fprintf (stderr, "%s", input_cols_value_txt);
  scanf ("%d", &MAX_COLS);

  /* read rows from the input. */
  fprintf (stderr, "%s", input_rows_value_txt);
  scanf ("%d", &MAX_ROWS);

  /* read evolution generations. */
  fprintf (stderr, "%s", input_gens_value_txt);
  scanf ("%d", &MAX_GENS);

  /* if any is zero or negative set it default value. */
  if (MAX_ROWS <= 0)
    MAX_ROWS = DEFAULT_VALUE;
  if (MAX_COLS <= 0)
    MAX_COLS = DEFAULT_VALUE;
  if (MAX_GENS <= 0)
    MAX_GENS = DEFAULT_VALUE;

  /* allocate `MAX_ROWS' pointers to `bool' data type memory space. */
  colony = (bool **) malloc (MAX_ROWS * (sizeof (bool *)));

  /* for each row allocate `MAX_COLS' `bool' data type memory space. */
  for (i = 0; i < MAX_ROWS; i++)
    colony[i] = (bool *) malloc (MAX_COLS * sizeof (bool));

  /* init the colony with false (none lives). */
  set_colony_value (colony, false);

  /* try to have a random colony. */
  set_colony_random (colony);

  /* print the initial colony message. */
  printf ("%s", init_rand_colony_txt);

  /* leave two blank lines. */
  printf ("%s%s", NEW_LINE, NEW_LINE);

  /* display the initial colony. */
  display_colony (colony);

  /* run evolution with `MAX_GENS' generations. */
  colony_evolution (colony, MAX_GENS);

  /* free each row of the colony. */
  for (i = MAX_ROWS - 1; i >= 0; i--)
    free (colony[i]);

  /* free the colony. */
  free (colony);

  return false;
}

/* the following function tries
   to see if a cell is limited. */
static bool
cell_is_limited (cint x, cint y)
{
  /* check to see if a cell is limited. */
  if (x == 0 || y == 0 || x == MAX_ROWS - 1 || y == MAX_COLS - 1)
    /* it's limited. */
    return true;
  else
    /* it's not limited. */
    return false;
}

/* the following function tries
   to see if a cell is a corner. */
static bool
cell_is_corner (cint x, cint y)
{
  /* check to see if a cell is a corner. */
  if ((x == MAX_ROWS - 1 && y == 0) ||
      (x == 0 && y == MAX_COLS - 1) ||
      (x == 0 && y == 0)            || (x == MAX_ROWS - 1 && y == MAX_COLS - 1))
    return true;
  else
    return false;
}

/* the following function displays the colony. */
static int
display_colony (bool * colony[])
{
  /* array indexes. */
  register int x, y;

  /* parse the array. */
  for (x = 0; x < MAX_ROWS; x++)
    {
      /* insert tab space. */
      printf (TAB_CHAR);

      for (y = 0; y < MAX_COLS; y++)
        {
          /* print the current cell. */
          if (y == MAX_COLS-1)
            printf ("%d", colony[x][y]);
          else
            printf ("%d ", colony[x][y]);
        }

      /* when we reach the eol print a new line char. */
      printf (NEW_LINE);
    }

  /* return without problem. */
  return false;
}

/* the following function tries to init a random colony. */
static int
set_colony_random (bool * colony[])
{
  /* array indexes. */
  register int x, y;

  /* parse the array. */
  for (x = 0; x < MAX_ROWS; x++)
    for (y = 0; y < MAX_COLS; y++)
      /* put a random boolean to the current cell. */
      colony[x][y] = random_bool ();

  /* return without problem. */
  return false;
}

/* the following function tries to init the colony with `init_value'. */
static int
set_colony_value (bool * colony[], cbool init_value)
{
  /* array indexes. */
  register int x, y;

  /* parse the array. */
  for (x = 0; x < MAX_ROWS; x++)
    for (y = 0; y < MAX_COLS; y++)
      /* put the `init_value' to the colony cells. */
      colony[x][y] = init_value;

  /* return without problem. */
  return false;
}

/* the following function tries to copy
   all values from `a_from' to `a_to'. */
static bool
copy_colony_values (bool * c_from[], bool * c_to[])
{
  /* array indexes. */
  register int x, y;

  /* copy all values from `c_from' to `c_to'. */
  for (x = 0; x < MAX_ROWS; x++)
    for (y = 0; y < MAX_COLS; y++)
      c_to[x][y] = c_from[x][y];

  /* return without problem. */
  return false;
}

/* the following function runs the evolution. */
static int
colony_evolution (bool * colony[], cint max_gen)
{
  /* the temp colony (two-dimensional array). */
  bool **temp;

  /* array indexes, counters. */
  register int i, g, x, y, gen_num;

  /* allocate `rows' pointers to `bool' data type memory space. */
  temp = (bool **) malloc (MAX_ROWS * (sizeof (bool *)));

  /* for each row allocate `cols' `bool' data type memory space. */
  for (i = 0; i < MAX_ROWS; i++)
    temp[i] = (bool *) malloc (MAX_COLS * sizeof (bool));

  /* init the temp colony with false. */
  set_colony_value (temp, false);

  /* parse the array `max_gen' generations. */
  for (g = 0, gen_num = 1; g < max_gen; g++, gen_num++)
    {
      /* print the generation message. */
      printf (NEW_LINE);
      printf ("%s%d", evolu_gen_number_txt, gen_num);
      printf ("%s%s", NEW_LINE, NEW_LINE);

      for (x = 0; x < MAX_ROWS; x++)
        for (y = 0; y < MAX_COLS; y++)
          /* decide the state of the current cell. */
          temp[x][y] = cell_lives_or_not (colony, x, y);

      /* copy all the values from `temp' to `colony'. */
      copy_colony_values (temp, colony);

      /* init the temp colony with false. */
      set_colony_value (temp, false);

      /* display the colony. */
      display_colony (colony);
    }

  /* free each row of the temp colony. */
  for (i = MAX_ROWS - 1; i >= 0; i--)
    free (temp[i]);

  /* free the temp colony. */
  free (temp);

  /* return without problem. */
  return false;
}

/* the following function decides the state of a cell. */
static bool
cell_lives_or_not (bool * colony[], cint x, cint y)
{
  /* the variable keeps the state
     of the cell after conditions. */
  bool cell_state;

  /* is the cell limited? */
  if (cell_is_limited (x, y))
    cell_state = check_conditions (colony, x, y, cons_lim_cells);
  else
    cell_state = check_conditions (colony, x, y, cons_sim_cells);

  /* return the state of the cell. */
  return cell_state;
}

/* the following function tries to return
   either false or true value, works with
   randomness. */
static bool
random_bool (void)
{
  /* extern function. */
  extern int random_number (cint min, cint max, unique filename);

  /* return either false or true. */
  return random_number (false, true, RANDOM_FILE);
}

/* the following function calls the appropriate
   condition checks and returns true or false. */
static bool
check_conditions (bool * colony[],
                  cint x,
                  cint y,
                  bool (*cons_func) (bool * colony[], cint x, cint y))
{
  return (*cons_func) (colony, x, y);
}

/* conditions for limited cells. */
static bool
cons_lim_cells (bool * colony[], cint x, cint y)
{
  /* keeps the max neighbours. */
  int neighbours = 0;

  if (cell_is_corner (x, y))
  {
    if (top_left_place (x, y))
      neighbours = colony[x + 1][y] + colony[x][y + 1] + colony[x + 1][y + 1];
    else if (top_right_place (x, y))
      neighbours = colony[x + 1][y] + colony[x][y - 1] + colony[x + 1][y - 1];
    else if (bottom_left_place (x, y))
      neighbours = colony[x - 1][y] + colony[x][y + 1] + colony[x - 1][y + 1];
    else if (bottom_right_place (x, y))
      neighbours = colony[x - 1][y] + colony[x][y - 1] + colony[x - 1][y - 1];

    return (neighbours < 2 || neighbours == 3) ? false : true;
  }
  else
  {
    if (top_center_place (x, y))
      neighbours = colony[x + 1][y] + colony[x][y + 1] + colony[x + 1][y + 1] + colony[x][y - 1] + colony[x + 1][y - 1];
    else if (left_center_place (x, y))
      neighbours = colony[x + 1][y] + colony[x][y + 1] + colony[x + 1][y + 1] + colony[x - 1][y] + colony[x - 1][y + 1];
    else if (bottom_center_place (x, y))
      neighbours = colony[x][y - 1] + colony[x][y + 1] + colony[x - 1][y - 1] + colony[x - 1][y] + colony[x - 1][y + 1];
    else if (right_center_place (x, y))
      neighbours = colony[x - 1][y] + colony[x + 1][y] + colony[x - 1][y - 1] + colony[x][y - 1] + colony[x + 1][y - 1];

    return (neighbours < 3 || neighbours == 5) ? false : true;
  }
}

/* conditions for cells (not limited). */
static bool
cons_sim_cells (bool * colony[], cint x, cint y)
{
  /* keeps the max neighbours. */
  int neighbours = 0;

  neighbours = colony[x][y + 1]     +
               colony[x][y - 1]     +
               colony[x + 1][y]     +
               colony[x - 1][y]     +
               colony[x + 1][y + 1] +
               colony[x - 1][y - 1] +
               colony[x - 1][y + 1] +
               colony[x + 1][y - 1];

  return (neighbours < 4 || neighbours == 8) ? false : true;
}

/* check to see if the cell is at the upper left place. */
static bool
top_left_place (cint x, cint y)
{
  return (x == 0 && y == 0) ? true : false;
}

/* check to see if the cell is at the upper right place. */
static bool
top_right_place (cint x, cint y)
{
  return (x == 0 && y == MAX_COLS - 1) ? true : false;
}

/* check to see if the cell is at the lower left place. */
static bool
bottom_left_place (cint x, cint y)
{
  return (x == MAX_ROWS - 1 && y == 0) ? true : false;
}

/* check to see if the cell is at the lower right place. */
static bool
bottom_right_place (cint x, cint y)
{
  return (x == MAX_ROWS - 1 && y == MAX_COLS - 1) ? true : false;
}

/* check to see if the cell is at the top center place. */
static bool
top_center_place (cint x, cint y)
{
  return (x == 0 && y > 0 && y < MAX_COLS - 1) ? true : false;
}

/* check to see if the cell is at the bottom center place. */
static bool
bottom_center_place (cint x, cint y)
{
  return (y > 0 && x == MAX_ROWS - 1 && y < MAX_COLS - 1) ? true : false;
}

/* check to see if the cell is at the left center place. */
static bool
left_center_place (cint x, cint y)
{
  return (y == 0 && x > 0 && x < MAX_ROWS - 1) ? true : false;
}

/* check to see if the cell is at the right center place. */
static bool
right_center_place (cint x, cint y)
{
  return (y == MAX_COLS - 1 && x > 0 && x < MAX_ROWS - 1) ? true : false;
}
