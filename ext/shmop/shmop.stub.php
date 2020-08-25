<?php

/** @generate-function-entries */

final class Shmop {}

function shmop_open(int $key, string $flags, int $mode, int $size): Shmop|false {}

function shmop_read(Shmop $shmid, int $start, int $count): string|false {}

/** @deprecated */
function shmop_close(Shmop $shmid): void {}

function shmop_size(Shmop $shmid): int {}

function shmop_write(Shmop $shmid, string $data, int $offset): int|false {}

function shmop_delete(Shmop $shmid): bool {}
