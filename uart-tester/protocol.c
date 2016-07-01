#include "protocol.h"

void protocol_calc_checksum(request* r)
{
    uint8_t* rp = (uint8_t*) r;
    uint8_t checksum = 0;

    for(int i=0; i<sizeof(request) - 1; i++)
    {
        checksum ^= rp[i];
    }
    r->checksum = checksum;
}
