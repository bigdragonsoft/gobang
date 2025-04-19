# Gobang Game

[简体中文](README.zh-CN.md) | [繁體中文](README.zh-TW.md) | English

## Introduction

This is a console-based Gobang (Five in a Row) game program. It provides a 15x15 board, supports player vs AI gameplay with different difficulty levels for the AI. The game implements an AI decision-making system based on the minimax algorithm, featuring core game logic such as board evaluation and win/loss determination. The program offers a user-friendly command-line interface for easy operation and game viewing, aiming to provide players with an entertaining and challenging Gobang game experience.

## Features

- Two-player mode
- Standard 15x15 game board
- Alternating black and white stone placement
- Automatic win/loss determination
- AI difficulty levels: Easy, Medium (default), Hard

## How to Play

1. Black stone moves first at the start of the game
2. Players take turns placing stones on empty intersections of the board
3. The first player to form an unbroken line of five stones horizontally, vertically, or diagonally wins
4. If the board is filled without a winner, the game is declared a draw

## Board Display

- The board displays the last move with a highlighted marker.
- AI difficulty is shown below the board in AI mode.

## In-game Commands
- Enter coordinates (e.g. "3 5" or "C E") to place a stone
- Type '?' during your turn to get AI assistance for the best move
- Type 'q' to quit the current game

## Usage
```
Usage: gobang [option]
Options:
    -1      Start the game in player vs AI mode
    -2      Start the game in player vs player mode
    -v      Display version information
    -h      Display help information
```

## Version

- Version: 0.1.3

## Author

- Author: Qiang Guo
- Email: bigdragonsoft@gmail.com
- Website: https://github.com/bigdragonsoft/gobang

