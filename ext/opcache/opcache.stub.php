<?php

/** @generate-class-entries */

namespace {

function opcache_reset(): bool {}

/**
 * @return array<string, mixed>|false
 * @refcount 1
 */
function opcache_get_status(bool $include_scripts = true): array|false {}

function opcache_compile_file(string $filename): bool {}

function opcache_invalidate(string $filename, bool $force = false): bool {}

function opcache_jit_blacklist(Closure $closure): void {}

/**
 * @return array<string, mixed>|false
 * @refcount 1
 */
function opcache_get_configuration(): array|false {}

function opcache_is_script_cached(string $filename): bool {}

function opcache_is_script_cached_in_file_cache(string $filename): bool {}

}

namespace OPcache {

class StaticCacheException extends \Exception
{
}

/** @strict-properties */
final readonly class StaticCacheInfo
{
	private function __construct() {}

	public bool $enabled;

	public bool $available;

	public bool $startup_failed;

	public bool $backend_initialized;

	public int $configured_memory;

	public int $shared_memory;

	public int $entry_count;

	public int $segment_count;

	public string $shared_model;

	public ?string $failure_reason;
}

#[\Attribute(13)] /* TARGET_CLASS | TARGET_METHOD | TARGET_PROPERTY */
final class PinnedStatic
{
}

enum CacheStrategy: int
{
	case Immediate = 0;
	case Tracking = 1;
}

#[\Attribute(13)] /* TARGET_CLASS | TARGET_METHOD | TARGET_PROPERTY */
final class VolatileStatic
{
	public readonly int $ttl;

	public readonly CacheStrategy $strategy;

	public function __construct(int $ttl = 0, CacheStrategy $strategy = CacheStrategy::Immediate) {}
}

function volatile_store(string $key, null|bool|int|float|string|array|object $value, int $ttl = 0): bool {}

function volatile_store_array(array $values, int $ttl = 0): bool {}

function volatile_fetch(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object {}

/**
 * @return array<string, null|bool|int|float|string|array|object>|null
 */
function volatile_fetch_array(array $keys, ?array $default = null): ?array {}

function volatile_exists(string $key): bool {}

function volatile_lock(string $key, int $lease = 0): bool {}

function volatile_unlock(string $key): bool {}

function volatile_delete(string $key_or_class): void {}

function volatile_delete_array(array $keys): void {}

function volatile_clear(): void {}

function volatile_cache_info(): StaticCacheInfo {}

function pinned_store(string $key, null|bool|int|float|string|array|object $value): bool {}

function pinned_store_array(array $values): bool {}

function pinned_fetch(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object {}

/**
 * @return array<string, null|bool|int|float|string|array|object>|null
 */
function pinned_fetch_array(array $keys, ?array $default = null): ?array {}

function pinned_exists(string $key): bool {}

function pinned_lock(string $key, int $lease = 0): bool {}

function pinned_unlock(string $key): bool {}

function pinned_delete(string $key_or_class): void {}

function pinned_delete_array(array $keys): void {}

function pinned_clear(): void {}

function pinned_atomic_increment(string $key, int $step = 1): int {}

function pinned_atomic_decrement(string $key, int $step = 1): int {}

function pinned_cache_info(): StaticCacheInfo {}

}
