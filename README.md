# AVR Triac Regulator (ATtiny13)

---

## ⚠️ Safety Disclaimer

This project involves **AC mains voltage**, which is **extremely dangerous** and can cause **serious injury or death**.  

By using this project, you accept **full responsibility** for any accidents, injuries, or damages. The author is **not liable** for any consequences resulting from its use.  

**Important Safety Guidelines:**
- Always use insulated tools and protective equipment.
- Work with a qualified professional if you are inexperienced with mains circuits.
- Use galvanic isolation (optotriacs/optocouplers) wherever possible.
- **Do not proceed** if you are unfamiliar with handling high voltage.

---

## Project Overview

This project implements a **power regulator for universal commutator motors** (motors with brushes) and resistive loads (e.g., light bulbs and heating elements) using the **ATtiny13** microcontroller.  
It provides **0–100% power control** through **phase-angle control of a triac** with **AC mains zero-cross detection**.

⚠️ **Warning:** The circuit connects directly to **230 V mains**. Extreme caution is required during assembly and testing.

---

## ✨ Features
- Measurement of the delay between zero-crossing and triac triggering.
- Adjustable firing angle using a potentiometer (or DIP switch).
- Generation of a trigger pulse for the optotriac in every half cycle.
- Motor power control from **fully OFF (0%)** to **fully ON (100%)**.

---

## 🧩 Components

### Triac driver (optotriac)
Must be of the **random-phase type** (no built-in zero-cross detection), e.g.:
- `MOC3021`
- `MOC3010`

### Zero-cross detector
The output must be a **rectangular pulse signal** for reliable edge detection by the MCU, e.g.:
- `H11L1` (recommended)

If using slower optocouplers such as `4N35`, you must add a **Schmitt trigger** circuit to clean up the signal.

### Microcontroller
- `ATtiny13` (or alternatively `ATmega8`)

### Firing angle adjustment
- **Potentiometer** (connected to ADC input)  
- or **DIP switch** (digital inputs)

---

## ⚡ Safety notes
- This regulator is suitable only for **universal commutator motors (brush motors)** and **resistive loads** (e.g., light bulbs and heating elements).
- It is **not suitable for asynchronous (induction) motors**.  
- The motor must **not** have its own built-in electronic regulation.  
- The motor must use **series excitation**.

---

## 🛠️ Project directory structure

- **HW/** – Eagle PCB design files (.sch, .brd)
- **SW/**  - Atmel Studio project files (.cproj, .atsln), Doxyfile
  - **src/** – Source files (.c)
  - **inc/** – Header files (.h)
  - **docs/** – Generated documentation (Doxygen)

---

## ⚙️ Fuse and clock configuration

- Set fuses for **internal oscillator 4.8 MHz**:  
  - **Low fuse:** `0x79`  
  - **High fuse:** `0xFF`  
- ⚠️ **Calibration is required** – see ATtiny13 datasheet for details.  
- Note: `clk I/O` = `clk CPU`. The difference is that during sleep mode, **I/O clocks are disabled**, while CPU/peripheral clocks may continue to run.

--- 

## 🔧 Building the Project
To build the project, launch the **Regulator.atsln** file with **Microchip Studio (version 7.0.2594)**.

- From the menu bar, select Build > Build Solution.
-  Alternatively, use the shortcut F7.

The output files will be located in the project's **Debug** or **Release** folder.