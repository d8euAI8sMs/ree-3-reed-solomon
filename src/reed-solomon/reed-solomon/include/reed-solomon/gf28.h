#pragma once

#include <reed-solomon/api.h>

#include <vector>

/**
 * API notes:
 * 
 * - use {@ref gf28::gfscal_t} operators to deal with
 *   arithmetic operations in GF
 * - use {@ref gf28::gfmul} and other for extended control
 *   on what's going on with numbers
 * 
 * Note that {@ref gf28::gfscal_t} operations are optimized
 * to be O(1) by the means of precalculated tables. However,
 * they are bound to GF and require {@ref gf28::gfinit} to
 * be called first. {@ref gf28::gfmul}-like operations not
 * bound to GF (in fact, {@ref gf28::gfinit} uses them internally).
 */
namespace gf28
{
    /**
     * Initializes Galois Field (GF(gm = 2^gp) / gv) arithmetics.
     * 
     * <p>See statics of {@ref gfscal_t} for details.
     * 
     * @param gv [in] generator polynomial
     * @return 0 on success
     *         1 if no primitive element found
     */
    RS_API errc_t gfinit(word_t gv);

    // to allow custom operator declaration
    struct gfscal_t
    {
        RS_API static word_t gv; ///< generator polynomial
        RS_API static byte_t gp; ///< actual order of GF calculated as floor(log2(gv))
        RS_API static byte_t gm; ///< 2^gp
        RS_API static byte_t pe; ///< primitive element of GF
        RS_API static byte_t multable[256][256]; ///< multiplication table i * j
        RS_API static byte_t divtable[256][256]; ///< division table i / j
        RS_API static byte_t powtable[256][256]; ///< table of powers i^j
        RS_API static byte_t pwitable[256][256]; ///< table of inverse powers i^(-j)
        byte_t val;
        gfscal_t(byte_t o) : val(o) {}
        gfscal_t & operator= (byte_t o) { val = o; return *this; }
        operator byte_t() const { return val; }
    };

    inline gfscal_t operator+ (gfscal_t s1, gfscal_t s2)
    {
        return s1.val ^ s2.val;
    }

    inline gfscal_t operator- (gfscal_t s1, gfscal_t s2)
    {
        return s1 + s2;
    }

    inline gfscal_t operator* (gfscal_t s1, gfscal_t s2)
    {
        return gfscal_t::multable[s1][s2];
    }

    inline gfscal_t operator/ (gfscal_t s1, gfscal_t s2)
    {
        return gfscal_t::divtable[s1][s2];
    }

    inline gfscal_t operator^ (gfscal_t s, int pow)
    {
        if (pow >= 0) return gfscal_t::powtable[s][pow];
        return gfscal_t::pwitable[s][-pow];
    }

    inline byte_t gflog2(word_t w)
    {
        for (int i = 15; i >= 0; --i)
            if (w & (1 << i)) return i;
        return 0;
    }

    inline word_t gfmul(gfscal_t a, gfscal_t b)
    {
        word_t w = 0;
        for (int i = 0; i < 8; ++i)
        {
            w ^= ((a & (1 << i)) >> i) * (((word_t)b) << i);
        }
        return w;
    }

    inline gfscal_t gfmod(word_t w, word_t m)
    {
        const word_t hmsk = (1 << gflog2(m));
        word_t t;
        for (int i = 6; i >= 0; --i)
        {
            if ((w & (hmsk << i)) == 0) continue;
            t = (w >> i);
            w ^= m << i;
        }
        return (byte_t) w;
    }
}
