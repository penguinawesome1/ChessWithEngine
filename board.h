/**
 * Purpose: Create the bitboards for pieces depending on type of chess and display board
 * 
 * Author: Owen Colley
 * Date: 8/7/24
 * 
 */

#include <iostream>
#include <string>
#include <cstdint>
#include <bits/stdc++.h>
#include <algorithm>
#include <random>
#include <cctype>
#ifndef BOARD_H
#define BOARD_H
#include <stdint.h>
#include <iomanip>

typedef uint64_t Bitboard;

// bitboard for en passant location
static Bitboard enPassant;

// bitboards for every piece type
static Bitboard whitePawns;
static Bitboard whiteKnights;
static Bitboard whiteBishops;
static Bitboard whiteRooks;
static Bitboard whiteQueens;
static Bitboard whiteKing;

static Bitboard blackPawns;
static Bitboard blackKnights;
static Bitboard blackBishops;
static Bitboard blackRooks;
static Bitboard blackQueens;
static Bitboard blackKing;

class Board {
private:
    // initial chessboard to convert into bitboards
    char chessBoard[8][8] = {
        { 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' },
        { 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
        { 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P' },
        { 'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' }
    };
    
public:
    Board(const char gameType) {
        // if playing chess960, shuffle rank1 and rank8 the same way
        if (gameType == 'H') {
            char pieces[8] = { 'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R' };
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            std::random_shuffle(std::begin(pieces), std::end(pieces));
            for (int i = 0; i < 8; ++i) {
                chessBoard[7][i] = pieces[i];
                chessBoard[0][i] = tolower(pieces[i]);
            }
        }
        arrayToBitboard();
    }
    
    // cycles through every part of array and sets up bitboards
    void arrayToBitboard() {
        for (int rank = 0; rank < 8; ++rank) {
            for (int file = 0; file < 8; ++file) {
                int index = rank * 8 + file;
                const Bitboard mask = 1ULL << index;
                
                switch (chessBoard[rank][file]) {
                    case 'P': whitePawns |= mask; break;
                    case 'N': whiteKnights |= mask; break;
                    case 'B': whiteBishops |= mask; break;
                    case 'R': whiteRooks |= mask; break;
                    case 'Q': whiteQueens |= mask; break;
                    case 'K': whiteKing |= mask; break;
                    case 'p': blackPawns |= mask; break;
                    case 'n': blackKnights |= mask; break;
                    case 'b': blackBishops |= mask; break;
                    case 'r': blackRooks |= mask; break;
                    case 'q': blackQueens |= mask; break;
                    case 'k': blackKing |= mask; break;
                }
            }
        }
    }
    
    void displayBoard(const int HALF_TURNS, const int MATERIAL, const float SCORE) {
        std::cout << "\n     0    1    2    3    4    5    6    7\n"
            << "  |----|----|----|----|----|----|----|----|\n";
        
        for (int i = 0; i < 64; ++i) {
            if (i % 8 == 0) {std::cout << (7 - i / 8) << " ";}
            
            if ((whitePawns >> i) & 1 == 1) {std::cout << "| WP ";}
            else if ((whiteKnights >> i) & 1 == 1) {std::cout << "| WN ";}
            else if ((whiteBishops >> i) & 1 == 1) {std::cout << "| WB ";}
            else if ((whiteRooks >> i) & 1 == 1) {std::cout << "| WR ";}
            else if ((whiteQueens >> i) & 1 == 1) {std::cout << "| WQ ";}
            else if ((whiteKing >> i) & 1 == 1) {std::cout << "| WK ";}
            else if ((blackPawns >> i) & 1 == 1) {std::cout << "| bp ";}
            else if ((blackKnights >> i) & 1 == 1) {std::cout << "| bn ";}
            else if ((blackBishops >> i) & 1 == 1) {std::cout << "| bb ";}
            else if ((blackRooks >> i) & 1 == 1) {std::cout << "| br ";}
            else if ((blackQueens >> i) & 1 == 1) {std::cout << "| bq ";}
            else if ((blackKing >> i) & 1 == 1) {std::cout << "| bk ";}
            else {std::cout << "|    ";}
            
            if (i % 8 == 7) {std::cout << "| " << (7 - i / 8)
                << "\n  |----|----|----|----|----|----|----|----|\n";}
        } std::cout << "     0    1    2    3    4    5    6    7\n"
            << "Turn: " << (HALF_TURNS / 2) << (HALF_TURNS % 2 == 0 ? "" : ".5")
            << ", Material: " << MATERIAL
            << ", Score: " << std::fixed << std::setprecision(1) << SCORE << "\n" << std::endl;
    }
};

#endif