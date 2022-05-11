#include "keccak_state.hpp"

/*
Input: integer t
Output: bit rc(t)
Steps:
1. If t mod 255 = 0, return 1
2. Let R = 10000000b
3. For i from 1 to t mod 255
    a. R = 0 || R
    b. R[0] = R[0] ⊕ R[8]
    c. R[4] = R[4] ⊕ R[8]
    d. R[5] = R[5] ⊕ R[8]
    e. R[6] = R[6] ⊕ R[8]
    f. R =Trunc8[R]
4. Return R[0]
*/

uint8_t rc_sm (uint64_t t)
{
    uint64_t tmod255 = t%255;
    // If t mod 255 = 0, return 1
    if (tmod255 == 0) return 1;

    // Let R = 10000000b
    uint8_t R[9];
    memset(R, 0, sizeof(R));
    R[0] = 1;

    // For i from 1 to t mod 255
    for (uint64_t i=0; i<tmod255; i++)
    {
        // R = 0 || R
        for (uint64_t j=8; j>0; j--)
        {
            R[j] = R[j-1];
        }
        R[0] = 0;

        // R[0] = R[0] ⊕ R[8]
        R[0] = R[0] ^ R[8];

        // R[4] = R[4] ⊕ R[8]
        R[4] = R[4] ^ R[8];

        // R[5] = R[5] ⊕ R[8]
        R[5] = R[5] ^ R[8];

        // R[6] = R[6] ⊕ R[8]
        R[6] = R[6] ^ R[8];

        // R =Trunc8[R]
        R[8] = 0;
    }

    return R[0] & 0x01;
}

/*
Steps:
1. For all triples (x, y, z) such that 0 ≤ x <5, 0 ≤ y < 5, and 0 ≤ z < w, let A′[x, y, z] = A[x, y, z]
2. Let RC = 0w.
3. For j from 0 to l, let RC[2^j – 1] = rc(j + 7ir).
4. For all z such that 0 ≤ z <w, let A′ [0, 0, z] = A′[0, 0, z] ⊕ RC[z].
5. Return A′
*/

void KeccakIota (KeccakState &S, uint64_t ir)
{
    // A′[x, y, z] = A[x, y, z]
    for (uint64_t x=0; x<5; x++)
    {
        for (uint64_t y=0; y<5; y++)
        {
            for (uint64_t z=0; z<64; z++)
            {
                S.SoutRefs[Bit(x, y, z)] = S.SinRefs[Bit(x, y, z)];
            }
        }
    }

    // Let RC = 0w
    uint8_t RC[64];
    memset(RC, 0, 64);

    // For j from 0 to l, let RC[2^j – 1] = rc(j + 7ir)
    for (uint64_t j=0; j<=6; j++)
    {
        RC[(1<<j) - 1] = rc_sm(j + (7*ir));
    }

    // For all z such that 0 ≤ z <w, let A′ [0, 0, z] = A′[0, 0, z] ⊕ RC[z]
    for (uint64_t z=0; z<64; z++)
    {
        if (RC[z]==0)
        {
            continue;
        }
        uint64_t aux;
        aux = S.getFreeRef();
        if (RC[z] == 1)
        {
            S.XOR( ZeroRef, pin_b, S.SoutRefs[Bit(0, 0, z)], pin_r, aux );
        }
        else
        {
            S.XOR( ZeroRef, pin_a, S.SoutRefs[Bit(0, 0, z)], pin_r, aux );
        }
        S.SoutRefs[Bit(0, 0, z)] = aux;
    }
}