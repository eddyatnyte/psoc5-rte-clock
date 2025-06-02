# ⏰ PSoC 5 Digital Clock — Event-Driven Architecture with Active Objects

This project implements a modular digital clock system on a **PSoC 5 microcontroller** using an **event-driven architecture** inspired by **RTE (Runtime Environment)** principles. It was developed as part of a **Master's course** on embedded systems architecture.

The system uses **hierarchical state machines** and **active objects** to separate input processing, control logic, and display output. It features:

- Time display on a TFT screen  
- Hour and minute editing using buttons  
- Long-press detection and auto-increment  
- Debounced button input handling  
- Clean architecture separation into container and widget layers

---

## 🧭 Architecture Overview

The clock logic is structured using two layers:

### 🧱 1. Container & Widget Hierarchical States

![Exercise_6 3 1](https://github.com/user-attachments/assets/6b65b2a9-a25f-40f5-8eff-b4644814bc82)


- The **container state machine** manages the high-level editing modes (`IsDisplaying`, `EditingHours`, `EditingMinutes`).
- Each **widget (Hour, Minute)** has its own local state machine (`Focus`, `AutoIncrement`) to manage interaction detail.

---

### ⚙️ 2. Software Component (SWC) View

![Exercise_6 3 2](https://github.com/user-attachments/assets/8c4c9aa3-6a83-413b-a393-66b3fd4788f8)


The application is split into three periodic runnables:
- `CLOCK_input_run()` — handles debounced button input and event generation  
- `CLOCK_control_run()` — processes state transitions and active object logic  
- `CLOCK_display_run()` — renders the UI and handles blinking/focus highlighting

---

## 🔧 Hardware Platform

This clock runs on a **Cypress PSoC 5LP** (CY8CKIT-059) with the following components:

- TFT Display for time visualization  
- Tactile buttons for input  
- Custom breakout board with additional ICs and indicators

![IMG_7890](https://github.com/user-attachments/assets/ad429c80-460f-4c90-8636-633d43339485)


---

## 📚 Educational Context

This project was created for a Master's-level embedded systems course. Its purpose was to demonstrate:

- Practical application of **active object patterns**  
- Use of **state machine composition** for embedded control  
- Modular separation between input, logic, and display layers  
- Real-time interaction on a resource-constrained platform

---

## 🛠️ Technologies Used

- C (bare-metal, no RTOS)
- PSoC Creator IDE
- Custom signal/event handling via RTE-style components
- State machines (manually coded, table-driven)
- SPI-based TFT display interface

---

