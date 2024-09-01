/**
 * Purpose: Evaluate position and check for game ending. Find best move for engine;
 * 
 * Author: Owen Colley
 * Date: 8/8/24
 * 
 */

#include <iostream>
#include <string>
#include <bit>
#ifndef EVALUATE_H
#define EVALUATE_H
#include <limits>
#include <algorithm>
#include <stdint.h>

typedef uint64_t Bitboard;

class Evaluate {
private:
    const float pieceLocationValues[8][8] = {
            { -.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f },
            { -.4f, -.2f,  .0f,  .0f,  .0f,  .0f, -.2f, -.4f },
            { -.4f,  .0f,  .1f,  .2f,  .2f,  .1f,  .0f, -.4f },
            { -.4f,  .0f,  .2f, .25f, .25f,  .2f,  .0f, -.4f },
            { -.4f,  .0f,  .2f, .25f, .25f,  .2f,  .0f, -.4f },
            { -.4f,  .0f,  .1f,  .2f,  .2f,  .1f,  .0f, -.4f },
            { -.4f, -.2f,  .0f,  .0f,  .0f,  .0f, -.2f, -.4f },
            { -.5f, -.4f, -.4f, -.4f, -.4f, -.4f, -.4f, -.5f }
        };
    
    const float kingLocationValues[8][8] = {
            { -.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f },
            { -.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f },
            { -.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f },
            { -.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f },
            { -.3f, -.4f, -.4f, -.5f, -.5f, -.4f, -.4f, -.3f },
            { -.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f, -.2f },
            {  .2f,  .2f,  .0f,  .0f,  .0f,  .0f,  .2f,  .2f },
            {  .2f,  .3f,  .1f,  .0f,  .0f,  .1f,  .3f,  .2f }
        };
    
    std::string bestMove;
    
public:
    float minimax(const int DEPTH, float alpha, float beta, const bool WHITE_TURN, 
        const bool FIRST_TIME, Moves moves1, Bitboard& enPassant, 
        Bitboard& whitePawns, Bitboard& whiteKnights, Bitboard& whiteBishops, 
        Bitboard& whiteRooks, Bitboard& whiteQueens, Bitboard& whiteKing,
        Bitboard& blackPawns, Bitboard& blackKnights, Bitboard& blackBishops, 
        Bitboard& blackRooks, Bitboard& blackQueens, Bitboard& blackKing) {
        
        if (DEPTH == 0 || gameOver(WHITE_TURN, moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing)) {
            return evaluate(WHITE_TURN, DEPTH, moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        }
        
        if (WHITE_TURN) {
            const std::string MOVES = moves1.possibleMovesWhite(enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
            float maxScore = std::numeric_limits<float>::lowest();
            for (int i = 0; i < MOVES.length(); i += 5) {
                const std::string MOVE = MOVES.substr(i, 5);
                moves1.doMove(MOVE, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                
                const bool WHITE_CHECKED = whiteKing & moves1.otherThreats(false, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                if (WHITE_CHECKED) {
                    moves1.undoMove(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                    continue;
                }
                
                const float SCORE = minimax(DEPTH - 1, alpha, beta, !WHITE_TURN, false, moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                moves1.undoMove(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                
                if (FIRST_TIME && SCORE > maxScore) {
                    bestMove = MOVE;
                }
                maxScore = std::max(SCORE, maxScore);
                alpha = std::max(alpha, maxScore);
                if (beta <= alpha) {
                    return maxScore;
                }
            }
            return maxScore;
        } else {
            const std::string MOVES = moves1.possibleMovesBlack(enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
            float minScore = std::numeric_limits<float>::max();
            for (int i = 0; i < MOVES.length(); i += 5) {
                const std::string MOVE = MOVES.substr(i, 5);
                moves1.doMove(MOVE, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                
                const bool BLACK_CHECKED = blackKing & moves1.otherThreats(true, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                if (BLACK_CHECKED) {
                    moves1.undoMove(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                    continue;
                }
                
                const float SCORE = minimax(DEPTH - 1, alpha, beta, !WHITE_TURN, false, moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                moves1.undoMove(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
                
                if (FIRST_TIME && SCORE < minScore) {
                    bestMove = MOVE;
                }
                minScore = std::min(SCORE, minScore);
                beta = std::min(beta, minScore);
                if (beta <= alpha) {
                    return minScore;
                }
            }
            return minScore;
        }
    }
    
    std::string getBestMove() {
        return bestMove;
    }
    
    float evaluate(const bool WHITE_TURN, const int DEPTH, Moves moves1, Bitboard enPassant,
            Bitboard whitePawns, Bitboard whiteKnights, Bitboard whiteBishops, 
            Bitboard whiteRooks, Bitboard whiteQueens, Bitboard whiteKing,
            Bitboard blackPawns, Bitboard blackKnights, Bitboard blackBishops, 
            Bitboard blackRooks, Bitboard blackQueens, Bitboard blackKing) {
        
        const bool BLACK_CHECKED = !WHITE_TURN && blackKing & moves1.otherThreats(true, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        const bool BLACK_MATED = BLACK_CHECKED && noBlackMoves(moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        if (BLACK_MATED) {
            return 1000 + DEPTH; // add depth to prioritize faster mates
        }
        
        const bool WHITE_CHECKED = WHITE_TURN && whiteKing & moves1.otherThreats(false, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        const bool WHITE_MATED = WHITE_CHECKED && noWhiteMoves(moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        if (WHITE_MATED) {
            return -1000 - DEPTH; // subtract depth to prioritize faster mates
        }
        
        const bool STALEMATE = (WHITE_TURN && noWhiteMoves(moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing))
            || (!WHITE_TURN && noBlackMoves(moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing))
            || notEnoughPieces(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens);
        if (STALEMATE) {
            return 0;
        }
        
        return materialScore(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens)
            + positionScore(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
    }
    
    bool gameOver(const bool WHITE_TURN, Moves moves1, Bitboard enPassant,
        Bitboard whitePawns, Bitboard whiteKnights, Bitboard whiteBishops, 
        Bitboard whiteRooks, Bitboard whiteQueens, Bitboard whiteKing,
        Bitboard blackPawns, Bitboard blackKnights, Bitboard blackBishops, 
        Bitboard blackRooks, Bitboard blackQueens, Bitboard blackKing) {
        
        return (WHITE_TURN && noWhiteMoves(moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing))
            || (!WHITE_TURN && noBlackMoves(moves1, enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing))
            || notEnoughPieces(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens);
    }
    
    bool noWhiteMoves(Moves moves1, Bitboard& enPassant,
            Bitboard& whitePawns, Bitboard& whiteKnights, Bitboard& whiteBishops, 
            Bitboard& whiteRooks, Bitboard& whiteQueens, Bitboard& whiteKing,
            Bitboard& blackPawns, Bitboard& blackKnights, Bitboard& blackBishops, 
            Bitboard& blackRooks, Bitboard& blackQueens, Bitboard& blackKing) {
        
        const std::string MOVES = moves1.possibleMovesWhite(enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        for (int i = 0; i < MOVES.length(); i += 5) {
            moves1.doMove(MOVES.substr(i, 5), enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
            const bool WHITE_CHECKED = whiteKing & moves1.otherThreats(false, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
            if (!WHITE_CHECKED) {
                return false;
            }
            moves1.undoMove(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        } return true;
    }
    
    bool noBlackMoves(Moves moves1, Bitboard& enPassant,
            Bitboard& whitePawns, Bitboard& whiteKnights, Bitboard& whiteBishops, 
            Bitboard& whiteRooks, Bitboard& whiteQueens, Bitboard& whiteKing,
            Bitboard& blackPawns, Bitboard& blackKnights, Bitboard& blackBishops, 
            Bitboard& blackRooks, Bitboard& blackQueens, Bitboard& blackKing) {
        
        const std::string MOVES = moves1.possibleMovesBlack(enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        for (int i = 0; i < MOVES.length(); i += 5) {
            moves1.doMove(MOVES.substr(i, 5), enPassant, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
            const bool BLACK_CHECKED = blackKing & moves1.otherThreats(true, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
            if (!BLACK_CHECKED) {
                return false;
            }
            moves1.undoMove(whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        } return true;
    }
    
    constexpr bool notEnoughPieces(
        const Bitboard whitePawns, const Bitboard whiteKnights, const Bitboard whiteBishops, 
        const Bitboard whiteRooks, const Bitboard whiteQueens,
        const Bitboard blackPawns, const Bitboard blackKnights, const Bitboard blackBishops, 
        const Bitboard blackRooks, const Bitboard blackQueens) {
        
        return __builtin_popcountll(whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens
            | blackPawns | blackKnights | blackBishops | blackRooks | blackQueens) == 0;
    }
    
    constexpr int materialScore(
        const Bitboard whitePawns, const Bitboard whiteKnights, const Bitboard whiteBishops, 
        const Bitboard whiteRooks, const Bitboard whiteQueens,
        const Bitboard blackPawns, const Bitboard blackKnights, const Bitboard blackBishops, 
        const Bitboard blackRooks, const Bitboard blackQueens) {
        
        return __builtin_popcountll(whitePawns)
            + 3 * __builtin_popcountll(whiteKnights | whiteBishops)
            + 5 * __builtin_popcountll(whiteRooks)
            + 9 * __builtin_popcountll(whiteQueens)
            - __builtin_popcountll(blackPawns)
            - 3 * __builtin_popcountll(blackKnights | blackBishops)
            - 5 * __builtin_popcountll(blackRooks)
            - 9 * __builtin_popcountll(blackQueens);
    }
    
    float positionScore(
        const Bitboard whitePawns, const Bitboard whiteKnights, const Bitboard whiteBishops, 
        const Bitboard whiteRooks, const Bitboard whiteQueens, const Bitboard whiteKing,
        const Bitboard blackPawns, const Bitboard blackKnights, const Bitboard blackBishops, 
        const Bitboard blackRooks, const Bitboard blackQueens, const Bitboard blackKing) {
        
        float score = 0;
        
        // location value for white pieces (excluding king)
        Bitboard pieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens;
        Bitboard smallestPiece;
        int pieceLocation;
        while(pieces) {
            smallestPiece = pieces & -pieces;
            pieces ^= smallestPiece;
            pieceLocation = __builtin_clzll(smallestPiece);
            score += pieceLocationValues[pieceLocation / 8][pieceLocation % 8];
        }
        
        // location value for black pieces (excluding king)
        pieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens;
        while(pieces) {
            smallestPiece = pieces & -pieces;
            pieces ^= smallestPiece;
            pieceLocation = __builtin_clzll(smallestPiece);
            score -= pieceLocationValues[pieceLocation / 8][pieceLocation % 8];
        }
        
        // location value for white king
        if (whiteKing) {
            pieceLocation = __builtin_clzll(whiteKing);
            score += kingLocationValues[pieceLocation / 8][pieceLocation % 8];
        }
        
        // location value for black king
        if (blackKing) {
            pieceLocation = __builtin_clzll(blackKing);
            score -= kingLocationValues[7 - pieceLocation / 8][pieceLocation % 8];
        }
        
        return score;
    }
    
};

#endif