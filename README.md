# Connect 6 Game

A modern implementation of the Connect 6 game with an advanced AI opponent using the Minimax algorithm with Alpha-Beta pruning.

![Connect 6 Game](screenshots/gameplay.png)

## Features

- 🎮 Multiple game modes:
  - Player vs Player
  - Player vs AI
  - AI vs AI
- 🤖 AI with three difficulty levels:
  - Easy (1-ply search)
  - Medium (3-ply search)
  - Hard (5-ply search)
- 🎯 Advanced AI using Minimax with Alpha-Beta pruning
- 💾 Save/Load game functionality
- 🎨 Modern UI with animations
- 📱 Cross-platform support

## Installation

### Prerequisites
- Qt 6.0 or higher
- C++17 compatible compiler
- CMake 3.15 or higher

### Building from Source

```bash
git clone https://github.com/yourusername/connect6.git
cd connect6

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the game
./connect6
```

## Game Rules

Connect 6 is a two-player game played on a 15x15 board. Players take turns placing their pieces (black and white) on the board. The first player to create a line of 6 or more pieces in any direction (horizontal, vertical, or diagonal) wins the game.

## AI Implementation

The game features an advanced AI opponent that uses:
- Minimax algorithm with Alpha-Beta pruning
- Pattern recognition for move evaluation
- Center control strategy
- Move ordering optimization

For detailed information about the AI implementation, see [Algorithm Documentation](docs/ALGORITHM.md).

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Qt Framework for the GUI
- Modern C++ features for performance optimization
- Alpha-Beta pruning algorithm for AI optimization