--TEST--
OPcache explicit cache locks are non-blocking for locked keys
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
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

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

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		opcache_static_cache_volatile_reset();
	} else {
		OPcache\StableCache::getInstance('default')->clear();
	}
}

function cache_store(string $backend, string $key, mixed $value): mixed
{
	if ($backend === 'volatile') {
		return OPcache\VolatileCache::getInstance('default')->store($key, $value);
	}

	OPcache\StableCache::getInstance('default')->store($key, $value);
	return null;
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->fetch($key, $default)
		: OPcache\StableCache::getInstance('default')->fetch($key, $default);
}

function cache_exists(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->has($key)
		: OPcache\StableCache::getInstance('default')->has($key);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->lock($key)
		: OPcache\StableCache::getInstance('default')->lock($key);
}

function run_child(string $backend, string $label, callable $operation): string
{
	$prefix = sys_get_temp_dir() . '/opcache_explicit_exists_lock_fork_' . getmypid() . '_' . $backend . '_' . $label;
	$readyFile = $prefix . '.ready';
	$resultFile = $prefix . '.result';
	@unlink($readyFile);
	@unlink($resultFile);

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		pcntl_async_signals(true);
		pcntl_signal(SIGALRM, static function () use ($resultFile): void {
			file_put_contents($resultFile, "timeout\n");
			exit(1);
		});
		pcntl_alarm(3);
		file_put_contents($readyFile, 'ready');
		file_put_contents($resultFile, $operation());
		pcntl_alarm(0);
		exit(0);
	}

	wait_for_file($readyFile);
	usleep(100000);

	return $pid . ':' . $resultFile . ':' . $readyFile;
}

function finish_child(string $child): string
{
	[$pid, $resultFile, $readyFile] = explode(':', $child);
	pcntl_waitpid((int) $pid, $status);
	$result = file_get_contents($resultFile);
	@unlink($readyFile);
	@unlink($resultFile);

	return $result;
}

function run_missing_lock_returns_false_when_locked(string $backend): void
{
	$key = $backend . '_missing_lock_blocks_' . getmypid();

	cache_clear($backend);
	var_dump(cache_lock($backend, $key));

	$child = run_child($backend, 'missing_lock', static function () use ($backend, $key): string {
		$locked = cache_lock($backend, $key);
		$exists = cache_exists($backend, $key);
		$value = cache_fetch($backend, $key, 'MISS');

		return ($locked ? 'locked' : 'not locked') . "\n" . ($exists ? 'true' : 'false') . "\n" . $value . "\n";
	});

	[, $resultFile] = explode(':', $child);
	echo 'missing-lock blocked: ', file_exists($resultFile) ? 'no' : 'yes', "\n";
	cache_store($backend, $key, 'built');
	echo finish_child($child);
}

function run_missing_without_lock_does_not_reserve(string $backend): void
{
	$key = $backend . '_missing_without_lock_' . getmypid();

	cache_clear($backend);
	var_dump(cache_exists($backend, $key));

	$child = run_child($backend, 'missing_no_lock', static function () use ($backend, $key): string {
		cache_store($backend, $key, 'child');

		return cache_fetch($backend, $key, 'MISS') . "\n";
	});

	echo finish_child($child);
}

function run_existing_exists_does_not_reserve(string $backend): void
{
	$key = $backend . '_existing_lock_' . getmypid();

	cache_clear($backend);
	cache_store($backend, $key, 'owner');
	var_dump(cache_exists($backend, $key));

	$child = run_child($backend, 'existing_lock', static function () use ($backend, $key): string {
		cache_store($backend, $key, 'child');

		return cache_fetch($backend, $key, 'MISS') . "\n";
	});

	echo finish_child($child);
}

foreach (['volatile', 'stable'] as $backend) {
	echo $backend, "\n";
	run_missing_lock_returns_false_when_locked($backend);
	run_missing_without_lock_does_not_reserve($backend);
	run_existing_exists_does_not_reserve($backend);
}

?>
--EXPECT--
volatile
bool(true)
missing-lock blocked: no
not locked
false
MISS
bool(false)
child
bool(true)
child
stable
bool(true)
missing-lock blocked: no
not locked
false
MISS
bool(false)
child
bool(true)
child
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_explicit_exists_lock_fork_*') ?: [] as $path) {
	@unlink($path);
}
?>
