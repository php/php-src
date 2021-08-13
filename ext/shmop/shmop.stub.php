<?php

/** @generate-class-entries */

/**
 * @strict-properties
 * @not-serializable
 */
final class Shmop {}

function shmop_open(int $key, string $mode, int $permissions, int $size): Shmop|false {}

function shmop_read(Shmop $shmop, int $offset, int $size): string {}

/** @deprecated */
function shmop_close(Shmop $shmop): void {}

function shmop_size(Shmop $shmop): int {}

function shmop_write(Shmop $shmop, string $data, int $offset): int {}

function shmop_delete(Shmop $shmop): bool {}
