<?php

/** @generate-function-entries */

final class Sysvsem
{
}

/**
 * @todo use bool for $auto_release
 */
function sem_get(int $key, int $max_acquire = 1, int $perm = 0666, int $auto_release = 1): Sysvsem|false {}

function sem_acquire(Sysvsem $sem_identifier, bool $nowait = false): bool {}

function sem_release(Sysvsem $sem_identifier): bool {}

function sem_remove(Sysvsem $sem_identifier): bool {}
