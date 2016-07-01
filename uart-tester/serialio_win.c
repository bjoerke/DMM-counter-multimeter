#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)

#include "serialio.h"
#include <windows.h>

static HANDLE hSerial;

bool serial_open(const char* port)
{
    char path[] = "\\\\.\\COMXX";
    //TODO: handle malicious argument
    path[7] = port[3];
    path[8] = port[4];

   // Declare variables and structures
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    // Open the highest available serial port number
    hSerial = CreateFile(
                path, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hSerial == INVALID_HANDLE_VALUE)
    {
            return false;
    }

    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        CloseHandle(hSerial);
        return false;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        CloseHandle(hSerial);
        return false;
    }

    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        CloseHandle(hSerial);
        return false;
    }
    return true;
}

unsigned int serial_write(const void* data, const unsigned int length)
{
    // Send specified text (remaining command line arguments)
    DWORD bytes_written;
    if(!WriteFile(hSerial, data, length, &bytes_written, NULL))
    {
        CloseHandle(hSerial);
        return 0;
    }
    return bytes_written;
}

unsigned int serial_read(void* buffer, const unsigned int length)
{
    DWORD bytes_read;
    if(!ReadFile(hSerial, buffer, length, &bytes_read, NULL))
    {
        CloseHandle(hSerial);
        return 0;
    }
    return bytes_read;
}

bool serial_close()
{
  // Close serial port
    if (CloseHandle(hSerial) == 0)
    {
        return false;
    }
    return true;
}

#endif
