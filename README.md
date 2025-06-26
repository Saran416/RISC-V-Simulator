# Cache Simulator Web App

## Overview

This project is a web application built around a C++ cache simulator. The simulator models the behavior of a cache memory system and supports RISC-V instructions. The web app provides an interactive interface for users to configure the cache, execute instructions, and visualize the state of registers, memory, and cache.

## Demo Images

![Editor](/demo/1.png)
![Memory](/demo/2.png)
![Cache](/demo/3.png)

## Features

### Simulator (Backend)
- Written in C++.
- Supports all basic RISC-V instructions of types R, I, S, etc.
- Implements 32 registers (x0 to x31) and byte-addressable memory.
- Configurable cache with parameters such as:
  - Cache size
  - Block size
  - Replacement policy
  - Write policy
  - Associativity
- Backend commands:
  - **Run**: Executes the instructions.
  - **Step**: Executes one instruction at a time.
  - **Reset**: Resets the simulator.

### Web App (Frontend)
- Built with React using Context API for state management.
- **Pages**:
  1. **Code Editor and Registers**:
     - Code editor with line numbers and line highlighter.
     - Buttons to send API requests for `run`, `step`, and `reset` commands.
     - Display area for logs and error messages.
     - Sidebar to show register states (x0 to x31).
  2. **Memory**:
     - Displays the current state of memory.
  3. **Cache Configuration**:
     - Allows users to set cache parameters like size, block size, replacement policy, etc.
- Local storage is used to save the code, registers, cache, and memory contexts.

### Server
- Built with Node.js and Express.
- **API Endpoints**:
  1. **`/getData`**:
     - **Input**: `code`, `pc`, `arg` (e.g., `run` or `step`), and cache configuration.
     - **Output**: Logs, register states, memory state, final program counter (PC), and cache statistics.
     - Uses the `child_process` module to execute the C++ program and fetch results via `stdout`.
  2. **`/getHex`**:
     - **Input**: Code from the editor.
     - **Output**: Hex codes of the instructions.


## Installation

You can create your own local copy of the simulator by following the below steps:

### Prerequisites
- Node.js
- C++ compiler (e.g., `g++`)
- React (included in the project setup)

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/Saran416/RISC-V-Simulator.git
   cd RISC-V-Simulator
   ```

2. Install backend dependencies:
   ```bash
   cd backend
   npm install
   ```

3. Compile the C++ Simulator
   ```bash
   cd Simulator
   make all
   ```

4. Build and start the server:
   ```bash
   node index.js
   ```

5. Install frontend dependencies:
   ```bash
   cd ...
   cd frontend
   npm install
   ```

6. Start the React development server:
   ```bash
   npm run dev
   ```

7. Run the application in your browser at `http://localhost:5173`.

## Project Structure

```

├── frontend/          # React frontend
│   ├── src/
│   ├── public/
│   └── package.json
├── backend/            # Node.js backend
│   ├── routes/
│   ├── index.js
│   ├── package.json
│   └── Simulator/
│       ├── Simulator.cpp   # for Cache simulation
│       └── Assembler.cpp   # for hex codes
└── README.md
```

## Usage

1. Open the web app in your browser.
2. You will be directed to the home page:
   - Write or paste your RISC-V assembly code.
   - Use the `Run`, `Step`, or `Reset` buttons to interact with the simulator.
   - View logs, errors, and register states.
3. Switch to the **Memory** page to view memory states.
4. Use the **Cache Configuration** page to set up the cache parameters.

## Contributors

A collaborative projects of [lakshsidhu04](https://github.com/lakshsidhu04) and [Saran416](https://github.com/Saran416)
