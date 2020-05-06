<?php

/** @generate-function-entries */

final class Sysvshm
{
}

/** @return Sysvshm|false */
function shm_attach(int $key, int $memsize = UNKNOWN, int $perm = 0666): Sysvshm|false {}

function shm_detach(Sysvshm $shm): bool {}

function shm_has_var(Sysvshm $shm, int $variable_key): bool {}

function shm_remove(Sysvshm $shm): bool {}

function shm_put_var(Sysvshm $shm, int $variable_key, $variable): bool {}

/** @return mixed */
function shm_get_var(Sysvshm $shm, int $variable_key) {}

function shm_remove_var(Sysvshm $shm, int $variable_key): bool {}
