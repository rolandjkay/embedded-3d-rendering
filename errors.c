#include "errors.h"

static enum Error _last_error = ERROR_OK;

void set_error(enum Error error)
{
  _last_error = error;
}

enum Error get_error()
{
  return _last_error;
}
