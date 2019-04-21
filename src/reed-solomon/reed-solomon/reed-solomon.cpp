// reed-solomon.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <reed-solomon/api.h>
#include <reed-solomon/rs.h>

errc_t rs_alloc(byte_t ** ptr, int len)
{
    *ptr = new byte_t[len];
    return 0;
}

RS_API errc_t rs_free(byte_t * ptr)
{
    delete[] ptr;
    return 0;
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
    else if ((size_t) out.len < reqlen) return 2; // insufficient space, TODO: magic constant

    rs::rs2_encoder coder(true);
    coder.encode(in, out);

    return 0;
}
    
RS_API errc_t rs_noise(buf_t inout, float freq)
{
    for (int i = 0; i < inout.len; ++i)
    {
        float p = (rand() / (RAND_MAX + 1.0f));
        inout.buf[i] = (p < freq) ? (rand() % 0xff) : inout.buf[i];
    }
    return 0;
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
    else if ((size_t) out.len < reqlen) return 2; // insufficient space, TODO: magic constant

    rs::rs2_encoder coder(true);
    coder.decode(in, out);

    return 0;
}
