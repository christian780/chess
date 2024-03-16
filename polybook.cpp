#include <iostream>
#include <fstream> // Include the header file for file stream operations
#include <vector>
#include <qglobal.h>

#include "defs.h"
#include "headrs/polykey.h"

struct S_POLY_BOOK_ENTRY {
    uint64_t key;
    unsigned short move;
    unsigned short weight;
    unsigned int learn;
};

long NumEntries = 0;

//S_POLY_BOOK_ENTRY *entries = NULL;

const int PolyKindOfPiece[13] = {
    -1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10
};

// Global variables
bool UseBook = false;

std::vector<S_POLY_BOOK_ENTRY> entries;

//EngineOptions.UseBook = false;


void InitPolyBook() {
     std::ifstream file("performance.bin", std::ios::binary);
    long position;
    long returnValue;
//    S_OPTIONS EngineOptions;


   bool Usebk = UseBook;
//    EngineOptions.UseBook = false;


    if (!file.is_open()) {
        std::cout << "Book File Not Read" << std::endl;
    } else {
        file.seekg(0, std::ios::end);
        position = file.tellg();

        if (position < sizeof(S_POLY_BOOK_ENTRY)) {
            std::cout << "No Entries Found" << std::endl;
            return;
        }

        size_t NumEntries = position / sizeof(S_POLY_BOOK_ENTRY);
        std::cout << NumEntries << " Entries Found In File" << std::endl;

        entries.resize(NumEntries);
//        entries = malloc(NumEntries * sizeof(S_POLY_BOOK_ENTRY)); /* NULL! */

        file.seekg(0, std::ios::beg);
        returnValue = file.read(reinterpret_cast<char*>(entries.data()), NumEntries * sizeof(S_POLY_BOOK_ENTRY)).gcount();
        std::cout << "Read " << returnValue / sizeof(S_POLY_BOOK_ENTRY) << " Entries from File" << std::endl;

        if (NumEntries > 0) {
            UseBook = true;
          //EngineOptions.UseBook = TRUE;
        }
    }

    file.close();
}

void CleanPolyBook(void) {
//    free(entries);
    entries.clear();
}

int HasPawnForCapture(const S_BOARD *board) {
    int sqWithPawn = 0;
    int targetPce = (board->side == Constants::Enums::WHITE) ? Constants::Enums::wP : Constants::Enums::bP;
    if(board->enPas != Constants::Enums::NO_SQ) {
        if(board->side == Constants::Enums::WHITE) {
          sqWithPawn = board->enPas - 10;
        } else {
          sqWithPawn = board->enPas + 10;
        }

        if(board->pieces[sqWithPawn + 1] == targetPce) {
          return TRUE;
        } else if(board->pieces[sqWithPawn - 1] == targetPce) {
          return TRUE;
        }
    }
    return FALSE;
}


U64 PolyKeyFromBoard(const S_BOARD *board) {
    int sq = 0, rank = 0, file = 0;
    U64 finalKey = 0;
    int piece = Constants::Enums::EMPTY;
    int polyPiece = 0;
    int offset = 0;

    for(sq = 0; sq < Constants::BRD_SQ_NUM; ++sq) {
        piece = board->pieces[sq];
        if(piece != Constants::Enums::NO_SQ && piece !=  Constants::Enums::EMPTY && piece != Constants::Enums::OFFBOARD) {
          ASSERT(piece >= Constants::Enums::wP && piece <= Constants::Enums::bK);
          polyPiece = PolyKindOfPiece[piece];
          rank = RanksBrd[sq];
          file = FilesBrd[sq];

          finalKey ^= Random64Poly[(64 * polyPiece) + (8 * rank) + file];
        }
    }

    offset = 768;
    if(board->castlePerm & Constants::Enums::WKCA) finalKey ^= Random64Poly[offset + 0];
    if(board->castlePerm & Constants::Enums::WQCA) finalKey ^= Random64Poly[offset + 1];
    if(board->castlePerm & Constants::Enums::BKCA) finalKey ^= Random64Poly[offset + 2];
    if(board->castlePerm & Constants::Enums::BQCA) finalKey ^= Random64Poly[offset + 3];

    offset = 772;
    if(HasPawnForCapture(board) == TRUE) {
        file = FilesBrd[board->enPas];
        finalKey ^= Random64Poly[offset + file];
    }

    if(board->side == Constants::Enums::WHITE)
        finalKey ^= Random64Poly[780];

    return finalKey;
}

unsigned short endian_swap_u16(unsigned short x) {
    x = (x >> 8) |
        (x << 8);
    return x;
}

unsigned int endian_swap_u32(unsigned int x) {
    x = (x >> 24) |
        ((x << 8) & 0x00ff0000) |
        ((x >> 8) & 0x0000ff00) |
        (x << 24);
    return x;
}

U64 endian_swap_u64(U64 x) {
    x = (x >> 56) |
        ((x << 40) & 0x00ff000000000000ULL) |
        ((x << 24) & 0x0000ff0000000000ULL) |
        ((x << 8)  & 0x000000ff00000000ULL) |
        ((x >> 8)  & 0x00000000ff000000ULL) |
        ((x >> 24) & 0x0000000000ff0000ULL) |
        ((x >> 40) & 0x000000000000ff00ULL) |
        (x << 56);
    return x;
}




int ConvertPolyMoveToInternalMove(unsigned short polyMove, S_BOARD *board) {
    int ff = (polyMove >> 6) & 7;
    int fr = (polyMove >> 9) & 7;
    int tf = (polyMove >> 0) & 7;
    int tr = (polyMove >> 3) & 7;
    int pp = (polyMove >> 12) & 7;
    char promChar = 'q';

    char moveString[6];
    if(pp == 0) {
        sprintf(moveString, "%c%c%c%c",
                FileChar[ff],
                RankChar[fr],
                FileChar[tf],
                RankChar[tr]);
    } else {
        switch(pp) {
        case 1: promChar = 'n';
        case 2: promChar = 'b';
        case 3: promChar = 'r';
        }

        sprintf(moveString, "%c%c%c%c%c",
                FileChar[ff],
                RankChar[fr],
                FileChar[tf],
                RankChar[tr],
                promChar);
    }

    return ParseMove(moveString, board);
}


static int PickMove(int *moves, unsigned short *weight, unsigned int cumWeight, int count) {
    int i, pick;

    pick = rand() % cumWeight;

    for(i = 0; i < count; i++) {
        if(pick < weight[i])
          return moves[i];
        pick -= weight[i];
        cumWeight -= weight[i];
    }
    return weight[count - 1];
}

int GetBookMove(S_BOARD *board) {
    S_POLY_BOOK_ENTRY *entry;
    unsigned short move;
    const int MAXBOOKMOVES = 32;
    int bookMoves[32]; /* !! */
    unsigned short weight[32];
    unsigned int cumWeight = 0;
    int tempMove = NOMOVE;
    int count = 0;

    U64 polyKey = PolyKeyFromBoard(board);

    //for(entry = entries; entry < entries.size() + NumEntries; entry++) {
         for (const auto& entry : entries) {
        if(polyKey == endian_swap_u64(entry.key)) {
          move = endian_swap_u16(entry.move);
          tempMove = ConvertPolyMoveToInternalMove(move, board);
          if(tempMove != NOMOVE) {
              weight[count] = entry.weight;
              cumWeight += weight[count];
              bookMoves[count++] = tempMove;
              if(count > MAXBOOKMOVES)
                  break;
          }
        }
    }

    if(count != 0) {
        return PickMove(bookMoves, weight, cumWeight, count);
    } else {
        return NOMOVE;
    }
}
