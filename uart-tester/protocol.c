#include <string.h>
#include <stdio.h>
#include "protocol.h"
#include "serialio.h"

#define UP_PAYLOAD_LEN 160
#define UP_PREAMBLE 0xAA
#define UP_ADDRESS 0x01

static struct
{
    uint8_t preamble;
    uint8_t address;
    uint8_t data[UP_PAYLOAD_LEN];
    uint8_t checksum;
    uint8_t cr;
} buffer;

bool protocol_send_request(request* requ)
{
    int i;
    buffer.preamble = UP_PREAMBLE;
    buffer.address = UP_ADDRESS;
    memcpy(buffer.data, requ, sizeof(request));
    for(i=sizeof(request); i<UP_PAYLOAD_LEN; i++)
    {
        buffer.data[i] = 0;
    }
    buffer.checksum = UP_PREAMBLE ^ UP_ADDRESS;
    for(i=0; i<sizeof(request); i++)
    {
        buffer.checksum ^= buffer.data[i];
    }
    buffer.cr = 0x0D;
    return serial_write(&buffer, sizeof(buffer));
}

bool protocol_wait_response(response* resp)
{
    if(!serial_read(&buffer, sizeof(buffer)))
    {
        fprintf(stderr, "A");
        return false;
    }
    if(buffer.preamble != UP_PREAMBLE || buffer.address != UP_ADDRESS || buffer.cr != 0x0d)
    {
        fprintf(stderr, "B");
        return false;
    }
    uint8_t checksum = UP_ADDRESS ^ UP_PREAMBLE;
    int i;
    for(i=0; i<UP_PAYLOAD_LEN; i++)
    {
        checksum ^= buffer.data[i];
    }
    if(buffer.checksum != checksum)
    {
        fprintf(stderr, "C");
        return false;
    }
    memcpy(buffer.data, resp, sizeof(response));
    return true;
}
