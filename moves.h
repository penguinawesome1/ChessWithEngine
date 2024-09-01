/**
 * Purpose: Do and undo moves as well as find all possible moves
 * 
 * Author: Owen Colley
 * Date: 8/7/24
 * 
 */

#include <iostream>
#include <string>
#include <bit>
#ifndef MOVES_H
#define MOVES_H
#include <stdint.h>
#include <vector>
#include <stdint.h>

typedef uint64_t Bitboard;

class Moves {
private:
    // save row masks for moves
    const Bitboard RANK_1 = -72057594037927936L;
    const Bitboard RANK_4 = 1095216660480L;;
    const Bitboard RANK_5 = 4278190080L;;
    const Bitboard RANK_8 = 255L;
    
    // save column masks for moves
    const Bitboard FILE_A = 72340172838076673L;
    const Bitboard FILE_AB = 217020518514230019L;
    const Bitboard FILE_H = -9187201950435737472L;
    const Bitboard FILE_GH = -4557430888798830400L;
    
    // masks for knight and king moves
    const Bitboard KNIGHT_SPAN = 43234889994L;
    const Bitboard KING_SPAN = 460039L;
    
    // Bitboard masks for ranks and files
    const Bitboard RANK_MASKS[8] = {
        0xFF, 0xFF00, 0xFF0000, 0xFF000000, 0xFF00000000, 0xFF0000000000, 
        0xFF000000000000, 0xFF00000000000000
    };
    
    const Bitboard FILE_MASKS[8] = {
        0x0101010101010101, 0x0202020202020202, 0x0404040404040404,
        0x0808080808080808, 0x1010101010101010, 0x2020202020202020,
        0x4040404040404040, 0x8080808080808080
    };
    
    // Bitboard masks for diagonals
    const Bitboard DIAGONAL_MASKS1[15] = {
        0x1L, 0x102L, 0x10204L, 0x1020408L, 0x102040810L, 0x10204081020L, 
        0x1020408102040L, 0x102040810204080L, 0x204081020408000L, 
        0x408102040800000L, 0x810204080000000L, 0x1020408000000000L, 
        0x2040800000000000L, 0x4080000000000000L, 0x8000000000000000L
    };
    
    const Bitboard DIAGONAL_MASKS2[15] = {
        0x80L, 0x8040L, 0x804020L, 0x80402010L, 0x8040201008L, 0x804020100804L, 
        0x80402010080402L, 0x8040201008040201L, 0x4020100804020100L, 
        0x2010080402010000L, 0x1008040201000000L, 0x804020100000000L, 
        0x402010000000000L, 0x201000000000000L, 0x100000000000000L
    };
    
    bool whiteShortCastle;
    bool whiteLongCastle;
    bool blackShortCastle;
    bool blackLongCastle;
    
    Bitboard whiteLeftRook;
    Bitboard whiteRightRook;
    Bitboard blackLeftRook;
    Bitboard blackRightRook;
    
    struct moveData {
        char type1;
        char type2;
        char type3;
        Bitboard board1;
        Bitboard board2;
        Bitboard board3;
        bool whiteShortCastle;
        bool whiteLongCastle;
        bool blackShortCastle;
        bool blackLongCastle;
    };
    
    std::vector<moveData> moveHistory;
    
public:
    Moves(Bitboard whiteRooks, Bitboard blackRooks) {
        // castling is legal at the start of games
        whiteShortCastle = false;
        whiteLongCastle = false;
        blackShortCastle = false;
        blackLongCastle = false;
        
        whiteLeftRook = whiteRooks & -whiteRooks;
        whiteRightRook = whiteRooks ^ whiteLeftRook;
        blackLeftRook = blackRooks & -blackRooks;
        blackRightRook = blackRooks ^ blackLeftRook;
    }
    
    void doMove(const std::string move, Bitboard& enPassant,
        Bitboard& whitePawns, Bitboard& whiteKnights, Bitboard& whiteBishops, 
        Bitboard& whiteRooks, Bitboard& whiteQueens, Bitboard& whiteKing,
        Bitboard& blackPawns, Bitboard& blackKnights, Bitboard& blackBishops, 
        Bitboard& blackRooks, Bitboard& blackQueens, Bitboard& blackKing) {
        
        moveData moveData;
        moveData.whiteShortCastle = whiteShortCastle;
        moveData.whiteLongCastle = whiteLongCastle;
        moveData.blackShortCastle = whiteShortCastle;
        moveData.blackLongCastle = whiteLongCastle;
        
        // grab coordinates from input string
        const int X1 = move[1] - '0';
        const int Y1 = move[2] - '0';
        const int X2 = move[3] - '0';
        const int Y2 = move[4] - '0';
        const Bitboard START_SQUARE = 1L << (X1 + 8 * (7 - Y1));
        const Bitboard END_SQUARE = 1L << (X2 + 8 * (7 - Y2));
        
        // set en passant temp and update en passant
        const Bitboard EN_PASSANT_TEMP = enPassant;
        const bool DOUBLE_PAWN_PUSH = (whitePawns | blackPawns) & START_SQUARE
            && (START_SQUARE << 16 | START_SQUARE >> 16) & END_SQUARE;
        enPassant = DOUBLE_PAWN_PUSH ? END_SQUARE : 0;
        
        // mark which piece was captured, then remove it
        const bool WHITE_DID_EN_PASSANT = EN_PASSANT_TEMP & END_SQUARE << 8  && whitePawns & START_SQUARE;
        const bool BLACK_DID_EN_PASSANT = EN_PASSANT_TEMP & END_SQUARE >> 8 && blackPawns & START_SQUARE;
        const char CAPTURED_PIECE_TYPE = (whitePawns & END_SQUARE) ? 'P' :
                                        (whiteKnights & END_SQUARE) ? 'N' :
                                        (whiteBishops & END_SQUARE) ? 'B' :
                                        (whiteRooks & END_SQUARE) ? 'R' :
                                        (whiteQueens & END_SQUARE) ? 'Q' :
                                        (blackPawns & END_SQUARE) ? 'p' :
                                        (blackKnights & END_SQUARE) ? 'n' :
                                        (blackBishops & END_SQUARE) ? 'b' :
                                        (blackRooks & END_SQUARE) ? 'r' :
                                        (blackQueens & END_SQUARE) ? 'q' : 0;
                                        (BLACK_DID_EN_PASSANT) ? 'E' :
                                        (WHITE_DID_EN_PASSANT) ? 'e' : 0;
        moveData.type1 = CAPTURED_PIECE_TYPE;
        switch (CAPTURED_PIECE_TYPE) {
            case ('P'): moveData.board1 = whitePawns; whitePawns ^= END_SQUARE; break;
            case ('N'): moveData.board1 = whiteKnights; whiteKnights ^= END_SQUARE; break;
            case ('B'): moveData.board1 = whiteBishops; whiteBishops ^= END_SQUARE; break;
            case ('R'): moveData.board1 = whiteRooks; whiteRooks ^= END_SQUARE;
                whiteShortCastle &= !(END_SQUARE == whiteRightRook);
                whiteLongCastle &= !(END_SQUARE == whiteLeftRook); break;
            case ('Q'): moveData.board1 = whiteQueens; whiteQueens ^= END_SQUARE; break;
            case ('p'): moveData.board1 = blackPawns; blackPawns ^= END_SQUARE; break;
            case ('n'): moveData.board1 = blackKnights; blackKnights ^= END_SQUARE; break;
            case ('b'): moveData.board1 = blackBishops; blackBishops ^= END_SQUARE; break;
            case ('r'): moveData.board1 = blackRooks; blackRooks ^= END_SQUARE;
                blackShortCastle &= !(END_SQUARE == blackRightRook);
                blackLongCastle &= !(END_SQUARE == blackLeftRook); break;
            case ('q'): moveData.board1 = blackQueens; blackQueens ^= END_SQUARE; break;
            case ('E'): moveData.board1 = whitePawns; whitePawns ^= EN_PASSANT_TEMP; break;
            case ('e'): moveData.board1 = blackPawns; blackPawns ^= EN_PASSANT_TEMP; break;
        }
        
        // mark which piece was moved, then move it
        const char PIECE_TYPE = (whitePawns     & START_SQUARE) ? 'P' :
                                (whiteKnights   & START_SQUARE) ? 'N' :
                                (whiteBishops   & START_SQUARE) ? 'B' :
                                (whiteRooks     & START_SQUARE) ? 'R' :
                                (whiteQueens    & START_SQUARE) ? 'Q' :
                                (whiteKing      & START_SQUARE) ? 'K' :
                                (blackPawns     & START_SQUARE) ? 'p' :
                                (blackKnights   & START_SQUARE) ? 'n' :
                                (blackBishops   & START_SQUARE) ? 'b' :
                                (blackRooks     & START_SQUARE) ? 'r' :
                                (blackQueens    & START_SQUARE) ? 'q' :
                                (blackKing      & START_SQUARE) ? 'k' : 0;
        moveData.type2 = PIECE_TYPE;
        switch (PIECE_TYPE) {
            case ('P'): moveData.board2 = whitePawns;    whitePawns   ^= START_SQUARE | END_SQUARE; break;
            case ('N'): moveData.board2 = whiteKnights;  whiteKnights ^= START_SQUARE | END_SQUARE; break;
            case ('B'): moveData.board2 = whiteBishops;  whiteBishops ^= START_SQUARE | END_SQUARE; break;
            case ('R'): moveData.board2 = whiteRooks;    whiteRooks   ^= START_SQUARE | END_SQUARE;
                whiteShortCastle &= !(START_SQUARE == whiteRightRook);
                whiteLongCastle &= !(START_SQUARE == whiteLeftRook); break;
            case ('Q'): moveData.board2 = whiteQueens;   whiteQueens  ^= START_SQUARE | END_SQUARE; break;
            case ('K'): moveData.board2 = whiteKing;     whiteKing    ^= START_SQUARE | END_SQUARE; break;
                whiteShortCastle = false; whiteLongCastle = false; break;
            case ('p'): moveData.board2 = blackPawns;    blackPawns   ^= START_SQUARE | END_SQUARE; break;
            case ('n'): moveData.board2 = blackKnights;  blackKnights ^= START_SQUARE | END_SQUARE; break;
            case ('b'): moveData.board2 = blackBishops;  blackBishops ^= START_SQUARE | END_SQUARE; break;
            case ('r'): moveData.board2 = blackRooks;    blackRooks   ^= START_SQUARE | END_SQUARE;
                blackShortCastle &= !(START_SQUARE == blackRightRook);
                blackLongCastle &= !(START_SQUARE == blackLeftRook); break;
            case ('q'): moveData.board2 = blackQueens;   blackQueens  ^= START_SQUARE | END_SQUARE; break;
            case ('k'): moveData.board2 = blackKing;     blackKing    ^= START_SQUARE | END_SQUARE;
                blackShortCastle = false; blackLongCastle = false; break;
        }
        
        // promote pawns
        const bool MOVED_LEFT = __builtin_clzll(START_SQUARE) < __builtin_clzll(END_SQUARE);
        moveData.type3 = move[0];
        switch (move[0]) {
            case ('N'): moveData.board3 = whiteKnights; whitePawns ^= END_SQUARE; whiteKnights |= END_SQUARE; break;
            case ('B'): moveData.board3 = whiteBishops; whitePawns ^= END_SQUARE; whiteBishops |= END_SQUARE; break;
            case ('R'): moveData.board3 = whiteRooks; whitePawns ^= END_SQUARE; whiteRooks |= END_SQUARE; break;
            case ('Q'): moveData.board3 = whiteQueens; whitePawns ^= END_SQUARE; whiteQueens |= END_SQUARE; break;
            case ('n'): moveData.board3 = blackKnights; blackPawns ^= END_SQUARE; blackKnights |= END_SQUARE; break;
            case ('b'): moveData.board3 = blackBishops; blackPawns ^= END_SQUARE; blackBishops |= END_SQUARE; break;
            case ('r'): moveData.board3 = blackRooks; blackPawns ^= END_SQUARE; blackRooks |= END_SQUARE; break;
            case ('q'): moveData.board3 = blackQueens; blackPawns ^= END_SQUARE; blackQueens |= END_SQUARE; break;
            case ('C'): moveData.board3 = whiteRooks;
                whiteRooks ^= (MOVED_LEFT ? whiteLeftRook | whiteKing << 1 : whiteRightRook | whiteKing >> 1); break;
            case ('c'): moveData.board3 = blackRooks;
                blackRooks ^= (MOVED_LEFT ? blackLeftRook | blackKing << 1 : blackRightRook | blackKing >> 1); break;
        }
        
        moveHistory.push_back(moveData);
    }
    
    void undoMove(Bitboard& whitePawns, Bitboard& whiteKnights, Bitboard& whiteBishops, 
                Bitboard& whiteRooks, Bitboard& whiteQueens, Bitboard& whiteKing,
                Bitboard& blackPawns, Bitboard& blackKnights, Bitboard& blackBishops, 
                Bitboard& blackRooks, Bitboard& blackQueens, Bitboard& blackKing) {
        
        const moveData moveData = moveHistory.back();
        const std::vector<char> TYPES = {moveData.type1, moveData.type2, moveData.type3};
        const std::vector<Bitboard> BOARDS = {moveData.board1, moveData.board2, moveData.board3};
        whiteShortCastle = moveData.whiteShortCastle;
        whiteLongCastle = moveData.whiteLongCastle;
        blackShortCastle = moveData.blackShortCastle;
        blackLongCastle = moveData.blackLongCastle;
        moveHistory.pop_back();
        
        for (int i = 0; i < 3; ++i) {
            switch (TYPES[i]) {
                case ('E'):
                case ('P'): whitePawns = BOARDS[i]; break;
                case ('N'): whiteKnights = BOARDS[i]; break;
                case ('B'): whiteBishops = BOARDS[i]; break;
                case ('C'):
                case ('R'): whiteRooks = BOARDS[i]; break;
                case ('Q'): whiteQueens = BOARDS[i]; break;
                case ('K'): whiteKing = BOARDS[i]; break;
                case ('e'):
                case ('p'): blackPawns = BOARDS[i]; break;
                case ('n'): blackKnights = BOARDS[i]; break;
                case ('b'): blackBishops = BOARDS[i]; break;
                case ('c'):
                case ('r'): blackRooks = BOARDS[i]; break;
                case ('q'): blackQueens = BOARDS[i]; break;
                case ('k'): blackKing = BOARDS[i]; break;
            }
        }
    }
    
    // return a string of all possible white moves from the current posttion given all bitboards
    // formatted as x1y1x2y2
    std::string possibleMovesWhite(Bitboard enPassant,
            Bitboard whitePawns, Bitboard whiteKnights, Bitboard whiteBishops, 
            Bitboard whiteRooks, Bitboard whiteQueens, Bitboard whiteKing,
            Bitboard blackPawns, Bitboard blackKnights, Bitboard blackBishops, 
            Bitboard blackRooks, Bitboard blackQueens, Bitboard blackKing) {
        
        const Bitboard cantCapture = whitePawns | whiteKnights | whiteBishops | whiteRooks 
            | whiteQueens | whiteKing | blackKing;
        const Bitboard empty = ~(whitePawns | whiteKnights | whiteBishops | whiteRooks
            | whiteQueens | whiteKing | blackPawns | blackKnights | blackBishops
            | blackRooks | blackQueens | blackKing);
        const Bitboard threats = otherThreats(false, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        
        return possibleP(true, cantCapture, empty, enPassant, whitePawns)
            + possibleN(cantCapture, whiteKnights)
            + possibleSliderMoves('b', cantCapture, empty, whiteBishops)
            + possibleSliderMoves('r', cantCapture, empty, whiteRooks)
            + possibleSliderMoves('q', cantCapture, empty, whiteQueens)
            + possibleK(cantCapture, empty, threats, whiteKing);
    }
    
    // return a string of all possible black moves from the current posttion given all bitboards
    // formatted as x1y1x2y2
    std::string possibleMovesBlack(Bitboard enPassant,
            Bitboard whitePawns, Bitboard whiteKnights, Bitboard whiteBishops, 
            Bitboard whiteRooks, Bitboard whiteQueens, Bitboard whiteKing,
            Bitboard blackPawns, Bitboard blackKnights, Bitboard blackBishops, 
            Bitboard blackRooks, Bitboard blackQueens, Bitboard blackKing) {
        
        const Bitboard cantCapture = blackPawns | blackKnights | blackBishops | blackRooks
            | blackQueens | blackKing | whiteKing;
        const Bitboard empty =
            ~(whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing
            | blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing);
        const Bitboard threats = otherThreats(true, whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing, blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing);
        
        return possibleP(false, cantCapture, empty, enPassant, blackPawns)
            + possibleN(cantCapture, blackKnights) 
            + possibleSliderMoves('b', cantCapture, empty, blackBishops)
            + possibleSliderMoves('r', cantCapture, empty, blackRooks)
            + possibleSliderMoves('q', cantCapture, empty, blackQueens)
            + possibleK(cantCapture, empty, threats, blackKing);
    }
    
    // return a string of all possible white pawn moves from the current posttion given white pawns
    std::string possibleP(const bool WHITE_MOVES, Bitboard cantCapture, Bitboard empty, Bitboard enPassant, Bitboard pawns) {
        std::string allMoves = "";
        
        if (!WHITE_MOVES) {
            empty = flipVertical(empty);
            cantCapture = flipVertical(cantCapture);
            enPassant = flipVertical(enPassant);
            pawns = flipVertical(pawns);
        }
        
        // left capture
        int row, col;
        Bitboard pawnMoves = pawns >> 9 & (~empty | (enPassant >> 8)) & ~cantCapture & ~FILE_H & ~RANK_8;
        while (pawnMoves) {
            row = __builtin_ctzll(pawnMoves) % 8;
            col = __builtin_ctzll(pawnMoves) / 8;
            allMoves += " " + std::to_string(row + 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            pawnMoves &= pawnMoves - 1;
        }
        
         // right capture
        pawnMoves = pawns >> 7 & (~empty | (enPassant >> 8)) & ~cantCapture & ~FILE_A & ~RANK_8;
        while (pawnMoves) {
            row = __builtin_ctzll(pawnMoves) % 8;
            col = __builtin_ctzll(pawnMoves) / 8;
            allMoves += " " + std::to_string(row - 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            pawnMoves &= pawnMoves - 1;
        }
        
        // push once
        pawnMoves = pawns >> 8 & empty & ~RANK_8;
        while (pawnMoves) {
            row = __builtin_ctzll(pawnMoves) % 8;
            col = __builtin_ctzll(pawnMoves) / 8;
            allMoves += " " + std::to_string(row) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            pawnMoves &= pawnMoves - 1;
        }
        
        // push twice
        pawnMoves = pawns >> 16 & RANK_4 & empty & empty >> 8;
        while (pawnMoves) {
            row = __builtin_ctzll(pawnMoves) % 8;
            col = __builtin_ctzll(pawnMoves) / 8;
            allMoves += " " + std::to_string(row) + std::to_string(7 - (col + 2)) + std::to_string(row) + std::to_string(7 - col);
            pawnMoves &= pawnMoves - 1;
        }
        
        // left capture to promote
        pawnMoves = pawns >> 9 & ~empty & ~cantCapture & ~FILE_H & RANK_8;
        while (pawnMoves) {
            int row = __builtin_ctzll(pawnMoves) % 8;
            int col = __builtin_ctzll(pawnMoves) / 8;
            allMoves += "N" + std::to_string(row + 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "B" + std::to_string(row + 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "R" + std::to_string(row + 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "Q" + std::to_string(row + 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            pawnMoves &= pawnMoves - 1;
        }
        
        // right capture to promote
        pawnMoves = pawns >> 7 & ~empty & ~cantCapture & ~FILE_A & RANK_8;
        while (pawnMoves) {
            int row = __builtin_ctzll(pawnMoves) % 8;
            int col = __builtin_ctzll(pawnMoves) / 8;
            allMoves += "N" + std::to_string(row - 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "B" + std::to_string(row - 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "R" + std::to_string(row - 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "Q" + std::to_string(row - 1) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            pawnMoves &= pawnMoves - 1;
        }
        
        // push once to promote
        pawnMoves = pawns >> 8 & empty & RANK_8;
        while (pawnMoves) {
            int row = __builtin_ctzll(pawnMoves) % 8;
            int col = __builtin_ctzll(pawnMoves) / 8;
            allMoves += "N" + std::to_string(row) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "B" + std::to_string(row) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "R" + std::to_string(row) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            allMoves += "Q" + std::to_string(row) + std::to_string(7 - (col + 1)) + std::to_string(row) + std::to_string(7 - col);
            pawnMoves &= pawnMoves - 1;
        }
        
        if (!WHITE_MOVES) {
            std::string temp = "";
            temp.reserve(allMoves.length());
            std::string lower;
            int y1, y2;
            for (int i = 0; i < allMoves.length(); i += 5) {
                lower = std::tolower(allMoves[i]);
                y1 = '7' - allMoves[i + 2];
                y2 = '7' - allMoves[i + 4];
                temp += lower + allMoves[i + 1] + std::to_string(y1) + allMoves[i + 3] + std::to_string(y2);
            }
            allMoves = temp;
        }
        
        return allMoves;
    }
    
    constexpr Bitboard flipVertical(const Bitboard b) {
        return (b << 56)
            | ((b << 40) & 0x00ff000000000000)
            | ((b << 24) & 0x0000ff0000000000)
            | ((b <<  8) & 0x000000ff00000000)
            | ((b >>  8) & 0x00000000ff000000)
            | ((b >> 24) & 0x0000000000ff0000)
            | ((b >> 40) & 0x000000000000ff00)
            | (b >> 56);
    }
    
    // return a string of all possible knight moves from the current posttion given knights
    std::string possibleN(const Bitboard cantCapture, Bitboard knights) {
        std::string allMoves = "";
        
        int possibilityLoc;
        int knightLoc;
        while(knights) {
            knightLoc = __builtin_ctzll(knights);
            Bitboard possibility = 0;
            // shift in correct direction
            possibility = knightLoc > 18 ?
                        KNIGHT_SPAN << (knightLoc - 18) :
                        KNIGHT_SPAN >> (18 - knightLoc);
            
            // get rid of overflow files and cant captures
            possibility &= knightLoc % 8 < 4 ?
                        ~FILE_GH & ~cantCapture :
                        ~FILE_AB & ~cantCapture;
            
            while (possibility) {
                possibilityLoc = __builtin_ctzll(possibility);
                allMoves += " " + std::to_string(knightLoc % 8) + std::to_string(7 - knightLoc / 8)
                        + std::to_string(possibilityLoc % 8) + std::to_string(7 - possibilityLoc / 8);
                possibility &= possibility - 1; // get rid of lowest
            }
            knights &= knights - 1; // get rid of smallest knight
        }
        return allMoves;
    }
    
    // return a string of all possible slider moves of the specified type from the current posttion
    std::string possibleSliderMoves(const char PIECE_TYPE, const Bitboard cantCapture, const Bitboard empty, Bitboard pieces) {
        std::string allMoves = "";
        
        Bitboard smallestPiece;
        Bitboard possibilities;
        int pieceLocation;
        int possibilityLocation;
        while (pieces) {
            pieceLocation = __builtin_ctzll(pieces);
            smallestPiece = pieces & -pieces;
            
            if (PIECE_TYPE == 'b') { // if the pieces are bishops
                possibilities = hypQuint(empty, smallestPiece, DIAGONAL_MASKS1[pieceLocation / 8 + pieceLocation % 8])
                    | hypQuint(empty, smallestPiece, DIAGONAL_MASKS2[pieceLocation / 8 + 7 - pieceLocation % 8]);
            } else if (PIECE_TYPE == 'r') { // if the pieces are rooks
                possibilities = hypQuint(empty, smallestPiece, FILE_MASKS[pieceLocation % 8])
                    | hypQuint(empty, smallestPiece, RANK_MASKS[pieceLocation / 8]);
            } else { // if the pieces are queens
                possibilities = hypQuint(empty, smallestPiece, DIAGONAL_MASKS1[pieceLocation / 8 + pieceLocation % 8])
                    | hypQuint(empty, smallestPiece, DIAGONAL_MASKS2[pieceLocation / 8 + 7 - pieceLocation % 8])
                    | hypQuint(empty, smallestPiece, FILE_MASKS[pieceLocation % 8])
                    | hypQuint(empty, smallestPiece, RANK_MASKS[pieceLocation / 8]);
            } possibilities &= ~cantCapture;
            
            while (possibilities) {
                possibilityLocation = __builtin_ctzll(possibilities);
                allMoves += " " + std::to_string( pieceLocation % 8 ) + std::to_string( 7 - pieceLocation / 8 )
                        + std::to_string( possibilityLocation % 8 ) + std::to_string( 7 - possibilityLocation / 8 );
                possibilities &= possibilities - 1;
            }
            pieces &= pieces - 1;
        }
        return allMoves;
    }
    
    // returns bitboard of all possible moves for chosen slider pieces given mask
    constexpr Bitboard hypQuint(const Bitboard empty, const Bitboard square, const Bitboard mask) {
        return ((((mask & ~empty) - (square * 2)) 
            ^ reverse(reverse(mask & ~empty) - (reverse(square) * 2))))
            & mask;
    }
    
    // Reverse function for Hyperbola Quintessence
    constexpr Bitboard reverse(Bitboard b) {
        b = (b & 0x5555555555555555) << 1 | ((b >> 1) & 0x5555555555555555);
        b = (b & 0x3333333333333333) << 2 | ((b >> 2) & 0x3333333333333333);
        b = (b & 0x0f0f0f0f0f0f0f0f) << 4 | ((b >> 4) & 0x0f0f0f0f0f0f0f0f);
        b = (b & 0x00ff00ff00ff00ff) << 8 | ((b >> 8) & 0x00ff00ff00ff00ff);
    
        return (b << 48) | ((b & 0xffff0000) << 16) | ((b >> 16) & 0xffff0000) | (b >> 48);
    }
    
    // return a string of all possible king moves from the current posttion given king
    std::string possibleK(const Bitboard cantCapture, const Bitboard empty, const Bitboard threats, const Bitboard king) {
        std::string allMoves = "";
        
        const int KING_LOC = __builtin_ctzll(king);
        Bitboard possibility = 0;
        int possibilityLocation;
        
        // shift in correct direction
        possibility = KING_LOC > 9 ? KING_SPAN << (KING_LOC - 9) :
                                    KING_SPAN >> (9 - KING_LOC);
        
        // get rid of overflow files and illegal moves.
        possibility &= KING_LOC % 8 < 4 ? ~FILE_GH : ~FILE_AB;
        possibility &= ~(cantCapture | threats);
        
        while (possibility) {
            possibilityLocation = __builtin_ctzll(possibility);
            allMoves += " " + std::to_string(KING_LOC % 8) + std::to_string(7 - KING_LOC / 8)
                    + std::to_string(possibilityLocation % 8) + std::to_string(7 - possibilityLocation / 8);
            possibility &= possibility - 1; // get rid of lowest
        }
        
        possibility = 0;
        const bool KING_FRONT_ROW = KING_LOC / 8 == 7;
        if (KING_FRONT_ROW) {
            if (whiteLongCastle) { // white long castle
                possibility |= king >> 2 & empty & empty >> 1 & ~(threats >> 1);
            }
            if (whiteShortCastle) { // white short castle
                possibility |= king << 2 & empty & empty << 1 & ~(threats << 1);
            }
            possibility &= ~threats;
            
            while (possibility) {
                int possibilityLocation = __builtin_ctzll(possibility);
                allMoves += "C" + std::to_string(KING_LOC % 8) + std::to_string(7 - KING_LOC / 8) 
                        + std::to_string(possibilityLocation % 8) + std::to_string(7 - possibilityLocation / 8);
                possibility &= possibility - 1; // get rid of lowest
            }
        } else {
            if (blackLongCastle) { // black long castle
                possibility |= king >> 2 & empty & empty >> 1 & ~(threats >> 1);
            }
            if (blackShortCastle) { // black short castle
                possibility |= king << 2 & empty & empty << 1 & ~(threats << 1);
            }
            possibility &= ~threats;
            
            while (possibility) {
                int possibilityLocation = __builtin_ctzll(possibility);
                allMoves += "c" + std::to_string(KING_LOC % 8) + std::to_string(7 - KING_LOC / 8) 
                        + std::to_string(possibilityLocation % 8) + std::to_string(7 - possibilityLocation / 8);
                possibility &= possibility - 1; // get rid of lowest
            }
        }
        
        return allMoves;
    }
    
    Bitboard otherThreats(const bool WHITES_THREATS,
            const Bitboard whitePawns, const Bitboard whiteKnights, const Bitboard whiteBishops, 
            const Bitboard whiteRooks, const Bitboard whiteQueens, const Bitboard whiteKing,
            const Bitboard blackPawns, const Bitboard blackKnights, const Bitboard blackBishops, 
            const Bitboard blackRooks, const Bitboard blackQueens, const Bitboard blackKing) {
        
        Bitboard pawns, knights, bishops, rooks, queens, king;
        if (WHITES_THREATS) {
            pawns = whitePawns; knights = whiteKnights;
            bishops = whiteBishops; rooks = whiteRooks;
            queens = whiteQueens; king = whiteKing;
        } else {
            pawns = blackPawns; knights = blackKnights;
            bishops = blackBishops; rooks = blackRooks;
            queens = blackQueens; king = blackKing;
        }
        
        const Bitboard empty = ~(whitePawns | whiteKnights | whiteBishops | whiteRooks
            | whiteQueens | whiteKing | blackPawns | blackKnights | blackBishops
            | blackRooks | blackQueens | blackKing);
        
        Bitboard possibility = 0;
        int pieceLocation;
        Bitboard smallestPiece;
        
        // add pawn attacks
        Bitboard allThreats = WHITES_THREATS ? (pawns >> 7 & ~FILE_A) | (pawns >> 9 & ~FILE_H)
                                    : (pawns << 7 & ~FILE_H) | (pawns << 9 & ~FILE_A);
        
        // add knight attacks
        while (knights) {
            pieceLocation = __builtin_ctzll(knights);
            possibility = (pieceLocation > 18 ? KNIGHT_SPAN << (pieceLocation - 18) 
                : KNIGHT_SPAN >> (18 - pieceLocation));
            possibility &= pieceLocation % 8 < 4 ? ~FILE_GH : ~FILE_AB;
            allThreats |= possibility;
            knights &= knights - 1;
        }
        
        // add bishop attacks
        while(bishops) {
            pieceLocation = __builtin_ctzll(bishops);
            smallestPiece = bishops & -bishops;
            allThreats |= hypQuint(empty, smallestPiece, DIAGONAL_MASKS1[pieceLocation / 8 + pieceLocation % 8])
                | hypQuint(empty, smallestPiece, DIAGONAL_MASKS2[pieceLocation / 8 + 7 - pieceLocation % 8]);
            bishops ^= smallestPiece;
        }
        
        // add rook attacks
        while(rooks) {
            pieceLocation = __builtin_ctzll(rooks);
            smallestPiece = rooks & -rooks;
            allThreats |= hypQuint(empty, smallestPiece, FILE_MASKS[pieceLocation % 8])
                    | hypQuint(empty, smallestPiece, RANK_MASKS[pieceLocation / 8]);
            rooks ^= smallestPiece;
        }
        
        // add queen attacks
        while(queens) {
            pieceLocation = __builtin_ctzll(queens);
            smallestPiece = queens & -queens;
            allThreats |= hypQuint(empty, smallestPiece, DIAGONAL_MASKS1[pieceLocation / 8 + pieceLocation % 8])
                | hypQuint(empty, smallestPiece, DIAGONAL_MASKS2[pieceLocation / 8 + 7 - pieceLocation % 8])
                | hypQuint(empty, smallestPiece, FILE_MASKS[pieceLocation % 8])
                | hypQuint(empty, smallestPiece, RANK_MASKS[pieceLocation / 8]);
            queens ^= smallestPiece;
        }
        
        // add king attacks
        pieceLocation = __builtin_ctzll(king);
        possibility = pieceLocation > 9 ? KING_SPAN << (pieceLocation - 9) 
                                        : KING_SPAN >> (9 - pieceLocation);
        possibility &= pieceLocation % 8 < 4 ? ~FILE_GH : ~FILE_AB;
        allThreats |= possibility;
        
        return allThreats;
    }
    
};

#endif