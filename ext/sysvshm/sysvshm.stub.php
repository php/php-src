<?php

/** @generate-function-entries */

final class SysvSharedMemory
{
}

function shm_attach(int $key, int $memsize = UNKNOWN, int $perm = 0666): SysvSharedMemory|false {}

function shm_detach(SysvSharedMemory $shm): bool {}

function shm_has_var(SysvSharedMemory $shm, int $variable_key): bool {}

function shm_remove(SysvSharedMemory $shm): bool {}

function shm_put_var(SysvSharedMemory $shm, int $variable_key, $variable): bool {}

/** @return mixed */
function shm_get_var(SysvSharedMemory $shm, int $variable_key) {}

function shm_remove_var(SysvSharedMemory $shm, int $variable_key): bool {}
