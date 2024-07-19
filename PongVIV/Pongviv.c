#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define DELAY 30000
#define PADDLE_HEIGHT 4
#define PADDLE_WIDTH 1
#define BALL_SYMBOL "O"

typedef struct {
  int x, y;
} Position;

typedef struct {
  Position pos;
  int height, width;
} Paddle;

typedef struct {
  Position pos;
  int x_velocity, y_velocity;
} Ball;

void InitializePaddle(Paddle* paddle, int x, int y) {
  paddle->pos.x = x;
  paddle->pos.y = y;
  paddle->height = PADDLE_HEIGHT;
  paddle->width = PADDLE_WIDTH;
}

void InitializeBall(Ball* ball, int x, int y, int x_vel, int y_vel) {
  ball->pos.x = x;
  ball->pos.y = y;
  ball->x_velocity = x_vel;
  ball->y_velocity = y_vel;
}

void DrawPaddle(Paddle* paddle) {
  for (int i = 0; i < paddle->height; i++) {
    mvprintw(paddle->pos.y + i, paddle->pos.x, "|");
  }
}

void DrawBall(Ball* ball) {
  mvprintw(ball->pos.y, ball->pos.x, BALL_SYMBOL);
}

void MovePaddle(Paddle* paddle, int direction) {
  paddle->pos.y += direction;
}

void UpdateBall(Ball* ball, Paddle* left_paddle, Paddle* right_paddle, int max_y, int max_x) {
  ball->pos.x += ball->x_velocity;
  ball->pos.y += ball->y_velocity;

  if (ball->pos.y <= 0 || ball->pos.y >= max_y) {
    ball->y_velocity = -ball->y_velocity;
  }

  if (ball->pos.x <= left_paddle->pos.x + PADDLE_WIDTH &&
      ball->pos.y >= left_paddle->pos.y &&
      ball->pos.y <= left_paddle->pos.y + PADDLE_HEIGHT) {
    ball->x_velocity = -ball->x_velocity;
  }

  if (ball->pos.x >= right_paddle->pos.x - PADDLE_WIDTH &&
      ball->pos.y >= right_paddle->pos.y &&
      ball->pos.y <= right_paddle->pos.y + PADDLE_HEIGHT) {
    ball->x_velocity = -ball->x_velocity;
  }

  if (ball->pos.x <= 0 || ball->pos.x >= max_x) {
    ball->pos.x = max_x / 2;
    ball->pos.y = max_y / 2;
    ball->x_velocity = -ball->x_velocity;
  }
}

int main() {
  int max_x, max_y;
  Paddle left_paddle, right_paddle;
  Ball ball;

  initscr();
  noecho();
  curs_set(FALSE);
  timeout(0);

  getmaxyx(stdscr, max_y, max_x);

  InitializePaddle(&left_paddle, 1, max_y / 2 - PADDLE_HEIGHT / 2);
  InitializePaddle(&right_paddle, max_x - 2, max_y / 2 - PADDLE_HEIGHT / 2);
  InitializeBall(&ball, max_x / 2, max_y / 2, 1, 1);

  while (1) {
    clear();
    DrawPaddle(&left_paddle);
    DrawPaddle(&right_paddle);
    DrawBall(&ball);
    refresh();

    usleep(DELAY);

    int ch = getch();
    if (ch == 'q') break;

    if (ch == 'w' && left_paddle.pos.y > 0) {
      MovePaddle(&left_paddle, -1);
    } else if (ch == 's' && left_paddle.pos.y + PADDLE_HEIGHT < max_y) {
      MovePaddle(&left_paddle, 1);
    }

    if (ch == 'o' && right_paddle.pos.y > 0) {
      MovePaddle(&right_paddle, -1);
    } else if (ch == 'l' && right_paddle.pos.y + PADDLE_HEIGHT < max_y) {
      MovePaddle(&right_paddle, 1);
    }

    UpdateBall(&ball, &left_paddle, &right_paddle, max_y, max_x);
  }

  endwin();
  return 0;
}
