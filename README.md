# PSoC 5 Digital Clock — Event-Driven Architecture with Active Objects

This project implements a modular digital clock system on a **PSoC 5 microcontroller** using an **event-driven architecture** inspired by **AUTOSAR RTE (Runtime Environment)** principles. It was developed as part of a **Master's course** on embedded systems architecture.

The system uses **hierarchical state machines** and **active objects** to separate input processing, control logic, and display output. It features:

- Time display on a TFT screen  
- Hour and minute editing using buttons  
- Long-press detection and auto-increment  
- Debounced button input handling  
- Clean architecture separation into container and widget layers

---

## Architecture Overview

The clock logic is structured using two layers:

### 1. Container & Widget Hierarchical States

![Exercise_6 3 1](https://github.com/user-attachments/assets/6b65b2a9-a25f-40f5-8eff-b4644814bc82)


- The **container state machine** manages the high-level editing modes (`IsDisplaying`, `EditingHours`, `EditingMinutes`).
- Each **widget (Hour, Minute)** has its own local state machine (`Focus`, `AutoIncrement`) to manage interaction detail.

---

###  2. Software Component (SWC) View

![Exercise_6 3 2](https://github.com/user-attachments/assets/8c4c9aa3-6a83-413b-a393-66b3fd4788f8)


The application is split into three periodic runnables:
- `CLOCK_input_run()` — handles debounced button input and event generation  
- `CLOCK_control_run()` — processes state transitions and active object logic  
- `CLOCK_display_run()` — renders the UI and handles blinking/focus highlighting

---

## Hardware Platform

This clock runs on a **Cypress PSoC 5LP** (CY8CKIT-059) with the following components:

- TFT Display for time visualization  
- Tactile buttons for input  
- Custom breakout board with additional ICs and indicators

![IMG_7890](https://github.com/user-attachments/assets/ad429c80-460f-4c90-8636-633d43339485)


---

## Context

These lab projects were developed as part They form part of the International Master's in Electrical Engineering program.


## Educational Context

This lab project was developed as part of the *Embedded Architectures and Applications* course (Prof. Dr. P. Fromm) at Hochschule Darmstadt (h_da), Germany. 
It was designed to demonstrate key concepts such as:

- The use of an **AUTOSAR RTE-inspired architecture** to structure signal and event communication between software components  
- The design and implementation of a **complex state machine** using hierarchical states for both container and widget logic  
- The application of the **look-up table pattern for state machines** to create scalable and maintainable control flow  
- The practical use of **active objects** to encapsulate behavior, manage internal state, and process events autonomously  
- The integration of a **TFT display** for visualizing real-time clock output and user interactions


---

##  Technologies Used

- C (using **Erika Enterprise RTOS**)
- PSoC Creator IDE
- Custom signal/event handling via AUTOSAR RTE-style components
- State machines (manually coded, table-driven)
- SPI-based TFT display interface

---

