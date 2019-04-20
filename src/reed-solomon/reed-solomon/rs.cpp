#include "stdafx.h"

#include <reed-solomon/rs.h>

namespace rs
{
    size_t blockstream::get(gf28::gfpoly_t & out)
    {
        size_t i;
        for (i = 0; i < _block; ++i, ++_cursor)
        {
            if (_cursor >= _data.len) break;
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
            if (_cursor >= _data.len) break;
            _data.buf[_cursor] = out[i];
        }

        return i;
    }

    size_t rs2_encoder::encbufsize(size_t in)
    {
        size_t blocksize = rs2::decblocksize();
        size_t nblocks = (in + (blocksize + 1) / 2) / blocksize;
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
        //TODO: correction logic
        gf28::gfdft(in, out);
    }
}