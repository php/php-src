<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_HASH_HMAC
 */
const HASH_HMAC = UNKNOWN;

/** @refcount 1 */
function hash(string $algo, string $data, bool $binary = false, array $options = []): string {}

/** @refcount 1 */
function hash_file(string $algo, string $filename, bool $binary = false, array $options = []): string|false {}

/**
 * @refcount 1
 */
function hash_hmac(string $algo, string $data, #[\SensitiveParameter] string $key, bool $binary = false): string {}

/**
 * @refcount 1
 */
function hash_hmac_file(string $algo, string $filename, #[\SensitiveParameter] string $key, bool $binary = false): string|false {}

/**
 * @refcount 1
 */
function hash_init(string $algo, int $flags = 0, #[\SensitiveParameter] string $key = "", array $options = []): HashContext {}

function hash_update(HashContext $context, string $data): bool {}

/** @param resource $stream */
function hash_update_stream(HashContext $context, $stream, int $length = -1): int {}

/** @param resource|null $stream_context */
function hash_update_file(HashContext $context, string $filename, $stream_context = null): bool {}

/** @refcount 1 */
function hash_final(HashContext $context, bool $binary = false): string {}

/** @refcount 1 */
function hash_copy(HashContext $context): HashContext {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function hash_algos(): array {}

/**
 * @return array<int, string>
 * @refcount 1
 */
function hash_hmac_algos(): array {}

/**
 * @refcount 1
 */
function hash_pbkdf2(string $algo, #[\SensitiveParameter] string $password, string $salt, int $iterations, int $length = 0, bool $binary = false, array $options = []): string {}

function hash_equals(#[\SensitiveParameter] string $known_string, #[\SensitiveParameter] string $user_string): bool {}

/**
 * @refcount 1
 */
function hash_hkdf(string $algo, #[\SensitiveParameter] string $key, int $length = 0, string $info = "", string $salt = ""): string {}

#ifdef PHP_MHASH_BC
/** @deprecated */
function mhash_get_block_size(int $algo): int|false {}

/**
 * @refcount 1
 * @deprecated
 */
function mhash_get_hash_name(int $algo): string|false {}

/**
 * @refcount 1
 * @deprecated
 */
function mhash_keygen_s2k(int $algo, string $password, string $salt, int $length): string|false {}

/** @deprecated */
function mhash_count(): int {}

/**
 * @refcount 1
 * @deprecated
 */
function mhash(int $algo, string $data, ?string $key = null): string|false {}
#endif

final class HashContext
{
    private function __construct() {}

    public function __serialize(): array {}

    public function __unserialize(array $data): void {}
}
