#include <Servo.h>

// ====================================================================
// HARDWARE DECLARATIONS & CONFIGURATION
// ====================================================================
Servo lidServo;   // Servo 1 -> Physical Pin 9
Servo armServo;   // Servo 2 -> Physical Pin 10
Servo baseServo;  // Servo 3 -> Physical Pin 6

const int switchPin = 2;

// Safety and positioning constants
const int LID_CLOSED = 90;      // Lid doesn't engage until 90 degrees
const int LID_SAFE_OPEN = 180;  // Fully open to clear the arm sweep
const int LID_PEEK = 135;       // Just open enough to crack the lid slightly

const int ARM_HOME = 220;       // Retracted completely inside the box
const int ARM_FAKE = 60;        // Extends out to tease the user
const int ARM_STRIKE = 0;       // Moves down to 0 to fully extend and hit the switch

const int BASE_CENTER = 90;

// Logic control & Idle Timer
bool firstRun = true;                        // Forces a standard baseline on flip #1
unsigned long lastActivityTime = 0;          // Tracks the last time the box moved
const unsigned long IDLE_TIMEOUT = 180000UL; // 3 minutes (3 * 60 * 1000 ms)

// ====================================================================
// SETUP & CALIBRATION
// ====================================================================
void setup() {
  Serial.begin(9600); // Mandatory 9600 baud rate
  Serial.println(F("--- Useless Box System Initializing ---"));

  pinMode(switchPin, INPUT_PULLUP);
  
  Serial.println(F("Calibrating starting positions sequentially..."));
  
  // 1. Initialize Lid Servo (Pin 9)
  lidServo.write(LID_CLOSED);  
  lidServo.attach(9);  
  delay(300);          // Power buffering delay
  
  // 2. Initialize Arm Servo (Pin 10)
  armServo.write(ARM_HOME); 
  armServo.attach(10); 
  delay(300);          
  
  // 3. Initialize Base Servo (Pin 6)
  baseServo.write(BASE_CENTER);  
  baseServo.attach(6);  
  delay(300);          
  
  // Seed the random generator using electrical noise from an empty analog pin
  randomSeed(analogRead(A0));
  
  // Start the 3-minute idle clock
  lastActivityTime = millis();
  
  Serial.println(F("System Ready. Flip the switch to begin."));
}

// ====================================================================
// MAIN LOOP
// ====================================================================
void loop() {
  // 1. Check if switch was flipped LOW (ON)
  if (digitalRead(switchPin) == LOW) {
    Serial.println(F("\n>> Event Triggered: Switch flipped ON."));
    delay(200); // Small debounce

    // Handle the first run experience
    if (firstRun) {
      Serial.println(F("Executing First Run: Standard Clean Combo"));
      routineStandard();
      firstRun = false; // Disable so subsequent flips use random personalities
    } 
    else {
      // Pick a random routine between 1 and 11 (upper bound is exclusive)
      int routineChoice = random(1, 12); 
      Serial.print(F("Executing Routine #"));
      Serial.println(routineChoice);

      switch (routineChoice) {
        case 1:  routineTheHesitantPeek(); break;
        case 2:  routineTheAbsoluteRage(); break;
        case 3:  routineTheSneakAttack(); break;
        case 4:  routineTheFakeOut(); break;
        case 5:  routineTheSleepyBox(); break;
        case 6:  routineTheWrongWayConfusion(); break;
        case 7:  routineTheMachineGun(); break;
        case 8:  routineTheSmoothCriminal(); break;
        case 9:  routineTheDoubleTake(); break;
        case 10: routineTheMorseTapper(); break;
        case 11: routineTheSlowMoThreat(); break;
      }
    }
    
    // Safety Fallback: Ensure everything pulls back safely and resets
    safeRetract();
    
    // Safety hold: block execution until the physical switch returns to OFF
    while(digitalRead(switchPin) == LOW) {
      delay(10); 
    }
    Serial.println(F("Switch reset to OFF."));
    
    // Reset the idle timer after user interaction completes
    lastActivityTime = millis();
  }
  
  // 2. Check if the box has been sitting idle for 3 minutes
  else if (millis() - lastActivityTime >= IDLE_TIMEOUT) {
    Serial.println(F("\n>> Idle Reminder (3 mins): Running basic routine..."));
    routineStandard();
    safeRetract();
    
    // Reset the clock for the next 3-minute interval
    lastActivityTime = millis();
  }
}

// ====================================================================
// SAFETY & HELPER FUNCTIONS
// ====================================================================

// Safely move the arm only if the lid is completely open
void safeArmMove(int targetAngle) {
  if (targetAngle < ARM_HOME) {
    lidServo.write(LID_SAFE_OPEN);
    delay(450); // Concrete delay allowing lid to physically clear the sweep area
  }
  armServo.write(targetAngle);
}

// Bring everything back to a known home state safely
void safeRetract() {
  armServo.write(ARM_HOME); // Pull arm straight back to 220
  delay(400);               // Allow time for arm to clear the lid radius before closing
  lidServo.write(LID_CLOSED);
  baseServo.write(BASE_CENTER);
  delay(500);               // Gives the base servo time to physically return to center
}

// ====================================================================
// PERSONALITY ROUTINES
// ====================================================================

// Baseline Routine: Normal, boring, clean combo
void routineStandard() {
  lidServo.write(LID_SAFE_OPEN);
  delay(500);
  armServo.write(ARM_STRIKE);
  delay(400); 
}

// 1. The Hesitant Peek (The Shy / Stutter Personality)
void routineTheHesitantPeek() {
  delay(800);
  
  // 1. Opens slightly, then closes
  lidServo.write(LID_PEEK);
  delay(400);
  lidServo.write(LID_CLOSED);
  delay(400);
  
  // 2. Opens, then closes
  lidServo.write(LID_PEEK);
  delay(400);
  lidServo.write(LID_CLOSED);
  delay(500);
  
  // 3. Opens and closes FAST
  lidServo.write(LID_PEEK);
  delay(150);
  lidServo.write(LID_CLOSED);
  delay(300);
  
  // 4. Nervous base wobble
  baseServo.write(180); delay(120);
  baseServo.write(0);   delay(120);
  baseServo.write(BASE_CENTER); 
  delay(400);
  
  // 5. Opens and triggers the arm for the final blow
  safeArmMove(ARM_STRIKE); 
  delay(400);
}

// 2. The Absolute Rage (The Angry Personality)
void routineTheAbsoluteRage() {
  // Rapid violent body shaking
  for(int i = 0; i < 3; i++) {
    baseServo.write(180); delay(100);
    baseServo.write(0);   delay(100);
  }
  baseServo.write(BASE_CENTER);
  
  // Instant maximum speed strike down to 0
  safeArmMove(ARM_STRIKE); 
  delay(300);
}

// 3. The Sneak Attack (The Ninja Personality)
void routineTheSneakAttack() {
  delay(2500); // Long psychological pause
  safeArmMove(ARM_STRIKE);
  delay(300);
}

// 4. The Fake-Out (The Tease Personality)
void routineTheFakeOut() {
  lidServo.write(LID_SAFE_OPEN);
  delay(400);
  
  // Extend arm out to tease
  safeArmMove(ARM_FAKE); 
  delay(600); 
  
  // Retreat completely back to 220 and close back up
  armServo.write(ARM_HOME);
  delay(300);
  lidServo.write(LID_CLOSED);
  delay(800);
  
  // The real swift attack down to 0
  safeArmMove(ARM_STRIKE);
  delay(400);
}

// 5. The Sleepy / Exhausted Box (Heavy Eyelids + Wind-Up Strike + Slow Retract)
void routineTheSleepyBox() {
  delay(600); // Heavy pause before waking up
  
  // Slowly creep open halfway (to 140)
  for (int angle = LID_CLOSED; angle <= 140; angle += 2) {
    lidServo.write(angle);
    delay(25);
  }
  delay(500); // Pause like it's staring sleepily
  
  // Droop back down slightly (to 110) like nodding off
  for (int angle = 140; angle >= 110; angle -= 1) {
    lidServo.write(angle);
    delay(30);
  }
  delay(700); // Brief "fell back asleep" pause
  
  // Reluctantly push the lid all the way open to 180
  for (int angle = 110; angle <= LID_SAFE_OPEN; angle += 2) {
    lidServo.write(angle);
    delay(20);
  }
  delay(300);
  
  // Lazily extend the arm until it touches/nudges the switch (20°)
  for (int angle = ARM_HOME; angle >= 20; angle -= 2) {
    armServo.write(angle);
    delay(20);
  }
  delay(350); // Pause against the switch ("ugh, too heavy...")
  
  // Back off partway to wind up (to 85°)
  armServo.write(85);
  delay(300);
  
  // Quick snap strike all the way to 0° with full momentum
  armServo.write(ARM_STRIKE);
  delay(500); // Hold briefly to ensure the switch trips
  
  // Slowly, exhaustedly drag the arm back inside to ARM_HOME (220°)
  for (int angle = ARM_STRIKE; angle <= ARM_HOME; angle += 2) {
    armServo.write(angle);
    delay(20);
  }
  delay(300);
}

// 6. The "Wrong Way" Confusion
void routineTheWrongWayConfusion() {
  lidServo.write(LID_SAFE_OPEN);
  delay(400);
  
  // Confused body tilting left/right while lid sits open empty
  baseServo.write(180); delay(300);
  baseServo.write(0);   delay(300);
  baseServo.write(BASE_CENTER); delay(400);
  
  // Strike down accurately to 0
  safeArmMove(ARM_STRIKE);
  delay(400);
}

// 7. The Machine Gun (The Spastic Personality + Rapid Base Shuffle)
void routineTheMachineGun() {
  // Open the lid FULLY first so the arm has a clear runway
  lidServo.write(LID_SAFE_OPEN);
  delay(400);
  
  // Rapid base shuffle paired with frantic mid-air arm jabs
  for(int i = 0; i < 4; i++) {
    armServo.write(40);   // Arm jabs forward
    baseServo.write(150); // Base snaps right
    delay(110);
    
    armServo.write(140);  // Arm pulls back halfway
    baseServo.write(45);  // Base snaps left (kept at 45 to stay clear of the 30° collision)
    delay(110);
  }
  
  baseServo.write(BASE_CENTER); // Snap base back to center before the final blow
  delay(200);
  
  // Final rapid knockout punch down to 0 to flip the switch
  armServo.write(ARM_STRIKE);
  delay(300);
}

// 8. The Smooth Criminal (The Casual Personality)
void routineTheSmoothCriminal() {
  // Smooth posture lean
  baseServo.write(150); delay(400); 
  
  // Smooth, fluid lid glide from 90 to 180
  for (int angle = LID_CLOSED; angle <= LID_SAFE_OPEN; angle += 5) {
    lidServo.write(angle);
    delay(10);
  }
  delay(200);
  
  armServo.write(ARM_STRIKE);
  delay(500);
}

// 9. The Dramatic Double-Take (Startled Reaction)
void routineTheDoubleTake() {
  lidServo.write(LID_SAFE_OPEN);
  delay(400);
  
  // Reach partway out, then freeze like it noticed the user watching
  armServo.write(ARM_FAKE);
  delay(450);
  
  // Startled high-side base jolt (stays in safe >= 90° direction)
  baseServo.write(165);
  delay(180);
  baseServo.write(BASE_CENTER);
  delay(250);
  
  // Slam shut and hide
  armServo.write(ARM_HOME);
  delay(300);
  lidServo.write(LID_CLOSED);
  delay(700);
  
  // Pop back up and finish the job
  safeArmMove(ARM_STRIKE);
  delay(350);
}

// 10. The Morse Code Tapper (Rhythmic Impatience)
void routineTheMorseTapper() {
  delay(500);
  
  // Dot - Dot - Dash lid taps (short, short, held open)
  for (int i = 0; i < 2; i++) {
    lidServo.write(LID_PEEK);
    delay(130);
    lidServo.write(LID_CLOSED);
    delay(130);
  }
  
  // Long "dash" glare
  lidServo.write(150);
  delay(650);
  lidServo.write(LID_CLOSED);
  delay(300);
  
  // Fling open and strike
  safeArmMove(ARM_STRIKE);
  delay(350);
}

// 11. The Slow-Motion Threat (Intimidating Creep)
void routineTheSlowMoThreat() {
  // Open lid smoothly
  for (int angle = LID_CLOSED; angle <= LID_SAFE_OPEN; angle += 3) {
    lidServo.write(angle);
    delay(12);
  }
  
  // Lean confidently in the safe high-angle direction
  baseServo.write(155);
  delay(300);
  
  // Creep the arm out slowly until hovering right on the switch (20°)
  for (int angle = ARM_HOME; angle >= 20; angle -= 3) {
    armServo.write(angle);
    delay(18);
  }
  
  // Menacing hover pause right before contact
  delay(650);
  
  // Level the base so the strike hits square, then drive fully to 0°
  baseServo.write(BASE_CENTER);
  delay(150);
  armServo.write(ARM_STRIKE);
  delay(550); // Extra dwell time to guarantee the switch fully trips before retracting
}