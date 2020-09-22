<?php

/** @generate-function-entries */

function hash(string $algorithm, string $data, bool $binary_output = false): string|false {}

function hash_file(string $algorithm, string $filename, bool $binary_output = false): string|false {}

function hash_hmac(string $algorithm, string $data, string $key, bool $binary_output = false): string|false {}

function hash_hmac_file(string $algorithm, string $data, string $key, bool $binary_output = false): string|false {}

function hash_init(string $algorithm, int $options = 0, string $key = ""): HashContext {}

function hash_update(HashContext $context, string $data): bool {}

/** @param resource $handle */
function hash_update_stream(HashContext $context, $handle, int $length = -1): int {}

/** @param resource|null $stream_context */
function hash_update_file(HashContext $context, string $filename, $stream_context = null): bool {}

function hash_final(HashContext $context, bool $binary_output = false): string {}

function hash_copy(HashContext $context): HashContext {}

function hash_algos(): array {}

function hash_hmac_algos(): array {}

function hash_pbkdf2(string $algorithm, string $password, string $salt, int $iterations, int $length = 0, bool $binary_output = false): string {}

function hash_equals(string $known_string, string $user_string): bool {}

function hash_hkdf(string $algorithm, string $key, int $length = 0, string $info = "", string $salt = ""): string {}

#ifdef PHP_MHASH_BC
function mhash_get_block_size(int $algorithm): int|false {}

function mhash_get_hash_name(int $algorithm): string|false {}

function mhash_keygen_s2k(int $algorithm, string $password, string $salt, int $bytes): string|false {}

function mhash_count(): int {}

function mhash(int $algorithm, string $data, ?string $key = null): string|false {}
#endif

final class HashContext
{
    private function __construct() {}

    public function __serialize(): array {}

    public function __unserialize(array $serialized): void {}
}
