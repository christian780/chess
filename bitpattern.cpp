#include "headrs/bitboard.h"

/*void setBit(U64 *bb, int sq) {
    const U64 SetMask[64] = {
        0x01ULL, 0x02ULL, 0x04ULL, 0x08ULL, 0x10ULL, 0x20ULL, 0x40ULL, 0x80ULL,
        0x0100ULL, 0x0200ULL, 0x0400ULL, 0x0800ULL, 0x1000ULL, 0x2000ULL, 0x4000ULL, 0x8000ULL,
        0x010000ULL, 0x020000ULL, 0x040000ULL, 0x080000ULL, 0x100000ULL, 0x200000ULL, 0x400000ULL, 0x800000ULL,
        0x01000000ULL, 0x02000000ULL, 0x04000000ULL, 0x08000000ULL, 0x10000000ULL, 0x20000000ULL, 0x40000000ULL, 0x80000000ULL,
        0x0100000000ULL, 0x0200000000ULL, 0x0400000000ULL, 0x0800000000ULL, 0x1000000000ULL, 0x2000000000ULL, 0x4000000000ULL, 0x8000000000ULL,
        0x010000000000ULL, 0x020000000000ULL, 0x040000000000ULL, 0x080000000000ULL, 0x100000000000ULL, 0x200000000000ULL, 0x400000000000ULL, 0x800000000000ULL,
        0x01000000000000ULL, 0x02000000000000ULL, 0x04000000000000ULL, 0x08000000000000ULL, 0x10000000000000ULL, 0x20000000000000ULL, 0x40000000000000ULL, 0x80000000000000ULL,
        0x0100000000000000ULL, 0x0200000000000000ULL, 0x0400000000000000ULL, 0x0800000000000000ULL, 0x1000000000000000ULL, 0x2000000000000000ULL, 0x4000000000000000ULL, 0x8000000000000000ULL
    };

    *bb |= SetMask[sq];
}
*/
/*void clearBit(quint64 &bb, int sq) {
    static const quint64 ClearMask[64] = {
        0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F,
        0xFEFF, 0xFDFF, 0xFBFF, 0xF7FF, 0xEFFF, 0xDFFF, 0xBFFF, 0x7FFF,
        0xFEFFFE, 0xFDFFFD, 0xFBFFFB, 0xF7FFF7, 0xEFFFEF, 0xDFFFDFF, 0xBFFFBFF, 0x7FFFEFFF,
        0xFEFFFEFFFE, 0xFDFFFDFFFD, 0xFBFFFBFFFB, 0xF7FFF7FFF7, 0xEFFFEFFFEF, 0xDFFFDFFFDFF, 0xBFFFBFFFBFF, 0x7FFFEFFF7F,
        0xFEFFFEFFFEFFFE, 0xFDFFFDFFFDFFFD, 0xFBFFFBFFFBFFFB, 0xF7FFF7FFF7FFF7, 0xEFFFEFFFEEFFFEEF, 0xDFFFDFFFDFFFDFFD, 0xBFFFBFFFBFFFBFFB, 0x7FFFEFFF7FFFEFFF,
        0xFEFFFEFFFEFFFEFFFE, 0xFDFFFDFFFDFFFDFFFD, 0xFBFFFBFFFBFFFBFFFB, 0xF7FFF7FFF7FFF7FFF7, 0xEFFFEFFFEEFFEEFFFEF, 0xDFFFDFFFDFFFDFFFDFFD, 0xBFFFBFFFBFFFBFFFBFFB, 0x7FFFEFFF7FFFEFFF7FFF,
        0xFEFFFEFFFEFFFEFFFEFF, 0xFDFFFDFFFDFFFDFFFDFFFD, 0xFBFFFBFFFBFFFBFFFBFFFB, 0xF7FFF7FFF7FFF7FFF7FFF7, 0xEFFFEFFFEEFFEEFFFEFFEF, 0xDFFFDFFFDFFFDFFFDFFFDFFD, 0xBFFFBFFFBFFFBFFFBFFFBFFB, 0x7FFFEFFF7FFFEFFF7FFFEFFF
    };

    bb &= ClearMask[sq];
}

*/
int CountBits(U64 b) {
    int r = 0;
    while (b) {
        r++;
        b &= b - 1;
    }
    return r;
}
int PopBit(U64 *bb) {
    quint64 b = *bb ^ (*bb - 1);
    unsigned int fold = static_cast<unsigned int>((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);

    return BitTable[(static_cast<quint64>(fold * 0x783a9b23)) >> 26];
}

void setBit(U64& bitboard, int position) {
    bitboard |= (1ULL << position);
}

void clearBit(quint64& bitboard, int position) {
    bitboard &= ~(1ULL << position);
}