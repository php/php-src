<?php

/** @generate-class-entries */

function opcache_reset(): bool {}

/**
 * @return array<string, mixed>|false
 * @refcount 1
 */
function opcache_get_status(bool $include_scripts = true): array|false {}

function opcache_compile_file(string $filename): bool {}

function opcache_invalidate(string $filename, bool $force = false): bool {}

/**
 * @return array<string, mixed>|false
 * @refcount 1
 */
function opcache_get_configuration(): array|false {}

function opcache_is_script_cached(string $filename): bool {}
