<?php

/** @generate-function-entries */

final class SysvSemaphore
{
}

function sem_get(int $key, int $max_acquire = 1, int $perm = 0666, bool $auto_release = true): SysvSemaphore|false {}

function sem_acquire(SysvSemaphore $semaphore, bool $nowait = false): bool {}

function sem_release(SysvSemaphore $semaphore): bool {}

function sem_remove(SysvSemaphore $semaphore): bool {}
