/*
 * 五子棋游戏
 * 版权所有 (C) 2024 [大龙软件 bigdragonsoft.com]
 * 
 * 这是一个在控制台中运行的五子棋游戏，主要功能包括：
 * 1. 提供一个15x15的棋盘供玩家和AI对弈
 * 2. 支持玩家与AI对战，AI有不同难度级别可选择
 * 3. 实现了基于极小极大算法的AI决策系统
 * 4. 具有棋局评估、胜负判定等核心游戏逻辑
 * 5. 提供友好的命令行界面，方便玩家操作和查看棋局
 * 
 * 本程序旨在为玩家提供一个有趣且具有挑战性的五子棋游戏体验。
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

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

#define VERSION "0.1"

int board[BOARD_SIZE][BOARD_SIZE] = {0};

int AI_DEPTH = MEDIUM_DEPTH;  // 默认中等难度

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
    const char *titleText[2] = {"GOBANG", VERSION};
    int titleWidth = strlen(titleText[0]) + strlen(titleText[1]) + 1;  // +1 for the space between
    int borderWidth = titleWidth + 4;  // 添加左右各两个'='

    // 打印标题
    printf("\n");
    
    // 上边框
    printf("%*s", (boardWidth - borderWidth) / 2, "");
    for (int i = 0; i < borderWidth; i++) printf("-");
    printf("\n");
    
    // 标题文字
    printf("%*s", (boardWidth - titleWidth) / 2, "");
    printf("%s v%s", titleText[0], titleText[1]);
    printf("\n");
    
    // 下边框
    printf("%*s", (boardWidth - borderWidth) / 2, "");
    for (int i = 0; i < borderWidth; i++) printf("-");
    printf("\n\n");

    // 打印列号
    printf("  ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d", i);
    }
    printf("\n");

    // 打印棋盘
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%2d", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            switch(board[i][j]) {
                case EMPTY: printf(" ·"); break;
                case BLACK: printf(" ●"); break;  // 黑子用实心圆
                case WHITE: printf(" ○"); break;  // 白子用空心圆
            }
        }
        printf("\n");
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
        // 向一个方向检查
        for (int i = 1; i <= 4; i++) {
            int newRow = row + directions[d][0] * i;
            int newCol = col + directions[d][1] * i;
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE || board[newRow][newCol] != player) {
                break;
            }
            count++;
        }
        // 向相反方向检查
        for (int i = 1; i <= 4; i++) {
            int newRow = row - directions[d][0] * i;
            int newCol = col - directions[d][1] * i;
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE || board[newRow][newCol] != player) {
                break;
            }
            count++;
        }
        if (count >= 5) return 1;
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
 * @brief 主游戏循环
 * 
 * 控制游戏的主要流程，包括玩家和电脑的回合
 */
void playGame() {
    int currentPlayer = BLACK;
    int row, col;
    int moves = 0;
    int gameMode;
    char playAgain;

    printf("Select game mode:\n1. Two Players\n2. Player vs AI\n");
    scanf("%d", &gameMode);
    while (getchar() != '\n');  // 清除输入缓冲区

    if (gameMode == 2) {
        printf("Select AI difficulty:\n1. Easy\n2. Medium\n3. Hard\n");
        int difficulty;
        scanf("%d", &difficulty);
        while (getchar() != '\n');  // 清除输入缓冲区

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
            default:
                printf("Invalid choice, using default difficulty (Medium).\n");
                AI_DEPTH = MEDIUM_DEPTH;
        }
    }

    do {
        srand(time(NULL));  // 初始化随机数生成器
        initBoard();
        moves = 0;
        currentPlayer = BLACK;

        while (1) {
            system("clear");
            printBoard();
            
            if (currentPlayer == BLACK || gameMode == 1) {
                int validMove = 0;
                while (!validMove) {
                    printf("Player %s\n", currentPlayer == BLACK ? "Black" : "White");
                    printf("Enter move position (row column), or 'q' to quit: ");
                    char input[10];
                    if (fgets(input, sizeof(input), stdin) == NULL) {
                        printf("Input error, please try again.\n");
                        continue;
                    }

                    // 检查是否输入了 'q'
                    if (input[0] == 'q' || input[0] == 'Q') {
                        printf("Game over.\n");
                        return;  // 退出游戏
                    }

                    if (sscanf(input, "%d %d", &row, &col) != 2) {
                        printf("Invalid input, please enter two numbers.\n");
                        continue;
                    }

                    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != EMPTY) {
                        printf("Invalid move position, please try again.\n");
                        continue;
                    }

                    validMove = 1;
                }

                board[row][col] = currentPlayer;
            } else {
                makeAIMove(&row, &col);
            }

            moves++;

            system("clear");
            printBoard();

            if (checkWin(row, col)) {
                if (gameMode == 2 && currentPlayer == WHITE) {
                    printf("AI wins!\n");
                } else {
                    printf("Player %s wins!\n", currentPlayer == BLACK ? "Black" : "White");
                }
                break;
            }

            if (moves == BOARD_SIZE * BOARD_SIZE) {
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
            return 0;
        } else {
            printf("Unknown parameter. Use -v to view version information.\n");
            return 1;
        }
    }

    playGame();
    return 0;
}