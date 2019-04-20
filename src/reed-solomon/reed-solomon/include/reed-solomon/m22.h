#pragma once

#include <reed-solomon/gf28.h>

namespace gf28
{
    struct m22
    {
        gfscal_t m11, m12, m21, m22;
    };

    struct v2
    {
        gfscal_t v1, v2;
    };

    inline gfscal_t det(const m22 & m)
    {
        return m.m11 * m.m22 - m.m12 * m.m21;
    }

    inline bool inv(const m22 & m, m22 & w)
    {
        auto d = det(m);
        if (d == 0) return false;
        w = { m.m22 / d, m.m12 / d, m.m21 / d, m.m11 / d };
        return true;
    }

    inline m22 operator* (const m22 & m, const m22 & w)
    {
        return
        {
            m.m11 * w.m11 + m.m12 * w.m21,
            m.m11 * w.m12 + m.m12 * w.m22,
            m.m21 * w.m11 + m.m22 * w.m21,
            m.m21 * w.m12 + m.m22 * w.m22
        };
    }

    inline v2 operator* (const m22 & m, const v2 & v)
    {
        return { m.m11 * v.v1 + m.m12 * v.v2, m.m21 * v.v1 + m.m22 * v.v2 };
    }

    inline bool lsol(const m22 & m, const v2 & v, v2 & w)
    {
        m22 mi;
        if (!inv(m, mi)) return false;
        w = mi * v; return true;
    }

    inline void poly(const v2 & v, gfpoly_t & p)
    {
        p[0] = 1; p[1] = v.v1; p[2] = v.v2;
    }

    inline void npoly(const v2 & v, gfpoly_t & p)
    {
        poly(v, p);
        if (p[2] != 0) { p[0] /= p[2]; p[1] /= p[2]; p[2] = 1; }
        else           { p[0] /= p[1]; p[1] = 1; }
    }
}
