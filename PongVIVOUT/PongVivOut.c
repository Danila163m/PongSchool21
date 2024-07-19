#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define DELAY 30000 // Задержка между кадрами в микросекундах
#define PADDLE_HEIGHT 4 // Высота ракетки
#define PADDLE_WIDTH 1 // Ширина ракетки
#define BALL_SYMBOL "O" // Символ, представляющий мяч

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

// Инициализация ракетки
void InitializePaddle(Paddle* paddle, int x, int y) {
  paddle->pos.x = x;
  paddle->pos.y = y;
  paddle->height = PADDLE_HEIGHT;
  paddle->width = PADDLE_WIDTH;
}

// Инициализация мяча
void InitializeBall(Ball* ball, int x, int y, int x_vel, int y_vel) {
  ball->pos.x = x;
  ball->pos.y = y;
  ball->x_velocity = x_vel;
  ball->y_velocity = y_vel;
}

// Отрисовка ракетки
void DrawPaddle(Paddle* paddle) {
  for (int i = 0; i < paddle->height; i++) {
    printf("\033[%d;%dH|", paddle->pos.y + i, paddle->pos.x);
  }
}

// Отрисовка мяча
void DrawBall(Ball* ball) {
  printf("\033[%d;%dH%s", ball->pos.y, ball->pos.x, BALL_SYMBOL);
}

// Перемещение ракетки
void MovePaddle(Paddle* paddle, int direction) {
  paddle->pos.y += direction;
}

// Обновление позиции мяча
void UpdateBall(Ball* ball, Paddle* left_paddle, Paddle* right_paddle, int max_y, int max_x) {
  ball->pos.x += ball->x_velocity;
  ball->pos.y += ball->y_velocity;

  // Отражение от верхнего и нижнего края экрана
  if (ball->pos.y <= 0 || ball->pos.y >= max_y) {
    ball->y_velocity = -ball->y_velocity;
  }

  // Отражение от левой ракетки
  if (ball->pos.x <= left_paddle->pos.x + PADDLE_WIDTH &&
      ball->pos.y >= left_paddle->pos.y &&
      ball->pos.y <= left_paddle->pos.y + PADDLE_HEIGHT) {
    ball->x_velocity = -ball->x_velocity;
  }

  // Отражение от правой ракетки
  if (ball->pos.x >= right_paddle->pos.x - PADDLE_WIDTH &&
      ball->pos.y >= right_paddle->pos.y &&
      ball->pos.y <= right_paddle->pos.y + PADDLE_HEIGHT) {
    ball->x_velocity = -ball->x_velocity;
  }

  // Перезапуск мяча, если он вышел за левый или правый край экрана
  if (ball->pos.x <= 0 || ball->pos.x >= max_x) {
    ball->pos.x = max_x / 2;
    ball->pos.y = max_y / 2;
    ball->x_velocity = -ball->x_velocity;
  }
}

// Очистка экрана
void ClearScreen() {
  printf("\033[2J");
}

// Перемещение курсора в начальную позицию
void MoveCursorToHome() {
  printf("\033[H");
}

int main() {
  int max_x = 80, max_y = 24;  // Размеры терминала по умолчанию
  Paddle left_paddle, right_paddle;
  Ball ball;

  // Инициализация ракеток и мяча
  InitializePaddle(&left_paddle, 1, max_y / 2 - PADDLE_HEIGHT / 2);
  InitializePaddle(&right_paddle, max_x - 2, max_y / 2 - PADDLE_HEIGHT / 2);
  InitializeBall(&ball, max_x / 2, max_y / 2, 1, 1);

  while (1) {
    ClearScreen();
    MoveCursorToHome();
    DrawPaddle(&left_paddle);
    DrawPaddle(&right_paddle);
    DrawBall(&ball);
    fflush(stdout);

    usleep(DELAY);

    char ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
      if (ch == 'q') break; // Выход из игры по нажатию клавиши 'q'

      // Управление левой ракеткой
      if (ch == 'w' && left_paddle.pos.y > 0) {
        MovePaddle(&left_paddle, -1);
      } else if (ch == 's' && left_paddle.pos.y + PADDLE_HEIGHT < max_y) {
        MovePaddle(&left_paddle, 1);
      }

      // Управление правой ракеткой
      if (ch == 'o' && right_paddle.pos.y > 0) {
        MovePaddle(&right_paddle, -1);
      } else if (ch == 'l' && right_paddle.pos.y + PADDLE_HEIGHT < max_y) {
        MovePaddle(&right_paddle, 1);
      }
    }

    // Обновление позиции мяча
    UpdateBall(&ball, &left_paddle, &right_paddle, max_y, max_x);
  }

  ClearScreen();
  MoveCursorToHome();
  return 0;
}
