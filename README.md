#  C++ Console Based Game

## Project Description

Rubble Ludo is a console-based C++ board game inspired by the classic Ludo concept. It is designed as a two-player turn-based game where players roll a dice, move their tokens, capture opponent tokens, and try to complete all their tokens before the other player. The project focuses on applying basic and intermediate C++ programming concepts in a practical game-based application.

This game has been improved with better logic, cleaner structure, optimized functions, and a more user-friendly console interface. It is suitable for first-semester or beginner-level programming students who want to understand how loops, arrays, functions, conditions, random numbers, and input handling are used in a real project.

## Features

* Two-player gameplay
* Dice rolling system
* Token movement system
* Token entry on rolling six
* Extra turn when a player rolls six
* Collision and capture system
* Safe cell handling
* Valid move checking
* Turn-based game flow
* Clear console-based board display
* Improved input validation
* Structured and optimized C++ code
* Winning condition detection

## Technologies Used

* C++
* Console-based UI
* Standard C++ libraries
* Random number generation
* Functions and arrays
* Object-oriented structure in the optimized version

## How to Run the Project

### Step 1: Open the Project Folder

Open the folder where the C++ file is saved.

### Step 2: Compile the Code

If the file name is:

```bash
optimized_rubble_ludo.cpp
```

Compile it using:

```bash
g++ optimized_rubble_ludo.cpp -o ludo.exe
```

### Step 3: Run the Game

On Windows PowerShell, run:

```bash
.\ludo.exe
```

On Linux or macOS, run:

```bash
./ludo
```

For Linux/macOS compilation, use:

```bash
g++ optimized_rubble_ludo.cpp -o ludo
```

## Game Rules

1. The game is played between two players.
2. Each player has multiple tokens.
3. A player rolls the dice on their turn.
4. A token can enter the board when the player rolls a six.
5. Rolling a six gives the player an extra turn.
6. Players can move existing tokens according to the dice value.
7. If a player lands on an opponent’s token, the opponent’s token is sent back home.
8. Some cells may be treated as safe cells where tokens cannot be captured.
9. The player who completes all tokens first wins the game.

## Project Purpose

The main purpose of this project is to demonstrate how C++ programming concepts can be used to build an interactive console game. It helps students practice problem-solving, logical thinking, modular programming, and game flow management. The project also shows how a basic game can be improved by optimizing code structure, improving the user interface, and fixing logical issues.

## Concepts Covered

* Variables and constants
* Arrays
* Functions
* Loops
* Conditional statements
* Random number generation
* Input validation
* Game logic
* Turn management
* Modular programming
* Basic object-oriented programming

## Future Improvements

* Add a graphical user interface
* Add four-player support
* Add computer/AI opponent
* Add sound effects
* Add save and load functionality
* Add score tracking
* Add difficulty levels
* Improve board design further
* Add colored console output

## Author

Developed as a beginner-friendly C++ console game project for programming practice and academic submission.

## License

This project is open for educational use. You can modify and improve it for learning purposes.
