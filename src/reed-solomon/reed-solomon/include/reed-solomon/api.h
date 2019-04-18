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
     * <p>Don't forget to call {@ref rs_free} with {@code out}
     * parameter to release allocated memory.
     * 
     * @param in [in] array of bytes to be encoded (null-terminated)
     * @param gv [in] generator polynomial
     * @param out [out] the result of the operation - a pointer
     *                  to null-terminated array of bytes
     * 
     * @return 0 on success
     */
    RS_API errc_t rs_encode(byte_t * in, byte_t gv, byte_t ** out);
    
    /**
     * Adds random noise to the given array of bytes.
     * 
     * @param inout [inout] array of bytes to be noised (null-terminated)
     * @param freq [in] noise rate per byte (0 - no bytes affected, 1 - all bytes affected)
     * 
     * @return 0 on success
     */
    RS_API errc_t rs_noise(byte_t * inout, float freq);
    
    /**
     * Decodes {@code in} trying to correct all errors (if any).
     * 
     * <p>Don't forget to call {@ref rs_free} with {@code out}
     * parameter to release allocated memory.
     * 
     * @param in [in] array of bytes to be decoded (null-terminated)
     * @param gv [in] generator polynomial
     * @param out [out] the result of the operation - a pointer
     *                  to null-terminated array of bytes
     * 
     * @return 0 on success
     */
    RS_API errc_t rs_decode(byte_t * in, byte_t gv, byte_t ** out);

    // C#-style aliases

    inline RS_API errc_t ReedSolomonAlloc(byte_t ** ptr, int len) { return rs_alloc(ptr, len); }

    inline RS_API errc_t ReedSolomonFree(byte_t * ptr) { return rs_free(ptr); }

    inline RS_API errc_t ReedSolomonEncode(byte_t * in, byte_t gv, byte_t ** out) { return rs_encode(in, gv, out); }

    inline RS_API errc_t ReedSolomonNoise(byte_t * inout, float freq) { return rs_noise(inout, freq); }

    inline RS_API errc_t ReedSolomonDecode(byte_t * in, byte_t gv, byte_t ** out) { return rs_decode(in, gv, out); }
}
