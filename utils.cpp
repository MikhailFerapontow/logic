#include "utils.hpp"

int prev_right(const size_t row_lenght, const int idx)
{
  int a = idx / row_lenght;
  int b = idx % row_lenght;

  if (b == 0)
  {
    #ifdef ADVANCED
      return a * row_lenght + row_lenght - 1;
    #endif
    return -1;
  }

  return idx - 1;
}

int prev_right_bottom(const size_t row_length, const int idx)
{
  int a = idx / row_length;
  int b = idx % row_length;

  if (a == 0)
  {
    return -1;
  }

  if (b == 0)
  {
    #ifdef ADVANCED
      return (a - 1) * row_length + row_length - 1;
    #endif
    return -1;
  }

  return (a - 1) * row_length + b - 1;
}

int next_right(size_t row_length, const int idx)
{
  int a = idx / row_length;
  int b = idx % row_length;

  if (b == row_length - 1)
  {
    #ifdef ADVANCED
      return a * row_length;
    #endif

    return -1;
  }

  return idx + 1;
}

int next_right_bottom(size_t row_length, const int idx)
{
  int a = idx / row_length;
  int b = idx % row_length;

  if (b == row_length - 1)
  {
    #ifdef ADVANCED
      b = -1;
    #endif
    #ifndef ADVANCED
    return -1;
    #endif
  }

  if (a == row_length - 1)
  {
    return -1;
  }

  return (a + 1) * row_length + b + 1;
}