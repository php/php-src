<?php

/** @generate-class-entries */

namespace Opcache {

/** @strict-properties */
final readonly class UserCacheInfo
{
	private function __construct() {}

	public string $scope;

	public bool $available;

	public ?string $unavailableReason;

	public bool $enabled;

	public bool $startupFailed;

	public bool $backendInitialized;

	public int $configuredMemory;

	public int $sharedMemorySize;

	public int $usedMemory;

	public int $freeMemory;

	public int $wastedMemory;

	public int $entryCount;

	public int $entryCapacity;

	public int $tombstoneCount;
}

/** @not-serializable */
final class UserCache
{
	public function __construct(string $scope) {}

	public function store(string $key, null|bool|int|float|string|array|object $value, int $ttl = 0): bool {}

	public function storeMultiple(array $values, int $ttl = 0): bool {}

	public function increment(string $key, int $step = 1, int $ttl = 0): int|false {}

	public function decrement(string $key, int $step = 1, int $ttl = 0): int|false {}

	public function fetch(string $key, null|bool|int|float|string|array|object $default = null): null|bool|int|float|string|array|object {}

	public function fetchMultiple(array $keys, mixed $default = null): array {}

	public function delete(string $key): bool {}

	public function deleteMultiple(array $keys): bool {}

	public function clear(): bool {}

	public function has(string $key): bool {}

	public function lock(string $key, int $lease = 0): bool {}

	public function unlock(string $key): bool {}

	public function remember(string $key, callable $callback, int $ttl = 0): mixed {}

	public function info(): UserCacheInfo {}
}

}
