#pragma once
#include <cstring>

template <typename WithStructSize>
void initStructSize(WithStructSize* data)
{
    memset(data, 0, sizeof(*data));
    data->StructSize = sizeof(*data);
}