#include<Arduino.h>

// I2C Display 7 Seg
#include <TM1637Display.h>
#define PinCLK 8
#define PinDIO  9

// Encoder
#define CLK 3
#define DT  4
#define SW  5

#define BT  7
#define SS  12

TM1637Display display(PinCLK, PinDIO);

int last   = 0;
int actual = 0;

int count    = 0;
int count_1  = 0;
int count_2  = 0;
int count_3  = 0;
int count_4  = 0;
int led      = 0;
int i        = 0;
int j        = 0;

bool add          = LOW;
bool sub          = LOW;
bool enable       = LOW;
bool sensor       = LOW;
bool delay_select = LOW;
bool delay_button = LOW;
bool delay_sensor = LOW;

bool select;
bool button;

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

void setup()
{
  Serial.begin(9600);

  pinMode(CLK, INPUT);
  pinMode(DT , INPUT);
  pinMode(SW , INPUT);

  pinMode(BT , INPUT);
  pinMode(SS , INPUT);

  display.setBrightness(0x0f);

  display.setSegments(SEG_DONE);
  delay(200);
  display.clear();
  delay(200);
  display.setSegments(SEG_DONE);
  Serial.println("READY!\n");

  last = digitalRead(CLK);
  actual = digitalRead(CLK);
}

void loop()
{
  actual = digitalRead(CLK);
  select = !digitalRead(SW);
  button = digitalRead(BT);
  sensor = digitalRead(SS);

  run_stop();

  if (enable == LOW) {
    led_select();
    select_number();
  }
  else {
    subtract_count();
  }

  //delay(100);
}

void add_count() {
  switch (led) {
    case 0:
      count_1++;
      if (count_1 > 9) {
        count_1 = 9;
      }
      break;

    case 1:
      count_2++;
      if (count_2 > 9) {
        count_2 = 9;
      }
      break;

    case 2:
      count_3++;
      if (count_3 > 9) {
        count_3 = 9;
      }
      break;

    case 3:
      count_4++;
      if (count_4 > 9) {
        count_4 = 9;
      }
      break;
  }
}

void sub_count() {
  switch (led) {
    case 0:
      count_1--;
      if (count_1 < 0) {
        count_1 = 0;
      }
      break;

    case 1:
      count_2--;
      if (count_2 < 0) {
        count_2 = 0;
      }
      break;

    case 2:
      count_3--;
      if (count_3 < 0) {
        count_3 = 0;
      }
      break;

    case 3:
      count_4--;
      if (count_4 < 0) {
        count_4 = 0;
      }
      break;
  }
}

void led_select() {
  if (select == HIGH && delay_select == LOW) {
    delay(10);
    delay_select = HIGH;
    led++;
    if (led > 3) {
      led = 0;
    }
  }
  if (select == LOW && delay_select == HIGH) {
    delay(10);
    delay_select = LOW;
  }
}

void select_number() {
  if (actual != last) {
    if (digitalRead(DT) != actual) { // Clockwise
      delay(10);
      i++;
      if (i > 2) {
        add_count();
        i = 0;
      }
    }
    else {                        //Counter Clockwise
      delay(10);
      j++;
      if (j > 2) {
        sub_count();
        j = 0;
      }
    }
    count = 1000 * count_1 + 100 * count_2 + 10 * count_3 + count_4;
    Serial.println(count);
    display.showNumberDec(count, true);
  }
  last = actual;
}

void run_stop() {
  if (button == HIGH && delay_button == LOW) {
    delay(10);
    enable = !enable;
    delay_button = HIGH;
  }
  if (button == LOW && delay_button == HIGH) {
    delay(10);
    delay_button = LOW;
  }
}

void subtract_count() {
  if (sensor == HIGH && delay_sensor == LOW) {
    delay(10);
    if (count != 0) {
      count_4--;
      if (count_4 < 0) {
        count_4 = 9;
        count_3--;
        if (count_3 < 0) {
          count_3 = 9;
          count_2--;
          if (count_2 < 0) {
            count_2 = 9;
            count_1--;
            if (count_1 < 0) {
              count_1 = 0;
            }
          }
        }
      }
      count = 1000 * count_1 + 100 * count_2 + 10 * count_3 + count_4;
      delay_sensor = HIGH;
      display.showNumberDec(count, true);
    }
    else {
      display.setSegments(SEG_DONE);
    }

  }
  if (sensor == LOW && delay_sensor == HIGH) {
    delay(10);
    delay_sensor = LOW;
  }
}
