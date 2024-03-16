#include "defs.h"

U64 GeneratePositionKey(const S_BOARD* pos) {
    int sq = 0;
    U64 finalKey = 0ULL;
    int piece = Constants::Enums::EMPTY;

    for (sq = 0; sq < Constants::BRD_SQ_NUM; ++sq) {
        piece = pos->pieces[sq];
        if (piece != Constants::Enums::OFFBOARD && piece != Constants::Enums::EMPTY) {
            ASSERT(piece >= Constants::Enums::wP && piece <= Constants::Enums::bK );
            finalKey ^= PieceKeys[piece][sq];
        }
    }

    if(pos->side == Constants::Enums::WHITE)
        finalKey ^= SideKey;

    if(pos->enPas != Constants::Enums::NO_SQ) {
        ASSERT(pos->enPas >= 0 && pos->enPas < Constants::BRD_SQ_NUM);
        finalKey ^= PieceKeys[Constants::Enums::EMPTY][pos->enPas];
    }

    ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);
    finalKey ^= CastleKeys[pos->castlePerm];
    return finalKey;
}
