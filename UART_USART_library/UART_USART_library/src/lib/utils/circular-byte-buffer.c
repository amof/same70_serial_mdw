#include "circular-byte-buffer.h"

uint32_t mask(uint32_t number, uint32_t capacity)
{
    return number & (capacity - 1);
}

uint8_t empty(circ_bbuf_t *c)
{
    return c->head == c->tail;
}

uint8_t full(circ_bbuf_t *c)
{
    return c->head - c->tail == c->capacity;
}

uint8_t circ_bbuf_push(circ_bbuf_t *c, uint8_t data)
{
    uint8_t result = CBB_SUCCESS;

    if(!full(c))
    {
        c->buffer[c->head++] = data;
    }else{
        result = CBB_OUT_OF_SPACE;
    }

    return result;
}

uint8_t circ_bbuf_pop(circ_bbuf_t *c, uint8_t *data)
{
    uint8_t result = CBB_SUCCESS;

    if(!empty(c))
    {
        *data = c->buffer[c->tail++];
    }else{
        result = CBB_EMPTY;
    }

    if(c->tail >= c->capacity && c->head >= c->tail)
    {
        c->tail -= c->capacity;
        c->head -= c->capacity;
    }

    return result;
}

uint8_t circ_bbuf_push_bytes(circ_bbuf_t *c, uint8_t *data, uint32_t len)
{
    uint8_t result = CBB_SUCCESS;

    for (uint32_t i = 0; i < len; ++i) {
        result = circ_bbuf_push(c, *data++);
        // If buffer full
        if(result != CBB_SUCCESS){
            break;
        }
    }

    return result;
}