/*-
 * Copyright 2009 Colin Percival
 * Copyright 2013-2018 Alexander Peslyak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file was originally written by Colin Percival as part of the Tarsnap
 * online backup system.
 */
#ifndef _YESCRYPT_H_
#define _YESCRYPT_H_

#include <stdint.h>
#include <stdlib.h> /* for size_t */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * crypto_scrypt(passwd, passwdlen, salt, saltlen, N, r, p, buf, buflen):
 * Compute scrypt(passwd[0 .. passwdlen - 1], salt[0 .. saltlen - 1], N, r,
 * p, buflen) and write the result into buf.  The parameters r, p, and buflen
 * must satisfy r * p < 2^30 and buflen <= (2^32 - 1) * 32.  The parameter N
 * must be a power of 2 greater than 1.
 *
 * Return 0 on success; or -1 on error.
 *
 * MT-safe as long as buf is local to the thread.
 */
extern int crypto_scrypt(const uint8_t *passwd, size_t passwdlen,
    const uint8_t *salt, size_t saltlen,
    uint64_t N, uint32_t r, uint32_t p, uint8_t *buf, size_t buflen);

/**
 * Internal type used by the memory allocator.  Please do not use it directly.
 * Use yescrypt_shared_t and yescrypt_local_t as appropriate instead, since
 * they might differ from each other in a future version.
 */
typedef struct {
	void *base, *aligned;
	size_t base_size, aligned_size;
} yescrypt_region_t;

/**
 * Types for shared (ROM) and thread-local (RAM) data structures.
 */
typedef yescrypt_region_t yescrypt_shared_t;
typedef yescrypt_region_t yescrypt_local_t;

/**
 * Two 64-bit tags placed 48 bytes to the end of a ROM in host byte endianness
 * (and followed by 32 bytes of the ROM digest).
 */
#define YESCRYPT_ROM_TAG1 0x7470797263736579ULL /* "yescrypt" */
#define YESCRYPT_ROM_TAG2 0x687361684d4f522dULL /* "-ROMhash" */

/**
 * Type and possible values for the flags argument of yescrypt_kdf(),
 * yescrypt_encode_params_r(), yescrypt_encode_params().  Most of these may be
 * OR'ed together, except that YESCRYPT_WORM stands on its own.
 * Please refer to the description of yescrypt_kdf() below for the meaning of
 * these flags.
 */
typedef uint32_t yescrypt_flags_t;
/* Public */
#define YESCRYPT_WORM			1
#define YESCRYPT_RW			0x002
#define YESCRYPT_ROUNDS_3		0x000
#define YESCRYPT_ROUNDS_6		0x004
#define YESCRYPT_GATHER_1		0x000
#define YESCRYPT_GATHER_2		0x008
#define YESCRYPT_GATHER_4		0x010
#define YESCRYPT_GATHER_8		0x018
#define YESCRYPT_SIMPLE_1		0x000
#define YESCRYPT_SIMPLE_2		0x020
#define YESCRYPT_SIMPLE_4		0x040
#define YESCRYPT_SIMPLE_8		0x060
#define YESCRYPT_SBOX_6K		0x000
#define YESCRYPT_SBOX_12K		0x080
#define YESCRYPT_SBOX_24K		0x100
#define YESCRYPT_SBOX_48K		0x180
#define YESCRYPT_SBOX_96K		0x200
#define YESCRYPT_SBOX_192K		0x280
#define YESCRYPT_SBOX_384K		0x300
#define YESCRYPT_SBOX_768K		0x380
/* Only valid for yescrypt_init_shared() */
#define YESCRYPT_SHARED_PREALLOCATED	0x10000
#ifdef YESCRYPT_INTERNAL
/* Private */
#define YESCRYPT_MODE_MASK		0x003
#define YESCRYPT_RW_FLAVOR_MASK		0x3fc
#define YESCRYPT_INIT_SHARED		0x01000000
#define YESCRYPT_ALLOC_ONLY		0x08000000
#define YESCRYPT_PREHASH		0x10000000
#endif

#define YESCRYPT_RW_DEFAULTS \
	(YESCRYPT_RW | \
	YESCRYPT_ROUNDS_6 | YESCRYPT_GATHER_4 | YESCRYPT_SIMPLE_2 | \
	YESCRYPT_SBOX_12K)

#define YESCRYPT_DEFAULTS YESCRYPT_RW_DEFAULTS

#ifdef YESCRYPT_INTERNAL
#define YESCRYPT_KNOWN_FLAGS \
	(YESCRYPT_MODE_MASK | YESCRYPT_RW_FLAVOR_MASK | \
	YESCRYPT_SHARED_PREALLOCATED | \
	YESCRYPT_INIT_SHARED | YESCRYPT_ALLOC_ONLY | YESCRYPT_PREHASH)
#endif

/**
 * yescrypt parameters combined into one struct.  N, r, p are the same as in
 * classic scrypt, except that the meaning of p changes when YESCRYPT_RW is
 * set.  flags, t, g, NROM are special to yescrypt.
 */
typedef struct {
	yescrypt_flags_t flags;
	uint64_t N;
	uint32_t r, p, t, g;
	uint64_t NROM;
} yescrypt_params_t;

/**
 * A 256-bit yescrypt hash, or a hash encryption key (which may itself have
 * been derived as a yescrypt hash of a human-specified key string).
 */
typedef union {
	unsigned char uc[32];
	uint64_t u64[4];
} yescrypt_binary_t;

/**
 * yescrypt_init_shared(shared, seed, seedlen, params):
 * Optionally allocate memory for and initialize the shared (ROM) data
 * structure.  The parameters flags, NROM, r, p, and t specify how the ROM is
 * to be initialized, and seed and seedlen specify the initial seed affecting
 * the data with which the ROM is filled.
 *
 * Return 0 on success; or -1 on error.
 *
 * If bit YESCRYPT_SHARED_PREALLOCATED in flags is set, then memory for the
 * ROM is assumed to have been preallocated by the caller, with shared->aligned
 * being the start address of the ROM and shared->aligned_size being its size
 * (which must be sufficient for NROM, r, p).  This may be used e.g. when the
 * ROM is to be placed in a SysV shared memory segment allocated by the caller.
 *
 * MT-safe as long as shared is local to the thread.
 */
extern int yescrypt_init_shared(yescrypt_shared_t *shared,
    const uint8_t *seed, size_t seedlen, const yescrypt_params_t *params);

/**
 * yescrypt_digest_shared(shared):
 * Extract the previously stored message digest of the provided yescrypt ROM.
 *
 * Return pointer to the message digest on success; or NULL on error.
 *
 * MT-unsafe.
 */
extern yescrypt_binary_t *yescrypt_digest_shared(yescrypt_shared_t *shared);

/**
 * yescrypt_free_shared(shared):
 * Free memory that had been allocated with yescrypt_init_shared().
 *
 * Return 0 on success; or -1 on error.
 *
 * MT-safe as long as shared is local to the thread.
 */
extern int yescrypt_free_shared(yescrypt_shared_t *shared);

/**
 * yescrypt_init_local(local):
 * Initialize the thread-local (RAM) data structure.  Actual memory allocation
 * is currently fully postponed until a call to yescrypt_kdf() or yescrypt_r().
 *
 * Return 0 on success; or -1 on error.
 *
 * MT-safe as long as local is local to the thread.
 */
extern int yescrypt_init_local(yescrypt_local_t *local);

/**
 * yescrypt_free_local(local):
 * Free memory that may have been allocated for an initialized thread-local
 * (RAM) data structure.
 *
 * Return 0 on success; or -1 on error.
 *
 * MT-safe as long as local is local to the thread.
 */
extern int yescrypt_free_local(yescrypt_local_t *local);

/**
 * yescrypt_kdf(shared, local, passwd, passwdlen, salt, saltlen, params,
 *     buf, buflen):
 * Compute scrypt(passwd[0 .. passwdlen - 1], salt[0 .. saltlen - 1], N, r,
 * p, buflen), or a revision of scrypt as requested by flags and shared, and
 * write the result into buf.  The parameters N, r, p, and buflen must satisfy
 * the same conditions as with crypto_scrypt().  t controls computation time
 * while not affecting peak memory usage (t = 0 is optimal unless higher N*r
 * is not affordable while higher t is).  g controls hash upgrades (g = 0 for
 * no upgrades so far).  shared and flags may request special modes.  local is
 * the thread-local data structure, allowing to preserve and reuse a memory
 * allocation across calls, thereby reducing processing overhead.
 *
 * Return 0 on success; or -1 on error.
 *
 * Classic scrypt is available by setting shared = NULL, flags = 0, and t = 0.
 *
 * Setting YESCRYPT_WORM enables only minimal deviations from classic scrypt:
 * support for the t parameter, and pre- and post-hashing.
 *
 * Setting YESCRYPT_RW fully enables yescrypt.  As a side effect of differences
 * between the algorithms, it also prevents p > 1 from growing the threads'
 * combined processing time and memory allocation (like it did with classic
 * scrypt and YESCRYPT_WORM), treating p as a divider rather than a multiplier.
 *
 * Passing a shared structure, with ROM contents previously computed by
 * yescrypt_init_shared(), enables the use of ROM and requires YESCRYPT_RW.
 *
 * In order to allow for initialization of the ROM to be split into a separate
 * program (or separate invocation of the same program), the shared->aligned
 * and shared->aligned_size fields may optionally be set by the caller directly
 * (e.g., to a mapped SysV shm segment), without using yescrypt_init_shared().
 *
 * local must be initialized with yescrypt_init_local().
 *
 * MT-safe as long as local and buf are local to the thread.
 */
extern int yescrypt_kdf(const yescrypt_shared_t *shared,
    yescrypt_local_t *local,
    const uint8_t *passwd, size_t passwdlen,
    const uint8_t *salt, size_t saltlen,
    const yescrypt_params_t *params,
    uint8_t *buf, size_t buflen);

/**
 * yescrypt_r(shared, local, passwd, passwdlen, setting, key, buf, buflen):
 * Compute and encode an scrypt or enhanced scrypt hash of passwd given the
 * parameters and salt value encoded in setting.  If shared is not NULL, a ROM
 * is used and YESCRYPT_RW is required.  Otherwise, whether to compute classic
 * scrypt, YESCRYPT_WORM (a slight deviation from classic scrypt), or
 * YESCRYPT_RW (time-memory tradeoff discouraging modification) is determined
 * by the setting string.  shared (if not NULL) and local must be initialized
 * as described above for yescrypt_kdf().  buf must be large enough (as
 * indicated by buflen) to hold the encoded hash string.
 *
 * Return the encoded hash string on success; or NULL on error.
 *
 * MT-safe as long as local and buf are local to the thread.
 */
extern uint8_t *yescrypt_r(const yescrypt_shared_t *shared,
    yescrypt_local_t *local,
    const uint8_t *passwd, size_t passwdlen,
    const uint8_t *setting,
    const yescrypt_binary_t *key,
    uint8_t *buf, size_t buflen);

/**
 * yescrypt(passwd, setting):
 * Compute and encode an scrypt or enhanced scrypt hash of passwd given the
 * parameters and salt value encoded in setting.  Whether to compute classic
 * scrypt, YESCRYPT_WORM (a slight deviation from classic scrypt), or
 * YESCRYPT_RW (time-memory tradeoff discouraging modification) is determined
 * by the setting string.
 *
 * Return the encoded hash string on success; or NULL on error.
 *
 * This is a crypt(3)-like interface, which is simpler to use than
 * yescrypt_r(), but it is not MT-safe, it does not allow for the use of a ROM,
 * and it is slower than yescrypt_r() for repeated calls because it allocates
 * and frees memory on each call.
 *
 * MT-unsafe.
 */
extern uint8_t *yescrypt(const uint8_t *passwd, const uint8_t *setting);

/**
 * yescrypt_reencrypt(hash, from_key, to_key):
 * Re-encrypt a yescrypt hash from one key to another.  Either key may be NULL
 * to indicate unencrypted hash.  The encoded hash string is modified in-place.
 *
 * Return the hash pointer on success; or NULL on error (in which case the hash
 * string is left unmodified).
 *
 * MT-safe as long as hash is local to the thread.
 */
extern uint8_t *yescrypt_reencrypt(uint8_t *hash,
    const yescrypt_binary_t *from_key,
    const yescrypt_binary_t *to_key);

/**
 * yescrypt_encode_params_r(params, src, srclen, buf, buflen):
 * Generate a setting string for use with yescrypt_r() and yescrypt() by
 * encoding into it the parameters flags, N, r, p, t, g, and a salt given by
 * src (of srclen bytes).  buf must be large enough (as indicated by buflen)
 * to hold the setting string.
 *
 * Return the setting string on success; or NULL on error.
 *
 * MT-safe as long as buf is local to the thread.
 */
extern uint8_t *yescrypt_encode_params_r(const yescrypt_params_t *params,
    const uint8_t *src, size_t srclen,
    uint8_t *buf, size_t buflen);

/**
 * yescrypt_encode_params(params, src, srclen):
 * Generate a setting string for use with yescrypt_r() and yescrypt().  This
 * function is the same as yescrypt_encode_params_r() except that it uses a
 * static buffer and thus is not MT-safe.
 *
 * Return the setting string on success; or NULL on error.
 *
 * MT-unsafe.
 */
extern uint8_t *yescrypt_encode_params(const yescrypt_params_t *params,
    const uint8_t *src, size_t srclen);

/* PHP changes: expose some macros and functions */
const uint8_t *yescrypt_parse_settings(const uint8_t *setting, yescrypt_params_t *params, const yescrypt_binary_t *key);

const uint8_t *yescrypt_decode64(uint8_t *dst, size_t *dstlen,
								 const uint8_t *src, size_t srclen);

uint8_t *yescrypt_encode64_uint32_fixed(uint8_t *dst, size_t dstlen,
										uint32_t src, uint32_t srcbits);

#define BYTES2CHARS(bytes) ((((bytes) * 8) + 5) / 6)

#define HASH_SIZE sizeof(yescrypt_binary_t) /* bytes */
#define HASH_LEN BYTES2CHARS(HASH_SIZE) /* base-64 chars */

/*
 * "$y$", up to 8 params of up to 6 chars each, '$', salt
 * Alternatively, but that's smaller:
 * "$7$", 3 params encoded as 1+5+5 chars, salt
 */
#define PREFIX_LEN (3 + 8 * 6 + 1 + BYTES2CHARS(32))

#ifdef __cplusplus
}
#endif

#endif /* !_YESCRYPT_H_ */
