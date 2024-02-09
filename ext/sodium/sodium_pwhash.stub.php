<?php

/** @generate-class-entries */

#if !defined(HAVE_ARGON2LIB) && (SODIUM_LIBRARY_VERSION_MAJOR > 9 || (SODIUM_LIBRARY_VERSION_MAJOR == 9 && SODIUM_LIBRARY_VERSION_MINOR >= 6))
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
 * @cvalue PHP_SODIUM_PWHASH_MEMLIMIT
 */
const PASSWORD_ARGON2_DEFAULT_MEMORY_COST = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_SODIUM_PWHASH_OPSLIMIT
 */
const PASSWORD_ARGON2_DEFAULT_TIME_COST = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_SODIUM_PWHASH_THREADS
 */
const PASSWORD_ARGON2_DEFAULT_THREADS = UNKNOWN;
/**
 * @var string
 */
const PASSWORD_ARGON2_PROVIDER = "sodium";
#endif
