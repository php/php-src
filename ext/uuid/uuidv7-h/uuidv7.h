/**
 * @file
 *
 * uuidv7.h - Single-file C/C++ UUIDv7 Library
 *
 * @version   v0.1.6
 * @author    LiosK
 * @copyright Licensed under the Apache License, Version 2.0
 * @see       https://github.com/LiosK/uuidv7-h
 */
/*
 * Copyright 2022 LiosK
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef UUIDV7_H_BAEDKYFQ
#define UUIDV7_H_BAEDKYFQ

#include <stddef.h>
#include <stdint.h>

/**
 * @name Status codes returned by uuidv7_generate()
 *
 * @{
 */

/**
 * Indicates that the `unix_ts_ms` passed was used because no preceding UUID was
 * specified.
 */
#define UUIDV7_STATUS_UNPRECEDENTED (0)

/**
 * Indicates that the `unix_ts_ms` passed was used because it was greater than
 * the previous one.
 */
#define UUIDV7_STATUS_NEW_TIMESTAMP (1)

/**
 * Indicates that the counter was incremented because the `unix_ts_ms` passed
 * was no greater than the previous one.
 */
#define UUIDV7_STATUS_COUNTER_INC (2)

/**
 * Indicates that the previous `unix_ts_ms` was incremented because the counter
 * reached its maximum value.
 */
#define UUIDV7_STATUS_TIMESTAMP_INC (3)

/**
 * Indicates that the monotonic order of generated UUIDs was broken because the
 * `unix_ts_ms` passed was less than the previous one by more than ten seconds.
 */
#define UUIDV7_STATUS_CLOCK_ROLLBACK (4)

/** Indicates that an invalid `unix_ts_ms` is passed. */
#define UUIDV7_STATUS_ERR_TIMESTAMP (-1)

/**
 * Indicates that the attempt to increment the previous `unix_ts_ms` failed
 * because it had reached its maximum value.
 */
#define UUIDV7_STATUS_ERR_TIMESTAMP_OVERFLOW (-2)

/** @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Low-level primitives
 *
 * @{
 */

/**
 * Generates a new UUIDv7 from the given Unix time, random bytes, and previous
 * UUID.
 *
 * @param uuid_out    16-byte byte array where the generated UUID is stored.
 * @param unix_ts_ms  Current Unix time in milliseconds.
 * @param rand_bytes  At least 10-byte byte array filled with random bytes. This
 *                    function consumes the leading 4 bytes or the whole 10
 *                    bytes per call depending on the conditions.
 *                    `uuidv7_status_n_rand_consumed()` maps the return value of
 *                    this function to the number of random bytes consumed.
 * @param uuid_prev   16-byte byte array representing the immediately preceding
 *                    UUID, from which the previous timestamp and counter are
 *                    extracted. This may be NULL if the caller does not care
 *                    the ascending order of UUIDs within the same timestamp.
 *                    This may point to the same location as `uuid_out`; this
 *                    function reads the value before writing.
 * @return            One of the `UUIDV7_STATUS_*` codes that describe the
 *                    characteristics of generated UUIDs. Callers can usually
 *                    ignore the status unless they need to guarantee the
 *                    monotonic order of UUIDs or fine-tune the generation
 *                    process.
 */
static inline int8_t uuidv7_generate(uint8_t *uuid_out, uint64_t unix_ts_ms,
                                     const uint8_t *rand_bytes,
                                     const uint8_t *uuid_prev) {
  static const uint64_t MAX_TIMESTAMP = ((uint64_t)1 << 48) - 1;
  static const uint64_t MAX_COUNTER = ((uint64_t)1 << 42) - 1;

  if (unix_ts_ms > MAX_TIMESTAMP) {
    return UUIDV7_STATUS_ERR_TIMESTAMP;
  }

  int8_t status;
  uint64_t timestamp = 0;
  if (uuid_prev == NULL) {
    status = UUIDV7_STATUS_UNPRECEDENTED;
    timestamp = unix_ts_ms;
  } else {
    for (int i = 0; i < 6; i++) {
      timestamp = (timestamp << 8) | uuid_prev[i];
    }

    if (unix_ts_ms > timestamp) {
      status = UUIDV7_STATUS_NEW_TIMESTAMP;
      timestamp = unix_ts_ms;
    } else if (unix_ts_ms + 10000 < timestamp) {
      // ignore prev if clock moves back by more than ten seconds
      status = UUIDV7_STATUS_CLOCK_ROLLBACK;
      timestamp = unix_ts_ms;
    } else {
      // increment prev counter
      uint64_t counter = uuid_prev[6] & 0x0f; // skip ver
      counter = (counter << 8) | uuid_prev[7];
      counter = (counter << 6) | (uuid_prev[8] & 0x3f); // skip var
      counter = (counter << 8) | uuid_prev[9];
      counter = (counter << 8) | uuid_prev[10];
      counter = (counter << 8) | uuid_prev[11];

      if (counter++ < MAX_COUNTER) {
        status = UUIDV7_STATUS_COUNTER_INC;
        uuid_out[6] = counter >> 38; // ver + bits 0-3
        uuid_out[7] = counter >> 30; // bits 4-11
        uuid_out[8] = counter >> 24; // var + bits 12-17
        uuid_out[9] = counter >> 16; // bits 18-25
        uuid_out[10] = counter >> 8; // bits 26-33
        uuid_out[11] = counter;      // bits 34-41
      } else {
        // increment prev timestamp at counter overflow
        status = UUIDV7_STATUS_TIMESTAMP_INC;
        timestamp++;
        if (timestamp > MAX_TIMESTAMP) {
          return UUIDV7_STATUS_ERR_TIMESTAMP_OVERFLOW;
        }
      }
    }
  }

  uuid_out[0] = timestamp >> 40;
  uuid_out[1] = timestamp >> 32;
  uuid_out[2] = timestamp >> 24;
  uuid_out[3] = timestamp >> 16;
  uuid_out[4] = timestamp >> 8;
  uuid_out[5] = timestamp;

  for (int i = (status == UUIDV7_STATUS_COUNTER_INC) ? 12 : 6; i < 16; i++) {
    uuid_out[i] = *rand_bytes++;
  }

  uuid_out[6] = 0x70 | (uuid_out[6] & 0x0f); // set ver
  uuid_out[8] = 0x80 | (uuid_out[8] & 0x3f); // set var

  return status;
}

/**
 * Determines the number of random bytes consumsed by `uuidv7_generate()` from
 * the `UUIDV7_STATUS_*` code returned.
 *
 * @param status  `UUIDV7_STATUS_*` code returned by `uuidv7_generate()`.
 * @return        `4` if `status` is `UUIDV7_STATUS_COUNTER_INC` or `10`
 *                otherwise.
 */
static inline int uuidv7_status_n_rand_consumed(int8_t status) {
  return status == UUIDV7_STATUS_COUNTER_INC ? 4 : 10;
}

/**
 * Encodes a UUID in the 8-4-4-4-12 hexadecimal string representation.
 *
 * @param uuid        16-byte byte array representing the UUID to encode.
 * @param string_out  Character array where the encoded string is stored. Its
 *                    length must be 37 (36 digits + NUL) or longer.
 */
static inline void uuidv7_to_string(const uint8_t *uuid, char *string_out) {
  static const char DIGITS[] = "0123456789abcdef";
  for (int i = 0; i < 16; i++) {
    uint_fast8_t e = uuid[i];
    *string_out++ = DIGITS[e >> 4];
    *string_out++ = DIGITS[e & 15];
    if (i == 3 || i == 5 || i == 7 || i == 9) {
      *string_out++ = '-';
    }
  }
  *string_out = '\0';
}

/**
 * Decodes the 8-4-4-4-12 hexadecimal string representation of a UUID.
 *
 * @param string    37-byte (36 digits + NUL) character array representing the
 *                  8-4-4-4-12 hexadecimal string representation.
 * @param uuid_out  16-byte byte array where the decoded UUID is stored.
 * @return          Zero on success or non-zero integer on failure.
 */
static inline int uuidv7_from_string(const char *string, uint8_t *uuid_out) {
  for (int i = 0; i < 32; i++) {
    char c = *string++;
    // clang-format off
    uint8_t x = c == '0' ?  0 : c == '1' ?  1 : c == '2' ?  2 : c == '3' ?  3
              : c == '4' ?  4 : c == '5' ?  5 : c == '6' ?  6 : c == '7' ?  7
              : c == '8' ?  8 : c == '9' ?  9 : c == 'a' ? 10 : c == 'b' ? 11
              : c == 'c' ? 12 : c == 'd' ? 13 : c == 'e' ? 14 : c == 'f' ? 15
              : c == 'A' ? 10 : c == 'B' ? 11 : c == 'C' ? 12 : c == 'D' ? 13
              : c == 'E' ? 14 : c == 'F' ? 15 : 0xff;
    // clang-format on
    if (x == 0xff) {
      return -1; // invalid digit
    }

    if ((i & 1) == 0) {
      uuid_out[i >> 1] = x << 4; // even i => hi 4 bits
    } else {
      uuid_out[i >> 1] |= x; // odd i => lo 4 bits
    }

    if ((i == 7 || i == 11 || i == 15 || i == 19) && (*string++ != '-')) {
      return -1; // invalid format
    }
  }
  if (*string != '\0') {
    return -1; // invalid length
  }
  return 0; // success
}

/** @} */

/**
 * @name High-level APIs that require platform integration
 *
 * @{
 */

/**
 * Generates a new UUIDv7 with the current Unix time.
 *
 * This declaration defines the interface to generate a new UUIDv7 with the
 * current time, default random number generator, and global shared state
 * holding the previously generated UUID. Since this single-file library does
 * not provide platform-specific implementations, users need to prepare a
 * concrete implementation (if necessary) by integrating a real-time clock,
 * cryptographically strong random number generator, and shared state storage
 * available in the target platform.
 *
 * @param uuid_out  16-byte byte array where the generated UUID is stored.
 * @return          One of the `UUIDV7_STATUS_*` codes that describe the
 *                  characteristics of generated UUIDs or an
 *                  implementation-dependent code. Callers can usually ignore
 *                  the `UUIDV7_STATUS_*` code unless they need to guarantee the
 *                  monotonic order of UUIDs or fine-tune the generation
 *                  process. The implementation-dependent code must be out of
 *                  the range of `int8_t` and negative if it reports an error.
 */
int uuidv7_new(uint8_t *uuid_out);

/**
 * Generates an 8-4-4-4-12 hexadecimal string representation of new UUIDv7.
 *
 * @param string_out  Character array where the encoded string is stored. Its
 *                    length must be 37 (36 digits + NUL) or longer.
 * @return            Return value of `uuidv7_new()`.
 * @note              Provide a concrete `uuidv7_new()` implementation to enable
 *                    this function.
 */
static inline int uuidv7_new_string(char *string_out) {
  uint8_t uuid[16];
  int result = uuidv7_new(uuid);
  uuidv7_to_string(uuid, string_out);
  return result;
}

/** @} */

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* #ifndef UUIDV7_H_BAEDKYFQ */
