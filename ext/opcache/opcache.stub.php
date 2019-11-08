<?php

function opcache_reset(): bool {}

function opcache_get_status(bool $fetch_scripts = true): array|false {}

function opcache_compile_file(string $file): bool {}

function opcache_invalidate(string $script, bool $force = false): bool {}

function opcache_get_configuration(): array|false {}

function opcache_is_script_cached(string $script): bool {}
