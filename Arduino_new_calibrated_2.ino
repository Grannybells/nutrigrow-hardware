#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>

#define RE 8
#define DE 7

byte val1, val2, val3;

#include "HX711.h"

const int LOADCELL_DOUT_PIN = 4;  // BLUE WIRE
const int LOADCELL_SCK_PIN = 5;   // WHITE WIRE

HX711 scale;

int buttonPin = 35;
int upButtonPin = 33;
int downButtonPin = 31;

int motorN = 12;
int motorK = 13;

int currentMenuIndex = 0;
int previousMenuIndex = 0;

const byte nitro[] = { 0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c };
const byte phos[] = { 0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc };
const byte pota[] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0 };

byte values[11];
SoftwareSerial mod(10, 11);

#define SensorPin A0

float calibration_value = 21.84;

unsigned long int avgValue;
float b;
int buf[10], temp;

int data = 0;
int moisture = 0;

LiquidCrystal_I2C lcd(0x27, 16, 4);

float newNt;
float newPo;
float newPs;

Servo Nservo;
Servo Pservo;
Servo Kservo;

int cabbage[3] = { 326, 271, 200 };
int bellpeper[3] = { 326, 163, 150 };
int okra[3] = { 217, 108, 100 };
int tomato[3] = { 391, 260, 300 };
int lettuce[3] = { 347, 65, 486 };
int ginger[3] = { 81, 108, 75 };
int kale[3] = { 326, 217, 200 };
int cauli[3] = { 326, 217, 200 };
int garlic[3] = { 86, 163, 150 };
int chilli[3] = { 326, 163, 150 };

float value1;
float value2;
float value3;

float land = 0.0001;

const int numMenuOptions = 11;

String menuOptions[numMenuOptions] = {
  "Choose plant ",
  "1. Cabbage   ",
  "2. Bellpeper ",
  "3. Okra      ",
  "4. Tomato    ",
  "5. Lettuce   ",
  "6. Ginger    ",
  "7. Kale      ",
  "8. Cauli     ",
  "9. Garlic    ",
  "10. Chilli   "
};

void setup() {
  Serial.begin(9600);
  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  pinMode(buttonPin, INPUT);
  pinMode(upButtonPin, INPUT);
  pinMode(downButtonPin, INPUT);

  pinMode(motorN, OUTPUT);
  digitalWrite(motorN, HIGH);

  pinMode(motorK, OUTPUT);
  digitalWrite(motorK, HIGH);

  pinMode(A2, OUTPUT);
  pinMode(A3, INPUT);

  lcd.init();
  lcd.begin(16, 4);
  lcd.backlight();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(104.38);  //104.09 or 104.38
  scale.tare();

  Nservo.attach(2);
  Pservo.attach(3);
  Kservo.attach(9);

  Nservo.write(90);
  Pservo.write(90);
  Kservo.write(90);
}

void loop() {
  val1 = nitrogen();
  delay(250);
  val2 = phosphorous() * 1.25;
  delay(250);
  val3 = potassium() * 2.75;
  delay(250);

  Serial.println(String(moisture) + "," + String(ph_level()) + "," + String(val1) + "," + String(val2) + "," + String(val3) + "," + menuOptions[currentMenuIndex] + "," + String(newNt) + "," + String(newPo) + "," + String(newPs) + "," + String(nitrogen_range()) + "," + String(phosphorous_range()) + "," + String(potassium_range()) + "," + String(ph_range()));

  lcd.setCursor(0, 0);
  lcd.print("N:   ");
  lcd.setCursor(2, 0);
  lcd.print(val1);
  lcd.setCursor(7, 0);
  lcd.print("P:   ");
  lcd.setCursor(9, 0);
  lcd.print(val2);
  lcd.setCursor(13, 0);
  lcd.print("K:   ");
  lcd.setCursor(15, 0);
  lcd.print(val3);

  // Print pH level Data in LCD
  lcd.setCursor(1, 1);
  lcd.print("pH:");
  lcd.setCursor(5, 1);
  lcd.print(ph_level(), 2);

  // Print moisture Data in LCD
  lcd.setCursor(12, 1);
  lcd.print("MC:    ");
  lcd.setCursor(16, 1);
  lcd.print(moisture);

  if (digitalRead(upButtonPin) == LOW) {
    previousMenuIndex = currentMenuIndex;
    currentMenuIndex = (currentMenuIndex + 1) % numMenuOptions;
    lcd_data();
    delay(250);  // debounce delay
  }

  if (digitalRead(downButtonPin) == LOW) {
    previousMenuIndex = currentMenuIndex;
    currentMenuIndex = (currentMenuIndex - 1 + numMenuOptions) % numMenuOptions;
    lcd_data();
    delay(250);  // debounce delay
  }

  if (digitalRead(buttonPin) == LOW) {
    performAction(currentMenuIndex);
    delay(250);  // debounce delay
  }
}

void lcd_data() {
  lcd.setCursor(4, 2);
  lcd.print(menuOptions[currentMenuIndex]);

  // Clear the entire line first to remove any existing characters
  lcd.setCursor(1, 3);
  lcd.print("      ");
  lcd.setCursor(8, 3);
  lcd.print("      ");
  lcd.setCursor(14, 3);
  lcd.print("      ");

  // Print the new values directly after clearing the line
  lcd.setCursor(1, 3);
  lcd.print(newNt);
  lcd.setCursor(8, 3);
  lcd.print(newPo);
  lcd.setCursor(14, 3);
  lcd.print(newPs);
}

void performAction(int menuIndex) {
  switch (menuIndex) {
    case 0:
      // Action for Option 0
      buttonActionClean();
      break;
    case 1:
      // Action for Option 1
      newNt = (cabbage[0] - ((val1 / 46) * 100)) * land;
      newPo = (cabbage[1] - ((val2 / 46) * 100)) * land;
      newPs = (cabbage[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 5.5);
      value2 = ((newPo * 1000) + 1);
      value3 = ((newPs * 1000) - 4.5);

      dispenseButton();
      break;
    case 2:
      // Action for Option 2
      newNt = (okra[0] - ((val1 / 46) * 100)) * land;
      newPo = (okra[1] - ((val2 / 46) * 100)) * land;
      newPs = (okra[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 4.5);
      value2 = ((newPo * 1000) - 4);
      value3 = ((newPs * 1000) - 5.5);

      dispenseButton();
      break;

    case 3:
      // Action for Option 3
      newNt = (tomato[0] - ((val1 / 46) * 100)) * land;
      newPo = (tomato[1] - ((val2 / 46) * 100)) * land;
      newPs = (tomato[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 2);
      value2 = ((newPo * 1000) + 1);
      value3 = ((newPs * 1000) - 5.5);

      dispenseButton();
      break;

    case 4:
      // Action for Option 4
      newNt = (lettuce[0] - ((val1 / 46) * 100)) * land;
      newPo = (lettuce[1] - ((val2 / 46) * 100)) * land;
      newPs = (lettuce[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 8);
      value2 = ((newPo * 1000) + 1);
      value3 = ((newPs * 1000) - 3);

      dispenseButton();
      break;

    case 5:
      // Action for Option 5
      newNt = (bellpeper[0] - ((val1 / 46) * 100)) * land;
      newPo = (bellpeper[1] - ((val2 / 46) * 100)) * land;
      newPs = (bellpeper[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 5);
      value2 = (newPo * 1000);
      value3 = ((newPs * 1000) - 10);

      dispenseButton();
      break;

    case 6:
      // Action for Option 6
      newNt = (ginger[0] - ((val1 / 46) * 100)) * land;
      newPo = (ginger[1] - ((val2 / 46) * 100)) * land;
      newPs = (ginger[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 1);
      value2 = ((newPo * 1000) - 4.25);
      value3 = ((newPs * 1000) - 3);

      dispenseButton();
      break;

    case 7:
      // Action for Option 7
      newNt = (kale[0] - ((val1 / 46) * 100)) * land;
      newPo = (kale[1] - ((val2 / 46) * 100)) * land;
      newPs = (kale[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 5.5);
      value2 = ((newPo * 1000) - 5.20);
      value3 = ((newPs * 1000) - 5.5);

      dispenseButton();
      break;

    case 8:
      // Action for Option 8
      newNt = (cauli[0] - ((val1 / 46) * 100)) * land;
      newPo = (cauli[1] - ((val2 / 46) * 100)) * land;
      newPs = (cauli[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 5.5);
      value2 = ((newPo * 1000) - 5.20);
      value3 = ((newPs * 1000) - 5.5);

      dispenseButton();
      break;

    case 9:
      // Action for Option 9
      newNt = (garlic[0] - ((val1 / 46) * 100)) * land;
      newPo = (garlic[1] - ((val2 / 46) * 100)) * land;
      newPs = (garlic[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 1.5);
      value2 = ((newPo * 1000) + 0.5);
      value3 = ((newPs * 1000) - 10);

      dispenseButton();
      break;

    case 10:
      // Action for Option 10
      newNt = (chilli[0] - ((val1 / 46) * 100)) * land;
      newPo = (chilli[1] - ((val2 / 46) * 100)) * land;
      newPs = (chilli[2] - ((val3 / 50) * 100)) * land;

      value1 = ((newNt * 1000) - 5.5);
      value2 = ((newPo * 1000) + 1);
      value3 = ((newPs * 1000) - 10);

      dispenseButton();
      break;
    // Add cases for other menu options as needed
    default:
      // Default action if menu option is not defined
      Serial.println("No action defined for this option");
      break;
  }
}

void dispenseButton() {
  // Get the weight from the scale and tare it (zero out the scale).
  float weight = scale.get_units(10);
  scale.tare();

  // Dispense the first ingredient until the weight reaches value1.
  do {
    weight = scale.get_units(10);
    Nservo.write(80);  // Move servo for ingredient N to dispensing position.
    Pservo.write(90);
    Kservo.write(90);
    digitalWrite(motorN, LOW);
    digitalWrite(motorK, HIGH);
  } while (weight <= value1);
  // Continue dispensing while weight is less than or equal to value1.
  // Reset servos and motors to neutral and off positions.
  Nservo.write(90);
  Pservo.write(90);
  Kservo.write(90);
  digitalWrite(motorN, HIGH);
  digitalWrite(motorK, HIGH);
  scale.tare();
  delay(3000);

  // Dispense the first ingredient until the weight reaches value2.
  do {
    weight = scale.get_units(10);
    Nservo.write(90);
    Pservo.write(70);  // Move servo for ingredient P to dispensing position.
    Kservo.write(90);
  } while (weight <= value2);
  // Continue dispensing while weight is less than or equal to value2.
  // Reset servos and motors to neutral and off positions.
  Nservo.write(90);
  Pservo.write(90);
  Kservo.write(90);
  scale.tare();
  delay(3000);

  // Dispense the first ingredient until the weight reaches value3.
  do {
    weight = scale.get_units(10);
    Nservo.write(90);
    Pservo.write(90);
    Kservo.write(80);  // Move servo for ingredient K to dispensing position.
    digitalWrite(motorN, HIGH);
    digitalWrite(motorK, LOW);
  } while (weight <= value3);
  // Continue dispensing while weight is less than or equal to value3.
  // Reset servos and motors to neutral and off positions.
  Nservo.write(90);
  Pservo.write(90);
  Kservo.write(90);
  digitalWrite(motorN, HIGH);
  digitalWrite(motorK, HIGH);
  scale.tare();
  delay(3000);
}

void buttonActionClean() {
  // Set servos to a position that cleans the system.
  Nservo.write(50);
  Pservo.write(50);
  Kservo.write(50);
  digitalWrite(motorN, LOW);
  digitalWrite(motorK, LOW);
  delay(10000);
  // Reset servos and motors to neutral and off positions after cleaning.
  Nservo.write(90);
  Pservo.write(90);
  Kservo.write(90);
  digitalWrite(motorN, HIGH);
  digitalWrite(motorK, HIGH);
}

void moisture_level() {
  digitalWrite(A2, HIGH);  // Apply power to the soil moisture sensor
  delay(10);
  data = analogRead(A3);
  moisture = data / 6;  // Turn off the sensor to reduce metal corrosion
  digitalWrite(A3, LOW);
}

float ph_level() {
  int buf[10];   // Array to store analog readings
  int temp;      // Temporary variable for sorting
  int avgValue;  // Variable to store the average value of readings
  // Read analog values from sensor multiple times for stability
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(SensorPin);  // Read analog value from sensor pin
    delay(30);
  }
  // Sort the readings in ascending order
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {  // If current reading is greater than the next one
        temp = buf[i];        // Swap the readings
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  // Calculate the average value of readings, excluding extreme values
  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  // Convert the average analog value to voltage
  float volt = (float)avgValue * 5.0 / 1024 / 6;  // Conversion formula
  // Convert voltage to pH level using calibration value and formula
  float ph_lvl = -5.70 * volt + calibration_value;  // pH calculation formula
  return ph_lvl;
}

String ph_range() {
  // Determine the pH range based on the pH level
  if (ph_level() <= 4.4) {
    return "Extreme Acidic";
  } else if (ph_level() <= 5.5) {
    return "Strongly Acidic";
  } else if (ph_level() <= 6.0) {
    return "Moderately Acidic";
  } else if (ph_level() <= 6.6) {
    return "Slightly Acidic";
  } else if (ph_level() <= 7.2) {
    return "Near Neutral";
  } else if (ph_level() <= 7.8) {
    return "Slightly Alkaline";
  } else if (ph_level() <= 8.9) {
    return "Moderately Alkaline";
  } else if (ph_level() <= 9.0) {
    return "Strongly Alkaline";
  } else {
    return "Extreme Alkaline";
  }
}

String nitrogen_range() {
  // Determine the nitrogen level range based on the value val1
  if (val1 <= 107) {
    return "Low";
  } else if (val1 <= 214 && val1 >= 108) {
    return "Medium";
  } else if (val1 <= 250 && val1 >= 215) {
    return "High";
  } else if (val1 <= 255 && val1 >= 216) {
    return "Very high";
  } else {
  }
}

String phosphorous_range() {
  // Determine the phosphorous level range based on the value val2
  if (val2 <= 5) {
    return "Low";
  } else if (val2 <= 12 && val2 >= 6) {
    return "Moderately medium";
  } else if (val2 <= 25 && val2 >= 13) {
    return "Moderately high";
  } else if (val2 <= 50 && val2 >= 26) {
    return "High";
  } else if (val2 <= 255 && val2 >= 51) {
    return "Very high";
  } else {
  }
}

String potassium_range() {
  // Determine the potassium level range based on the value val3
  if (val3 <= 75) {
    return "Low";
  } else if (val3 <= 113 && val3 >= 76) {
    return "Sufficient";
  } else if (val3 <= 150 && val3 >= 114) {
    return "Sufficient+";
  } else if (val3 <= 250 && val3 >= 151) {
    return "Sufficient++";
  } else if (val3 <= 255 && val3 >= 251) {
    return "Sufficient+++";
  } else {
  }
}

byte nitrogen() {
  // Activate the DE (Enable) and RE (Receiver Enable) pins
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  // Write the command 'nitro' to the module and check if 8 bytes were sent successfully
  if (mod.write(nitro, sizeof(nitro)) == 8) {
    // Deactivate the DE and RE pins
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    // Read data from the module into the 'values' array
    for (byte i = 0; i < 7; i++) {
      values[i] = mod.read();
    }
  }
  return values[4];
}

byte phosphorous() {
  // Activate the DE (Enable) and RE (Receiver Enable) pins
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  // Write the command 'phos' to the module and check if 8 bytes were sent successfully
  if (mod.write(phos, sizeof(phos)) == 8) {
    // Deactivate the DE and RE pins
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    // Read data from the module into the 'values' array
    for (byte i = 0; i < 7; i++) {
      values[i] = mod.read();
    }
  }
  return values[4];
}

byte potassium() {
  // Activate the DE (Enable) and RE (Receiver Enable) pins
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  // Write the command 'pota' to the module and check if 8 bytes were sent successfully
  if (mod.write(pota, sizeof(pota)) == 8) {
    // Deactivate the DE and RE pins
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    // Read data from the module into the 'values' array
    for (byte i = 0; i < 7; i++) {
      values[i] = mod.read();
    }
  }
  return values[4];
}