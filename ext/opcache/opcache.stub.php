<?php

/** @generate-class-entries */

namespace {

function opcache_reset(): bool {}

function opcache_static_cache_volatile_reset(): bool {}

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
final class StableStatic
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

interface StaticCacheInterface
{
	public static function getInstance(string $pool_name): static;

	public function fetch(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object;

	/**
	 * @return array<string, null|bool|int|float|string|array|object>|false
	 */
	public function fetchMultiple(array $keys, ?array $default = null): array|false;

	public function store(string $key, null|bool|int|float|string|array|object $value): bool;

	public function storeMultiple(array $values): bool;

	public function has(string $key): bool;

	public function delete(string $key): bool;

	public function deleteMultiple(array $keys): bool;

	public function clear(): bool;

	public function lock(string $key, int $lease = 0): bool;

	public function unlock(string $key): bool;

	public function getCacheStoreType(string $key): CacheStoreType;

	public static function info(): StaticCacheInfo;

	public static function getCacheStoreTypeByProperty(string $class_name, string $property_name): CacheStoreType;

	public static function getCacheStoreTypeByMethod(string $class_name, string $method_name, string $variable_name): CacheStoreType;
}

/** @not-serializable */
final class VolatileCache implements StaticCacheInterface
{
	private function __construct() {}

	public static function getInstance(string $pool_name): static {}

	public function fetch(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object {}

	/**
	 * @return array<string, null|bool|int|float|string|array|object>|false
	 */
	public function fetchMultiple(array $keys, ?array $default = null): array|false {}

	public function store(string $key, null|bool|int|float|string|array|object $value): bool {}

	public function storeMultiple(array $values): bool {}

	public function has(string $key): bool {}

	public function delete(string $key): bool {}

	public function deleteMultiple(array $keys): bool {}

	public function clear(): bool {}

	public function lock(string $key, int $lease = 0): bool {}

	public function unlock(string $key): bool {}

	public function getCacheStoreType(string $key): CacheStoreType {}

	public static function info(): StaticCacheInfo {}

	public static function getCacheStoreTypeByProperty(string $class_name, string $property_name): CacheStoreType {}

	public static function getCacheStoreTypeByMethod(string $class_name, string $method_name, string $variable_name): CacheStoreType {}
}

/** @not-serializable */
final class StableCache implements StaticCacheInterface
{
	private function __construct() {}

	public static function getInstance(string $pool_name): static {}

	public function fetch(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object {}

	/**
	 * @return array<string, null|bool|int|float|string|array|object>|false
	 */
	public function fetchMultiple(array $keys, ?array $default = null): array|false {}

	public function store(string $key, null|bool|int|float|string|array|object $value): bool {}

	public function storeMultiple(array $values): bool {}

	public function storeWithTtl(string $key, null|bool|int|float|string|array|object $value, int $ttl): bool {}

	public function storeMultipleWithTtl(array $values, int $ttl): bool {}

	public function has(string $key): bool {}

	public function delete(string $key): bool {}

	public function deleteMultiple(array $keys): bool {}

	public function clear(): bool {}

	public function lock(string $key, int $lease = 0): bool {}

	public function unlock(string $key): bool {}

	public function increment(string $key, int $step = 1): int|false {}

	public function decrement(string $key, int $step = 1): int|false {}

	public function getCacheStoreType(string $key): CacheStoreType {}

	public static function info(): StaticCacheInfo {}

	public static function getCacheStoreTypeByProperty(string $class_name, string $property_name): CacheStoreType {}

	public static function getCacheStoreTypeByMethod(string $class_name, string $method_name, string $variable_name): CacheStoreType {}
}

}
