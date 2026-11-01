# Digital Logic Circuit Simulator

## Brief Description

Digital Logic Circuit Simulator is a C++ desktop application project intended to help users explore, design, evaluate, and validate digital logic circuits and Boolean expressions through a graphical interface. The project combines a Qt-based user interface with a C++ logic-processing core.

The project is under development. The availability and completeness of individual tools depend on their current implementation.

## Problem Statement / Objective

Physical digital logic experiments often depend on breadboards and integrated circuits (ICs), which can be affected by loose connections, damaged components, and other hardware faults. Manually calculating truth tables and simplifying Boolean expressions can also be time-consuming.

The objective of Digital Logic Circuit Simulator is to provide a software environment for working with digital logic, evaluating Boolean expressions, exploring logic-design concepts, and developing circuit-validation tools without requiring physical hardware for every experiment.

## Team Members

- **Archit Jirwan:** Team Lead & Circuit Router (Graph DAGs)
- **Arnav Negi:** System Architect (Logic Gate OOP)
- **Ishaan Negi:** Developer (Expression Parser via Stacks)
- **Gurpindar Singh:** Front-end, DB & Testing (File I/O)

## Technologies / Tools Used

- C++17
- Qt 6 Widgets
- CMake
- Standard Template Library (STL)
- File I/O (`fstream`)
- GCC / MinGW or another compatible C++ compiler
- Visual Studio Code or another compatible IDE
- Git / GitHub

## Project Structure

```text
DigitalLogicProject/
├── CMakeLists.txt
├── .gitignore
├── README.md
└── src/
    ├── main.cpp
    ├── cpp/
    │   ├── circuit/
    │   ├── expression/
    │   ├── kmap/
    │   └── sequential/
    └── qt/
```

- `src/main.cpp` — application entry point.
- `src/cpp/` — C++ core logic and design modules.
- `src/qt/` — Qt windows and interface widgets.
- `CMakeLists.txt` — build configuration.
- `.gitignore` — excludes generated build files and local development files.

## Major Features / Modules

The project is intended to bring together the following tools:

- **Logic Gate Core:** Represents logic components and supports Boolean logic operations.
- **Expression Parser and Evaluator:** Processes Boolean expressions and evaluates them for input assignments.
- **Truth Table Generator:** Generates truth-table rows for supported Boolean expressions.
- **Boolean Simplifier:** Simplifies Boolean functions using supplied minterms and optional don't-care terms.
- **K-map Tool:** Provides an interface for exploring Karnaugh-map-based simplification.
- **Circuit Workspace:** Provides a graphical area for placing and moving circuit-component representations.
- **Flip-Flop Conversion Tool:** Supports exploration of conversions between supported flip-flop types.
- **Counter Designer:** Generates state sequences for supported counter configurations.
- **Number-System Converter:** Converts integer values between bases 2 and 16.
- **Circuit Routing and Validation:** The circuit graph module is intended to represent connections and support circuit checks.
- **Save and Load:** Local circuit-configuration persistence is a planned part of the application.



## Project Setup / Installation Instructions

### Requirements

- Qt 6 with the Widgets module
- CMake
- A C++17-compatible compiler, such as a compatible GCC or MinGW toolchain
- Git, if cloning the repository

### 1. Clone the Repository

Clone the repository or download the project files. Open a terminal in the project root, where `CMakeLists.txt` is located.

### 2. Configure the Project

```bash
cmake -S . -B build
```

If CMake cannot find Qt, configure `CMAKE_PREFIX_PATH` to point to the Qt installation that matches your compiler.

### 3. Build the Project

```bash
cmake --build build
```

### 4. Run the Application

The executable location depends on your operating system, build configuration, and CMake generator. Look inside the generated `build` directory for the application executable.

If configuration or compilation fails, check that all source files listed in `CMakeLists.txt` exist and that Qt 6 Widgets is installed for your selected compiler.

