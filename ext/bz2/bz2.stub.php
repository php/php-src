<?php

/**
 * @param string|resource $file
 * @return resource|false
 */
 function bzopen($file, string $mode) {}

/**
 * @param resource $bz
 * @return string|false
 */
function bzread ($bz, int $length = 1024) {}

/**
 * @param resource $bz
 * @return int|false
 */
function bzwrite ($bz, string $str, int $length = UNKNOWN) {}

/**
 * @param resource $bz
 */
function bzflush ($bz): bool {}

/**
 * @param resource $bz
 */
function bzclose ($bz): bool {}

/**
 * @param resource $bz
 */
function bzerrno ($bz): int {}

/**
 * @param resource $bz
 */
function bzerrstr ($bz): string {}

/**
 * @param resource $bz
 */
function bzerror ($bz): array {}

/**
 * @return string|int
 */
function bzcompress (string $source, int $blocksize = 4, int $workfactor = 0) {}

/**
 * @return string|int
 */
function bzdecompress (string $source, int $small = 0) {}
