// reed-solomon.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <reed-solomon/api.h>
#include <reed-solomon/rs.h>

errc_t rs_alloc(byte_t ** ptr, int len)
{
    *ptr = new byte_t[len];
    return errc::errc_success;
}

RS_API errc_t rs_free(byte_t * ptr)
{
    delete[] ptr;
    return errc::errc_success;
}
    
RS_API errc_t rs_encode(buf_t in, word_t gv, buf_t & out)
{
    auto err = gf28::gfinit(gv);
    if (err) return err;

    auto reqlen = rs::rs2_encoder::encbufsize(in.len, true);

    if (out.len == 0)
    {
        out.len = reqlen;
        err = rs_alloc(&out.buf, out.len);
        if (err) return err;
    }
    else if ((size_t) out.len < reqlen) return errc::errc_insufficient_buffer_space;

    rs::rs2_encoder coder(true);
    coder.encode(in, out);

    return errc::errc_success;
}
    
RS_API errc_t rs_noise(buf_t inout, word_t gv, size_t ec, float freq)
{
    auto err = gf28::gfinit(gv);
    if (err) return err;

    size_t block = rs::rs2::encblocksize();

    buf_t copy = { inout.len, nullptr };
    
    err = rs_alloc(&copy.buf, copy.len);
    if (err) return err;

    // blockstream may perform ahead writes to output buffer,
    // so we cannot directly write to inout...
    rs::blockstream in(inout, gf28::gfscal_t::gp, block);
    rs::blockstream out(copy, gf28::gfscal_t::gp, block);
    
    gf28::gfpoly_t buf;

    size_t read;

    while (read = in.get(buf))
    {
        if (rand() / (RAND_MAX + 1.0f) <= freq)
        {
            size_t ecc = (ec <= 1) ? 1 : (((size_t)rand() % ec) + 1);
            for (size_t i = 0; i < ecc; ++i)
            {
                buf[rand() % read] = rand() % gf28::gfscal_t::gm;
            }
        }
        out.put(buf);
    }

    for (size_t i = 0; i < (size_t)copy.len; ++i)
    {
        inout.buf[i] = copy.buf[i];
    }

    rs_free(copy.buf);

    return errc::errc_success;
}

RS_API errc_t rs_decode(buf_t in, word_t gv, buf_t & out)
{
    auto err = gf28::gfinit(gv);
    if (err) return err;

    auto reqlen = rs::rs2_encoder::decbufsize(in.len, true);

    if (out.len == 0)
    {
        out.len = reqlen;
        err = rs_alloc(&out.buf, out.len);
        if (err) return err;
    }
    else if ((size_t) out.len < reqlen) return errc::errc_insufficient_buffer_space;

    rs::rs2_encoder coder(true);
    coder.decode(in, out);

    return errc::errc_success;
}
