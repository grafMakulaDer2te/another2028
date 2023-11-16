#define SCR_CLK 13;
#define SCR_DIN 11;
#define SCR_DC A2;
#define SCR_CS A1;
#define SCR_RST A0;

#define BTN_UP_PIN 9;
#define BTN_RIGHT_PIN 7;
#define BTN_DOWN_PIN 6;
#define BTN_LEFT_PIN 8;
#define BTN_A_PIN 4;
#define BTN_B_PIN 2;
#define BTN_C_PIN A3;

#define BuzzerPin 3;

#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

enum directions {
  down,
  left,
  up,
  right
};

extern const uint8_t font3x5[];
extern const uint8_t font5x7[];

#define initScreenFont font5x7
#define initScreenText "another2048"

#define numberFont font3x5
#define numberStartX 7
#define numberStartY 4
#define numberXstepWidth 18
#define numberYstepWidth 9

#define gridLineWidth 1
#define gridStartX 5
#define gridStartY 2
#define gridXstepWidth 18
#define gridYstepWidth 9

#define scoreX 5
#define scoreY 40

#define maxNumber 8192
#define victoryRectX 2
#define victoryRectY 13
#define victoryRectWidth 80
#define victoryRectHeight 14
#define victoryTextX 19
#define victoryTextY 16
#define victoryTextFont font5x7
#define victoryTextt "Victory!"

#define debounceDelay 100

long debounceNextMillis = 0;

int16_t gameMatrix[4][4] = { {0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 2, 0, 0 }, { 0, 0, 0, 0 } };
int16_t oldMatrix[4][4];
int32_t score = 4;

bool victory = false;

uint8_t randGen = 0;
uint8_t randGenFor4 = 0;

void setup() {
  //for debuging
  //Serial.begin(115200);

  // put your setup code here, to run once:
  gb.begin();
  gb.display.setFont(initScreenFont);
  gb.titleScreen(F(initScreenText));
  gb.pickRandomSeed();
  gb.battery.show = false;  //hide the battery indicator
}

void loop() {
  // put your main code here, to run repeatedly:
  randGen++;
  randGenFor4++;
  randGenFor4 = positive_modulo(randGenFor4, 10);

  boolean btnPressed = false;

  if ((millis() > debounceNextMillis) && !victory) {
    copyOldMatrix();

    if (gb.buttons.pressed(BTN_DOWN)) {
      moveMatrix(down);
      btnPressed = true;
    }
    if (gb.buttons.pressed(BTN_UP)) {
      moveMatrix(up);
      btnPressed = true;
    };
    if (gb.buttons.pressed(BTN_LEFT)) {
      moveMatrix(left);
      btnPressed = true;
    };
    if (gb.buttons.pressed(BTN_RIGHT)) {
      moveMatrix(right);
      btnPressed = true;
    };

    if (btnPressed) {
      victory = checkVictory();
      debounceNextMillis = millis() + debounceDelay;
      gb.sound.playTick();
      if (!checkOldMatrix()) {
        randomPlaceNumber();
      }
    }
  }

  if (gb.update()) {
    drawGameMatrix();
  }
}

void drawGameMatrix() {
  gb.display.setFont(numberFont);
  //draw numbers
  for (int8_t ix = 0; ix < 4; ix++) {
    for (int iy = 0; iy < 4; iy++) {
      gb.display.cursorX = numberStartX + (numberXstepWidth * ix);
      gb.display.cursorY = numberStartY + (numberYstepWidth * iy);
      if (gameMatrix[iy][ix] > 0) gb.display.print(gameMatrix[iy][ix]);
    }
  }
  //draw grid
  for (int8_t it = 0; it < 5; it++) {
    gb.display.fillRect(gridStartX, gridStartY + (it * gridYstepWidth), 4 * gridXstepWidth, gridLineWidth);  //x lines
    gb.display.fillRect(gridStartX + (it * gridXstepWidth), gridStartY, gridLineWidth, 4 * gridYstepWidth);  //y lines
  }
  //draw score
  gb.display.cursorX = scoreX;
  gb.display.cursorY = scoreY;
  gb.display.print("score: " + String(score));
  //victory screen
  if (victory) {
    gb.display.setColor(BLACK, WHITE);
    gb.display.fillRect(victoryRectX, victoryRectY, victoryRectWidth, victoryRectHeight);
    gb.display.setColor(WHITE, BLACK);
    gb.display.setFont(victoryTextFont);
    gb.display.cursorX = victoryTextX;
    gb.display.cursorY = victoryTextY;
    gb.display.print(victoryTextt);
    gb.display.setColor(BLACK, WHITE);
  }
}

void moveMatrix(int8_t dir) {
  switch (dir) {
    case down:
      for (int8_t ix = 0; ix < 4; ix++) {
        for (int8_t iy = 3; iy > -1; iy--) {  //go from down to up
          if (!(iy == 3)) {                   //skip if last
            int8_t nextt = iy + 1;
            while ((gameMatrix[nextt][ix] == 0) && (nextt < 3)) nextt++;

            if (gameMatrix[nextt][ix] == 0) {
              gameMatrix[nextt][ix] = gameMatrix[iy][ix];
              gameMatrix[iy][ix] = 0;
            }
            if (gameMatrix[nextt][ix] == gameMatrix[iy][ix]) {
              gameMatrix[nextt][ix] *= 2;
              gameMatrix[iy][ix] = 0;
            } else {
              gameMatrix[nextt - 1][ix] = gameMatrix[iy][ix];
              if (!((nextt - 1) == iy)) gameMatrix[iy][ix] = 0;
            }
          }
        }
      }
      break;
    case up:
      for (int8_t ix = 0; ix < 4; ix++) {
        for (int8_t iy = 0; iy < 4; iy++) {  //go from up to down
          if (!(iy == 0)) {                  //skip if first
            int8_t nextt = iy - 1;
            while ((gameMatrix[nextt][ix] == 0) && (nextt > 0)) nextt--;

            if (gameMatrix[nextt][ix] == 0) {
              gameMatrix[nextt][ix] = gameMatrix[iy][ix];
              gameMatrix[iy][ix] = 0;
            }
            if (gameMatrix[nextt][ix] == gameMatrix[iy][ix]) {
              gameMatrix[nextt][ix] *= 2;
              gameMatrix[iy][ix] = 0;
            } else {
              gameMatrix[nextt + 1][ix] = gameMatrix[iy][ix];
              if (!((nextt + 1) == iy)) gameMatrix[iy][ix] = 0;
            }
          }
        }
      }
      break;
    case right:
      for (int8_t ix = 3; ix > -1; ix--) {  //go from right to left
        for (int8_t iy = 3; iy > -1; iy--) {
          if (!(ix == 3)) {  //skip if last
            int8_t nextt = ix + 1;
            while ((gameMatrix[iy][nextt] == 0) && (nextt < 3)) nextt++;

            if (gameMatrix[iy][nextt] == 0) {
              gameMatrix[iy][nextt] = gameMatrix[iy][ix];
              gameMatrix[iy][ix] = 0;
            }
            if (gameMatrix[iy][nextt] == gameMatrix[iy][ix]) {
              gameMatrix[iy][nextt] *= 2;
              gameMatrix[iy][ix] = 0;
            } else {
              gameMatrix[iy][nextt - 1] = gameMatrix[iy][ix];
              if (!((nextt - 1) == ix)) gameMatrix[iy][ix] = 0;
            }
          }
        }
      }
      break;
    case left:
      for (int8_t ix = 0; ix < 4; ix++) {  //go from left to right
        for (int8_t iy = 3; iy > -1; iy--) {
          if (!(ix == 0)) {  //skip if last
            int8_t nextt = ix - 1;
            while ((gameMatrix[iy][nextt] == 0) && (nextt > 0)) nextt--;

            if (gameMatrix[iy][nextt] == 0) {
              gameMatrix[iy][nextt] = gameMatrix[iy][ix];
              gameMatrix[iy][ix] = 0;
            }
            if (gameMatrix[iy][nextt] == gameMatrix[iy][ix]) {
              gameMatrix[iy][nextt] *= 2;
              gameMatrix[iy][ix] = 0;
            } else {
              gameMatrix[iy][nextt + 1] = gameMatrix[iy][ix];
              if (!((nextt + 1) == ix)) gameMatrix[iy][ix] = 0;
            }
          }
        }
      }
      break;
  }
}

int8_t randomPlaceNumber() {
  //first count number of empty fields
  int8_t emptyFilds = 0;
  for (int8_t ix = 0; ix < 4; ix++) {
    for (int8_t iy = 0; iy < 4; iy++) {
      if (gameMatrix[iy][ix] == 0) emptyFilds++;
    }
  }
  if (emptyFilds == 0) return 0;  //break if no empty field left

  //get random position
  //int8_t randPos = randGen % emptyFilds;
  int8_t randPos = positive_modulo(randGen, emptyFilds);

  //Serial.println(String(randPos) + " " + String(emptyFilds));

  //place 2 to random position
  for (int8_t ix = 0; ix < 4; ix++) {
    for (int8_t iy = 0; iy < 4; iy++) {
      if (gameMatrix[iy][ix] == 0) {
        if (randPos == 0) {
          //gameMatrix[iy][ix] = 2;
          gameMatrix[iy][ix] = (randGenFor4 == 0) ? 4 : 2;
          score += gameMatrix[iy][ix];
        }  //else Serial.println("falsch");
        randPos--;
      }
    }
  }

  return emptyFilds;
}

void copyOldMatrix() {
  for (int8_t ix = 0; ix < 4; ix++) {
    for (int8_t iy = 0; iy < 4; iy++) {
      oldMatrix[iy][ix] = gameMatrix[iy][ix];
    }
  }
}

boolean checkOldMatrix() {
  for (int8_t ix = 0; ix < 4; ix++) {
    for (int8_t iy = 0; iy < 4; iy++) {
      if (oldMatrix[iy][ix] != gameMatrix[iy][ix]) return false;
    }
  }
  return true;
}

boolean checkVictory() {
  for (int8_t ix = 0; ix < 4; ix++) {
    for (int8_t iy = 0; iy < 4; iy++) {
      if (gameMatrix[iy][ix] > maxNumber) return true;
    }
  }
  return false;
}

int8_t positive_modulo(uint8_t i, int8_t n) {
  return (i % n + n) % n;
}