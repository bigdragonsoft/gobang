/*
 * 五子棋游戏
 * 版权所有 (C) 2024~2025 [大龙软件 bigdragonsoft.com]
 * 
 * 这是一个在控制台中运行的五子棋游戏，主要功能包括：
 * 1. 提供一个15x15的棋盘供玩家和AI对弈
 * 2. 支持玩家与AI对战，AI有不同难度级别可选择
 * 3. 实现了基于极小极大算法的AI决策系统
 * 4. 具有棋局评估、胜负判定等核心游戏逻辑
 * 5. 提供友好的命令行界面，方便玩家操作和查看棋局
 * 
 * Gobang Game
 * Copyright (C) 2024~2025 [bigdragonsoft.com]
 * This is a console-based Gobang game, with main features including:
 * 1. Providing a 15x15 board for players and AI to compete
 * 2. Supporting player vs AI matches, with AI having different difficulty levels to choose from
 * 3. Implementing an AI decision-making system based on the minimax algorithm
 * 4. Possessing core game logic such as board evaluation and win/loss determination
 * 5. Offering a user-friendly command-line interface for easy player operation and game viewing
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define BOARD_SIZE 15
#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define MAX_DEPTH 5
#define SEARCH_RANGE 2

// 使用 INT_MAX 和 INT_MIN 替代 INF
#define INF INT_MAX
#define NEG_INF INT_MIN

#define EASY_DEPTH 2
#define MEDIUM_DEPTH 3
#define HARD_DEPTH 4

#define VERSION "0.1.3"
#define AUTHOR "Qiang Guo"
#define EMAIL "bigdragonsoft@gmail.com"
#define WEBSITE "https://github.com/bigdragonsoft/gobang"

int board[BOARD_SIZE][BOARD_SIZE] = {{0}};

// 记录获胜的五子位置
int winningPositions[5][2] = {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}};
int hasWinner = 0;  // 标记是否有获胜者

int AI_DEPTH = MEDIUM_DEPTH;  // 默认中等难度

int gameMode = 0;  // 0: 未设置, 1: 双人模式, 2: 人机模式

// 记录最后落子的位置
int lastMoveRow = -1;
int lastMoveCol = -1;

/**
 * @brief 初始化棋盘
 * 
 * 将棋盘上所有位置初始化为空（'.'）
 */
void initBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
}

/**
 * @brief 打印棋盘
 * 
 * 在控制台上显示居中的游戏标题和当前棋盘状态
 */
void printBoard() {
    int boardWidth = BOARD_SIZE * 2 + 2;  // 棋盘的宽度（每个格子占2个字符，加上行号）
    const char *titleText = "Gobang Game";
    int titleWidth = strlen(titleText);
    int versionWidth = strlen(VERSION);
    int totalWidth = titleWidth > boardWidth ? titleWidth : boardWidth;

    // 打印标题
    printf("\n");
    
    // 上边框
    printf("%*s", (totalWidth - titleWidth) / 2, "");
    for (int i = 0; i < titleWidth; i++) printf("-");
    printf("\n");
    
    // 标题文字
    printf("%*s%s\n", (totalWidth - titleWidth) / 2, "", titleText);
    
    // 下边框
    printf("%*s", (totalWidth - titleWidth) / 2, "");
    for (int i = 0; i < titleWidth; i++) printf("-");
    printf("\n");

    // 版本号
    printf("%*sv%s\n\n", (totalWidth - versionWidth) / 2, "", VERSION);

    // 打印列号
    printf("  ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i < 10) {
            printf("%2d", i);
        } else {
            printf("%2c", 'A' + (i - 10));
        }
    }
    printf("\n");

    // 打印棋盘
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i < 10) {
            printf("%2d", i);
        } else {
            printf("%2c", 'A' + (i - 10));
        }
        for (int j = 0; j < BOARD_SIZE; j++) {
            // 检查是否是获胜位置
            int isWinningPosition = 0;
            if (hasWinner) {
                for (int w = 0; w < 5; w++) {
                    if (winningPositions[w][0] == i && winningPositions[w][1] == j) {
                        isWinningPosition = 1;
                        break;
                    }
                }
            }
            
            if (isWinningPosition) {
                // 获胜位置使用闪烁高亮显示（红色）
                switch(board[i][j]) {
                    case EMPTY: printf(" \033[1;31m·\033[0m"); break;
                    case BLACK: printf(" \033[1;31m●\033[0m"); break;  // 黑子用实心圆（红色高亮）
                    case WHITE: printf(" \033[1;31m○\033[0m"); break;  // 白子用空心圆（红色高亮）
                }
            } else if (i == lastMoveRow && j == lastMoveCol) {
                // 最后落子的位置使用黄色高亮显示
                switch(board[i][j]) {
                    case EMPTY: printf(" \033[1;33m·\033[0m"); break;
                    case BLACK: printf(" \033[1;33m●\033[0m"); break;  // 黑子用实心圆（黄色高亮）
                    case WHITE: printf(" \033[1;33m○\033[0m"); break;  // 白子用空心圆（黄色高亮）
                }
            } else {
                switch(board[i][j]) {
                    case EMPTY: printf(" ·"); break;
                    case BLACK: printf(" ●"); break;  // 黑子用实心圆
                    case WHITE: printf(" ○"); break;  // 白子用空心圆
                }
            }
        }
        printf("\n");
    }

    // 在棋盘下方显示AI难度
    if (gameMode == 2) {
        switch (AI_DEPTH) {
            case EASY_DEPTH:
                printf("\nAI Difficulty Easy\n");
                break;
            case MEDIUM_DEPTH:
                printf("\nAI Difficulty Medium\n");
                break;
            case HARD_DEPTH:
                printf("\nAI Difficulty Hard\n");
                break;
            default:
                printf("\nAI Difficulty Unknown\n");
        }
    }
}

/**
 * @brief 检查是否有五子连珠
 * 
 * @param row 最后一步棋的行号
 * @param col 最后一步棋的列号
 * @return int 如果有五子连珠返回1，否则返回0
 */
int checkWin(int row, int col) {
    int player = board[row][col];
    int directions[4][2] = {{0,1}, {1,0}, {1,1}, {1,-1}};
    
    for (int d = 0; d < 4; d++) {
        int count = 1;
        int positions[9][2]; // 最多可能有9个连续棋子（双向各4个加中心点）
        positions[0][0] = row;
        positions[0][1] = col;
        int posCount = 1;
        
        // 向一个方向检查
        for (int i = 1; i <= 4; i++) {
            int newRow = row + directions[d][0] * i;
            int newCol = col + directions[d][1] * i;
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE || board[newRow][newCol] != player) {
                break;
            }
            positions[posCount][0] = newRow;
            positions[posCount][1] = newCol;
            posCount++;
            count++;
        }
        // 向相反方向检查
        for (int i = 1; i <= 4; i++) {
            int newRow = row - directions[d][0] * i;
            int newCol = col - directions[d][1] * i;
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE || board[newRow][newCol] != player) {
                break;
            }
            positions[posCount][0] = newRow;
            positions[posCount][1] = newCol;
            posCount++;
            count++;
        }
        if (count >= 5) {
            // 找到五子连珠，记录位置
            hasWinner = 1;
            // 只记录前5个位置
            for (int i = 0; i < 5 && i < posCount; i++) {
                winningPositions[i][0] = positions[i][0];
                winningPositions[i][1] = positions[i][1];
            }
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 评估棋盘状态
 * 
 * @param row 最后一步棋的行号
 * @param col 最后一步棋的列号
 * @param player 当前玩家的棋符号
 * @return int 盘状态的评分
 */
int evaluatePosition(int row, int col, int player) {
    int score = 0;
    int directions[4][2] = {{0,1}, {1,0}, {1,1}, {1,-1}};
    
    for (int d = 0; d < 4; d++) {
        int count = 1;
        int block = 0;
        
        // 向一个方向检查
        for (int i = 1; i <= 4; i++) {
            int newRow = row + directions[d][0] * i;
            int newCol = col + directions[d][1] * i;
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE) {
                block++;
                break;
            }
            if (board[newRow][newCol] == player) count++;
            else if (board[newRow][newCol] != EMPTY) {
                block++;
                break;
            }
            else break;
        }
        
        // 向相反方向检查
        for (int i = 1; i <= 4; i++) {
            int newRow = row - directions[d][0] * i;
            int newCol = col - directions[d][1] * i;
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE) {
                block++;
                break;
            }
            if (board[newRow][newCol] == player) count++;
            else if (board[newRow][newCol] != EMPTY) {
                block++;
                break;
            }
            else break;
        }
        
        // 评估
        if (count >= 5) score += 100000;
        else if (count == 4 && block == 0) score += 10000;
        else if (count == 4 && block == 1) score += 1000;
        else if (count == 3 && block == 0) score += 1000;
        else if (count == 3 && block == 1) score += 100;
        else if (count == 2 && block == 0) score += 100;
    }
    
    return score;
}

/**
 * @brief 评估整个棋盘状态
 * 
 * @return int 棋盘状态的评分
 */
int evaluateBoard() {
    int score = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == WHITE) {
                score += evaluatePosition(i, j, WHITE);
            } else if (board[i][j] == BLACK) {
                score -= evaluatePosition(i, j, BLACK);
            }
        }
    }
    return score;
}

/**
 * @brief 检查是否在搜索围内
 * 
 * @param row 行号
 * @param col 列号
 * @return int 如果在搜索范围内返回1，否则返回0
 */
int inSearchRange(int row, int col) {
    for (int i = row - SEARCH_RANGE; i <= row + SEARCH_RANGE; i++) {
        for (int j = col - SEARCH_RANGE; j <= col + SEARCH_RANGE; j++) {
            if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE) {
                if (board[i][j] != EMPTY) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/**
 * @brief 极小化极大算法的实现
 * 
 * @param depth 当搜索深度
 * @param alpha Alpha值
 * @param beta Beta值
 * @param maximizingPlayer 是否为极大化玩家
 * @return int 最佳评分
 */
int minimax(int depth, int alpha, int beta, int maximizingPlayer) {
    if (depth == 0 || depth >= MAX_DEPTH) {
        return evaluateBoard();
    }

    if (maximizingPlayer) {
        int maxEval = NEG_INF;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == EMPTY && inSearchRange(i, j)) {
                    board[i][j] = WHITE;
                    int eval = minimax(depth - 1, alpha, beta, 0);
                    board[i][j] = EMPTY;
                    maxEval = (eval > maxEval) ? eval : maxEval;
                    alpha = (alpha > eval) ? alpha : eval;
                    if (beta <= alpha) {
                        return maxEval;
                    }
                }
            }
        }
        return maxEval;
    } else {
        int minEval = INF;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == EMPTY && inSearchRange(i, j)) {
                    board[i][j] = BLACK;
                    int eval = minimax(depth - 1, alpha, beta, 1);
                    board[i][j] = EMPTY;
                    minEval = (eval < minEval) ? eval : minEval;
                    beta = (beta < eval) ? beta : eval;
                    if (beta <= alpha) {
                        return minEval;
                    }
                }
            }
        }
        return minEval;
    }
}

/**
 * @brief 电脑下棋
 * 
 * 使用极小化极大算法选择最佳落子位置
 */
void makeAIMove(int* row, int* col) {
    int bestScore = NEG_INF;
    int bestRow = -1, bestCol = -1;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == EMPTY && inSearchRange(i, j)) {
                board[i][j] = WHITE;
                int score = minimax(AI_DEPTH - 1, NEG_INF, INF, 0);  // 使用 AI_DEPTH
                board[i][j] = EMPTY;
                
                if (score > bestScore) {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }
    
    board[bestRow][bestCol] = WHITE;
    printf("AI placed a move at (%d, %d)\n", bestRow, bestCol);
    
    *row = bestRow;
    *col = bestCol;
}

/**
 * @brief AI帮助玩家下棋
 * 
 * 使用极小化极大算法为玩家选择最佳落子位置
 */
void makeAIHelpMove(int player, int* row, int* col) {
    int bestScore = NEG_INF;
    int bestRow = -1, bestCol = -1;
    
    // 检查棋盘是否为空
    int boardEmpty = 1;
    for (int i = 0; i < BOARD_SIZE && boardEmpty; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] != EMPTY) {
                boardEmpty = 0;
                break;
            }
        }
    }
    
    // 如果棋盘为空，选择中央位置或附近
    if (boardEmpty) {
        // 棋盘中央位置及周围的位置
        int center = BOARD_SIZE / 2;
        bestRow = center + (rand() % 3 - 1); // 中央位置加减1范围内随机选择
        bestCol = center + (rand() % 3 - 1);
    } else {
        // 原有的落子逻辑
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == EMPTY && inSearchRange(i, j)) {
                    board[i][j] = player;
                    int score = minimax(AI_DEPTH - 1, NEG_INF, INF, player == WHITE);
                    board[i][j] = EMPTY;
                    
                    if (score > bestScore) {
                        bestScore = score;
                        bestRow = i;
                        bestCol = j;
                    }
                }
            }
        }
    }
    
    // 确保bestRow和bestCol有有效值
    if (bestRow == -1 || bestCol == -1) {
        // 如果没有找到合适的位置，随机选择一个空位置
        do {
            bestRow = rand() % BOARD_SIZE;
            bestCol = rand() % BOARD_SIZE;
        } while (board[bestRow][bestCol] != EMPTY);
    }
    
    board[bestRow][bestCol] = player;
    printf("AI helps placed a move at (%d, %d)\n", bestRow, bestCol);
    
    *row = bestRow;
    *col = bestCol;
}

/**
 * @brief 主游戏循环
 * 
 * 控制游戏的主要流程，包括玩家和电脑的回合
 */
void playGame() {
    int currentPlayer = BLACK;
    int row, col;
    int moves = 0;
    char playAgain;

    // 如果游戏模式未预设,则询问用户
    if (gameMode == 0) {
        do {
            printf("Select game mode:\n1. Player vs Player\n2. Player vs AI\n");
            if (scanf("%d", &gameMode) != 1 || (gameMode != 1 && gameMode != 2)) {
                printf("Invalid choice, please try again.\n");
                while (getchar() != '\n');  // 清除输入缓冲区
                gameMode = 0;  // 重置gameMode
            } else {
                while (getchar() != '\n');  // 清除输入缓冲区
            }
        } while (gameMode == 0);
    }

    if (gameMode == 2) {
        int difficulty = 0;
        do {
            printf("Select AI difficulty:\n1. Easy\n2. Medium\n3. Hard\n");
            if (scanf("%d", &difficulty) != 1 || difficulty < 1 || difficulty > 3) {
                printf("Invalid choice, please try again.\n");
                while (getchar() != '\n');  // 清除输入缓冲区
                difficulty = 0;  // 重置difficulty
            } else {
                while (getchar() != '\n');  // 清除输入缓冲区
            }
        } while (difficulty == 0);

        switch (difficulty) {
            case 1:
                AI_DEPTH = EASY_DEPTH;
                break;
            case 2:
                AI_DEPTH = MEDIUM_DEPTH;
                break;
            case 3:
                AI_DEPTH = HARD_DEPTH;
                break;
        }
    }

    do {
        srand(time(NULL));  // 初始化随机数生成器
        initBoard();
        moves = 0;
        currentPlayer = BLACK;
        // 重置最后落子位置
        lastMoveRow = -1;
        lastMoveCol = -1;
        // 重置获胜状态
        hasWinner = 0;
        for (int i = 0; i < 5; i++) {
            winningPositions[i][0] = -1;
            winningPositions[i][1] = -1;
        }

        while (1) {
            system("clear");
            printBoard();
            
            if (currentPlayer == BLACK || gameMode == 1) {
                int validMove = 0;
                char input[10] = {0};  // 将input变量声明移到外层作用域
                
                while (!validMove) {
                    printf("Player %s\n", currentPlayer == BLACK ? "Black" : "White");
                    printf("Enter move position, or 'q' to quit, or '?' for AI help: ");
                    
                    if (fgets(input, sizeof(input), stdin) == NULL) {
                        printf("Input error, please try again.\n");
                        continue;
                    }

                    // 检查是否输入了 'q'
                    if (input[0] == 'q' || input[0] == 'Q') {
                        printf("Game over.\n");
                        return;  // 退出游戏
                    }

                    // 检查是否输入了 '?'
                    if (input[0] == '?') {
                        makeAIHelpMove(currentPlayer, &row, &col);
                        validMove = 1;
                        // 更新最后落子位置
                        lastMoveRow = row;
                        lastMoveCol = col;
                        break;
                    }

                    char rowInput, colInput;
                    if (sscanf(input, "%c %c", &rowInput, &colInput) != 2) {
                        printf("Invalid input, please enter two characters.\n");
                        continue;
                    }

                    // 将输入转换为大写
                    rowInput = toupper(rowInput);
                    colInput = toupper(colInput);

                    // 处理行输入
                    if (rowInput >= '0' && rowInput <= '9') {
                        row = rowInput - '0';
                    } else if (rowInput >= 'A' && rowInput <= 'E') {
                        row = 10 + (rowInput - 'A');
                    } else {
                        printf("Invalid row coordinate, please try again.\n");
                        continue;
                    }

                    // 处理列输入
                    if (colInput >= '0' && colInput <= '9') {
                        col = colInput - '0';
                    } else if (colInput >= 'A' && colInput <= 'E') {
                        col = 10 + (colInput - 'A');
                    } else {
                        printf("Invalid column coordinate, please try again.\n");
                        continue;
                    }

                    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != EMPTY) {
                        printf("Invalid move position, please try again.\n");
                        continue;
                    }

                    validMove = 1;
                    // 更新最后落子位置
                    lastMoveRow = row;
                    lastMoveCol = col;
                }

                // 如果是AI帮助落子，则已经在makeAIHelpMove函数中设置了棋盘
                // 否则，我们需要在这里设置棋盘
                if (input[0] != '?') {
                    board[row][col] = currentPlayer;
                }
            } else {
                makeAIMove(&row, &col);
                // 更新最后落子位置
                lastMoveRow = row;
                lastMoveCol = col;
            }

            moves++;

            // 实现落子闪烁效果
            for (int i = 0; i < 3; i++) {  // 闪烁3次
                system("clear");
                printBoard();
                usleep(200000);  // 暂停200毫秒
                
                // 暂时取消高亮
                int tempRow = lastMoveRow;
                int tempCol = lastMoveCol;
                lastMoveRow = -1;
                lastMoveCol = -1;
                
                system("clear");
                printBoard();
                usleep(200000);  // 暂停200毫秒
                
                // 恢复高亮
                lastMoveRow = tempRow;
                lastMoveCol = tempCol;
            }

            if (checkWin(row, col)) {
                // 获胜者闪烁效果
                for (int flash = 0; flash < 5; flash++) {
                    // 显示获胜位置
                    system("clear");
                    printBoard();
                    usleep(300000);  // 暂停300毫秒
                    
                    // 暂时隐藏获胜位置
                    int tempHasWinner = hasWinner;
                    hasWinner = 0;
                    
                    system("clear");
                    printBoard();
                    usleep(300000);  // 暂停300毫秒
                    
                    // 恢复获胜位置
                    hasWinner = tempHasWinner;
                }
                
                system("clear");
                printBoard();
                if (gameMode == 2 && currentPlayer == WHITE) {
                    printf("AI wins!\n");
                } else {
                    printf("Player %s wins!\n", currentPlayer == BLACK ? "Black" : "White");
                }
                break;
            }

            if (moves == BOARD_SIZE * BOARD_SIZE) {
                system("clear");
                printBoard();
                printf("It's a draw!\n");
                break;
            }

            currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
        }

        // 询问是否再玩一局
        printf("Play again? (y/n): ");
        scanf(" %c", &playAgain);
        while (getchar() != '\n');  // 清除输入缓冲区

    } while (playAgain == 'y' || playAgain == 'Y');

    printf("Thanks for playing, goodbye!\n");
}

/**
 * @brief 主函数
 * 
 * @return int 程序退出状态
 */
int main(int argc, char *argv[]) {
    // 检查命令行参数
    if (argc > 1) {
        if (strcmp(argv[1], "-v") == 0) {
            printf("Gobang Game Version %s\n", VERSION);
            printf("Author: %s\n", AUTHOR);
            printf("Email: %s\n", EMAIL);
            printf("Website: %s\n", WEBSITE);
            printf("Copyright (C) 2024~2025 BigDragonSoft.com\n");
            return 0;
        } else if (strcmp(argv[1], "-h") == 0) {
            printf("Gobang Game\n\n");
            printf("This is a console-based Five in a Row game. Main features include:\n");
            printf("1. 15x15 game board\n");
            printf("2. Support for player vs player or player vs AI\n");
            printf("3. Three difficulty levels for AI\n");
            printf("4. AI decision-making using minimax algorithm\n");
            printf("5. User-friendly command-line interface\n\n");
            printf("Usage:\n");
            printf("  ./gobang         Start the game\n");
            printf("  ./gobang -v      Display version information\n");
            printf("  ./gobang -h      Display this help information\n");
            printf("  ./gobang -2      Start the game in player vs player mode\n");
            printf("  ./gobang -1      Start the game in player vs AI mode\n");
            printf("\nFor more information, please use 'man gobang' to view the game manual page\n");
            return 0;
        } else if (strcmp(argv[1], "-2") == 0) {
            // 直接进入双人模式
            gameMode = 1;
            playGame();
            return 0;
        } else if (strcmp(argv[1], "-1") == 0) {
            // 直接进入人机对战模式
            gameMode = 2;
            AI_DEPTH = MEDIUM_DEPTH;  // 默认使用中等难度
            playGame();
            return 0;
        } else {
            printf("Unknown parameter.\nUse -h to view help information, or -v to view version information.\n");
            return 1;
        }
    }

    playGame();
    return 0;
}
