<?php

/** @return resource|false */
function shmop_open(int $key, string $flags, int $mode, int $size) {}

/**
 * @param resource $shmid
 * @return string|false
 */
function shmop_read($shmid, int $start, int $count) {}

/**
 * @param resource $shmid
 */
function shmop_close($shmid): void {}

/**
 * @param resource $shmid
 */
function shmop_size($shmid): int {}

/**
 * @param resource $shmid
 * @return int|false
 */
function shmop_write($shmid, string $data, int $offset) {}

/**
 * @param resource $shmid
 */
function shmop_delete($shmid): bool {}
