#include <stdio.h>
#include <stdlib.h>
#include "serialio.h"
#include "protocol.h"

int main()
{
    /** Open serial port **/
    if(!serial_open("COM3"))
    {
        fprintf(stderr, "Cannot open");
        return -1;
    }

    /** send request **/
    request request = {
        .direct_voltage = UP_MEASURE_RANGE_AUTO,
        .direct_current = UP_MEASURE_RANGE_AUTO,
        .alternating_voltage = UP_MEASURE_RANGE_AUTO,
        .alternating_current = UP_MEASURE_RANGE_AUTO,
        .resistance = UP_MEASURE_RANGE_AUTO,
        .frequency = UP_MEASURE_RANGE_AUTO,
        .duty_cycle = UP_MEASURE_RANGE_AUTO,
    };
    protocol_send_request(&request);

    /** read response **/
    response response;
    if(!protocol_wait_response(&response))
    {
        fprintf(stderr, "Response error!");
        return -1;
    }
    protocol_print_response(&response);

    /** close serial port **/
    if(!serial_close())
    {
        fprintf(stderr, "Cannot close");
        return -1;
    }
    return 0;
}
