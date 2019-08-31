<?php

function opcache_reset(): bool {}

/** @return array|false */
function opcache_get_status(bool $fetch_scripts = true) {}

function opcache_compile_file(string $file): bool {}

function opcache_invalidate(string $script, bool $force = false): bool {}

/** @return array|false */
function opcache_get_configuration() {}

function opcache_is_script_cached(string $script): bool {}
