/*
 Arduino SIMON Says Game
 Created by Gabe and Tyrell
 Date: 2022-05-08
CMP 521A

 This game was built on the Arduino UNO board.

 It uses the following hardware
 
 4 - LEDs used to display the feedback
 4 - Buttons for Input
 1 - Buzzer
 1 - Arduino Uno Board
 1 - Arduino Uno
 3 - 220 ohm Resistors for LED
 1 - 200 ohm Resistor for LED
 1 - 16x2 LCD for Game Info and Scoring
 1 - 10K Potentiometer
 1 - 48400 ohm Resistor for LCD
 30- Wires

 WE NEED TO ADD MORE OF THE STUFF WE USED LIKE THE LCD STUFF
 
  */

//Variables

#include <LiquidCrystal.h>

#define CHOICE_OFF      0 //Used to control LEDs
#define CHOICE_NONE     0 //Used to check buttons
#define CHOICE_YELLOW  (1 << 0) // Binary: 0000000000000001 , or 1 in decimal
#define CHOICE_RED     (1 << 1) // Binary: 0000000000000010 , or 2 in decimal
#define CHOICE_WHITE   (1 << 2) // Binary: 0000000000000100 , or 4 in decimal
#define CHOICE_BLUE    (1 << 3) // Binary: 0000000000001000 , or 8 in decimal
#define LED_YELLOW     5
#define LED_RED        12  
#define LED_WHITE      2 
#define LED_BLUE       10 



// Definitions on Buttons information

#define BUTTON_YELLOW  6 
#define BUTTON_RED    13
#define BUTTON_WHITE   3 
#define BUTTON_BLUE    9 

// Definitions Of Buzzer Sounds

#define BUZZER1  7 
#define BUZZER2  4 

// Definitions of the game parameters

#define ROUNDS_TO_WIN      8  // You need to win 8 times to win
#define TIME_LIMIT      4000  // You have 4 seconds before timing out
#define SIMON              0

// Game state variables

byte gameMode = SIMON; //This is the gamemode
byte gameBoard[32]; // Contains the combination of buttons as the game continues
byte gameRound = 0; // Counts the number of succesful levels the player has made it to


const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5; 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup(){
  
//This will welcome the user when the arduino first turns on and then turn off to inform the user the max round they can go to and the previous round they or someone else got to

  lcd.begin(16,2);
  delay(300);
  lcd.clear();
  lcd.print("Welcome To");
  lcd.setCursor(0,1);
  lcd.print("SIMON SAYS GAME");
  delay(5000);

//Setup hardware inputs/outputs. These pins are defined in the hardware_versions header file
//Enable pull ups on inputs

  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_WHITE, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);

//Checking the Mode DC style

  gameMode = SIMON; // SIMON Says Game

}

void loop(){

//This will tell you what the highest round you can go to and tell you the the previous round you got to
  lcd.begin(16,2);
  lcd.print("Max Round: 8 "); // If you increase the number of levels don't forget to adjust this to the new level
  lcd.setCursor(0,1);
  lcd.print("Previous Score:");
  lcd.setCursor(15,1);
  lcd.print(gameRound);
  

  
  attractMode(); // Blink lights while waiting for user to press a button
  setLEDs(CHOICE_YELLOW | CHOICE_RED | CHOICE_WHITE | CHOICE_BLUE); // Turn LEDs on red - green - blue - yellow
  delay(1000);
  setLEDs(CHOICE_OFF); // Turn off LEDs
  delay(250);
  if (gameMode == SIMON)
  {
    if (play_SIMON() == true) 
      play_loser(); // Player lost
  }
}
boolean play_SIMON(void)
{
  randomSeed(millis()); // Seed the random generator with random amount of millis()
  gameRound = 0; // Resets the game back to square one
  while (gameRound < ROUNDS_TO_WIN)  // Game will last as long as rounds_to_win is not passed
  {
    add_to_moves(); // Add a button and light flash to the current moves, then play them back to player
    playMoves(); // Repeats back the current game board path.  Then require the player to repeat the path.

    for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) //++ is  post increment 
    {
      byte choice = wait_for_button(); // See what button the user presses to see if they got the right button
      if (choice == 0) return false; // If wait timed out, player loses
      if (choice != gameBoard[currentMove]) return false; // If the choice is incorect, player loses
    }
    delay(1250); // Player was correct, delay of 1.25 seconds before playing moves
  }
  return true; // User completed all the levels, and won!!!!!  
}
void playMoves(void)
{
  for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) 
  {
    toner(gameBoard[currentMove], 175); //this shows how long the player gets to see the game pattern
    delay(175);  //This is the playback speed
  }
}
void add_to_moves(void)
{
  byte newButton = random(0, 4); //min (included), max (exluded)
  // We have to convert this number, 0 to 3, to CHOICEs
  if(newButton == 0) newButton = CHOICE_YELLOW;
  else if(newButton == 1) newButton = CHOICE_RED;
  else if(newButton == 2) newButton = CHOICE_WHITE;
  else if(newButton == 3) newButton = CHOICE_BLUE;
  gameBoard[gameRound++] = newButton; // Add this new button to the game array
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//The following functions control the hardware
// Lights a given LEDs
// Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc

void setLEDs(byte leds)
{
  if ((leds & CHOICE_YELLOW) != 0) 
    digitalWrite(LED_YELLOW, HIGH);
  else
    digitalWrite(LED_YELLOW, LOW);

  if ((leds & CHOICE_RED) != 0)
    digitalWrite(LED_RED, HIGH);
  else
    digitalWrite(LED_RED, LOW);

  if ((leds & CHOICE_WHITE) != 0)
    digitalWrite(LED_WHITE, HIGH);
  else
    digitalWrite(LED_WHITE, LOW);

  if ((leds & CHOICE_BLUE) != 0)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);
}

// Wait for a button to be pressed. 
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out

byte wait_for_button(void)
{
  long startTime = millis(); // Remember the time we started the this loop
  while ( (millis() - startTime) < TIME_LIMIT) // Loop until too much time has passed
  {
    byte button = checkButton();
    if (button != CHOICE_NONE)
    { 
      toner(button, 175); // Play the button the user just pressed
      while(checkButton() != CHOICE_NONE) ;  // Now let's wait for user to release button
      delay(10); // This helps with debouncing and accidental double taps
      return button;
    }
  }
  return CHOICE_NONE; // If we get here, we've timed out!
}
// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
byte checkButton(void)
{
  if (digitalRead(BUTTON_YELLOW) == 0) return(CHOICE_YELLOW);
  else if (digitalRead(BUTTON_RED) == 0) return(CHOICE_RED);
  else if (digitalRead(BUTTON_WHITE) == 0) return(CHOICE_WHITE);
  else if (digitalRead(BUTTON_BLUE) == 0) return(CHOICE_BLUE);
  return(CHOICE_NONE); // If no button is pressed, return none
}
// Light an LED and play tone
void toner(byte which, int buzz_length_ms)
{
  setLEDs(which); //Turn on a given LED
  //Play the sound associated with the given LED
  switch(which) 
  {
  case CHOICE_YELLOW:
    buzz_sound(buzz_length_ms, 2840); 
    break;
  case CHOICE_RED:
    buzz_sound(buzz_length_ms, 470); 
    break;
  case CHOICE_WHITE:
    buzz_sound(buzz_length_ms, 800); 
    break;
  case CHOICE_BLUE:
    buzz_sound(buzz_length_ms, 2044); 
    break;
  }
  setLEDs(CHOICE_OFF); // Turn off all LEDs
}
// Toggle buzzer every buzz_delay_us, for a duration of buzz_length_ms.
void buzz_sound(int buzz_length_ms, int buzz_delay_us)
{
  // Convert total play time from milliseconds to microseconds
  long buzz_length_us = buzz_length_ms * (long)1200; //duration of buzz for each LED
  // Loop until the remaining play time is less than a single buzz_delay_us
  while (buzz_length_us > (buzz_delay_us * 2))
  {
    buzz_length_us -= buzz_delay_us * 2; //Decrease the remaining play time
    // Toggle the buzzer at various speeds
    digitalWrite(BUZZER1, LOW);
    digitalWrite(BUZZER2, HIGH);
    delayMicroseconds(buzz_delay_us);
    digitalWrite(BUZZER1, HIGH); 
    digitalWrite(BUZZER2, LOW);
    delayMicroseconds(buzz_delay_us);
  }
}
// Play the winner sound and lights
void winner_sound(void)
{
  // Toggle the buzzer at various speeds
  for (byte x = 250 ; x > 70 ; x--)
  {
    for (byte y = 0 ; y < 3 ; y++)
    {
      digitalWrite(BUZZER2, HIGH);
      digitalWrite(BUZZER1, LOW);
      delayMicroseconds(x);
      digitalWrite(BUZZER2, LOW);
      digitalWrite(BUZZER1, HIGH);
      delayMicroseconds(x);
    }
  }
}
// Play the loser sound/lights
void play_loser(void)
{
  setLEDs(CHOICE_YELLOW | CHOICE_RED);
  buzz_sound(255, 1500);
  setLEDs(CHOICE_WHITE | CHOICE_BLUE);
  buzz_sound(255, 1500);
  setLEDs(CHOICE_YELLOW | CHOICE_RED);
  buzz_sound(255, 1500);
  setLEDs(CHOICE_WHITE | CHOICE_BLUE);
  buzz_sound(255, 1500);
}
// attractMode is flashing the lights till someone is ready to play
void attractMode(void)
{
  while(1) 
  {
    setLEDs(CHOICE_YELLOW);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
    setLEDs(CHOICE_WHITE);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
    setLEDs(CHOICE_RED);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
    setLEDs(CHOICE_BLUE);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
  }  
}
