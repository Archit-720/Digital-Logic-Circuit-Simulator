# C++ Logic Circuit Simulator

## Brief Description
A terminal-based software simulator that allows users to instantly build, test, and verify digital logic circuits and complex boolean expressions without the friction of physical hardware failures.

## Problem Statement / Objective
Physical digital logic experiments rely heavily on breadboards and integrated circuits (ICs) that are highly susceptible to hardware faults (loose wiring, burnt ICs). This project provides a reliable, interactive C++ environment to rapidly prototype and test circuit logic, eliminating tedious manual calculations and hardware debugging.

## Team Members
* **Archit Jirwan:** Team Lead & Circuit Router (Graph DAGs)
* **Arnav Negi:** System Architect (Logic Gate OOP)
* **Ishaan Negi:** Developer (Expression Parser via Stacks)
* **Gurpindar Singh:** DB & Testing (File I/O)

## Technologies / Tools Used
* C++ (Standard Template Library)
* File I/O (`fstream` for database layer)
* GCC / MinGW Compiler
* Visual Studio Code
* Git / GitHub

## Project Setup / Installation Instructions
1. Clone this repository to your local machine.
2. Ensure you have a C++11 compatible compiler (like GCC/MinGW) installed.
3. Open the terminal and navigate to the project directory.
4. Compile the source code: `g++ src/*.cpp -o simulator`
5. Run the executable: `./simulator` (Linux/Mac) or `simulator.exe` (Windows)

## Major Features / Modules
* **Logic Gate OOP Core:** Evaluates basic gate logic using object-oriented principles.
* **Expression Parser:** Parses standard boolean string expressions using Stacks.
* **Circuit Routing Engine:** Connects gates and manages signal flow using Directed Acyclic Graphs (DAGs).
* **Database Layer:** Saves and loads local circuit configurations via `.txt` files.
* **Number system converter:** It converts any Radix r from one number system to another number system.

## Current Project Status / Progress
* **Status:** In Development (Initial Architecture Phase)
* **Progress:** 
  * Project workflow and OOP class hierarchy mapped out.
  * C++ environment configured and base `Gate` classes drafted.
  * Local `.txt` file structures defined for state saving.
  * Initial GitHub repository established.
