#ifndef DRIVER_H
#define DRIVER_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "main.h"
#include <inttypes.h>

#define HEADER 0x55
#define PACKET_TYPE_383 0x53    // s1:5331
#define PACKET_TYPE_330 0x7A    // z1:7A31
#define PACKET_TYPE_RTK 0x73    // s1:7331
#define PI 3.1415926

static uint64_t time_count = 0;
static int8_t head;
static int16_t p_type;
static int8_t length = 0;

typedef struct imuData *imuDataPointer;
struct imuData {
    union time {
        float t_383;
        uint32_t t_330;
    } time;
    uint32_t count;
    float accx;
    float accy;
    float accz;
    float gyrox;
    float gyroy;
    float gyroz;
};

typedef struct rtkData *rtkDataPointer;
struct rtkData {
    uint16_t GPS_Week;
    uint32_t GPS_TimeOfWeek;
    float accx;
    float accy;
    float accz;
    float gyrox;
    float gyroy;
    float gyroz;
};

int16_t reverse(int16_t x)
{
    x = (((x & 0xff00) >> 8) | ((x & 0x00ff) << 8));
    return x;
}

int16_t concat_16(int8_t a, int8_t b)
{
    return a << 8 | (b & 0x00ff);
}

int32_t concat_32(int8_t a, int8_t b, int8_t c, int8_t d)
{
    return concat_16(a, b) << 16 | (concat_16(c, d) & 0x0000ffff);
}

uint16_t CalculateCRC(uint8_t *buf, uint16_t length)
{
    uint16_t crc = 0x1D0F;

    for (int i = 0; i < length; i++) {
        crc ^= buf[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
        }
    }

    return ((crc << 8) & 0xFF00) | ((crc >> 8) & 0xFF);
}

void parse_data_383(int8_t *data, imuDataPointer result)
{
    result->time.t_383 =
        (float) (uint16_t) (concat_16(data[20], data[21])) * 15.259022;
    if (result->time.t_383 == 0 || result->time.t_383 == 1000000)
        time_count++;
    result->count = time_count;
    result->accx =
        (float) (concat_16(data[0], data[1])) * 20 / (1 << 16);
    result->accy =
        (float) (concat_16(data[2], data[3])) * 20 / (1 << 16);
    result->accz =
        (float) (concat_16(data[4], data[5])) * 20 / (1 << 16);
    result->gyrox =
        (float) (concat_16(data[6], data[7])) * 7 * PI / (1 << 16);
    result->gyroy =
        (float) (concat_16(data[8], data[9])) * 7 * PI / (1 << 16);
    result->gyroz =
        (float) (concat_16(data[10], data[11])) * 7 * PI / (1 << 16);
    printf("%f ", result->accx);    // unit (g)
    printf("%f ", result->accy);
    printf("%f ", result->accz);
    printf("%f ", result->gyrox);   // unit (rad/s)
    printf("%f ", result->gyroy);
    printf("%f ", result->gyroz);
    printf("%"PRIu32, result->count);
    printf("%f ", result->time.t_383);  // unit (uS)
    printf("\n");
    return;
}

void parse_data_330(uint8_t *data, imuDataPointer result)
{
    int32_t temp;
    temp = concat_32(data[3], data[2], data[1], data[0]);
    result->time.t_330 = temp;
    temp = concat_32(data[7], data[6], data[5], data[4]);
    result->accx = *((float *) ((void *) (&temp)));
    temp = concat_32(data[11], data[10], data[9], data[8]);
    result->accy = *((float *) ((void *) (&temp)));
    temp = concat_32(data[15], data[14], data[13], data[12]);
    result->accz = *((float *) ((void *) (&temp)));
    temp = concat_32(data[19], data[18], data[17], data[16]);
    result->gyrox = *((float *) ((void *) (&temp)));
    temp = concat_32(data[23], data[22], data[21], data[20]);
    result->gyroy = *((float *) ((void *) (&temp)));
    temp = concat_32(data[27], data[26], data[25], data[24]);
    result->gyroz = *((float *) ((void *) (&temp)));

    printf("%f ", result->accx);
    printf("%f ", result->accy);
    printf("%f ", result->accz);
    printf("%f ", result->gyrox);
    printf("%f ", result->gyroy);
    printf("%f ", result->gyroz);
    printf("%"PRIu32, result->time.t_330);
    printf("\r\n");

    return;
}

void parse_data_rtk(int8_t *data, rtkDataPointer result)
{
    int16_t temp1;
    int32_t temp2;
    temp1 = concat_16(data[1], data[0]);
    result->GPS_Week = temp1;
    temp2 = concat_32(data[5], data[4], data[3], data[2]);
    result->GPS_TimeOfWeek = temp2;
    temp2 = concat_32(data[9], data[8], data[7], data[6]);
    result->accx = *((float *) ((void *) (&temp2)));
    temp2 = concat_32(data[13], data[12], data[11], data[10]);
    result->accy = *((float *) ((void *) (&temp2)));
    temp2 = concat_32(data[17], data[16], data[15], data[14]);
    result->accz = *((float *) ((void *) (&temp2)));
    temp2 = concat_32(data[21], data[20], data[19], data[18]);
    result->gyrox = *((float *) ((void *) (&temp2)));
    temp2 = concat_32(data[25], data[24], data[23], data[22]);
    result->gyroy = *((float *) ((void *) (&temp2)));
    temp2 = concat_32(data[29], data[28], data[27], data[26]);
    result->gyroz = *((float *) ((void *) (&temp2)));

    printf("%f,", result->accx);
    printf("%f,", result->accy);
    printf("%f,", result->accz);
    printf("%f,", result->gyrox);
    printf("%f,", result->gyroy);
    printf("%f,", result->gyroz);
    printf("%d,", result->GPS_Week);
    printf("%.3f", result->GPS_TimeOfWeek/1000.0);
    printf("\n");
    return;
}

uint8_t *launch_driver_8(UART_HandleTypeDef huart, int8_t header, int8_t packet_type)
{
    uint8_t *buffer = (uint8_t *) malloc((50) * sizeof(uint8_t));
    if (HAL_UART_Receive(&huart, &buffer[0], sizeof(uint8_t), 1)==HAL_OK) {
        if (buffer[0] == header) {
            if (HAL_UART_Receive(&huart, &buffer[0], sizeof(uint8_t), 1)==HAL_OK) {
                if (buffer[0] == header) {
                    if (HAL_UART_Receive(&huart, &buffer[0], sizeof(uint8_t), 1)==HAL_OK) {
                        if (buffer[0] == packet_type) {
                            if (HAL_UART_Receive(&huart, &buffer[1], sizeof(uint8_t), 1)==HAL_OK) {
                                if (buffer[1] == 0x31) {
                                    if (HAL_UART_Receive(&huart, &buffer[2], sizeof(uint8_t), 1)==HAL_OK) {
                                        int32_t count = 0;
                                        while (count < buffer[2] + 2) {
                                            if (HAL_UART_Receive(&huart, &buffer[count + 3], sizeof(uint8_t), 1)==HAL_OK) {
                                                count++;
                                            }
                                        }
                                        if (CalculateCRC(buffer, buffer[2] + 3) == (uint16_t) concat_16(buffer[buffer[2] + 4], buffer[buffer[2] + 3]))
                                            return buffer;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    free(buffer);
    return NULL;
}

#endif /* DRIVER_H */