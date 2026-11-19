# Multi-Personality Arduino Useless Box

An animatronic "Useless Box" powered by an Arduino Nano and three hobby servos (Lid, Arm, and Tilt Base). Instead of simply turning itself off, the box reacts with **11 distinct randomized behaviors**, collision-safe motion guards, and an automated idle reminder.

## Features

* **First-Run Baseline:** Executes a clean, standard open-and-strike combo on the very first flip after power-on before unlocking the randomized personalities.
* **11 Randomized Personalities:**
  1. **The Hesitant Peek:** Stutters the lid open and closed nervously, wobbles the base, and finally commits to the switch.
  2. **The Absolute Rage:** Violently shakes the entire box back and forth before slamming the switch off.
  3. **The Sneak Attack:** Waits in total silence for 2.5 seconds to lull the user into thinking it's broken, then strikes instantly.
  4. **The Fake-Out:** Extends the arm partway out to hover over the switch, retreats and closes the lid, then pops back up for the real hit.
  5. **The Sleepy / Exhausted Box:** Slowly creeps the lid open like heavy eyelids, nods off, pushes the arm lazily against the switch, backs off to wind up for momentum, strikes, and slowly drags itself back inside.
  6. **The "Wrong Way" Confusion:** Opens the lid and tilts side-to-side looking around before locating the switch.
  7. **The Machine Gun:** Fires rapid mid-air arm jabs paired with a fast base shuffle before delivering the knockout blow.
  8. **The Smooth Criminal:** Leans smoothly into a confident posture, glides the lid open, and casually flips the switch.
  9. **The Dramatic Double-Take:** Reaches out, freezes with a startled base jolt as if caught in the act, slams shut, and finishes the job.
  10. **The Morse Code Tapper:** Taps out a rhythmic *dot-dot-dash* impatience sequence with the lid before striking.
  11. **The Slow-Motion Threat:** Creeps the arm out degree-by-degree until hovering millimeters from the switch, pauses for tension, levels out, and snaps down.
* **Mechanical Collision Protection:** Sequential servo initialization, `safeArmMove()`, and `safeRetract()` helpers ensure the lid clears the arm sweep path and prevent low-angle base tilts from binding against the retracted arm.
* **3-Minute Idle Reminder:** Uses non-blocking `millis()` tracking to perform a brief "hey, I'm still turned on" lid-and-arm wave every 3 minutes of inactivity so you don't accidentally drain the batteries.

## Hardware & Wiring

| Component | Arduino Pin | Notes |
| :--- | :--- | :--- |
| **Lid Servo** | `D9` | `90°` Closed / `135°` Peek / `180°` Open |
| **Arm Servo** | `D10` | `220°` Home (Retracted) / `60°` Tease / `0°` Strike |
| **Base Tilt Servo** | `D6` | `90°` Center (`45°`–`180°` safe active sweep) |
| **Toggle Switch** | `D2` | Wired to `GND` using internal `INPUT_PULLUP` (`LOW` = ON) |
| **RNG Seed** | `A0` | Left floating for `randomSeed(analogRead(A0))` |

> **Power Note:** Driving three servos simultaneously (especially during rapid shake routines) causes high current spikes. Use an external 5V–6V power supply dedicated to the servo rail with a shared ground to the Arduino, plus a bulk decoupling capacitor (e.g., `1000µF–2200µF`) across the servo power rails.

## Setup & Calibration

1. Open the `.ino` sketch in the Arduino IDE and select your board (e.g., **Arduino Nano**).
2. Verify your servo linkage geometry before mounting horns permanently:
   * `LID_CLOSED` (`90`) should rest flush without stalling the lid servo against the frame.
   * `ARM_HOME` (`220`) should tuck cleanly inside the box without colliding with the base tilt mechanism.
   * `ARM_STRIKE` (`0`) should comfortably throw the toggle switch to the `OFF` position without over-driving the linkage.
3. Upload the sketch and open the Serial Monitor at **`9600` baud** to view live routine logs and state transitions.
