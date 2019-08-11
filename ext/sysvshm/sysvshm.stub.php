<?php

/** @return resource|false */
function shm_attach(int $key, int $memsize = 10000, int $perm = 0666) {}

/**
 * @param resource $shm_identifier
 */
function shm_detach($shm_identifier): bool {}

/**
 * @param resource $id
 */
function shm_has_var($id, int $variable_key): bool {}

/**
 * @param resource $shm_identifier
 */
function shm_remove($shm_identifier): bool {}

/**
 * @param resource $shm_identifier
 */
function shm_put_var($shm_identifier, int $variable_key, $variable): bool {}

/**
 * @param resource $id
 */
function shm_get_var($id, int $variable_key) {}

/**
 * @param resource $id
 */
function shm_remove_var($id, int $variable_key): bool {}
