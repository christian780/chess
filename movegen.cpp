#include "defs.h"



#define MOVE(f, t, ca, pro, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((pro) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == Constants::Enums::OFFBOARD)


const int LoopSlidePce[8] = { Constants::Enums::wB, Constants::Enums::wR, Constants::Enums::wQ, 0, Constants::Enums::bB, Constants::Enums::bR, Constants::Enums::bQ, 0 };
const int LoopSlideIndex[2] = { 0, 4 };
const int LoopNonSlidePce[6] = { Constants::Enums::wN, Constants::Enums::wK, 0, Constants::Enums::bN, Constants::Enums::bK, 0 };
const int LoopNonSlideIndex[2] = { 0, 3 };


const int PceDir[13][8] = {
    { 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19,	-21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10,	1, 10, 0, 0, 0, 0 },
    { -1, -10,	1, 10, -9, -11, 11, 9 },
    { -1, -10,	1, 10, -9, -11, 11, 9 },
    { 0, 0, 0, 0, 0, 0, 0 },
    { -8, -19,	-21, -12, 8, 19, 21, 12 },
    { -9, -11, 11, 9, 0, 0, 0, 0 },
    { -1, -10,	1, 10, 0, 0, 0, 0 },
    { -1, -10,	1, 10, -9, -11, 11, 9 },
    { -1, -10,	1, 10, -9, -11, 11, 9 }
};

const int NumDir[13] = { 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8 };

const int VictimScore[13] = { 0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600 };
static int MvvLvaScores[13][13];



static void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]] + 1000000;
    list->count++;
}


static void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    ASSERT(PieceValidEmpty(cap));
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    if(RanksBrd[from] == Constants::Enums::RANK_2) {
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::bQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::bR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::bB, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::bN, 0), list);
    } else {
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::EMPTY, 0), list);
    }
}

static void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;

    if(pos->searchKillers[0][pos->ply] == move) {
        list->moves[list->count].score = 900000;
    } else if(pos->searchKillers[1][pos->ply] == move) {
        list->moves[list->count].score = 800000;
    } else {
        list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
    }
    list->count++;
}


static void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    if(RanksBrd[from] == Constants::Enums::RANK_2) {
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::bQ, 0), list);
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::bR, 0), list);
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::bB, 0), list);
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::bN, 0), list);
    } else {
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::EMPTY, 0), list);
    }
}


static void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = 105 + 1000000;
    list->count++;
}





static void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list) {
    ASSERT(PieceValidEmpty(cap));
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    if (RanksBrd[from] == Constants::Enums::RANK_7) {
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::wQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::wR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::wB, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::wN, 0), list);
    } else {
        AddCaptureMove(pos, MOVE(from, to, cap, Constants::Enums::EMPTY, 0), list);
    }
}


/*static void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list) {
    list->moves[list->count].move = move;

    if(pos->searchKillers[0][pos->ply] == move) {
        list->moves[list->count].score = 900000;
    } else if(pos->searchKillers[1][pos->ply] == move) {
        list->moves[list->count].score = 800000;
    } else {
        list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][TOSQ(move)];
    }
    list->count++;
}

*/




static void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list) {
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    if(RanksBrd[from] == Constants::Enums::RANK_7) {
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::wQ, 0), list);
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::wR, 0), list);
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::wB, 0), list);
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::wN, 0), list);
    } else {
        AddQuietMove(pos, MOVE(from, to, Constants::Enums::EMPTY, Constants::Enums::EMPTY, 0), list);
    }
}

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list) {
    int pce = Constants::Enums::EMPTY;
    int side = pos->side;
    int sq = 0, t_sq = 0;
    int pceNum = 0;

    int dir = 0;
    int index = 0;
    int pceIndex = 0;

    ASSERT(CheckBoard(pos));
    list->count = 0;

    if (side == Constants::Enums::WHITE) {
        for (pceNum = 0; pceNum < pos->pceNum[Constants::Enums::wP]; ++pceNum) {
            sq = pos->pList[Constants::Enums::wP][pceNum];
            ASSERT(SqOnBoard(sq));

            if (pos->pieces[sq + 10] == Constants::Enums::EMPTY) {
                AddWhitePawnMove(pos, sq, sq + 10, list);
                if (RanksBrd[sq] == Constants::Enums::RANK_2 && pos->pieces[sq + 20] ==Constants::Enums::EMPTY) {
                    AddQuietMove(pos, MOVE(sq, (sq + 20), Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGPS), list);
                }
            }

            if (!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == Constants::Enums::BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if (!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == Constants::Enums::BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
            }

            if (pos->enPas != Constants::Enums::NO_SQ) {
                if (sq + 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq + 9, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
                if (sq + 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq + 11, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
            }
        }

        if (pos->castlePerm & Constants::Enums::WKCA) {
            if (pos->pieces[Constants::Enums::F1] == Constants::Enums::EMPTY && pos->pieces[Constants::Enums::G1] == Constants::Enums::EMPTY) {
                if (!SqAttacked(Constants::Enums::E1, Constants::Enums::BLACK, pos) && !SqAttacked(Constants::Enums::F1, Constants::Enums::BLACK, pos)) {
                    AddQuietMove(pos, MOVE(Constants::Enums::E1, Constants::Enums::G1, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGCA), list);
                }
            }
        }

        if (pos->castlePerm & Constants::Enums::WQCA) {
            if (pos->pieces[Constants::Enums::D1] == Constants::Enums::EMPTY && pos->pieces[Constants::Enums::C1] == Constants::Enums::EMPTY && pos->pieces[Constants::Enums::B1] == Constants::Enums::EMPTY) {
                if (!SqAttacked(Constants::Enums::E1, Constants::Enums::BLACK, pos) && !SqAttacked(Constants::Enums::D1, Constants::Enums::BLACK, pos)) {
                    AddQuietMove(pos, MOVE(Constants::Enums::E1, Constants::Enums::C1, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGCA), list);
                }
            }
        }
    } else {
        for (pceNum = 0; pceNum < pos->pceNum[Constants::Enums::bP]; ++pceNum) {
            sq = pos->pList[Constants::Enums::bP][pceNum];
            assert(SqOnBoard(sq));

            if (pos->pieces[sq - 10] == Constants::Enums::EMPTY) {
                AddBlackPawnMove(pos, sq, sq - 10, list);
                if (RanksBrd[sq] == Constants::Enums::RANK_7 && pos->pieces[sq - 20] == Constants::Enums::EMPTY) {
                    AddQuietMove(pos, MOVE(sq, (sq - 20), Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGPS), list);
                }
            }

            if (!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == Constants::Enums::WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if (!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == Constants::Enums::WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
            }

            if (pos->enPas != Constants::Enums::NO_SQ) {
                if (sq - 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq - 9, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
                if (sq - 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq - 11, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
            }
        }

        if (pos->castlePerm & Constants::Enums::BKCA) {
            if (pos->pieces[Constants::Enums::F8] == Constants::Enums::EMPTY && pos->pieces[Constants::Enums::G8] == Constants::Enums::EMPTY) {
                if (!SqAttacked(Constants::Enums::E8, Constants::Enums::WHITE, pos) && !SqAttacked(Constants::Enums::F8, Constants::Enums::WHITE, pos)) {
                    AddQuietMove(pos, MOVE(Constants::Enums::E8, Constants::Enums::G8, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGCA), list);
                }
            }
        }

        if (pos->castlePerm & Constants::Enums::BQCA) {
            if (pos->pieces[Constants::Enums::D8] == Constants::Enums::EMPTY && pos->pieces[Constants::Enums::C8] == Constants::Enums::EMPTY && pos->pieces[Constants::Enums::B8] == Constants::Enums::EMPTY) {
                if ((!SqAttacked(Constants::Enums::E8, Constants::Enums::WHITE, pos)) && (!SqAttacked(Constants::Enums::D8, Constants::Enums::WHITE, pos))) {
                    AddQuietMove(pos, MOVE(Constants::Enums::E8, Constants::Enums::C8, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGCA), list);
                }
            }
        }
    }

    pceIndex = LoopSlideIndex[side];
    pce = LoopSlidePce[pceIndex++];
    while (pce != 0) {
        assert(PieceValid(pce));

        for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            assert(SqOnBoard(sq));

            for (index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;

                while (!SQOFFBOARD(t_sq)) {

                    if (pos->pieces[t_sq] != Constants::Enums::EMPTY) {
                        if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                            AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], Constants::Enums::EMPTY, 0), list);
                        }
                        break;
                    }
                    AddQuietMove(pos, MOVE(sq, t_sq, Constants::Enums::EMPTY, Constants::Enums::EMPTY, 0), list);
                    t_sq += dir;
                }
            }
        }
        pce = LoopSlidePce[pceIndex++];
    }

    pceIndex = LoopNonSlideIndex[side];
    pce = LoopNonSlidePce[pceIndex++];
    while (pce != 0) {
        ASSERT(PieceValid(pce));

        for (pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));

            for (index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;

                if (SQOFFBOARD(t_sq))
                    continue;

                if (pos->pieces[t_sq] != Constants::Enums::EMPTY) {
                    if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], Constants::Enums::EMPTY, 0), list);
                    }
                    continue;
                }
                AddQuietMove(pos, MOVE(sq, t_sq, Constants::Enums::EMPTY, Constants::Enums::EMPTY, 0), list);
            }
        }
        pce = LoopNonSlidePce[pceIndex++];
    }
}



void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list) {
    int pce = Constants::Enums::EMPTY;
    int side = pos->side;
    int sq = 0, t_sq = 0;
    int pceNum = 0;

    int dir = 0;
    int index = 0;
    int pceIndex = 0;

    ASSERT(CheckBoard(pos));
    list->count = 0;

    if(side == Constants::Enums::WHITE) {
        for(pceNum = 0; pceNum < pos->pceNum[Constants::Enums::wP]; ++pceNum) {
            sq = pos->pList[Constants::Enums::wP][pceNum];
            ASSERT(SqOnBoard(sq));

            if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == Constants::Enums::BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == Constants::Enums::BLACK) {
                AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
            }

            if(pos->enPas != Constants::Enums::NO_SQ) {
                if(sq + 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq + 9, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
                if(sq + 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq + 11, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
            }
        }

    } else {
        for(pceNum = 0; pceNum < pos->pceNum[Constants::Enums::bP]; ++pceNum) {
            sq = pos->pList[Constants::Enums::bP][pceNum];
            ASSERT(SqOnBoard(sq));

            if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == Constants::Enums::WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == Constants::Enums::WHITE) {
                AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
            }

            if(pos->enPas != Constants::Enums::NO_SQ) {
                if(sq - 9 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq - 9, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
                if(sq - 11 == pos->enPas) {
                    AddEnPassantMove(pos, MOVE(sq, sq - 11, Constants::Enums::EMPTY, Constants::Enums::EMPTY, MFLAGEP), list);
                }
            }
        }

    }

    pceIndex = LoopSlideIndex[side];
    pce = LoopSlidePce[pceIndex++];
    while(pce != 0) {
        ASSERT(PieceValid(pce));

        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));

            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;

                while(!SQOFFBOARD(t_sq)) {

                    if(pos->pieces[t_sq] != Constants::Enums::EMPTY) {
                        if(PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                            AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], Constants::Enums::EMPTY, 0), list);
                        }
                        break;
                    }
                    t_sq += dir;
                }
            }
        }
        pce = LoopSlidePce[pceIndex++];
    }

    pceIndex = LoopNonSlideIndex[side];
    pce = LoopNonSlidePce[pceIndex++];
    while(pce != 0) {
        ASSERT(PieceValid(pce));

        for(pceNum = 0; pceNum < pos->pceNum[pce]; ++pceNum) {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));

            for(index = 0; index < NumDir[pce]; ++index) {
                dir = PceDir[pce][index];
                t_sq = sq + dir;

                if(SQOFFBOARD(t_sq))
                    continue;

                if(pos->pieces[t_sq] != Constants::Enums::EMPTY) {
                    if(PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], Constants::Enums::EMPTY, 0), list);
                    }
                    continue;
                }
            }
        }
        pce = LoopNonSlidePce[pceIndex++];
    }
}

int MoveExists(S_BOARD *pos, const int move) {
    int MoveNum = 0;

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
        if(!MakeMove(pos, list->moves[MoveNum].move)) {
            continue;
        }
        TakeMove(pos);
        if(list->moves[MoveNum].move == move) {
            return TRUE;
        }
    }
    return FALSE;
}

