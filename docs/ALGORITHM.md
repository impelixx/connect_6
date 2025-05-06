# Connect 6 Game Algorithm Documentation

## Overview
The Connect 6 game uses an optimized Minimax algorithm with Alpha-Beta pruning for AI decision making. The algorithm is designed to find the best possible move for the AI player while considering the opponent's responses.

## Core Components

### 1. Minimax Algorithm with Alpha-Beta Pruning
```cpp
int minimax(const GameBoard& board, int depth, int alpha, int beta, bool maximizing, Player player)
```

The algorithm uses a depth-limited minimax search with alpha-beta pruning to optimize performance:

- **Depth**: Controls how many moves ahead the AI looks
  - Easy: 1 move ahead
  - Medium: 3 moves ahead
  - Hard: 5 moves ahead

- **Alpha-Beta Pruning**: Optimizes the search by eliminating branches that cannot affect the final decision
  - Alpha: Best value for maximizing player
  - Beta: Best value for minimizing player
  - Pruning occurs when alpha ≥ beta

### 2. Move Evaluation
```cpp
int evaluateMove(const GameBoard& board, int row, int col, Player player)
```

Each potential move is evaluated based on several factors:

1. **Center Control**
   - Moves closer to the center are preferred
   - Score decreases with distance from center

2. **Pattern Recognition**
   - Evaluates potential patterns in all directions
   - Considers both offensive and defensive patterns

3. **Pattern Scoring**
   - 6 in a row: 100,000 points
   - 5 in a row with open end: 50,000 points
   - 4 in a row with two open ends: 10,000 points
   - 4 in a row with one open end: 5,000 points
   - 3 in a row with two open ends: 1,000 points
   - 3 in a row with one open end: 500 points
   - 2 in a row with two open ends: 100 points
   - 2 in a row with one open end: 50 points

### 3. Board Evaluation
```cpp
int evaluateBoard(const GameBoard& board, Player player)
```

The board evaluation considers:

1. **Center Control**
   - Bonus points for pieces in the center area
   - Weighted based on distance from center

2. **Pattern Analysis**
   - Evaluates all possible patterns for both players
   - Defensive patterns are weighted more heavily (2x)

## Optimization Techniques

1. **Move Ordering**
   - Moves are sorted by evaluation score before search
   - Improves alpha-beta pruning efficiency
   - Most promising moves are evaluated first

2. **Early Termination**
   - Search stops when a winning position is found
   - Immediate return on alpha-beta cutoffs
   - Memory management optimization

3. **Pattern Recognition**
   - Efficient pattern detection in all directions
   - Considers blocked and open patterns
   - Weighted scoring based on pattern strength

## Performance Considerations

1. **Memory Management**
   - Temporary boards are created and deleted efficiently
   - Early cleanup on cutoffs
   - Smart pointer usage for board management

2. **Search Optimization**
   - Depth-limited search to control computation time
   - Alpha-beta pruning to reduce search space
   - Move ordering to improve pruning efficiency

3. **Evaluation Optimization**
   - Cached pattern evaluations
   - Efficient center control calculation
   - Weighted scoring system

## Usage Example

```cpp
GameAI ai;
ai.setDifficulty(AIDifficulty::HARD);
std::pair<int, int> bestMove = ai.getBestMove(board, Player::BLACK);
```

## Time Complexity
- Best case: O(b^(d/2)) with perfect move ordering
- Average case: O(b^(3d/4)) with good move ordering
- Worst case: O(b^d) without pruning

Where:
- b = branching factor (average number of legal moves)
- d = search depth 