<?php

/** @generate-function-entries */

final class SysvSharedMemory
{
}

function shm_attach(int $key, ?int $size = null, int $permissions = 0666): SysvSharedMemory|false {}

function shm_detach(SysvSharedMemory $shm): bool {}

function shm_has_var(SysvSharedMemory $shm, int $key): bool {}

function shm_remove(SysvSharedMemory $shm): bool {}

function shm_put_var(SysvSharedMemory $shm, int $key, mixed $value): bool {}

function shm_get_var(SysvSharedMemory $shm, int $key): mixed {}

function shm_remove_var(SysvSharedMemory $shm, int $key): bool {}
