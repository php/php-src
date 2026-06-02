--TEST--
OPcache explicit cache destructive mutators do not wait on reservation locks
--EXTENSIONS--
opcache
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
	die('skip pcntl_fork() not available');
}
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

const ENTRY_LOCK_STRIPES = 256;

function stripe_for_key(string $key): int
{
	$hash = 5381;
	for ($i = 0, $len = strlen($key); $i < $len; $i++) {
		$hash = (($hash * 33) + ord($key[$i])) & 0xff;
	}

	return $hash;
}

function key_for_stripe(string $prefix, int $stripe): string
{
	for ($i = 0; $i < 100000; $i++) {
		$key = $prefix . ':' . $i;
		if (stripe_for_key($key) === $stripe) {
			return $key;
		}
	}

	throw new RuntimeException("unable to find key for stripe {$stripe}");
}

function wait_for_file(string $path): void
{
	$deadline = microtime(true) + 5.0;
	while (!file_exists($path)) {
		if (microtime(true) >= $deadline) {
			throw new RuntimeException("timed out waiting for {$path}");
		}
		usleep(1000);
	}
}

function wait_for_result(string $path, float $seconds): bool
{
	$deadline = microtime(true) + $seconds;
	while (!file_exists($path)) {
		if (microtime(true) >= $deadline) {
			return false;
		}
		usleep(1000);
	}

	return true;
}

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::clear();
	} else {
		OPcache\PinnedCache::clear();
	}
}

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::set($key, $value);
	} else {
		OPcache\PinnedCache::set($key, $value);
	}
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::get($key, $default)
		: OPcache\PinnedCache::get($key, $default);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::lock($key)
		: OPcache\PinnedCache::lock($key);
}

function cache_delete(string $backend, string $key): void
{
	if ($backend === 'volatile') {
		OPcache\VolatileCache::delete($key);
	} else {
		OPcache\PinnedCache::delete($key);
	}
}

function run_reservation_cycle(string $backend, string $label, callable $operation): void
{
	$prefix = sys_get_temp_dir() . '/opcache_destructive_mutator_no_deadlock_' . getmypid() . '_' . $backend . '_' . $label;
	$readyFile = $prefix . '.ready';
	$operationFile = $prefix . '.operation';
	$deleteFile = $prefix . '.delete';
	@unlink($readyFile);
	@unlink($operationFile);
	@unlink($deleteFile);

	$reservedKey = key_for_stripe($prefix . ':reserved', 200);
	$blockedKey = key_for_stripe($prefix . ':blocked', 1);

	cache_clear($backend);
	if (!cache_lock($backend, $reservedKey)) {
		throw new RuntimeException("failed to reserve {$backend} key");
	}

	$operationPid = pcntl_fork();
	if ($operationPid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($operationPid === 0) {
		file_put_contents($readyFile, 'ready');
		$operation();
		file_put_contents($operationFile, "{$label}: done\n");
		exit(0);
	}

	wait_for_file($readyFile);
	usleep(200000);

	$deletePid = pcntl_fork();
	if ($deletePid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($deletePid === 0) {
		cache_delete($backend, $blockedKey);
		file_put_contents($deleteFile, 'delete-low: done');
		exit(0);
	}

	$operationDone = wait_for_result($operationFile, 2.0);
	$deleteDone = wait_for_result($deleteFile, 2.0);
	if (!$operationDone || !$deleteDone) {
		echo "{$backend} {$label}: timeout\n";
	}

	cache_store($backend, $reservedKey, 'release');
	pcntl_waitpid($operationPid, $status);
	pcntl_waitpid($deletePid, $status);

	if ($operationDone && $deleteDone) {
		echo "{$backend} {$label}: no deadlock\n";
	}

	@unlink($readyFile);
	@unlink($operationFile);
	@unlink($deleteFile);
}

function run_delete_wait_cycle(string $backend): void
{
	$prefix = sys_get_temp_dir() . '/opcache_destructive_mutator_no_deadlock_' . getmypid() . '_' . $backend . '_delete';
	$resultFile = $prefix . '.result';
	@unlink($resultFile);

	$key = key_for_stripe($prefix . ':reserved-delete', 17);

	cache_clear($backend);
	cache_store($backend, $key, 'value');
	if (!cache_lock($backend, $key)) {
		throw new RuntimeException("failed to reserve {$backend} delete key");
	}

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		cache_delete($backend, $key);
		file_put_contents($resultFile, 'delete: done');
		exit(0);
	}

	$deleteDone = wait_for_result($resultFile, 2.0);
	if (!$deleteDone) {
		echo "{$backend} delete: timeout\n";
	}

	if ($deleteDone) {
		echo "{$backend} delete: no wait\n";
		echo "{$backend} delete fetch: ", cache_fetch($backend, $key, 'MISS'), "\n";
	}

	cache_store($backend, $key, 'release');
	pcntl_waitpid($pid, $status);

	@unlink($resultFile);
}

run_reservation_cycle('volatile', 'clear', static fn () => cache_clear('volatile'));
run_reservation_cycle('pinned', 'clear', static fn () => cache_clear('pinned'));
run_reservation_cycle('volatile', 'reset', static fn () => opcache_reset());
run_delete_wait_cycle('volatile');
run_delete_wait_cycle('pinned');

?>
--EXPECT--
volatile clear: no deadlock
pinned clear: no deadlock
volatile reset: no deadlock
volatile delete: no wait
volatile delete fetch: MISS
pinned delete: no wait
pinned delete fetch: MISS
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_destructive_mutator_no_deadlock_*') ?: [] as $path) {
	@unlink($path);
}
?>
