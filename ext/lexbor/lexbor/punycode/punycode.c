/*
 * Copyright (C) 2023-2024 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/punycode/punycode.h"
#include "lexbor/encoding/encoding.h"


enum {
    LXB_PUNYCODE_BASE = 36,
    LXB_PUNYCODE_TMIN = 1,
    LXB_PUNYCODE_TMAX = 26,
    LXB_PUNYCODE_SKEW = 38,
    LXB_PUNYCODE_DAMP = 700,
    LXB_PUNYCODE_INITIAL_BIAS = 72,
    LXB_PUNYCODE_INITIAL_N = 0x80,
    LXB_PUNYCODE_DELIMITER = 0x2D
};


static lxb_status_t
lxb_punycode_callback_cp(const lxb_codepoint_t *cps, size_t len, void *ctx);


lxb_inline lxb_char_t *
lxb_punycode_encode_realloc(lxb_char_t *p, lxb_char_t **buf,
                            const lxb_char_t **end, const lxb_char_t *buffer)
{
    size_t cur_size = *end - *buf;
    size_t nsize = cur_size * 2;
    lxb_char_t *tmp;

    if (*buf == buffer) {
        tmp = lexbor_malloc(nsize);
        if (tmp == NULL) {
            return NULL;
        }

        memcpy(tmp, *buf, cur_size);
    }
    else {
        tmp = lexbor_realloc(*buf, nsize);
        if (tmp == NULL) {
            return lexbor_free(*buf);
        }
    }

    *buf = tmp;
    *end = tmp + nsize;

    return tmp + cur_size;
}

lxb_inline lxb_codepoint_t *
lxb_punycode_decode_realloc(lxb_codepoint_t *p, lxb_codepoint_t **buf,
                            const lxb_codepoint_t **end,
                            const lxb_codepoint_t *buffer)
{
    size_t cur_size = *end - *buf;
    size_t nsize = cur_size * 2;
    lxb_codepoint_t *tmp;

    if (*buf == buffer) {
        tmp = lexbor_malloc(nsize * sizeof(lxb_codepoint_t));
        if (tmp == NULL) {
            return NULL;
        }

        memcpy(tmp, *buf, cur_size * sizeof(lxb_codepoint_t));
    }
    else {
        tmp = lexbor_realloc(*buf, nsize * sizeof(lxb_codepoint_t));
        if (tmp == NULL) {
            return lexbor_free(*buf);
        }
    }

    *buf = tmp;
    *end = tmp + nsize;

    return tmp + cur_size;
}

static char
lxb_punycode_encode_digit(size_t d) {
    return d + 22 + 75 * (d < 26);
}

static size_t
lxb_punycode_decode_digit(lxb_codepoint_t cp)
{
    return  cp - 48 < 10 ? cp - 22 : cp - 65 < 26 ? cp - 65
          : cp - 97 < 26 ? cp - 97 : LXB_PUNYCODE_BASE;
}

static size_t
lxb_punycode_adapt(size_t delta, size_t numpoints, bool firsttime)
{
    size_t k;

    delta = firsttime ? delta / LXB_PUNYCODE_DAMP : delta >> 1;
    delta += delta / numpoints;

    for (k = 0;
         delta > ((LXB_PUNYCODE_BASE - LXB_PUNYCODE_TMIN) * LXB_PUNYCODE_TMAX) / 2;
         k += LXB_PUNYCODE_BASE)
    {
        delta /= LXB_PUNYCODE_BASE - LXB_PUNYCODE_TMIN;
    }

    return k + (LXB_PUNYCODE_BASE - LXB_PUNYCODE_TMIN + 1)
           * delta / (delta + LXB_PUNYCODE_SKEW);
}

static lxb_status_t
lxb_punycode_encode_body(const lxb_codepoint_t *cps, const lxb_codepoint_t *cps_end,
                         lxb_char_t *p, lxb_char_t *buf, const lxb_char_t *end,
                         const lxb_char_t *buffer, lxb_punycode_encode_cb_f cb,
                         void *ctx)
{
    bool unchanged;
    size_t h, b, n, q, k, t, delta, bias;
    lxb_status_t status;
    lxb_codepoint_t cp, m;
    const lxb_codepoint_t *cps_t, *cps_p;

    n = LXB_PUNYCODE_INITIAL_N;
    bias = LXB_PUNYCODE_INITIAL_BIAS;
    delta = 0;
    b = p - buf;
    cps_p = cps + b;

    if (cps_p >= cps_end) {
        unchanged = true;
        goto done;
    }

    if (p > buf) {
        *p++ = LXB_PUNYCODE_DELIMITER;
    }

    unchanged = false;

    while (cps_p < cps_end) {
        m = UINT32_MAX;
        cps_t = cps;

        while (cps_t < cps_end) {
            cp = *cps_t++;

            if (cp >= n && cp < m) {
                m = cp;
            }
        }

        h = (cps_p - cps) + 1;

        if (m - n > (UINT32_MAX - delta) / h) {
            status = LXB_STATUS_ERROR_OVERFLOW;
            goto failed;
        }

        delta += (m - n) * h;
        n = m;

        cps_t = cps;

        while (cps_t < cps_end) {
            cp = *cps_t++;

            if (cp < n) {
                if (++delta == 0) {
                    status = LXB_STATUS_ERROR_OVERFLOW;
                    goto failed;
                }
            }

            if (cp == n) {
                q = delta;
                k = LXB_PUNYCODE_BASE;

                for (;; k += LXB_PUNYCODE_BASE) {
                    t = k <= bias ? LXB_PUNYCODE_TMIN :
                    k >= bias + LXB_PUNYCODE_TMAX
                    ? LXB_PUNYCODE_TMAX : k - bias;

                    if (q < t) {
                        break;
                    }

                    if (p >= end) {
                        p = lxb_punycode_encode_realloc(p, &buf, &end, buffer);
                        if (p == NULL) {
                            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                        }
                    }

                    *p++ = lxb_punycode_encode_digit(t + (q - t)
                                                     % (LXB_PUNYCODE_BASE - t));
                    q = (q - t) / (LXB_PUNYCODE_BASE - t);
                }

                h = cps_p - cps;

                if (p >= end) {
                    p = lxb_punycode_encode_realloc(p, &buf, &end, buffer);
                    if (p == NULL) {
                        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                    }
                }

                *p++ = lxb_punycode_encode_digit(q);
                bias = lxb_punycode_adapt(delta, h + 1, h == b);
                delta = 0;
                cps_p += 1;
            }
        }

        delta += 1;
        n += 1;
    }

done:

    status = cb(buf, p - buf, ctx, unchanged);

failed:

    if (buf != buffer) {
        (void) lexbor_free(buf);
    }

    return status;
}

lxb_status_t
lxb_punycode_encode(const lxb_char_t *data, size_t length,
                    lxb_punycode_encode_cb_f cb, void *ctx)
{
    size_t cp_length;
    uint8_t len;
    lxb_char_t *p, *buf;
    lxb_status_t status;
    lxb_codepoint_t cp, *cps, *cps_p;
    const lxb_char_t *data_p, *data_end, *end;
    const lxb_codepoint_t *cps_end;
    lxb_char_t buffer[4096];
    lxb_codepoint_t input[4096];

    /*
     * Make GCC happy.
     * length variable can be 0.
     */
    input[0] = 0x00;

    p = buffer;
    buf = buffer;
    end = buffer + sizeof(buffer);

    data_p = data;
    data_end = data + length;
    cp_length = 0;

    while (data_p < data_end) {
        len = lxb_encoding_decode_utf_8_length(*data_p);
        if (len == 0) {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        data_p += len;
        cp_length += 1;
    }

    if (cp_length <= sizeof(input) / sizeof(lxb_codepoint_t)) {
        cps = input;
    }
    else {
        cps = lexbor_malloc(cp_length * sizeof(lxb_codepoint_t));
        if (cps == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    data_p = data;

    cps_p = cps;
    cps_end = cps + cp_length;

    while (data_p < data_end) {
        cp = lxb_encoding_decode_valid_utf_8_single(&data_p, data_end);
        if (cp == LXB_ENCODING_DECODE_ERROR) {
            status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
            goto done;
        }

        *cps_p++ = cp;

        if (cp < 0x80) {
            if (p >= end) {
                p = lxb_punycode_encode_realloc(p, &buf, &end, buffer);
                if (p == NULL) {
                    status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                    goto done;
                }
            }

            *p++ = cp;
        }
    }

    status = lxb_punycode_encode_body(cps, cps_end, p, buf, end, buffer,
                                      cb, ctx);
done:

    if (cps != input) {
        (void) lexbor_free(cps);
    }

    return status;
}

lxb_status_t
lxb_punycode_encode_cp(const lxb_codepoint_t *cps, size_t length,
                       lxb_punycode_encode_cb_f cb, void *ctx)
{
    lxb_char_t *p, *buf;
    lxb_codepoint_t cp;
    const lxb_char_t *end;
    const lxb_codepoint_t *cps_p, *cps_end;
    lxb_char_t buffer[4096];

    p = buffer;
    buf = buffer;
    end = buffer + sizeof(buffer);

    cps_p = cps;
    cps_end = cps + length;

    while (cps_p < cps_end) {
        cp = *cps_p++;

        if (cp < 0x80) {
            if (p >= end) {
                p = lxb_punycode_encode_realloc(p, &buf, &end, buffer);
                if (p == NULL) {
                    return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                }
            }

            *p++ = cp;
        }
    }

    return lxb_punycode_encode_body(cps, cps_end, p, buf, end, buffer, cb, ctx);
}

lxb_status_t
lxb_punycode_decode(const lxb_char_t *data, size_t length,
                    lexbor_serialize_cb_f cb, void *ctx)
{
    lexbor_serialize_ctx_t nctx = {.cb = cb, .ctx = ctx};

    return lxb_punycode_decode_cb_cp(data, length, lxb_punycode_callback_cp,
                                     &nctx);
}

static lxb_status_t
lxb_punycode_callback_cp(const lxb_codepoint_t *cps, size_t len, void *ctx)
{
    uint8_t i;
    size_t length;
    lxb_status_t status;
    const lxb_codepoint_t *cps_p, *cps_end;
    lexbor_serialize_ctx_t *nctx = ctx;
    lxb_char_t *p, *buf, *end;
    lxb_char_t buffer[4096];

    /*
     * Make GCC happy.
     * len variable can be 0.
     */
    buffer[0] = 0x00;

    cps_p = cps;
    cps_end = cps_p + len;
    length = 0;

    while (cps_p < cps_end) {
        i = lxb_encoding_encode_utf_8_length(*cps_p++);
        if (i == 0) {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        length += i;
    }

    buf = buffer;
    end = buffer + sizeof(buffer);

    if (buf + length > end) {
        buf = lexbor_malloc(length);
        if (buf == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        end = buf + length;
    }

    p = buf;
    cps_p = cps;

    while (cps_p < cps_end) {
        (void) lxb_encoding_encode_utf_8_single(NULL, &p, end, *cps_p++);
    }

    status = nctx->cb(buf, p - buf, nctx->ctx);

    if (buf != buffer) {
        (void) lexbor_free(buf);
    }

    return status;
}

lxb_status_t
lxb_punycode_decode_cp(const lxb_codepoint_t *data, size_t length,
                       lexbor_serialize_cb_cp_f cb, void *ctx)
{
    size_t buf_len, digit, oldi, bias, w, k, t, i, h, in;
    const lxb_codepoint_t *delimiter, *data_p, *data_end;
    lxb_status_t status;
    lxb_codepoint_t cp, n;
    lxb_codepoint_t *p, *buf;
    const lxb_codepoint_t *end;
    lxb_codepoint_t buffer[4096];

    p = buffer;
    buf = buffer;
    buf_len = sizeof(buffer) / sizeof(lxb_codepoint_t);
    end = buffer + buf_len;

    data_p = data;
    data_end = data + length;
    delimiter = data_end;

    while (delimiter != data) {
        delimiter -= 1;

        if (*delimiter == LXB_PUNYCODE_DELIMITER) {
            break;
        }
    }

    while (data_p < delimiter) {
        cp = *data_p++;

        if (cp >= 0x80) {
            status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
            goto done;
        }

        if (p >= end) {
            p = lxb_punycode_decode_realloc(p, &buf, &end, buffer);
            if (p == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }

        *p++ = cp;
    }

    i = 0;
    n = LXB_PUNYCODE_INITIAL_N;
    bias = LXB_PUNYCODE_INITIAL_BIAS;
    data_p = (delimiter != data) ? delimiter + 1: data;
    in = data_p - data;

    for (; in < length; p++) {
        for (oldi = i, w = 1, k = LXB_PUNYCODE_BASE; ; k += LXB_PUNYCODE_BASE) {
            if (in >= length) {
                status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
                goto done;
            }

            cp = data[in++];
            digit = lxb_punycode_decode_digit(cp);

            if (digit >= LXB_PUNYCODE_BASE) {
                status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
                goto done;
            }

            if (digit > (UINT32_MAX - i) / w) {
                status = LXB_STATUS_ERROR_OVERFLOW;
                goto done;
            }

            i += digit * w;
            t = k <= bias ? LXB_PUNYCODE_TMIN
            : k >= bias + LXB_PUNYCODE_TMAX ? LXB_PUNYCODE_TMAX : k - bias;

            if (digit < t) {
                break;
            }

            if (w > UINT32_MAX / (LXB_PUNYCODE_BASE - t)) {
                status = LXB_STATUS_ERROR_OVERFLOW;
                goto done;
            }

            w *= (LXB_PUNYCODE_BASE - t);
        }

        h = (p - buf) + 1;

        bias = lxb_punycode_adapt(i - oldi, h, oldi == 0);

        if (i / h > UINT32_MAX - n) {
            status = LXB_STATUS_ERROR_OVERFLOW;
            goto done;
        }

        n += i / h;
        i %= h;

        if (p >= end) {
            p = lxb_punycode_decode_realloc(p, &buf, &end, buffer);
            if (p == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }

        memmove(buf + i + 1, buf + i, ((h - 1) - i) * sizeof(lxb_codepoint_t));
        buf[i++] = n;
    }

    status = cb(buf, p - buf, ctx);

done:

    if (buffer != buf) {
        (void) lexbor_free(buf);
    }

    return status;
}

lxb_status_t
lxb_punycode_decode_cb_cp(const lxb_char_t *data, size_t length,
                          lexbor_serialize_cb_cp_f cb, void *ctx)
{
    size_t buf_len, digit, oldi, bias, w, k, t, i, h, in;
    const lxb_char_t *delimiter, *data_p, *data_end;
    lxb_status_t status;
    lxb_codepoint_t cp, n;
    lxb_codepoint_t *p, *buf;
    const lxb_codepoint_t *end;
    lxb_codepoint_t buffer[4096];

    p = buffer;
    buf = buffer;
    buf_len = sizeof(buffer) / sizeof(lxb_codepoint_t);
    end = buffer + buf_len;

    data_p = data;
    data_end = data + length;
    delimiter = data_end;

    while (delimiter != data) {
        delimiter -= 1;

        if (*delimiter == LXB_PUNYCODE_DELIMITER) {
            break;
        }
    }

    while (data_p < delimiter) {
        cp = *data_p++;

        if (cp >= 0x80) {
            status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
            goto done;
        }

        if (p >= end) {
            p = lxb_punycode_decode_realloc(p, &buf, &end, buffer);
            if (p == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }

        *p++ = cp;
    }

    i = 0;
    n = LXB_PUNYCODE_INITIAL_N;
    bias = LXB_PUNYCODE_INITIAL_BIAS;
    data_p = (delimiter != data) ? delimiter + 1: data;
    in = data_p - data;

    for (; in < length; p++) {
        for (oldi = i, w = 1, k = LXB_PUNYCODE_BASE; ; k += LXB_PUNYCODE_BASE) {
            if (in >= length) {
                status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
                goto done;
            }

            cp = data[in++];
            digit = lxb_punycode_decode_digit(cp);

            if (digit >= LXB_PUNYCODE_BASE) {
                status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
                goto done;
            }

            if (digit > (UINT32_MAX - i) / w) {
                status = LXB_STATUS_ERROR_OVERFLOW;
                goto done;
            }

            i += digit * w;
            t = k <= bias ? LXB_PUNYCODE_TMIN
              : k >= bias + LXB_PUNYCODE_TMAX ? LXB_PUNYCODE_TMAX : k - bias;

            if (digit < t) {
                break;
            }

            if (w > UINT32_MAX / (LXB_PUNYCODE_BASE - t)) {
                status = LXB_STATUS_ERROR_OVERFLOW;
                goto done;
            }

            w *= (LXB_PUNYCODE_BASE - t);
        }

        h = (p - buf) + 1;

        bias = lxb_punycode_adapt(i - oldi, h, oldi == 0);

        if (i / h > UINT32_MAX - n) {
            status = LXB_STATUS_ERROR_OVERFLOW;
            goto done;
        }

        n += i / h;
        i %= h;

        if (p >= end) {
            p = lxb_punycode_decode_realloc(p, &buf, &end, buffer);
            if (p == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }

        memmove(buf + i + 1, buf + i, ((h - 1) - i) * sizeof(lxb_codepoint_t));
        buf[i++] = n;
    }

    status = cb(buf, p - buf, ctx);

done:

    if (buffer != buf) {
        (void) lexbor_free(buf);
    }

    return status;
}
