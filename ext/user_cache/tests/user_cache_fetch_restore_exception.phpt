--TEST--
UserCache\Cache: fetch/remember/fetchMultiple propagate restore-hook exceptions and keep the entry
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('fetch-restore-exception');
$cache->clear();

class RestoreWakeupThrows
{
	public static bool $failNext = false;
	public int $value = 0;

	public function __wakeup(): void
	{
		if (self::$failNext) {
			self::$failNext = false;
			throw new RuntimeException('wakeup failure');
		}
	}
}

RestoreWakeupThrows::$failNext = false;

var_dump($cache->store('k', new RestoreWakeupThrows()));

/* fetch(): a throwing __wakeup propagates like native unserialize() and the
 * entry is retained because it is not corrupt. */
RestoreWakeupThrows::$failNext = true;
try {
	$cache->fetch('k', 'DEFAULT');
	echo "no exception\n";
} catch (RuntimeException $e) {
	echo "fetch caught: ", $e->getMessage(), "\n";
}
var_dump($cache->has('k'));

/* remember(): the entry exists, so the exception propagates instead of
 * recomputing via the callback. */
RestoreWakeupThrows::$failNext = true;
try {
	$cache->remember('k', fn (): string => 'computed');
	echo "no exception\n";
} catch (RuntimeException $e) {
	echo "remember caught: ", $e->getMessage(), "\n";
}
var_dump($cache->has('k'));

/* fetchMultiple(): a throwing restore aborts the whole batch. */
RestoreWakeupThrows::$failNext = true;
try {
	$cache->fetchMultiple(['k'], 'DEFAULT');
	echo "no exception\n";
} catch (RuntimeException $e) {
	echo "fetchMultiple caught: ", $e->getMessage(), "\n";
}
var_dump($cache->has('k'));

/* Once the hook no longer throws, the retained entry restores normally. */
$restored = $cache->fetch('k', 'DEFAULT');
var_dump($restored instanceof RestoreWakeupThrows);
?>
--EXPECT--
bool(true)
fetch caught: wakeup failure
bool(true)
remember caught: wakeup failure
bool(true)
fetchMultiple caught: wakeup failure
bool(true)
bool(true)
