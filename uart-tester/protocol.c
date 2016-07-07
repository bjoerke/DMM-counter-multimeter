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
    buffer.checksum = 0;
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
        return false;
    }
    if(buffer.preamble != UP_PREAMBLE || buffer.address != UP_ADDRESS || buffer.cr != 0x0d)
    {
        return false;
    }
    uint8_t checksum = 0;
    int i;
    for(i=0; i<UP_PAYLOAD_LEN; i++)
    {
        checksum ^= buffer.data[i];
    }
    if(buffer.checksum != checksum)
    {
        return false;
    }
    memcpy(resp, buffer.data, sizeof(response));
    return true;
}

void protocol_print_response(response* r)
{
    if(r->direct_voltage.range != UP_DONT_MEASURE)
        printf("Direct voltage: 0x%X\n", r->direct_voltage.value);
    if(r->direct_current.range != UP_DONT_MEASURE)
        printf("Direct current: 0x%X\n", r->direct_current.value);
    if(r->alternating_voltage.range != UP_DONT_MEASURE)
        printf("Alternating voltage: 0x%X\n", r->alternating_voltage.value);
    if(r->alternating_current.range != UP_DONT_MEASURE)
        printf("Alternating current: 0x%X\n", r->alternating_current.value);
    if(r->resistance.range != UP_DONT_MEASURE)
        printf("Resistance: 0x%X\n", r->resistance.value);
    if(r->frequency.range != UP_DONT_MEASURE)
        printf("Frequency: 0x%X\n", r->frequency.value);
    if(r->duty_cycle.range != UP_DONT_MEASURE)
        printf("Duty Cycle: 0x%X\n", r->duty_cycle.value);
}
