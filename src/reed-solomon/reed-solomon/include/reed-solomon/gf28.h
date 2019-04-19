#pragma once

#include <reed-solomon/api.h>

#include <vector>

namespace gf28
{
    RS_API errc_t gfinit(word_t gv);

    // to allow custom operator declaration
    struct gfscal_t
    {
        RS_API static word_t gv;
        RS_API static byte_t pe;
        RS_API static byte_t multable[256][256];
        RS_API static byte_t divtable[256][256];
        RS_API static byte_t powtable[256][256];
        RS_API static byte_t pwitable[256][256];
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
