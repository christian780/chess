
#include <iostream>

#include "defs.h"
#include "headrs/sqattack.h"





char *PrSq(const int sq) {
    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];

    sprintf(SqStr, "%c%c", ('a' + file), ('1' + rank));
    return SqStr;
}


std::string PrintMove( int move) {
    std::string MvStr;
    char pchar = '\0';

    int ff = FilesBrd[FROMSQ(move)];
    int rf = RanksBrd[FROMSQ(move)];
    int ft = FilesBrd[TOSQ(move)];
    int rt = RanksBrd[TOSQ(move)];

    int promoted = PROMOTED(move);

    if (promoted) {
        pchar = 'q';
        if (IsKn(promoted))
            pchar = 'n';
        else if (IsRQ(promoted) && !IsBQ(promoted))
            pchar = 'r';
        else if (!IsRQ(promoted) && IsBQ(promoted))
            pchar = 'b';

        MvStr = std::string(1, 'a' + ff) + std::to_string(1 + rf) + std::string(1, 'a' + ft) + std::to_string(1 + rt) + pchar;
    } else {
        MvStr = std::string(1, 'a' + ff) + std::to_string(1 + rf) + std::string(1, 'a' + ft) + std::to_string(1 + rt);
    }

    return MvStr;
}

int ParseMove(char *ptrChar, S_BOARD *pos) {
    int from, to;
    int MoveNum = 0, Move = 0, PromPce = EMPTY;
    S_MOVELIST list;

    if (ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
    if (ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
    if (ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
    if (ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

    from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    ASSERT(SqOnBoard(from) && SqOnBoard(to));

    GenerateAllMoves(pos, &list);

    for (MoveNum = 0; MoveNum < list.count; ++MoveNum) {
        Move = list.moves[MoveNum].move;
        if (FROMSQ(Move) == from && TOSQ(Move) == to) {
            PromPce = PROMOTED(Move);
            if (PromPce != EMPTY) {
                if (IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4] == 'r') {
                    return Move;
                } else if (!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'b') {
                    return Move;
                } else if (IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'q') {
                    return Move;
                } else if (IsKn(PromPce) && ptrChar[4] == 'n') {
                    return Move;
                }
                continue;
            }
            return Move;
        }
    }

    return NOMOVE;
}


void PrintMoveList(const S_MOVELIST *list) {
    int index = 0;
    int score = 0;
    int move = 0;
    printf("MoveList:\n");

    for(index = 0; index < list->count; ++index) {
        move = list->moves[index].move;
        score = list->moves[index].score;

        printf("Move:%d > %s (score:%d)\n", index + 1, PrintMove(move), score);
    }

    printf("MoveList Total %d Moves.\n\n", list->count);
}
