#include <QVector>
#include <QtGlobal> // for Q_ASSERT
#include <QString>
#include <iostream>
#include <string>
#include <stdio.h>

#define DEBUG
#include "board.h"
#include "defs.h"
#include "headrs/bitboard.h"
//#include "headrs/sqattack.h"






int ConvertSq64ToSq120(int sq64) {
    return Sq64ToSq120[sq64];
}



//std::string PceChar = ".PNBRQKpnbrqk";

//char SideChar[] = "wb-";
//char RankChar[] = "12345678";
//char FileChar[] = "abcdefgh";


#define MAX 120
int CheckBoard(const S_BOARD *pos) {
    QVector<int> t_pceNum(13, 0);
    QVector<int> t_bigPce(2, 0);
    QVector<int> t_majPce(2, 0);
    QVector<int> t_minPce(2, 0);
    QVector<int> t_material(2, 0);

    int sq120, t_piece, t_pce_num, sq64, colour, pcount;

    ASSERT(pos != nullptr);

    QHash<int, U64> t_pawns;
    t_pawns[Constants::Enums::WHITE] = pos->pawns[Constants::Enums::WHITE];
    t_pawns[Constants::Enums::BLACK] = pos->pawns[Constants::Enums::BLACK];
    t_pawns[Constants::Enums::BOTH] = pos->pawns[Constants::Enums::BOTH];

    for (t_piece = Constants::Enums::wP; t_piece <= Constants::Enums::bP; ++t_piece) {
        for (t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; ++t_pce_num) {
            sq120 = pos->pList[t_piece][t_pce_num];
          //  Q_ASSERT(pos->pieces[sq120] == t_piece);
            ASSERT(pos->pieces[Sq64ToSq120[sq120]] == t_piece);
        }
    }


    for(t_piece = Constants::Enums::wP; t_piece <= Constants::Enums::bK; ++t_piece)
        ASSERT(t_pceNum[t_piece] == pos->pceNum[t_piece]);

    pcount = CountBits(t_pawns[Constants::Enums::WHITE]);
    ASSERT(pcount == pos->pceNum[Constants::Enums::wP]);

    // Count the number of black pawns and assert it matches the number of black pawns in the board
    pcount = CountBits(t_pawns[Constants::Enums::BLACK]);
    ASSERT(pcount == pos->pceNum[Constants::Enums::bP]);

    // Count the total number of pawns (both white and black) and assert it matches the total number of pawns in the board
    pcount = CountBits(t_pawns[Constants::Enums::BOTH]);
    ASSERT(pcount == pos->pceNum[Constants::Enums::wP] + pos->pceNum[Constants::Enums::bP]);


    while(t_pawns[Constants::Enums::WHITE]) {
        sq64 = PopBit(&t_pawns[Constants::Enums::WHITE]);
        ASSERT(pos->pieces[Sq64ToSq120[sq64]] == Constants::Enums::wP);
    }


    while(t_pawns[Constants::Enums::BLACK]) {
        sq64 = PopBit(&t_pawns[Constants::Enums::BLACK]);
        ASSERT(pos->pieces[Sq64ToSq120[sq64]] == Constants::Enums::bP);
    }

    while(t_pawns[Constants::Enums::BOTH]) {
        sq64 =PopBit(&t_pawns[Constants::Enums::BOTH]);
        ASSERT(pos->pieces[Sq64ToSq120[sq64]] == Constants::Enums::wP|| pos->pieces[Sq64ToSq120[sq64]] == Constants::Enums::bP);
    }



    ASSERT(t_material[Constants::Enums::WHITE] == pos->material[Constants::Enums::WHITE] && t_material[Constants::Enums::BLACK] == pos->material[Constants::Enums::BLACK]);
    ASSERT(t_minPce[Constants::Enums::WHITE] == pos->minPce[Constants::Enums::WHITE] && t_minPce[Constants::Enums::BLACK] == pos->minPce[Constants::Enums::BLACK]);
    ASSERT(t_majPce[Constants::Enums::WHITE] == pos->majPce[Constants::Enums::WHITE] && t_majPce[Constants::Enums::BLACK] == pos->majPce[Constants::Enums::BLACK]);
    ASSERT(t_bigPce[Constants::Enums::WHITE] == pos->bigPce[Constants::Enums::WHITE] && t_bigPce[Constants::Enums::BLACK] == pos->bigPce[Constants::Enums::BLACK]);

    ASSERT(pos->side == Constants::Enums::WHITE || pos->side == Constants::Enums::BLACK);
    ASSERT(GeneratePositionKey(pos) == pos->posKey);



    // Further implementation of the function goes here

    return 1; // Return value just for compilation, replace it with actual return value
}


void PrintBoard(const S_BOARD* pos) {

 int sq, file, rank, piece;
 std::ostream& os = std::cout;
std::string PceChar = ".PNBRQKpnbrqk";

    os << "\nGame Board:\n\n";

  for (rank = Constants::Enums::RANK_8; rank >= Constants::Enums::RANK_1; --rank) {
        std::cout << rank + 1 << "  ";
        for (file = Constants::Enums::FILE_A; file <= Constants::Enums::FILE_H; ++file) {
            sq = FR2SQ(file, rank);
            piece = pos->pieces[sq];
//            os << " | " << PieceToChar[pos.piece_on(make_square(f, r))];

            // Print piece character
            std::cout<< PceChar[piece] << " ";
        }
        std::cout << std::endl;
  }


    std::cout << "\n   ";
    for (file =Constants::Enums::FILE_A; file <= Constants::Enums::FILE_H; file++)
        std::cout << static_cast<char>('a' + file)<<" ";

    std::cout << std::endl;
    std::cout << "side: " << SideChar[pos->side] << std::endl;
    std::cout << "enPas: " << pos->enPas << std::endl;
    std::cout << "castle: "
              << (pos->castlePerm & Constants::Enums::WKCA ? 'K' : '-')
              << (pos->castlePerm &  Constants::Enums::WQCA ? 'Q' : '-')
              << (pos->castlePerm & Constants::Enums::BKCA ? 'k' : '-')
              << (pos->castlePerm & Constants::Enums::BQCA ? 'q' : '-')
              << std::endl;

    std::cout << "PosKey: " << std::hex << pos->posKey << std::endl;
//        printf("PosKey:%0llx\n", pos-posKey);

}


int ParseFen(char *fen, S_BOARD* pos) {
    int rank = Constants::Enums::RANK_8;;
    int file =  Constants::Enums::FILE_A;
    int piece = 0;
    int count = 0;
    int i = 0;
    int sq64 = 0;
    int sq120 = 0;
//     S_BOARD* bd;


    ASSERT(fen != NULL);
//    ASSERT(pos != NULL);

    ResetBoard(pos);

    while((rank >= Constants::Enums::RANK_1) && *fen) {
        count = 1;
        switch(*fen) {
        case 'p': piece = Constants::Enums::bP; break;
        case 'r': piece = Constants::Enums::bR; ; break;
        case 'n': piece =  Constants::Enums::bN;; break;
        case 'b': piece = Constants::Enums::bB; break;
        case 'k': piece = Constants::Enums::bK; break;
        case 'q': piece = Constants::Enums::bQ; break;
        case 'P': piece = Constants::Enums::wP; break;
        case 'R': piece =Constants::Enums::wR; break;
        case 'N': piece = Constants::Enums::wN; break;
        case 'B': piece = Constants::Enums::wB; break;
        case 'K': piece = Constants::Enums::wK; break;
        case 'Q': piece = Constants::Enums::wQ; break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
            piece = Constants::Enums::EMPTY;
            count = *fen - '0';
            break;

        case '/':
        case ' ':
            rank--;
            file = Constants::Enums::FILE_A;
            fen++;
            continue;

        default:
            printf("FEN error!\n");
            return -1;
        }

        for(i = 0; i < count; i++) {
            sq64 = rank * 8 + file;
            sq120 = ConvertSq64ToSq120(sq64);;
            if(piece != Constants::Enums::EMPTY)
                pos->pieces[sq120] = piece;
            file++;
        }
        fen++;
    }

    ASSERT(*fen == 'w' || *fen == 'b');

    pos->side = (*fen == 'w') ? Constants::Enums::WHITE :Constants::Enums::BLACK;
    fen += 2;

    for(i = 0; i < 4; i++) {
        if(*fen == ' ')
            break;

        switch(*fen) {
        case 'K': pos->castlePerm |=Constants::Enums::WKCA; break;
        case 'Q': pos->castlePerm |=Constants::Enums::WQCA; break;
        case 'k': pos->castlePerm |= Constants::Enums::BKCA; break;
        case 'q': pos->castlePerm |= Constants::Enums::BQCA; break;
        }
        fen++;
    }
    fen++;

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);

    if(*fen != '-') {
        file = fen[0] - 'a';
        rank = fen[1] - '1';

        ASSERT(file >= Constants::Enums::FILE_A && file <= Constants::Enums::FILE_H);
        ASSERT(rank >=Constants::Enums::RANK_1 && rank <= Constants::Enums::RANK_8);

        pos->enPas = FR2SQ(file, rank);
    }

    pos->posKey = GeneratePositionKey(pos);
//    std::cout << "poskey: " << poss <<std::endl;

    UpdateListsMaterial(pos);
    return 0;
}

void ResetBoard(S_BOARD* pos) {
    for (int index = 0; index < Constants::BRD_SQ_NUM; index++)
        pos->pieces[index] = Constants::Enums::OFFBOARD;

    for (int index = 0; index < 64; index++)
        pos->pieces[Sq120ToSq64[index]] =Constants::Enums::EMPTY;

    for (int index = 0; index < 2; index++) {
        pos->bigPce[index] = 0;
        pos->majPce[index] = 0;
        pos->minPce[index] = 0;
        pos->material[index] = 0;
    }

    for (int index = 0; index < 3; index++)
        pos->pawns[index] = 0ULL;

    for (int index = 0; index < 13; index++)
        pos->pceNum[index] = 0;

    pos->KingSq[Constants::Enums::WHITE] = pos->KingSq[Constants::Enums::BLACK] = Constants::Enums::NO_SQ;

    pos->material[Constants::Enums::WHITE] = pos->material[Constants::Enums::BLACK] = 0;

    pos->side = Constants::Enums::BOTH;
    pos->enPas =Constants::Enums::NO_SQ;
    pos->fiftyMove = 0;

    pos->ply = 0;
    pos->hisPly = 0;

    pos->castlePerm = 0;

    pos->posKey = 0ULL;
}

void UpdateListsMaterial(S_BOARD* pos) {
    int piece, sq, index, colour;

    for (index = 0; index < Constants::BRD_SQ_NUM; ++index) {
        sq = index;
        piece = pos->pieces[index];
        if (piece !=Constants::Enums::EMPTY) {
            colour = PieceCol[piece];
            if (PieceBig[piece]) pos->bigPce[colour]++;
            if (PieceMin[piece]) pos->minPce[colour]++;
            if (PieceMaj[piece]) pos->majPce[colour]++;

            pos->material[colour] += PieceVal[piece];

            pos->pList[piece][pos->pceNum[piece]] = sq;
            pos->pceNum[piece]++;

            if (piece ==Constants::Enums::wK) pos->KingSq[Constants::Enums::WHITE] = sq;
            if (piece ==Constants::Enums:: bK) pos->KingSq[Constants::Enums::BLACK] = sq;

           /* if (piece ==Constants::Enums::wP || piece == Constants::Enums::bP) */
            if(piece == Constants::Enums::wP){

                setBit(pos->pawns[Constants::Enums::WHITE], SQ64(sq));
                setBit(pos->pawns[Constants::Enums::BOTH], SQ64(sq));
            }
            else{
                (piece == Constants::Enums::bP) ? setBit(pos->pawns[Constants::Enums::BLACK], SQ64(sq)) :  setBit(pos->pawns[Constants::Enums::BOTH], SQ64(sq));

            }
//                setBit(pos.pawns[Constants::Enums::WHITE], SQ64(sq));
//                pos.pawns[colour][SQ64(sq)] = true;
//                pos.pawns[Constants::Enums::BOTH][SQ64(sq)] = true;
//            }
        }
    }
}


void MirrorBoard(S_BOARD *pos) {

    int tempPiecesArray[64];
    int tempSide = pos->side^1;
    int SwapPiece[13] = { Constants::Enums::EMPTY,  Constants::Enums::bP, Constants::Enums::bN, Constants::Enums::bB, Constants::Enums::bR,
                         Constants::Enums::bQ, Constants::Enums::bK, Constants::Enums::wP, Constants::Enums::wN, Constants::Enums::wB, Constants::Enums::wR, Constants::Enums::wQ, Constants::Enums::wK };
    int tempCastlePerm = 0;
    int tempEnPas = Constants::Enums::NO_SQ;

    int sq;
    int tp;

    if (pos->castlePerm & Constants::Enums::WKCA) tempCastlePerm |= Constants::Enums::BKCA;
    if (pos->castlePerm & Constants::Enums::WQCA) tempCastlePerm |= Constants::Enums::BQCA;

    if (pos->castlePerm & Constants::Enums::BKCA) tempCastlePerm |= Constants::Enums::WKCA;
    if (pos->castlePerm & Constants::Enums::BQCA) tempCastlePerm |= Constants::Enums::WQCA;

    if (pos->enPas != Constants::Enums::NO_SQ)  {
        tempEnPas =  Sq120(Mirror64[SQ64(pos->enPas)]);
    }

    for (sq = 0; sq < 64; sq++) {
        tempPiecesArray[sq] = pos->pieces[ Sq120(Mirror64[sq])];
    }

    ResetBoard(pos);

    for (sq = 0; sq < 64; sq++) {
        tp = SwapPiece[tempPiecesArray[sq]];
        pos->pieces[ Sq120(sq)] = tp;
    }

    pos->side = tempSide;
    pos->castlePerm = tempCastlePerm;
    pos->enPas = tempEnPas;

    pos->posKey = GeneratePositionKey(pos);

    UpdateListsMaterial(pos);

    ASSERT(CheckBoard(pos));
}


