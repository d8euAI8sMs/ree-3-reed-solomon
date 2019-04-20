#pragma once

#include <reed-solomon/api.h>
#include <reed-solomon/gf28.h>

#include <vector>

namespace rs
{
    /**
     * Input-output block-oriented byte stream.
     */
    class blockstream
    {
    private:
        buf_t _data;
        size_t _block;
        size_t _cursor;
    public:
        /**
         * Creates new stream backed by the given buffer
         * with the provided block size.
         */
        blockstream(buf_t data, size_t block) : _data(data), _block(block), _cursor(0) {}
    public:
        /**
         * Reads bytes from the stream till its end and
         * stores them to {@code out}. If the number of
         * bytes read less than block size, fills the
         * rest of the block with zeros.
         * 
         * @param out [out] buffer
         * @return a number of bytes actually read
         */
        RS_API size_t get(gf28::gfpoly_t & out);
        /**
         * Writes data from the given buffer to the
         * backing byte buffer.
         * 
         * @param in [in] buffer
         * @return a number of bytes actually written
         */
        RS_API size_t put(const gf28::gfpoly_t & in);
    };

    /**
     * Encapsulates Reed-Solomon code over GF(2^gp)
     * encoding-decoding logic for a single message
     * of length (gm - 1) symbols, 2k of which are redundant.
     * 
     * <p>In case of this class k = 2, i.e. there are
     * 2k = 4 redundant symbols per message.
     * 
     * <p>The class is mostly for internal needs, use
     * {@ref rs2_encoder} to encode/decode blocks of messages.
     */
    class rs2
    {
    public:
        static size_t encblocksize() { return gf28::gfpoly_t::pwr(); }
        static size_t decblocksize() { return gf28::gfpoly_t::pwr() - 2 * 2; }
    private:
        mutable gf28::gfpoly_t _p, _f, _e;
    public:
        rs2() {}
    public:
        /**
         * Encodes {@code in} message and stores result to {@code out}.
         * Expects last 2k symbols are zero.
         */
        RS_API void encode(const gf28::gfpoly_t & in, gf28::gfpoly_t & out) const;
        /**
         * Decodes {@code in} message and stores result to {@code out}.
         */
        RS_API void decode(const gf28::gfpoly_t & in, gf28::gfpoly_t & out) const;
    };

    class rs2_encoder
    {
    public:
        RS_API static size_t encbufsize(size_t in);
        RS_API static size_t decbufsize(size_t in);
    private:
        rs2 _rs;
        mutable gf28::gfpoly_t _msg;
        mutable gf28::gfpoly_t _buf;
    public:
        rs2_encoder() {}
    public:
        RS_API void encode(buf_t in, buf_t out) const;
        RS_API void decode(buf_t in, buf_t out) const;
    };
}
