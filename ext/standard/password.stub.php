<?php

/** @generate-class-entries */

/**
 * @var string
 */
const PASSWORD_DEFAULT = "2y";
/**
 * @var string
 */
const PASSWORD_BCRYPT = "2y";
/**
 * @var int
 * @cvalue PHP_PASSWORD_BCRYPT_COST
 */
const PASSWORD_BCRYPT_DEFAULT_COST = UNKNOWN;

#ifdef HAVE_ARGON2LIB
/**
 * @var string
 */
const PASSWORD_ARGON2I = "argon2i";
/**
 * @var string
 */
const PASSWORD_ARGON2ID = "argon2id";
/**
 * @var string
 */
const PASSWORD_ARGON2_PROVIDER = "standard";
/**
 * @var int
 * @cvalue PHP_PASSWORD_ARGON2_MEMORY_COST
 */
const PASSWORD_ARGON2_DEFAULT_MEMORY_COST = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_PASSWORD_ARGON2_TIME_COST
 */
const PASSWORD_ARGON2_DEFAULT_TIME_COST = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_PASSWORD_ARGON2_THREADS
 */
const PASSWORD_ARGON2_DEFAULT_THREADS = UNKNOWN;
#endif
