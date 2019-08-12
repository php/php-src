<?php

/**
 * @todo use bool for $auto_release
 * @return resource|false
 */
function sem_get(int $key, int $max_acquire = 1, int $perm = 0666, int $auto_release = 1) {}

/**
 * @param resource $sem_identifier
 */
function sem_acquire($sem_identifier, bool $nowait = false): bool {}

/**
 * @param resource $sem_identifier
 */
function sem_release($sem_identifier): bool {}

/**
 * @param resource $sem_identifier
 */
function sem_remove($sem_identifier): bool {}
