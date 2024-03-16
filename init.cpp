#include <QRandomGenerator>
#include <QDebug>

#include "headrs/init.h"


int Sq120ToSq64[Constants::BRD_SQ_NUM];
int Sq64ToSq120[64];
U64 CastleKeys[16];
U64 PieceKeys[13][120];
U64 SideKey;
U64 ClearMask[64];
U64 SetMask[64];
int FilesBrd[Constants::BRD_SQ_NUM];
int  RanksBrd[Constants::BRD_SQ_NUM];
 U64 FileBBMask[8];
 U64 RankBBMask[8];

 U64 BlackPassedMask[64];
 U64 WhitePassedMask[64];
 U64 IsolatedMask[64];




void InitEvalMasks() {

    int sq, tsq, r, f;

    for (int sq = 0; sq < 8; ++sq) {
        FileBBMask[sq] = 0ULL;
        RankBBMask[sq] = 0ULL;
    }


    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            int sq = r * 8 + f;
            FileBBMask[f] |= (1ULL << sq);
            RankBBMask[r] |= (1ULL << sq);
        }

        for(sq = 0; sq < 64; ++sq) {
            IsolatedMask[sq] = 0ULL;
            BlackPassedMask[sq] = 0ULL;
            WhitePassedMask[sq] = 0ULL;
        }
    }

    for(sq = 0; sq < 64; ++sq) {
        tsq = sq + 8;

        while(tsq < 64) {
            WhitePassedMask[sq] |= (1ULL << tsq);
            tsq += 8;
        }

        tsq = sq - 8;
        while(tsq >= 0) {
            BlackPassedMask[sq] |= (1ULL << tsq);
            tsq -= 8;
        }

        if(FilesBrd[ Sq64ToSq120[sq]] > Constants::Enums::FILE_A) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[Sq64ToSq120[sq]] - 1];

            tsq = sq + 7;
            while(tsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 9;
            while(tsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }

        if(FilesBrd[Sq64ToSq120[sq]] < Constants::Enums::FILE_H) {
            IsolatedMask[sq] |= FileBBMask[FilesBrd[Sq64ToSq120[sq]] + 1];

            tsq = sq + 9;
            while(tsq < 64) {
                WhitePassedMask[sq] |= (1ULL << tsq);
                tsq += 8;
            }

            tsq = sq - 7;
            while(tsq >= 0) {
                BlackPassedMask[sq] |= (1ULL << tsq);
                tsq -= 8;
            }
        }
    }
}

void InitFilesRanksBrd() {
    int file = 0;
    int rank = 0;
    int sq = 0;

    for (int index = 0; index < Constants::BRD_SQ_NUM; ++index) {
        FilesBrd[index] = Constants::Enums::OFFBOARD;
        RanksBrd[index] =  Constants::Enums::OFFBOARD;
    }

    for (rank = 0; rank < 8; ++rank) {
        for (file = 0; file < 8; ++file) {
            sq = rank * 8 + file;
            FilesBrd[sq] = file;
            RanksBrd[sq] = rank;
        }
    }
}

void InitHashKeys() {
    int index = 0;
    int index2 = 0;

    for(index = 0; index < 13; index++)
        for(index2 = 0; index2 < 120; index2++)
            PieceKeys[index][index2] = RAND_64;

    SideKey = RAND_64;

    for(index = 0; index < 16; index++)
        CastleKeys[index] = RAND_64;
}

void InitBitMasks() {
    int index = 0;

    for(index = 0; index < 64; index++) {
        SetMask[index] = 0ULL;
        ClearMask[index] = 0ULL;
    }

    for(index = 0; index < 64; index++) {
        SetMask[index] |= (1ULL << index);
        ClearMask[index] = ~SetMask[index];
    }
}
void InitSq120To64() {
    int index = 0;
    int file = Constants::Enums::FILE_A;
    int rank = Constants::Enums::RANK_1;
    int sq = Constants::Enums::A1;
    int sq64 = 0;

    for(index = 0; index < Constants::BRD_SQ_NUM; ++index)
        Sq120ToSq64[index] = 65;

    for(index = 0; index < 64; ++index)
        Sq64ToSq120[index] = 120;

    for(rank = Constants::Enums::RANK_1; rank <= Constants::Enums::RANK_8; ++rank) {
        for(file =Constants::Enums::FILE_A; file <= Constants::Enums::FILE_H; ++file) {
            sq = FR2SQ(file, rank);
            Sq64ToSq120[sq64] = sq;
            Sq120ToSq64[sq] = sq64;
            sq64++;
        }
    }
}

int SQ64(int sq120);

void AllInit(void) {
    srand(time(NULL));
    InitSq120To64();
    InitBitMasks();
    InitHashKeys();
   InitFilesRanksBrd();
 //   InitEvalMasks();
 //   InitMvvLva();
//    InitPolyBook();
}
