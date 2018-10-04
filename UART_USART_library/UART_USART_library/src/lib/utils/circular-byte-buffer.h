/******************************************************************************

Based on Siddharth Chandrasekaran library : https://github.com/EmbedJournal/c-utils

******************************************************************************/
#ifndef __CIRCULAR_BYTE_BUFFER_H_
#define __CIRCULAR_BYTE_BUFFER_H_

#include <stdint.h>

typedef struct {
    uint8_t * const buffer;
    uint32_t head;
    uint32_t tail;
    const uint32_t capacity;
} circ_bbuf_t;

enum CBB_RESULT{CBB_SUCCESS, CBB_EMPTY, CBB_OUT_OF_SPACE};

#define CIRC_BBUF_DEF(x,y)                \
    uint8_t x##_data_space[y];            \
    circ_bbuf_t x = {                     \
        .buffer = x##_data_space,         \
        .head = 0,                        \
        .tail = 0,                        \
        .capacity = y                     \
    }

/**
 * Check if circular buffer is empty
 * @param c
 * @return FALSE
 *         TRUE
 */
uint8_t circ_bbuf_is_empty(circ_bbuf_t *c);

/**
 * Check if circular buffer is full
 * @param c
 * @return FALSE
 *         TRUE
 */
uint8_t circ_bbuf_is_full(circ_bbuf_t *c);

/**
 * Retrieve one byte from buffer
 * @param c
 * @param data
 * @return CBB_SUCCESS
 *         CBB_EMPTY
 */
uint8_t circ_bbuf_pop(circ_bbuf_t *c, uint8_t *data);


/**
 * Insert one byte in buffer
 * @param c
 * @param data
 * @return CBB_SUCCESS
 *         CBB_OUT_OF_SPACE
 */
uint8_t circ_bbuf_push(circ_bbuf_t *c, uint8_t data);

/**
 * Insert multiples bytes into buffer
 * @param c
 * @param data
 * @param len
 * @return CBB_SUCCESS
 *         CBB_OUT_OF_SPACE
 */
uint8_t circ_bbuf_push_bytes(circ_bbuf_t *c, uint8_t *data, uint32_t len);

#endif /* __CIRCULAR_BYTE_BUFFER_H_ */
