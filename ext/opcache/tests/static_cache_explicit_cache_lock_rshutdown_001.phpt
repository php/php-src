--TEST--
OPcache explicit cache locks are released when request exits without store
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
		OPcache\volatile_clear();
	} else {
		OPcache\pinned_clear();
	}
}

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_store($key, $value);
	} else {
		OPcache\pinned_store($key, $value);
	}
}

function cache_fetch(string $backend, string $key, mixed $default = null): mixed
{
	return $backend === 'volatile'
		? OPcache\volatile_fetch($key, $default)
		: OPcache\pinned_fetch($key, $default);
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_lock($key)
		: OPcache\pinned_lock($key);
}

foreach (['volatile', 'pinned'] as $backend) {
	$prefix = sys_get_temp_dir() . '/opcache_explicit_exists_lock_rshutdown_' . getmypid() . '_' . $backend;
	$readyFile = $prefix . '.ready';
	$resultFile = $prefix . '.result';
	$key = $backend . '_exists_lock_rshutdown_' . getmypid();
	@unlink($readyFile);
	@unlink($resultFile);

	echo $backend, "\n";
	cache_clear($backend);

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		$locked = cache_lock($backend, $key);
		file_put_contents($resultFile, ($locked ? 'true' : 'false') . "\n");
		file_put_contents($readyFile, 'ready');
		exit(0);
	}

	wait_for_file($readyFile);
	pcntl_waitpid($pid, $status);
	echo file_get_contents($resultFile);

	pcntl_async_signals(true);
	pcntl_signal(SIGALRM, static function (): void {
		echo "timeout\n";
		exit(1);
	});
	pcntl_alarm(3);
	var_dump(cache_lock($backend, $key));
	pcntl_alarm(0);
	cache_store($backend, $key, 'after');
	var_dump(cache_fetch($backend, $key));

	@unlink($readyFile);
	@unlink($resultFile);
}

?>
--EXPECT--
volatile
true
bool(true)
string(5) "after"
pinned
true
bool(true)
string(5) "after"
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_explicit_exists_lock_rshutdown_*') ?: [] as $path) {
	@unlink($path);
}
?>
