#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>

int prev_right(const size_t row_lenght, const int idx);
int prev_right_bottom(const size_t row_length, const int idx);

int next_right(const size_t row_length, const int idx);
int next_right_bottom(const size_t row_length, const int idx);

#endif