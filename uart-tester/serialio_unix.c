//#if defined(linux) || defined(__LINUX__) || defined(unix) || defined(__UNIX__)

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "serialio.h"

static int fd;
static struct termios  config;

bool serial_open(const char* port)
{
    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1)
    {
        return false;
    }
    // Check if the file descriptor is pointing to a TTY device or not.
    if(!isatty(fd))
    {
        return false;
    }

    // Get the current configuration of the serial interface
    if(tcgetattr(fd, &config) < 0) return false;

    // Input flags - Turn off input processing
    // convert break to null byte, no CR to NL translation,
    // no NL to CR translation, don't mark parity errors or breaks
    // no input parity check, don't strip high bit off,
    // no XON/XOFF software flow control
    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
                     INLCR | PARMRK | INPCK | ISTRIP | IXON);

    // Output flags - Turn off output processing
    //
    // no CR to NL translation, no NL to CR-NL translation,
    // no NL to CR translation, no column 0 CR suppression,
    // no Ctrl-D suppression, no fill characters, no case mapping,
    // no local output processing
    //
    // config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
    //                     ONOCR | ONOEOT| OFILL | OLCUC | OPOST);
    config.c_oflag = 0;

    // No line processing
    // echo off, echo newline off, canonical mode off,
    // extended input processing off, signal chars off
    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    // Turn off character processing
    // clear current char size mask, no parity checking,
    // no output processing, force 8 bit input
    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8;

    // One input byte is enough to return from read()
    // Inter-character timer off
    config.c_cc[VMIN]  = 1;
    config.c_cc[VTIME] = 0;

    // Communication speed (simple version, using the predefined
    // constants)
    if(cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0)
    {
        return false;
    }

    // Finally, apply the configuration
    if(tcsetattr(fd, TCSAFLUSH, &config) < 0)
    {
        return false;
    }
    return true;
}
bool serial_write(const void* data, const unsigned int length)
{
    write(fd, data, length);
    tcdrain(fd);
    sleep(1);  //TODO: tcdrain does not work?!
    return true;
}

bool serial_read(void* buffer, const unsigned int length)
{
    return read(fd, buffer, length) == length;
}

bool serial_close()
{
    close(fd);
    return true;
}

//#endif // defined
