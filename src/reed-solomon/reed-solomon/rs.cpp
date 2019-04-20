#include "stdafx.h"

#include <reed-solomon/rs.h>
#include <reed-solomon/m22.h>

namespace rs
{
    size_t blockstream::get(gf28::gfpoly_t & out)
    {
        size_t i;
        for (i = 0; i < _block; ++i, ++_cursor)
        {
            if (_cursor >= (size_t)_data.len) break;
            out[i] = _data.buf[_cursor];
        }

        for (size_t j = i; j < _block; ++j)
            out[j] = 0;

        return i;
    }

    size_t blockstream::put(const gf28::gfpoly_t & out)
    {
        size_t i;
        for (i = 0; i < _block; ++i, ++_cursor)
        {
            if (_cursor >= (size_t)_data.len) break;
            _data.buf[_cursor] = out[i];
        }

        return i;
    }

    size_t rs2_encoder::encbufsize(size_t in)
    {
        size_t blocksize = rs2::decblocksize();
        size_t nblocks = (in + (blocksize - 1)) / blocksize;
        return nblocks * rs2::encblocksize();
    }

    size_t rs2_encoder::decbufsize(size_t in)
    {
        size_t blocksize = rs2::encblocksize();
        size_t nblocks = in / blocksize;
        return nblocks * rs2::decblocksize();
    }

    void rs2_encoder::encode(buf_t in, buf_t out) const
    {
        blockstream instream(in, rs2::decblocksize());
        blockstream outstream(out, rs2::encblocksize());

        while (instream.get(_msg))
        {
            _rs.encode(_msg, _buf);
            outstream.put(_buf);
        }
    }

    void rs2_encoder::decode(buf_t in, buf_t out) const
    {
        blockstream instream(in, rs2::encblocksize());
        blockstream outstream(out, rs2::decblocksize());

        while (instream.get(_msg))
        {
            _rs.decode(_msg, _buf);
            outstream.put(_buf);
        }
    }

    void rs2::encode(const gf28::gfpoly_t & in, gf28::gfpoly_t & out) const
    {
        gf28::gfdfti(in, out);
    }

    void rs2::decode(const gf28::gfpoly_t & in, gf28::gfpoly_t & out) const
    {
        // Implementation Details
        // ----------------------
        //
        // The algo accepts `in` - input message of length `L = (gm - 1)`.
        // In fact, `in = encode(msg) + e`, where `e` is additive
        // error part. We can try to decode `in`, that results to:
        // `out = decode(in) = msg + decode(e)`.
        //
        // Let `S = decode(e)`. Our `out` then has the form:
        // `out = W0 W1 W2 ... W(L-4-1) S0 S1 S2 S3`, where
        // `S` is so called syndrome. Our goal is to compensate `e`
        // in our `in` message.
        //
        // We must solve two problems:
        //   - locate symbols `Wi` affected by errors
        //   - correct errors by calculating `ei`, so `Mi = Wi + ei`
        //
        // We will use 'Berlekamp - Massey' algorithm to locate
        // errors and 'Forney' algorithm to calculate corrections.
        // We also assume the number of error is at most 2.
        //
        // 1. Berlekamp - Massey Algorithm
        //
        // Iterative algorithm of error locations detecting. Due to
        // previous assumption, we start from `ec = 2`.
        //
        // Error locator `L` is a solution of matrix equation `m L = v`,
        // where `m = { { S1 S0 }, { S2, S1 } }` and `v = { S2, S3 }`.
        // If error count `ec` is actually 2, then `det m != 0` and
        // the system can be solved (by inverting `m` and multiplying
        // `v` by `m^(-1)`).
        //
        // Roots of error locator polynomial `L(x) = 1 + L0 x + L1 x^2`
        // are exactly positions of symbols with errors. Let `r0, r1`
        // be roots of `L(2^r) = 0` (note that we use `2^r` instead of
        // just `r` here!). We will refer to them later.
        //
        // If we get `det m = 0`, our assumption `ec = 2` was incorrect,
        // so we reduce the order of `m`, `L` and `v` from 2 to 1:
        // `m L = v |=> S0 L = S1 |=> L = S1 / S0`. Error locator
        // polynomial is now `L(x) = 1 + L x` and its single root is `r`.
        //
        // 2. Forley Algorithm
        //
        // As we get `r0, r1, L(x)`, we can calculate `ei`.
        //
        // First, we normalize `L(x)` by dividing its coefficients
        // by `L1`. We now have something like `L(x) = x^2 + L0 x + L1`.
        // 
        // Algorithm's theory says that each product written in form
        // 
        // `Oi 1 + O(i+1) L0 + O(i+2) L1 = 0`.
        // 
        // For `i = (L-4-1)` we get:
        // 
        // `W0 ... W(L-4-1) S0 S1 S2 S3`
        // `0  ... 1        L0 L1 0  0 `
        // 
        // Now we can easily solve `W(L-4-1)` from equation above.
        // Having `W(i)` we can calculate `W(i-1)` and so on.
        //
        // Values of `O(x) = W0 + W1 x + ... + S3 x^L` calculated in
        // points `2^r0` and `2^r1` are `e(r0)` and `e(r1)` correspondingly.
        //
        // The only thing remaining is to add `e` to `in` and decode
        // the message in a regular way.
        //
        // ----------------------

        gf28::gfdft(in, out);
        
        // get syndrome symbols
        auto len = rs::rs2::encblocksize();
        auto s3 = out[len - 1], s2 = out[len - 2],
             s1 = out[len - 3], s0 = out[len - 4];

        // no error, exit
        if (s0 == 0 && s1 == 0 && s2 == 0 && s3 == 0) return;

        // 1.1 calculate error locator
        gf28::m22 m = { s1, s0, s2, s1 };
        gf28::v2 v = { s2, s3 };
        gf28::v2 l;

        size_t ec = 2; // number of errors

        // suppose we have exactly 2 errors
        // then |m| != 0 and we can solve our system
        // if |m| = 0, our assumption was incorrect,
        // we have exactly one error (due to
        // previous assumption that number of errors
        // is at most 2)
        if (!gf28::lsol(m, v, l))
        {
            l = { s1 / s0, 0 }; ec = 1;
        }
        
        // 1.2 its roots - error positions
        gf28::npoly(l, _p);
        gf28::gfdfti(_p, _f);

        // iterate result to get actual error positions
        gf28::gfscal_t r[2];
        size_t rn = 0;
        for (byte_t i = 0; (i < gf28::gfpoly_t::pwr()) && (rn < 2); ++i)
        {
            if (_f[i] == 0) r[rn++] = i;
        }

        //TODO: assert(ri == ec);

        // 2.1 calculate error correction polynomial
        //     convolve error locator polynomial with code symbols
        auto dlen = rs::rs2::decblocksize();
        for (byte_t i = 0; i < rs::rs2::decblocksize(); ++i)
        {
            auto j = dlen - 1 - i;
            if (ec == 2) out[j] = _p[1] * out[j + 1] + _p[0] * out[j + 2];
            else         out[j] = _p[0] * out[j + 1];
        }

        _e = in;

        // 2.2 evaluate actual corrections and apply them

        _e[r[0]] += out(gf28::gfscal_t::pepwr(r[0]));
        if (ec == 2) _e[r[1]] += out(gf28::gfscal_t::pepwr(r[1]));

        // 3 decode with corrected errors

        gf28::gfdft(_e, out);
    }
}