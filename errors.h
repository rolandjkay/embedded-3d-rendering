#ifndef _ERRORS_H
#define _ERRORS_H

enum Error
{
  ERROR_OK = 0,
  ERROR_DISPLAY_HW_ERROR = 1
};

void set_error(enum Error error);
enum Error get_error();

#endif
