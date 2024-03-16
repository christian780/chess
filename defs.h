#ifndef DEFS_H
#define DEFS_H
#include <iostream>
#include <cstdlib>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QString>
#include <QtGlobal>
#include <QCoreApplication>
#include <QDateTime>


#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
do {\
        if (!(n)) { \
            qDebug() << QDate::currentDate(); \
            qDebug() << QTime::currentTime(); \
            qDebug() << "Assertion" << #n << "failed."; \
            qDebug() << "File" << __FILE__; \
            qDebug() << "Line" << __LINE__; \
            getchar(); \
            QCoreApplication::exit(1); \
    } \
} while(0);
#endif




typedef quint64 U64;
//typedef std::vector<std::vector<U64>> U64Vector2D;
//typedef std::vector<int> IntVector;

namespace Constants {
const char NAME[] = "Seth";
const int BRD_SQ_NUM = 120;

const int MAXGAMEMOVES = 2048;
const int MAXPOSITIONMOVES = 256;
const int MAXDEPTH = 64;
const int MAX_HASH = 1024;

const int INF = 30000;
const int ISMATE = (INF - MAXDEPTH);

//const QString START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";



namespace Enums {
enum PieceType { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum FileType { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum RankType { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum ColorType { WHITE, BLACK, BOTH };
enum GameModeType { UCIMODE, XBOARDMODE, CONSOLEMODE };

enum SquareType {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8,
    NO_SQ, OFFBOARD
};

//enum  { FALSE, TRUE };

enum CastlingRightsType { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

enum HashFlagType { HFNONE, HFALPHA, HFBETA, HFEXACT };
}
}
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define INF 30000

#define ISMATE (INF - Constants::MAXDEPTH)

#define MFLAGEP   0x00040000
#define MFLAGPS   0x00080000
#define MFLAGCA   0x01000000
#define MFLAGCAP  0x0007C000
#define MFLAGPROM 0x00f00000

#define NOMOVE 0


enum Boolean { FALSE, TRUE };

 struct S_MOVE{
    int move;
    int score;
 };

 struct S_MOVELIST {
    S_MOVE* moves;
    int count;
 };

 struct S_HASHENTRY {
    U64 posKey;
    int move, score, depth, flags;
};

 struct S_HASHTABLE {
    S_HASHENTRY* pTable;
    int numEntries, newWrite, overWrite, hit, cut;
};

 struct  S_UNDO {
    int move, castlePerm, enPas, fiftyMoves;
    U64 posKey;
};

 extern int Sq120ToSq64[Constants::BRD_SQ_NUM];
 extern int Sq64ToSq120[64];
 extern U64 SetMask[64];
 extern U64 ClearMask[64];
 extern U64 PieceKeys[13][120];
 extern U64 SideKey;
 extern U64 CastleKeys[16];
// extern std::string PceChar;
 extern std::string SideChar;
 extern std::string RankChar;
 extern std::string FileChar;


 extern int FilesBrd[Constants::BRD_SQ_NUM];
 extern int  RanksBrd[Constants::BRD_SQ_NUM];

 extern int PiecePawn[13];
 extern int PieceKnight[13];
 extern int PieceKing[13];
 extern int PieceRookQueen[13];
 extern int PieceBishopQueen[13];
 extern int PieceSlides[13];


 extern int PieceCol[13];
 extern int PieceBig[13];
 extern int PieceMaj[13];
 extern int PieceMin[13];
 extern int PieceVal[13];
extern int PieceCol[13];


 extern int Mirror64[64];

 extern U64 FileBBMask[8];
 extern U64 RankBBMask[8];

 extern U64   BlackPassedMask[64];
 extern U64 WhitePassedMask[64];
 extern U64 IsolatedMask[64];



 struct S_BOARD {
    int pieces[Constants::BRD_SQ_NUM];
    U64 pawns[3];

    int KingSq[2];
    int side, enPas, fiftyMove;
    int ply, hisPly, castlePerm;

    U64 posKey;

    int pceNum[13];
    int bigPce[2];
    int majPce[2];
    int minPce[2];
    int material[2];

    S_UNDO history[Constants::MAXPOSITIONMOVES];
    int pList[13][10];

    S_HASHTABLE HashTable[1];

    int PvArray[Constants::MAXDEPTH];
    int searchHistory[13][Constants::BRD_SQ_NUM];
    int searchKillers[2][Constants::MAXDEPTH];
};

 struct S_SEARCHINFO{
    int starttime, stoptime, depth, timeset, movestogo;
    int quit, stopped, nullCut;
    int GAME_MODE, POST_THINKING;

    long nodes;
    float fh, fhf;
};


 extern bool UseBook;


// extern S_BOARD bd;
//extern S_OPTIONS EngineOptions;

#define FROMSQ(m) ((m) & 0x7f)
#define TOSQ(m) (((m) >> 7) & 0x7f)
#define CAPTURED(m) (((m) >> 14) & 0xf)
#define PROMOTED(m) (((m) >> 20) & 0xf)

#define MFLAGEP   0x00040000
#define MFLAGPS   0x00080000
#define MFLAGCA   0x01000000
#define MFLAGCAP  0x0007C000
#define MFLAGPROM 0x00f00000
#define USEBOOK TRUE

#define NOMOVE 0

inline int FR2SQ(int f, int r) {
    return (21 + f) + (r * 10);
}




inline int SQ64(int sq120) {
    return Sq120ToSq64[sq120];
}



inline  int Sq120(int sq64) {
     /*const int Sq64ToSq120[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 70, 76, 82, 88, 94, 100, 106, 112, 118, 124, 130, 136, 142, 148, 154,
        65, 71, 77, 83, 89, 95, 101, 107, 113, 119, 125, 131, 137, 143, 149, 155,
        66, 72, 78, 84, 90, 96, 102, 108, 114, 120, 126, 132, 138, 144, 150, 156,
        67, 73, 79, 85, 91, 97, 103, 109, 115, 121, 127, 133, 139, 145, 151, 157,
        68, 74, 80, 86, 92, 98, 104, 110, 116, 122, 128, 134, 140, 146, 152, 158,
        69, 75, 81, 87, 93, 99, 105, 111, 117, 123, 129, 135, 141, 147, 153, 159
    };
*/
    return Sq64ToSq120[sq64];
}


class TimeUtility {
public:
    static qint64 GetTimeMs() {
#ifdef Q_OS_WIN
        return QDateTime::currentMSecsSinceEpoch();
#else
        struct timeval t;
        gettimeofday(&t, nullptr);
        return static_cast<qint64>(t.tv_sec) * 1000 + t.tv_usec / 1000;
#endif
    }
};

extern void AllInit(void);
extern void clearBit(quint64 &bb, int sq);
extern U64 GeneratePositionKey(const S_BOARD* pos);
// int sqAttacked(const int sq, const int side, S_BOARD& pos);
extern int SqAttacked(const int sq, const int side, const S_BOARD* pos);

extern int MaterialDraw(const S_BOARD* pos);
extern void  ParseGo(char* line, S_SEARCHINFO *info, S_BOARD& pos);
extern std::string PrintMove(int move);
extern int ParseFen(char *fen, S_BOARD* pos);
extern void PrintBoard(const S_BOARD* pos);
extern void ResetBoard(S_BOARD* pos);
extern void UpdateListsMaterial(S_BOARD* pos);
extern int ParseMove(char *ptrChar, S_BOARD *pos);
extern char *PrSq(const int sq);
extern void PrintMoveList(const S_MOVELIST *list);
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);

extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);

extern void MirrorBoard(S_BOARD *pos);

extern int MakeMove(S_BOARD *pos, int move);
extern void TakeMove(S_BOARD *pos);
extern void ReadInput(S_SEARCHINFO *info);
extern int EvalPosition(const S_BOARD *pos);
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);
extern void MakeNullMove(S_BOARD *pos);
extern void TakeNullMove(S_BOARD *pos);

extern int ProbePvMove(const S_BOARD *pos);
extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
extern int MoveExists(S_BOARD *pos, const int move);
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
extern void ClearHashTable(S_HASHTABLE *table);
extern int GetPvLine(const int depth, S_BOARD *pos);

extern void InitPolyBook();
extern void CleanPolyBook();
extern int GetBookMove(S_BOARD *board);

extern void InitHashTable(S_HASHTABLE *table, const int MB);

extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);
//extern static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull);
/*extern static void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);
extern static void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list);
extern static void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list);
extern static void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list);
//extern static void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);
extern static void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list);
extern static void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list);

*/

extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);
extern TimeUtility tm;
extern void MirrorEvalTest(S_BOARD *pos);


inline int fromSquare(int move) {
    return move & 0x7f;
}

inline int toSquare(int move) {
    return (move >> 7) & 0x7f;
}

inline int capturedPiece(int move) {
    return (move >> 14) & 0xf;
}

inline int promotedPiece(int move) {
    return (move >> 20) & 0xf;
}



#endif // DEFS_H
