#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdbool.h>

bool serial_open(const char* port);

bool serial_write(const void* data, const unsigned int length);

bool serial_read(void* buffer, const unsigned int length);

bool serial_close();

#endif
