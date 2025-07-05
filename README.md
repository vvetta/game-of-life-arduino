# game-of-life-arduino
A handheld Arduino-based version of Conway's Game of Life with an OLED display, custom controls, and battery-powered design.

---

## About the Project

I am a participant in the educational program "School 21," where I am currently learning the C programming language. One of the assignments was to create a cellular automaton — Conway’s Game of Life — in C using the `ncurses` library. I found this task quite interesting and decided to take it a step further: since I enjoy building things with my own hands, I chose to implement the Game of Life as a portable game console based on Arduino.

## Component Selection

For this project, I chose the **Arduino UNO** board. I picked it because of its availability and ease of use — no soldering is required to build the basic circuit. Although I originally hoped to avoid using a soldering iron entirely, I ended up needing it after all.

First, I planned out which components my game console would include:

- **Display**: To render the game grid, I chose an SSD1306 OLED display with a resolution of 128x64 pixels. It uses the I2C interface, is simple to connect, and is quite inexpensive.
    
- **Controls**: I decided to implement a D-pad for moving the cursor around the field, and a separate button to place or remove cells. In total, I used 5 tactile buttons and 6 resistors (220 Ohms each).
    
- **Power supply**: I didn’t want to just plug the Arduino into a power bank — I wanted the device to have its own battery. For this, I used a 18650 lithium battery, a TP4056 charging board, and a DC-DC boost converter (3.7V → 5V). I also added a power switch.
    
- **Enclosure**: Since I don’t own a 3D printer, there is no proper case. The frame is made of two solderless breadboards, connected with plastic zip ties. All components are mounted directly onto the boards. The power button is glued on with superglue.
    

## Wiring the Components

### Display

Connecting the display is extremely simple (the schematic might show a different display model, but the principle is the same):

![[Screenshot from 2025-07-04 13.13.50.png]]

|Arduino|SSD1306 Display|
|---|---|
|5V|VCC|
|GND|GND|
|A5|SCL|
|A4|SDA|

This is the standard setup for I2C interface devices.

### Control Buttons

This was probably the most challenging part of the project for me. I wasn’t exactly a regular attendee in physics class 😅. I wanted to connect all five buttons to **a single** Arduino pin to avoid wasting multiple digital pins.

After researching possible solutions, I settled on using a **voltage divider**:

![[Screenshot from 2025-07-04 13.27.56.png]]

When no button is pressed, current flows through all the resistors. When a button is pressed, a part of the circuit is bypassed, which changes the voltage. By measuring this voltage, I can determine which button was pressed.

Here’s an example with the first button pressed:

![[Pressed first button.png]]

If the second button is pressed, the circuit changes in a similar way:

![[Pressed second button.png]]

If multiple buttons are pressed at once, the system detects **only the one** that is **closest to ground**. For example:

![[Pressed five button.png]]

This is important to consider when developing your logic.

### Power Supply

As I mentioned earlier, I wanted the device to be **completely standalone**. The final power circuit looks like this:

![[Screenshot From 2025-07-05 13-06-31.png]]

Soldering was required here, but it was fairly simple — I only had to solder a couple of wires.

### Full Wiring Diagram

![[Full_scheme.png]]

With the hardware part completed, let’s move on to the software!

---

## Software

I won’t include the full code here, but I’ll describe the key points. You’ll be able to find the complete project in the repository.

### Button Handling

Button input is handled by the `int getButton()` function. It reads the analog value from pin A0 and determines which button was pressed based on predefined ranges.

```c
int getButton() {
	int analogValue = analogRead(A0);

	if (analogValue >= 820) return 5;
	if (analogValue >= 400) return 4;
	if (analogValue >= 300) return 3;
	if (analogValue >= 230) return 2;
	if (analogValue >= 190) return 1;

	return 0;
}
```

The values were determined empirically — they may vary depending on your specific resistors. To calibrate them, you can temporarily print `analogValue` to the serial monitor and log the values for each button.

#### Debouncing Issue

One common problem when working with buttons is **contact bounce**. Arduino might detect multiple presses instead of just one. To fix this, I added a simple debounce mechanism using a timer:

```c
int button = getButton();
unsigned long currentTime = millis();

if (button != 0 && (currentTime - lastButtonPressTime) > DEBOUNCE_DELAY) {
	lastButtonPressTime = currentTime;
	lastInputTime = currentTime;
}
```

The idea is simple — a button is only registered if enough time (`DEBOUNCE_DELAY`) has passed since the last press. This isn’t the most elegant solution, but it works reliably.

---

### Operating Modes

The game has two modes:

- **Edit mode**: You can move the cursor around the field and use button #5 to place or remove cells.
    
- **Simulation mode**: After setting up your configuration, the simulation automatically starts after 5 seconds. If you press any button during the simulation, it will stop and return to edit mode.
    

---

## My Device

Front view:

![[Screenshot From 2025-07-05 14-24-52.png]]

Back view:

![[Screenshot From 2025-07-05 14-25-17.png]]

## Conclusion

This project is perfect for beginners who are just getting started with Arduino and want to go beyond writing console applications to building real, physical devices they can hold in their hands.

In the future, I plan to move the project into a proper enclosure, use a printed circuit board, and switch to an Arduino Nano to make the device more compact.
