#include "output_handler.h"

#include "Arduino.h"
#include "constants.h"
#include <LiquidCrystal_I2C.h>  // Include the LiquidCrystal_I2C library for the display

// Set the LCD address and dimensions (40x2)
#define LCD_ADDRESS 0x27  // Change this to your display's I2C address
#define LCD_COLS 20
#define LCD_ROWS 4

// Initialize the LiquidCrystal_I2C object
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// The pin of the Arduino's built-in LED
int led = LED_BUILTIN;

// Track whether the function has run at least once
bool initialized = false;

// Animates a dot across the screen to represent the current x and y values
void HandleOutput(tflite::ErrorReporter* error_reporter, float x_value,float x_value2,
                  float y_value,
                  float y_value2) {
  // Do this only once
  if (!initialized) {
      // Set the LED pin to output
      pinMode(led, OUTPUT);
      initialized = true;
      // Initialize the LCD
      lcd.init();
      
      // Turn on the backlight (if available)
      lcd.backlight();
  }


  


  Serial.print("sleep score: ");
  Serial.println(y_value);
  Serial.print("awake score: ");
  Serial.println(y_value2);

  //lcd
  lcd.setCursor(4, 0); 
  lcd.print("TinyML Model");

  
  lcd.setCursor(0, 1); 
  lcd.print("AngZ:");
  lcd.setCursor(5, 1); 
  lcd.print(x_value);

  lcd.setCursor(12, 1); 
  lcd.print("En:");
  lcd.setCursor(15, 1); 
  lcd.print(x_value2);


  lcd.setCursor(2, 2); 
  lcd.print("Score:");
  lcd.setCursor(9, 2); 
  lcd.print(y_value);

  lcd.setCursor(14, 2); 
  lcd.print(",");
  lcd.setCursor(15, 2); 
  lcd.print(y_value2);


  lcd.setCursor(4, 3); 
  lcd.print("Status:");
  lcd.setCursor(12, 3);  
 
  Serial.print("Status: ");
  if(y_value>=0.65)
  {
    Serial.println("Sleeping");
    Serial.println("        ");
    lcd.print("Sleeping");
    delay(5000);
  }
  else
  {
    Serial.println("        ");
    Serial.println("Awake");
    lcd.print("Awake");
  }

  // Log the current brightness value for display in the Arduino plotter
  TF_LITE_REPORT_ERROR(error_reporter, "%d\n", y_value);
}
