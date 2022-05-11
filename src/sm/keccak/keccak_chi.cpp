#include "keccak_state.hpp"

/*
Steps:
1. For all triples (x, y, z) such that 0 ≤ x <5, 0 ≤ y < 5, and 0 ≤ z < w
    A′ [x, y, z] = A[x, y, z] ⊕ ((A[(x+1) mod 5, y, z] ⊕ 1) ⋅ A[(x+2) mod 5, y, z])
2. Return A′
*/

void KeccakChi (KeccakState &S)
{
    // A′ [x, y, z] = A[x, y, z] ⊕ ( (A[(x+1) mod 5, y, z] ⊕ 1) ⋅ A[(x+2) mod 5, y, z] )
    for (uint64_t x=0; x<5; x++)
    {
        for (uint64_t y=0; y<5; y++)
        {
            for (uint64_t z=0; z<64; z++)
            {
                uint64_t aux1 = S.getFreeRef();
                S.ANDP(S.SinRefs[Bit((x+1)%5, y, z)], S.SinRefs[Bit((x+2)%5, y, z)], aux1);
                uint64_t aux2;
                aux2 = S.getFreeRef();
                S.XOR(aux1, S.SinRefs[Bit(x, y, z)], aux2);
                S.SoutRefs[Bit(x, y, z)] = aux2;
            }
        }
    }
}