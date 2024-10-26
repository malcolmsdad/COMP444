/*
  SparkFun Inventor’s Kit
  Circuit 4C - Heads Up Game

  HAAACCCKKK
*/

#include <LiquidCrystal.h>                //the liquid crystal library contains commands for printing to the display
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  // tell the RedBoard what pins are connected to the display

int buttonPin = 2;        //pin that the button is connected to
int buzzerPin = 6;        //pin for driving the buzzer
int buttonPressTime = 0;  //variable to show how much time the player has left to guess the word (and press the button)

long timeLimit = 10000;  //time limit for the player to guess each word
long startTime = 0;      //used to measure time that has passed for each word
int roundNumber = 0;     //keeps track of the roundNumber so that it can be displayed at the end of the game

const int arrayCatSize = 2;
const int arrayWordSize = 25;


int randomCategoryIndex = 0;
char* allCategories[arrayCatSize] = { "Animals", "Video Games" };

char* allWords[arrayCatSize][arrayWordSize] = {
  { "Moose", "Beaver", "Bear", "Goose", "Dog", "Cat", "Squirrel", "Bird", "Elephant", "Horse", "Bull", "Giraffe", "Seal", "Bat", "Skunk", "Turtle", "Whale", "Rhino", "Lion", "Monkey", "Frog", "Alligator", "Kangaroo", "Hippo", "Rabbit" },
  { "Pong", "Minecraft", "Tetris", "Donkey Kong", "Mario Kart", "Mario Party", "Jumpman", "Kirby", "Sonic", "Mega Man", "Metroid", "Just Dance", "Wii Sports", "Contra", "Harry Potter", "Yoshi Island", "Fortnight", "Pacman", "Excite Bike", "Golf", "Kungfu", "Duckhunt", "Zelda", "Pokemon", "Fifa" }
};
char* words[arrayWordSize] = { *allWords[1] };
char category = &allCategories[0];

// the start value in the sequence array must have a value that could never be an index of an array
// or at least a value outside the range of 0 to the size of the words array - 1; in this case, it can't be between 0 to 24
int sequence[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };  //start with an array full of -1's

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);  //set the button pin as an input

  lcd.begin(16, 2);  //tell the LCD library the size of the screen

  generateRandomOrder();  //generate an array of random numbers from 0 to 24 that will determine which order the words are shown in

  showStartSequence();  //print the start sequence text
}

void loop() {

  for (int i = 0; i < arrayWordSize; i++) {  //for each of the 25 words in the sequence

    lcd.clear();  //clear off the array

    roundNumber = i + 1;                                    //the array starts at 0, but the roundNumber will start counting from 1
    lcd.print(roundNumber);                                 //print the roundNumber (this is the current round number)
    lcd.print(": ");                                        //spacer between the number and the word
    lcd.print(allWords[randomCategoryIndex][sequence[i]]);  //print a random word from the word array

    startTime = millis();  //record the time that this round started

    while (digitalRead(buttonPin) == HIGH) {  //do this until the button is pressed...

      int roundedTime = round((timeLimit - (millis() - startTime)) / 1000);  //calculate the time left in the round (dividing by 1000 converts the number to seconds
      lcd.setCursor(14, 1);                                                  //set the cursor in the lower right corner of the screen
      lcd.print("  ");
      lcd.setCursor(14, 1);    //set the cursor in the lower right corner of the screen
      lcd.print(roundedTime);  //print the time left in the time limit
      delay(15);

      if (millis() - startTime > timeLimit) {  //if the time limit is up before the button is pressed
        gameOver();                            //end the game
      }

      if (digitalRead(buttonPin) == LOW) {
        tone(buzzerPin, 272, 10);  //emit a short beep when the button is pressed
      }

    }  //exit this loop when the button is pressed

    delay(500);  //delay for a moment before going onto the next round, so that the button press doesn't get registered twice
  }
  //if you finish all 25 words
  winner();  //show the you win message
}


//--------------FUNCTIONS------------------------------

//DISPLAYS A COUNTDOWN TO START THE GAME
void showStartSequence() {

  lcd.clear();  //clear the screen

  lcd.setCursor(0, 0);     //move the cursor to the top left corner
  lcd.print("Category:");  //print "Category:"

  lcd.setCursor(0, 1);                            //move the cursor to the bottom left corner
  lcd.print(allCategories[randomCategoryIndex]);  //print "Animals:"

  delay(2000);  //Wait 2 seconds

  lcd.clear();              //clear the screen
  lcd.print("Get ready!");  //print "Get ready!"
  delay(1000);              //wait 1 second

  lcd.clear();     //clear the screen
  lcd.print("3");  //print "3"
  delay(1000);     //wait 1 second

  lcd.clear();     //clear the screen
  lcd.print("2");  //print "3"
  delay(1000);     //wait 1 second

  lcd.clear();     //clear the screen
  lcd.print("1");  //print "3"
  delay(1000);     //wait 1 second
}

//GENERATES A RANDOM ORDER FOR THE WORDS TO BE DISPLAYED
void generateRandomOrder() {

  randomSeed(analogRead(0));  //reset the random seed (Arduino needs this to generate truly random numbers

  randomCategoryIndex = random(0, arrayCatSize);

  lcd.clear();
  lcd.write("category:");
  lcd.write(category);

  for (int i = 0; i < arrayWordSize; i++) {  //do this until all 25 positions are filled

    int currentNumber = 0;  //variable to hold the current number
    boolean match = false;  //does the currentNumber match any of the previous numbers?

    //generate random numbers until you've generated one that doesn't match any of the other numbers in the array
    do {
      currentNumber = random(0, arrayWordSize);  //generate a random number from 0 to 24
      match = false;                             //we haven't checked for matches yet, so start by assuming that it doesn't match
      for (int i = 0; i < arrayWordSize; i++) {  //for all 25 numbers in the array
        if (currentNumber == sequence[i]) {      //does the currentNumber match any of the numbers?
          match = true;                          //if so, set the match variable to true
        }
      }
    } while (match == true);      //if the match variable is true, generate another random number and try again
    sequence[i] = currentNumber;  //if the match variable is false (the new number is unique) then add it to the sequence
  }
}

//GAME OVER
void gameOver() {
  lcd.clear();  //clear the screen

  lcd.setCursor(0, 0);     //move the cursor the top left corner
  lcd.print("Game Over");  //print "Game Over"

  lcd.setCursor(0, 1);         //move to the bottom row
  lcd.print("Score: ");        //print a label for the score
  lcd.print(roundNumber - 1);  //print the score (the score is equal to the previous level/round number)

  tone(buzzerPin, 1318, 150);  //E6
  delay(175);

  //play the losing fog horn
  int i = 3000;
  int p = 30;

  while (i > 1000 && p < 3000) {
    i = i - 20;
    p = p + 10;

    tone(buzzerPin, i, 25);  //E6
    delay(15);
    tone(buzzerPin, p, 25);  //E6
    delay(15);
  }

  tone(buzzerPin, 1023, 1000);  //E6
  delay(150);
  tone(buzzerPin, 1010, 1500);  //E6

  while (true) {}  //get stuck in this loop forever
}

//WINNER
void winner() {
  lcd.clear();  //clear the screen

  lcd.setCursor(7, 0);  //move the cursor to the top center of the screen
  lcd.print("YOU");     //print "You"

  lcd.setCursor(7, 1);  //move the cursor to the bottom center of the screen
  lcd.print("WIN!");    //print "WIN!"


  //play the losing fog horn
  int i = 3000;
  int p = 30;

  while (i > 1000 && p < 3000) {
    i = i - 10;
    p = p + 30;

    tone(buzzerPin, i, 25);  //E6
    delay(15);
    tone(buzzerPin, p, 25);  //E6
    delay(15);
  }

  tone(buzzerPin, 1150, 750);  //E6
  delay(150);
  tone(buzzerPin, 1150, 750);  //E6
  delay(150);
  tone(buzzerPin, 1240, 1500);  //E6

  while (true) {}  //get stuck in this loop forever
}
