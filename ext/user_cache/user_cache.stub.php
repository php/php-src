<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace UserCache {

enum CacheAvailability
{
	case Available;
	case DisabledByIni;
	case DisabledBySapi;
	case UnavailableBySharedMemoryInitializationFailed;
	case UnavailableByBackendNotInitializedBeforeWorkerStartup;
	case UnavailableByBackendInitializedAfterWorkerStartup;
	case UnavailableByCgiFastCgiBoundary;
	case UnavailableByLsapiBoundary;
	case UnavailableByUnknownReason;
}

/**
 * @strict-properties
 * @not-serializable
 */
final readonly class CacheStatus
{
	private function __construct() {}

	public function getAvailability(): CacheAvailability {}

	public function getConfiguredMemory(): int {}

	public function getSharedMemorySize(): int {}

	public function getUsedMemory(): int {}

	public function getFreeMemory(): int {}

	public function getWastedMemory(): int {}

	public function getEntryCount(): int {}

	public function getEntryCapacity(): int {}

	public function getTombstoneCount(): int {}

	/**
	 * Number of times memory pressure forced a full cache wipe. Routine
	 * removal of expired entries is not counted.
	 */
	public function getExpungeCount(): int {}

	public function getStoreFailureCount(): int {}

	public function getGraphPinSlotsInUse(): int {}

	public function getGraphPinnedReferences(): int {}

	public function getDeadPinOwnersReclaimed(): int {}

	public function getDeadPinsStripped(): int {}
}

/**
 * @strict-properties
 * @not-serializable
 */
final readonly class CachePoolStatus
{
	private function __construct() {}

	public function getPoolName(): string {}

	public function getEntryCount(): int {}

	/** @return list<string> */
	public function getEntryKeys(): array {}

	public function getUsedMemory(): int {}
}

/** @not-serializable */
final class Cache
{
	public static function hasPool(string $pool): bool {}

	public static function getPool(string $pool): Cache {}

	public static function deletePool(string $pool): bool {}

	/** @return array<string, Cache> */
	public static function getPools(): array {}

	public static function getStatus(): CacheStatus {}

	private function __construct() {}

	public function store(string $key, null|bool|int|float|string|array|object $value, int $ttl = 0): bool {}

	public function add(string $key, null|bool|int|float|string|array|object $value, int $ttl = 0): bool {}

	public function storeMultiple(array $values, int $ttl = 0): bool {}

	public function increment(string $key, int $step = 1, int $ttl = 0): ?int {}

	public function decrement(string $key, int $step = 1, int $ttl = 0): ?int {}

	public function fetch(string $key, mixed $default = null): mixed {}

	public function fetchMultiple(array $keys, mixed $default = null): array {}

	public function delete(string $key): bool {}

	public function deleteMultiple(array $keys): bool {}

	public function clear(): bool {}

	public function has(string $key): bool {}

	public function lock(string $key, int $lease = 0): bool {}

	public function unlock(string $key): bool {}

	public function remember(string $key, callable $callback, int $ttl = 0): mixed {}

	public function getPoolStatus(): CachePoolStatus {}
}

}
