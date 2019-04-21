#include "stdafx.h"

#include <reed-solomon/gf28.h>

namespace gf28
{
    word_t gfscal_t::gv = 0;
    byte_t gfscal_t::gp = 0;
    byte_t gfscal_t::gm = 0;
    byte_t gfscal_t::pe = 0;
    byte_t gfscal_t::multable[256][256];
    byte_t gfscal_t::divtable[256][256];
    byte_t gfscal_t::powtable[256][256];
    byte_t gfscal_t::pwitable[256][256];

    errc_t gfinit(word_t gv)
    {
        auto p = gfscal_t::gp = gflog2(gv);
        auto l = gfscal_t::gm = (1 << p);

        gfscal_t::gv = gv;

        for (int i = 0; i < l; ++i)
        for (int j = 0; j <= i; ++j)
        {
            gfscal_t::multable[i][j] = gfmod(gfmul(i, j), gv);
            gfscal_t::multable[j][i] = gfscal_t::multable[i][j];
        }
        
        for (int i = 1; i < l; ++i)
        for (int j = 1; j <= i; ++j)
        {
            gfscal_t::divtable[gfscal_t::multable[i][j]][i] = j;
            gfscal_t::divtable[gfscal_t::multable[i][j]][j] = i;
        }

        for (int i = 0; i < l; ++i)
        {
            gfscal_t::powtable[i][0] = 1;
            gfscal_t::powtable[0][i] = 0;
        }

        for (int i = 1; i < l; ++i)
        for (int j = 1; j < l; ++j)
        {
            gfscal_t::powtable[i][j] = gfmod(gfmul(i, gfscal_t::powtable[i][j - 1]), gv);
        }

        for (int i = 0; i < l; ++i)
        for (int j = 0; j < l; ++j)
        {
            gfscal_t::pwitable[i][j] = gfscal_t::powtable[i][l - j - 1];
        }

        byte_t nums[256];

        for (int i = 2; i < l; ++i)
        {
            for (int j = 1; j < l; ++j) nums[j] = 0;
            for (int j = 1; j < l; ++j) nums[gfscal_t::powtable[i][j]] = 1;
            for (int j = 1; j < l; ++j) if (nums[j] == 0) goto loop;
            gfscal_t::pe = i; break;
            loop:;
        }

        if (gfscal_t::pe == 0) return errc::errc_invalid_argument;

        return errc::errc_success;
    }
}