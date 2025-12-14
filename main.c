#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> //for usleep()
#include <locale.h> //for unicode characters
#include <time.h>   //for random numbers


#define WIDTH 250
#define HEIGHT 35


#define NUM_DIRS 8
#define NUM_DIFF 2
//                          N        NE       E       SE      S       SW       W        NW
#define DEFAULT_DIRECTIONS {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}}

enum DIFFS{
  DROW = 0,
  DCOL = 1
};

enum SYMS{
  DEAD = 0,
  ALIVE = 1
};

static inline unsigned char *CELL(unsigned char *buf, int row, int col, int width){
  return buf + (row * width) + col;
}

int validCell(int row, int col, int width, int height){
  return (row >= 0 && row < height && col >= 0 && col < width);
}

int getNeighbors(unsigned char* current, int row, int col, int width, int height){
  int directions[NUM_DIRS][NUM_DIFF] = DEFAULT_DIRECTIONS;
  int neighbors = 0;
  for (int dir = 0; dir<NUM_DIRS; dir++){
    int nrow = row + directions[dir][DROW];
    int ncol = col + directions[dir][DCOL];
    if (validCell(nrow, ncol, width, height)){
      if (*CELL(current, nrow, ncol, width) == ALIVE) neighbors++;
    }
  }
  return neighbors;
}

void render(unsigned char *buf, int width, int height){
  if (!buf) exit(EXIT_FAILURE);
  for (int i = 0; i < width * height; i++){
    if (i%width == 0) putchar('\n');
    if (buf[i] == ALIVE) printf("\u2588");
    else putchar(' ');
  }
  putchar('\n');
}

void applyLogic(unsigned char *current, unsigned char *output, int width, int height){
  if (!current || !output) exit(EXIT_FAILURE);

  for (int row = 0; row<HEIGHT; row++){
    for (int col = 0; col<WIDTH; col++){

      unsigned char currentCell = *CELL(current, row, col, width);
      int neighbors = getNeighbors(current, row, col, width, height);

      if (currentCell == ALIVE)
        *CELL(output, row, col, width) = (neighbors < 2 || neighbors > 3) ? DEAD : ALIVE;
      else if (currentCell == DEAD)
        *CELL(output, row, col, width) = (neighbors == 3) ? ALIVE : DEAD;

    }
  }
}

void startRandom(unsigned char *buf, int numAlive, int width, int height){
  for (int _ = 0; _<numAlive; _++){
    int row = rand() % height;
    int col = rand() % width;
    *CELL(buf, row, col, width) = ALIVE;
  }
}

int main(void){
  setlocale(LC_ALL, "");
  srand(time(NULL));

  unsigned char *outputState = calloc(WIDTH*HEIGHT, sizeof(unsigned char));

  if (!outputState){
    fprintf(stderr, "Error allocating outputState\n");
    exit(EXIT_FAILURE);
  }
  unsigned char *currentState = calloc(WIDTH*HEIGHT, sizeof(unsigned char));
  if (!currentState){
    fprintf(stderr, "Error allocating currentState\n");
    free(outputState);
    exit(EXIT_FAILURE);
  }

  startRandom(currentState, WIDTH*HEIGHT/4, WIDTH, HEIGHT);

  struct timespec ts;
  ts.tv_sec=0;
  ts.tv_nsec= 200000000;
  

  while(1){
    printf("\x1b[2J\x1b[H");
    render(currentState, WIDTH, HEIGHT);
    applyLogic(currentState, outputState, WIDTH, HEIGHT);
    for (int i = 0; i < WIDTH*HEIGHT; i++){
      currentState[i] = outputState[i];
      outputState[i] = 0;
    }
    nanosleep(&ts, NULL);
    
  }
  

  free(outputState);
  free(currentState);


  return 0;
}
