// reed-solomon.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <reed-solomon/api.h>

errc_t rs_alloc(byte_t ** ptr, int len)
{
    return 0;
}

RS_API errc_t rs_free(byte_t * ptr)
{
    return 0;
}
    
RS_API errc_t rs_encode(buf_t in, word_t gv, buf_t & out)
{
    return 0;
}
    
RS_API errc_t rs_noise(buf_t inout, float freq)
{
    return 0;
}

RS_API errc_t rs_decode(buf_t in, word_t gv, buf_t & out)
{
    return 0;
}
