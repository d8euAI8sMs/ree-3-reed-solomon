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
    
RS_API errc_t rs_encode(byte_t * in, byte_t gv, byte_t ** out)
{
    return 0;
}
    
RS_API errc_t rs_noise(byte_t * inout, float freq)
{
    return 0;
}

RS_API errc_t rs_decode(byte_t * in, byte_t gv, byte_t ** out)
{
    return 0;
}
