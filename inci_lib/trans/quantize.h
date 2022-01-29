#ifndef QUAN_H
#define QUAN_H

#include<stdint.h>

namespace quan{
inline static void quantizeNaive(char *data_ptr, uint32_t size)
{
    int factor = 1000000;
    int *int_data_ptr = (int *)data_ptr;
    float *float_data_ptr = (float *)data_ptr;
    for (uint32_t i = 0; i < size; i++)
    {
        int_data_ptr[i] = (int)(float_data_ptr[i] * factor);
    }
}

// translate back to float and scale down
// without any further optimization
inline static void dequantizeNaive(char *data_ptr, uint32_t size)
{
    float factor = 1000000.0;
    int *int_data_ptr = (int *)data_ptr;
    float *float_data_ptr = (float *)data_ptr;
    for (uint32_t i = 0; i < size; i++)
    {
        float_data_ptr[i] = (float)(int_data_ptr[i] / factor);
    }
}

inline static void signalNaive(char *data_ptr, uint32_t size)
{
    int *int_data_ptr = (int *)data_ptr;
    uint32_t *uint_data_ptr = (uint32_t *)data_ptr;
    for (uint32_t i = 0; i < size; i++)
    {
        int_data_ptr[i] = (int)(uint_data_ptr[i]);
    }
}

// translate back to float and scale down
// without any further optimization
inline static void designalNaive(char *data_ptr, uint32_t size)
{
    int *int_data_ptr = (int *)data_ptr;
    uint32_t *uint_data_ptr = (uint32_t *)data_ptr;
    for (uint32_t i = 0; i < size; i++)
    {
        uint_data_ptr[i] = (uint32_t)(int_data_ptr[i]);
    }
}
}

#endif