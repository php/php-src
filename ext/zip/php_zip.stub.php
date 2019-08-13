<?php

/** @return resource|int|false */
function zip_open(string $filename) {}

/**
 * @param resource $zip
 */
function zip_close($zip): void {}

/**
 * @param resource $zip
 * @return resource|false
 */
function zip_read($zip) {}

/**
 * @param resource $zip_dp
 * @param resource $zip_entry
 */
function zip_entry_open($zip_dp, $zip_entry, string $mode = 'rb'): bool {}

/**
 * @param resource $zip_ent
 */
function zip_entry_close($zip_ent): bool {}

/**
 * @param resource $zip_entry
 * @return string|false
 */
function zip_entry_read($zip_entry, int $len = 1024) {}

/**
 * @param resource $zip_entry
 * @return string|false
 */
function zip_entry_name($zip_entry) {}

/**
 * @param resource $zip_entry
 * @return int|false
 */
function zip_entry_compressedsize($zip_entry) {}

/**
 * @param resource $zip_entry
 * @return int|false
 */
function zip_entry_filesize($zip_entry) {}

/**
 * @param resource $zip_entry
 * @return string|false
 */
function zip_entry_compressionmethod($zip_entry) {}
