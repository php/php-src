<?php

/** @generate-class-entries */

#if PHP_OPENSSL_API_VERSION >= 0x30200
/**
 * @var string
 */
const PASSWORD_ARGON2I = "argon2i";
/**
 * @var string
 */
const PASSWORD_ARGON2ID = "argon2id";
/**
 * @var int
 * @cvalue PHP_OPENSSL_PWHASH_MEMLIMIT
 */
const PASSWORD_ARGON2_DEFAULT_MEMORY_COST = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OPENSSL_PWHASH_OPSLIMIT
 */
const PASSWORD_ARGON2_DEFAULT_TIME_COST = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_OPENSSL_PWHASH_THREADS
 */
const PASSWORD_ARGON2_DEFAULT_THREADS = UNKNOWN;
/**
 * @var string
 */
const PASSWORD_ARGON2_PROVIDER = "openssl";

function openssl_password_hash(string $algo, #[\SensitiveParameter] string $password, array $options = []): string {}
function openssl_password_verify(string $algo, #[\SensitiveParameter] string $password, string $hash): bool {}
#endif

