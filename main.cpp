/**
 * Purpose: Set up objects and run chess main game loop
 * 
 * Author: Owen Colley
 * Date: 8/7/24
 * 
 */

#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include "board.h"
#include "moves.h"
#include "evaluate.h"
#include <limits>
#include <stdint.h>

typedef uint64_t Bitboard;

enum GameType {CHESS, CHESS960};
enum OpponentType {PLAYER, ENGINE};
enum PlayerColor {WHITE, BLACK};

GameType getGameType() {
    char gameType;

    do {
        std::cout << "Play [c]hess or c[h]ess960? ";
        std::cin >> gameType;
        gameType = toupper(gameType);
    } while ((gameType != 'C') && (gameType != 'H'));

    return gameType == 'C' ? CHESS : CHESS960;
}

OpponentType getOpponent() {
    char opponent;

    do {
        std::cout << "Verse [p]layer or [e]ngine? ";
        std::cin >> opponent;
        opponent = toupper(opponent);
    } while ((opponent != 'P') && (opponent != 'E'));

    return opponent == 'P' ? PLAYER : ENGINE;
}

PlayerColor getPlayerColor() {
    char playerColor;

    do {
        std::cout << "Play as [w]hite or [b]lack? ";
        std::cin >> playerColor;
        playerColor = toupper(playerColor);
    } while ((playerColor != 'W') && (playerColor != 'B'));

    return playerColor == 'W' ? WHITE : BLACK;
}

int getEngineDepth() {
    int depth;

    do {
        std::cout << "What engine depth (int 1-5)? ";
        std::cin >> depth;
        depth = toupper(depth);
    } while ((depth < 1) || (depth > 5));

    return depth;
}

std::string getPlayerMove(const bool WHITE_TURN, Moves moves1) {
    std::string move;
    const std::string MOVES = WHITE_TURN ?
        moves1.possibleMovesWhite(enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing)
        : moves1.possibleMovesBlack(enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
    
    bool illegalCheck;
    do {
        std::cout << "Choose your move (xyxy) ";
        std::cin >> move;
        moves1.doMove(" " + move, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        illegalCheck = (WHITE_TURN && whiteKing & moves1.otherThreats(false, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing))
                                || (!WHITE_TURN && blackKing & moves1.otherThreats(true, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing));
        moves1.undoMove(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
    } while (move.length() != 4 || MOVES.find(move) == std::string::npos || illegalCheck);
    
    // if pawn is promoting ask what user would like to promote to or castle
    char specialMoveType = ' ';
    if (MOVES.find('Q' + move) != std::string::npos) {
        do {
            std::cout << "Promote to what (N/B/R/Q)? ";
            std::cin >> specialMoveType;
        } while ((specialMoveType != 'N') && (specialMoveType != 'B') && (specialMoveType != 'R') && (specialMoveType != 'Q'));
    } else if (MOVES.find('C' + move) != std::string::npos) {
        specialMoveType = 'C';
        // if castle and not castle both possible with same king move, ask which
        if (MOVES.find(' ' + move) != std::string::npos) {
            char castle;
            do {
                std::cout << "Castle (y/n)? ";
                std::cin >> castle;
            } while (castle != 'y' && castle != 'n');
            specialMoveType = castle == 'y' ? 'C' : ' ';
        }
    }
    
    return specialMoveType + move;
}

int main() {
    const GameType GAME_TYPE = getGameType();
    const OpponentType OPPONENT_TYPE = getOpponent();
    const PlayerColor PLAYER_COLOR = OPPONENT_TYPE == ENGINE ? getPlayerColor() : WHITE;
    const int DEPTH = OPPONENT_TYPE == ENGINE ? getEngineDepth() : 0;
    
    Board board1(GAME_TYPE);
    Evaluate evaluate1;
    Moves moves1(whiteRooks, blackRooks);
    board1.displayBoard(0, evaluate1.materialScore(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens), evaluate1.evaluate(true, 0, moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing));
    
    bool whiteTurn = true;
    int halfTurns = 2;
    float score = 0;
    
    //begin main game loop playing against engine
    while (OPPONENT_TYPE == ENGINE && !evaluate1.gameOver(whiteTurn, moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing)) {
        const bool PLAYER_TURN = (whiteTurn && PLAYER_COLOR == WHITE)
                            || (!whiteTurn && PLAYER_COLOR != WHITE);
        if (!PLAYER_TURN) {
            score = evaluate1.minimax(DEPTH, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), whiteTurn, true, moves1, enPassant, whitePawns,  whiteKnights,  whiteBishops, whiteRooks,  whiteQueens,  whiteKing, blackPawns,  blackKnights,  blackBishops, blackRooks,  blackQueens,  blackKing);
        }
        const std::string MOVE = PLAYER_TURN ? getPlayerMove(whiteTurn, moves1)
            : evaluate1.getBestMove();
        moves1.doMove(MOVE, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        board1.displayBoard(halfTurns, evaluate1.materialScore(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens), score);
        
        whiteTurn = !whiteTurn;
        halfTurns++;
    }
    
    //begin main game loop playing against player
    while (OPPONENT_TYPE == PLAYER && !evaluate1.gameOver(whiteTurn, moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing)) {
        const std::string MOVE = getPlayerMove(whiteTurn, moves1);
        moves1.doMove(MOVE, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        board1.displayBoard(halfTurns, evaluate1.materialScore(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens), score);
        
        whiteTurn = !whiteTurn;
        halfTurns++;
    }
    
    const bool BLACK_CHECKED = blackKing & moves1.otherThreats(true, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
    const bool WHITE_CHECKED = whiteKing & moves1.otherThreats(false, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
    if (!whiteTurn && BLACK_CHECKED) {
        std::cout << "White wins!";
    } else if (whiteTurn && WHITE_CHECKED) {
        std::cout << "Black wins!";
    } else {
        std::cout << "It's a stalemate!";
    }
    
    return 0;
    
}
