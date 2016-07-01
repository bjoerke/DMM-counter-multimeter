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

    /** create request **/
    request request = {
        .address = UP_ADDRESS,
        .direct_voltage = UP_MEASURE_RANGE_AUTO,
        .direct_current = UP_MEASURE_RANGE_AUTO,
        .alternating_voltage = UP_MEASURE_RANGE_AUTO,
        .alternating_current = UP_MEASURE_RANGE_AUTO,
        .resistance = UP_MEASURE_RANGE_AUTO,
        .frequency = UP_MEASURE_RANGE_AUTO,
        .duty_cycle = UP_MEASURE_RANGE_AUTO,
        ._reserved = 0
    };
	protocol_calc_checksum(&request);

	/** send request **/
    serial_write(&request, sizeof(request));

    /** read response **/
    response response;
    unsigned int bytes_read = serial_read(&response, sizeof(response));
    if(bytes_read < sizeof(response))
    {
        fprintf(stderr, "cannot read response");
        return -1;
    }

    /** close serial port **/
    if(!serial_close())
    {
        fprintf(stderr, "Cannot close");
        return -1;
    }
    return 0;
}
