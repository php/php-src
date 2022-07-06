<?php

/** @generate-function-entries */

function hash(string $algo, string $data, bool $binary = false): string {}

function hash_file(string $algo, string $filename, bool $binary = false): string|false {}

function hash_hmac(string $algo, string $data, string $key, bool $binary = false): string {}

function hash_hmac_file(string $algo, string $filename, string $key, bool $binary = false): string|false {}

function hash_init(string $algo, int $flags = 0, string $key = ""): HashContext {}

function hash_update(HashContext $context, string $data): bool {}

/** @param resource $stream */
function hash_update_stream(HashContext $context, $stream, int $length = -1): int {}

/** @param resource|null $stream_context */
function hash_update_file(HashContext $context, string $filename, $stream_context = null): bool {}

function hash_final(HashContext $context, bool $binary = false): string {}

function hash_copy(HashContext $context): HashContext {}

function hash_algos(): array {}

function hash_hmac_algos(): array {}

function hash_pbkdf2(string $algo, string $password, string $salt, int $iterations, int $length = 0, bool $binary = false): string {}

function hash_equals(string $known_string, string $user_string): bool {}

function hash_hkdf(string $algo, string $key, int $length = 0, string $info = "", string $salt = ""): string {}

#ifdef PHP_MHASH_BC
function mhash_get_block_size(int $algo): int|false {}

function mhash_get_hash_name(int $algo): string|false {}

function mhash_keygen_s2k(int $algo, string $password, string $salt, int $length): string|false {}

function mhash_count(): int {}

function mhash(int $algo, string $data, ?string $key = null): string|false {}
#endif

final class HashContext
{
    private function __construct() {}

    public function __serialize(): array {}

    public function __unserialize(array $data): void {}
}
