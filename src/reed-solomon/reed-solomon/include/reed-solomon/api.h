#pragma once

#include <cstdint>

#ifdef RS_DLL
#define RS_API __declspec(dllexport)
#else
#define RS_API __declspec(dllimport)
#endif

extern "C"
{
    using byte_t = std::uint8_t;
    using word_t = std::uint16_t;

    struct buf_t { int len; byte_t * buf; };

    using errc_t = int;

    /**
     * Allocates {@code len} bytes and stores
     * the pointer to {@code ptr}.
     * 
     * @param ptr [out] pointer to newly allocated memory
     * @param len [in] requested length (in bytes)
     * 
     * @return 0 on success
     */
    RS_API errc_t rs_alloc(byte_t ** ptr, int len);
    
    /**
     * Deallocates memory prevously allocated by {@ref rs_alloc}.
     * 
     * @param ptr [in] pointer to memory
     * 
     * @return 0 on success
     */
    RS_API errc_t rs_free(byte_t * ptr);
    
    /**
     * Encodes the given number of bytes using
     * RS code built over the GF(2^8) with generator
     * polynomial {@code gv}.
     * 
     * <p>If buffer of zero length passed in {@code out}
     * parameter, the function will allocate new buffer
     * with {@ref rs_alloc}. Don't forget to call
     * {@ref rs_free} with {@code out.buf} parameter to
     * release allocated memory.
     * 
     * @param in [in] array of bytes to be encoded (null-terminated)
     * @param gv [in] generator polynomial
     * @param out [out] the result of the operation
     * 
     * @return 0 on success,
     *         2 if {@code out.len != 0} and {@code out}
     *         has insufficient space
     */
    RS_API errc_t rs_encode(buf_t in, word_t gv, buf_t & out);
    
    /**
     * Adds random noise to the given array of bytes.
     * 
     * @param inout [inout] array of bytes to be noised (null-terminated)
     * @param freq [in] noise rate per byte (0 - no bytes affected, 1 - all bytes affected)
     * 
     * @return 0 on success
     */
    RS_API errc_t rs_noise(buf_t inout, float freq);
    
    /**
     * Decodes {@code in} trying to correct all errors (if any).
     * 
     * <p>If buffer of zero length passed in {@code out}
     * parameter, the function will allocate new buffer
     * with {@ref rs_alloc}. Don't forget to call
     * {@ref rs_free} with {@code out.buf} parameter to
     * release allocated memory.
     * 
     * @param in [in] array of bytes to be decoded (null-terminated)
     * @param gv [in] generator polynomial
     * @param out [out] the result of the operation
     * 
     * @return 0 on success,
     *         2 if {@code out.len != 0} and {@code out}
     *         has insufficient space
     */
    RS_API errc_t rs_decode(buf_t in, word_t gv, buf_t & out);

    // C#-style aliases

    inline RS_API errc_t ReedSolomonAlloc(byte_t ** ptr, int len) { return rs_alloc(ptr, len); }

    inline RS_API errc_t ReedSolomonFree(byte_t * ptr) { return rs_free(ptr); }

    inline RS_API errc_t ReedSolomonEncode(buf_t in, word_t gv, buf_t & out) { return rs_encode(in, gv, out); }

    inline RS_API errc_t ReedSolomonNoise(buf_t inout, float freq) { return rs_noise(inout, freq); }

    inline RS_API errc_t ReedSolomonDecode(buf_t in, byte_t gv, buf_t & out) { return rs_decode(in, gv, out); }
}
