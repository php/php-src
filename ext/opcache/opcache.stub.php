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

enum CacheStoreType
{
	case NotFound;
	case Scalar;
	case SharedGraph;
	case OPcacheSerialized;
	case PHPSerialized;
}

#[\Attribute(13)] /* TARGET_CLASS | TARGET_METHOD | TARGET_PROPERTY */
final class VolatileStatic
{
	public readonly int $ttl;

	public readonly CacheStrategy $strategy;

	public function __construct(int $ttl = 0, CacheStrategy $strategy = CacheStrategy::Immediate) {}
}

final class VolatileCache
{
	public static function get(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object {}

	/**
	 * @return array<string, null|bool|int|float|string|array|object>|false
	 */
	public static function getMultiple(array $keys, ?array $default = null): array|false {}

	public static function set(string $key, null|bool|int|float|string|array|object $value, int $ttl = 0): bool {}

	public static function setMultiple(array $values, int $ttl = 0): bool {}

	public static function has(string $key): bool {}

	public static function delete(string $key_or_class): bool {}

	public static function deleteMultiple(array $keys): bool {}

	public static function clear(): bool {}

	public static function lock(string $key, int $lease = 0): bool {}

	public static function unlock(string $key): bool {}

	public static function getCacheStoreType(string $key_or_property, ?string $class_name = null): CacheStoreType {}

	public static function info(): StaticCacheInfo {}
}

final class PinnedCache
{
	public static function get(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object {}

	/**
	 * @return array<string, null|bool|int|float|string|array|object>|false
	 */
	public static function getMultiple(array $keys, ?array $default = null): array|false {}

	public static function set(string $key, null|bool|int|float|string|array|object $value): bool {}

	public static function setMultiple(array $values): bool {}

	public static function has(string $key): bool {}

	public static function delete(string $key_or_class): bool {}

	public static function deleteMultiple(array $keys): bool {}

	public static function clear(): bool {}

	public static function lock(string $key, int $lease = 0): bool {}

	public static function unlock(string $key): bool {}

	public static function increment(string $key, int $step = 1): int|false {}

	public static function decrement(string $key, int $step = 1): int|false {}

	public static function getCacheStoreType(string $key_or_property, ?string $class_name = null): CacheStoreType {}

	public static function info(): StaticCacheInfo {}
}

}
