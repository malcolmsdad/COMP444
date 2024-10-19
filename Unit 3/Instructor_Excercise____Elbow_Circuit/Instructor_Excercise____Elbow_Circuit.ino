/*
 Controlling a piston position using a trigonometry
 by Tim Blake

 Units 3 - Robotic Arm Design: Elbow Circuit

 We want to be able to bend an elbow by specifying degrees "D", but since we're using a piston / linear actuator along "C" then we must calculate the length of C to move.  
 Unfortunately, that also means that we need to know the values of A and B.  I'll assign const values for those in the code.

A quick google, and I was able to find this calculation " Law of Cosines " to calculate the length:
c2 = b2 + a2 – 2ab cos γ

     / \
 a  /   \    c
   /     \ 
  /       \
 /  y      \
  ----------
     	b

  a = lower arm link
  b = upper arm link    
  c = length of piston  (controlled by the code below)
  y = angle of elbow    (controlled by potentiometer)

*/

class Piston
{
  private:
    int _length; // current length of the piston
    int _pin;    // pin piston pump control is connected to
  public: 
    int length(){ return _length; }
    void moveBy(int len) { _length += len; }
    void attach(int attachPin) { _pin = attachPin; }
};


Piston mypiston;  // create piston object to control a piston

int potpin = A0;  // analog pin used to connect the potentiometer
int val;          // length of the piston
int currentLength;// current length of the piston
int desiredLength;// desired length the user wants the elbow to move to...

const int A;      // the length of the robots lower arm
const int B;      // the length of the robots upper arm

void setup() {
  mypiston.attach(9);  // attaches the piston on pin 9 to the piston object
}

int calculateLength(int angleY){
  //c2 = b2 + a2 – 2ab cos γ
  double cVal = (B * B) + (A * A) - 2 * A * B * cos(angleY);
  return sqrt(cVal);
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);     // scale it for use with the servo (value between 0 and 180)

  currentLength = mypiston.length();      // get the current length of the piston
  desiredLength = calculateLength(val);   // use trig function to calculate how long the piston should be

  mypiston.moveBy(desiredLength - currentLength); // move the piston by the difference between current length and desired.
  
  delay(15);                           // waits for the servo to get there
}

