#include <Stepper.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define STEPS 2038 // the number of steps in one revolution of your motor (28BYJ-48)
#define encoder_left 3    // s2
#define encoder_right 2   // s1
#define encoder_button 4  // key 
#define OLED_RESET 4
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

Adafruit_SSD1306 display(OLED_RESET);
Stepper stepper(STEPS, 8, 10, 9, 11);

int counter = 0;
int aState;
int aLastState;
int button_val = 0;
int startPos = 0; //14336
int rightLock = 0;
int leftLock = 28672;
int endStep = 1;
int endPos = 3300;
int minutes = 5;
long runtime = 0;
int span = 0;
int spanRemain = 0;
int stepDelay = 0;
int min_add = 5;

String encdir = "";

//------------------------- SETUP --------------------------------
void setup() {
  pinMode (encoder_left, INPUT);
  pinMode (encoder_right, INPUT);
  pinMode (encoder_button, INPUT);

  Serial.begin (9600);
  aLastState = digitalRead(encoder_left);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.display();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
}

void loop() {    
  
  //------------------------- START POSITION --------------------------------
  //Serial.println("Start angle L/R ");
  display.clearDisplay();
  display.setCursor(32, 17);
  display.print("START");
  display.display();
  stepper.setSpeed(3);

  while (endStep) {
    aState = digitalRead(encoder_left);
    if (aState != aLastState) {
      if (digitalRead(encoder_right) != aState) {
        stepper.step(+100);
        startPos = startPos + 100;
      } else {
        stepper.step(-100);
        startPos = startPos - 100;
      }
    }
    aLastState = aState;

    button_val = digitalRead(encoder_button);
    if (button_val == LOW) {
      //Serial.println("START POSSITION SET");
      endStep = 0;
      delay(250);
    }
  }
  display.clearDisplay();
  endStep = 1;

  //------------------------- END POSITION --------------------------------
  //Serial.println("End angle L/R   ");
  display.setCursor(47, 17);
  display.print("END");
  display.display();
  endPos = startPos;

  while (endStep) {
    aState = digitalRead(encoder_left);
    if (aState != aLastState) {
      if (digitalRead(encoder_right) != aState) {
        stepper.step(+100);
        endPos = endPos + 100;
      } else {
        stepper.step(-100);
        endPos = endPos - 100;
      }
      //Serial.print("Position: ");
      //Serial.println(endPos);
    }
    aLastState = aState;

    button_val = digitalRead(encoder_button);
    if (button_val == LOW) {
      //Serial.println("END POSSITION SET");
      endStep = 0;
      delay(250);
    }
  }
  display.clearDisplay();
  endStep = 1;

  //------------------------- TIME --------------------------------
  //Serial.print("Run time: L/R   ");
  display.setCursor(32, 17);
  display.print("TIME");
  display.display();

  while (endStep) {
    aState = digitalRead(encoder_left);
    if (aState != aLastState) {
      if (digitalRead(encoder_right) != aState) {
        minutes = minutes + min_add;
        display.clearDisplay();
      } else {
        minutes = minutes - min_add;
        if (minutes <= 0) {
          minutes = 1;
        }
        display.clearDisplay();
      }
      //Serial.print("Minutes: ");
      //Serial.println(minutes);
      display.setCursor(50, 17);
      display.print(minutes);
      display.display();
    }
    aLastState = aState;

    button_val = digitalRead(encoder_button);
    if (button_val == LOW) {
      //Serial.println("TIME SET");
      endStep = 0;
      delay(250);
    }
  }
  display.clearDisplay();

  //------------------------- RUN ROTATION --------------------------------

  runtime = (minutes * 60000);
  //Serial.println("Program Running");
  //Serial.println("    Minutes Left");
  display.setCursor(32, 17);
  display.print("LEFT");
  display.display();

  if (startPos > endPos) {
    span = startPos - endPos;
    spanRemain = span;
    stepDelay = (runtime / span);
    stepper.step(span);
    while (spanRemain) {
      display.clearDisplay();
      stepper.step(-1);
      runtime = (runtime - stepDelay);
      //Serial.println((runtime / 60000) + 1);
      spanRemain--;
      delay(stepDelay);
      display.setCursor(50, 17);
      display.print((runtime / 60000) + 1);
      display.display();
    }

  } else {
    span = endPos - startPos;
    spanRemain = span;
    stepDelay = (runtime / span);
    stepper.step(span * -1);
    while (spanRemain) {
      display.clearDisplay();
      stepper.step(1);
      runtime = (runtime - stepDelay);
      //Serial.println((runtime / 60000) + 1);
      spanRemain--;
      delay(stepDelay);
      display.setCursor(50, 17);
      display.print((runtime / 60000) + 1);
      display.display();
    }
  }
  endStep = 1;

  display.clearDisplay();
  display.setCursor(32, 17);
  display.print("DONE");
  display.display();
  delay(1000);
  //Serial.println("Program Complete");
  /*
  */
}
