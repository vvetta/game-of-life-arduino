#include <U8x8lib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define GRID_WIDTH 16
#define GRID_HEIGHT 8

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

bool grid[GRID_WIDTH][GRID_HEIGHT] = {0};
bool nextGrid[GRID_WIDTH][GRID_HEIGHT] = {0};

int cursorX = 0;
int cursorY = 0;

bool isRunning = false;

bool isCursorVisible = false;

unsigned long lastCursorUpdate = 0;
#define CURSOR_BLINK_INTERVAL 50

unsigned long lastInputTime = 0;
#define IDLE_TIME_TO_START 5000

unsigned long lastButtonPressTime = 0;
#define DEBOUNCE_DELAY 50

void computeNextGeneration();
void drawGrid();
void drawCursor();
int countLiveNeighbors(int x, int y);
int getButton();
void handleInput(int button);

void setup() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.clearDisplay();

  drawGrid();
}

void loop() {
  int button = getButton();
  unsigned long currentTime = millis();

  if (button != 0 && (currentTime - lastButtonPressTime) > DEBOUNCE_DELAY) {
    lastButtonPressTime = currentTime; 
    handleInput(button);
    lastInputTime = currentTime;
  }

  if (!isRunning && (currentTime - lastInputTime > IDLE_TIME_TO_START)) {
    isRunning = true;
  }

  if (isRunning) {
    delay(500);
    computeNextGeneration();
    memcpy(grid, nextGrid, sizeof(grid));
  }

  drawGrid();
  drawCursor();
}

void computeNextGeneration() {
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      int liveNeighbors = countLiveNeighbors(x, y);

      if (grid[x][y] == 1) {
        if (liveNeighbors < 2 || liveNeighbors > 3) {
          nextGrid[x][y] = 0;
        } else {
          nextGrid[x][y] = 1;
        }
      } else {
        if (liveNeighbors == 3) {
          nextGrid[x][y] = 1;
        } else {
          nextGrid[x][y] = 0;
        }
      }
    }
  }
}

int countLiveNeighbors(int x, int y) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) continue;

      int nx = (x + i + GRID_WIDTH) % GRID_WIDTH;
      int ny = (y + j + GRID_HEIGHT) % GRID_HEIGHT;

      if (grid[nx][ny] == 1) {
        count++;
      }
    }
  }
  return count;
}

void drawGrid() {
  u8x8.clearDisplay();
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      if (grid[x][y] == 1) {
        u8x8.drawGlyph(x, y, '*');
      } else {
        u8x8.drawGlyph(x, y, ' ');
      }
    }
  }
}

void drawCursor() {
  unsigned long currentTime = millis();
  if (currentTime - lastCursorUpdate > CURSOR_BLINK_INTERVAL) {
    lastCursorUpdate = currentTime;
    isCursorVisible = !isCursorVisible;
  }

  if (isCursorVisible) {
    u8x8.drawGlyph(cursorX, cursorY, 'X');
  }
}

void handleInput(int button) {
  switch (button) {
    case 1: 
      cursorY = (cursorY - 1 + GRID_HEIGHT) % GRID_HEIGHT;
      break;
    case 2: 
      cursorY = (cursorY + 1) % GRID_HEIGHT;
      break;
    case 3: 
      cursorX = (cursorX - 1 + GRID_WIDTH) % GRID_WIDTH;
      break;
    case 4: 
      cursorX = (cursorX + 1) % GRID_WIDTH;
      break;
    case 5:
      grid[cursorX][cursorY] = !grid[cursorX][cursorY];
      isRunning = false;
      break;
  }
}

int getButton() {
  int analogValue = analogRead(A0);

  if (analogValue >= 820) return 5;
  if (analogValue >= 400) return 4; 
  if (analogValue >= 300) return 3; 
  if (analogValue >= 230) return 2; 
  if (analogValue >= 190) return 1; 
  return 0;
}
