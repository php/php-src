<?php

function hash(string $algo, string $data, bool $raw_output = false): string|false {}

function hash_file(string $algo, string $filename, bool $raw_output = false): string|false {}

function hash_hmac(string $algo, string $data, string $key, bool $raw_output = false): string|false {}

function hash_hmac_file(string $algo, string $data, string $key, bool $raw_output = false): string|false {}

function hash_init(string $algo, int $options = 0, string $key = UNKNOWN): HashContext {}

function hash_update(HashContext $context, string $data): bool {}

/** @param resource $handle */
function hash_update_stream(HashContext $context, $handle, int $length = -1): int {}

/** @param resource $stream_context */
function hash_update_file(HashContext $context, string $filename, $stream_context = UNKNOWN): bool {}

function hash_final(HashContext $context, bool $raw_output = false): string {}

function hash_copy(HashContext $context): HashContext {}

function hash_algos(): array {}

function hash_pbkdf2(string $algo, string $password, string $salt, int $iterations, int $length = 0, bool $raw_output = false): string {}

/**
 * @param $known_string no type juggling is performed
 * @param $user_string no type juggling is performed
 */
function hash_equals(string $known_string, string $user_string) : bool {}

function hash_hkdf(string $algo, string $ikm, int $length = 0, string $info = '', string $salt = ''): string {}

#ifdef PHP_MHASH_BC
function mhash_get_block_size(int $hash): int|false {}

function mhash_get_hash_name(int $hash): string|false {}

function mhash_keygen_s2k(int $hash, string $input_password, string $salt, int $bytes): string|false {}

function mhash_count(): int {}

function mhash(int $hash, string $data, string $key = UNKNOWN): string|false {}
#endif

class HashContext {
    private function __construct() {}
}
