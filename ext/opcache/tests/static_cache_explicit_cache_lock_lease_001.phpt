--TEST--
OPcache explicit cache lock leases survive request shutdown until expiry
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

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_clear();
	} else {
		OPcache\pinned_clear();
	}
}

function cache_lock(string $backend, string $key, int $lease = 0): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_lock($key, $lease)
		: OPcache\pinned_lock($key, $lease);
}

function cache_unlock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_unlock($key)
		: OPcache\pinned_unlock($key);
}

foreach (['volatile', 'pinned'] as $backend) {
	$key = $backend . '_lock_lease_' . getmypid();
	$resultFile = sys_get_temp_dir() . '/opcache_explicit_cache_lock_lease_' . getmypid() . '_' . $backend . '.result';
	@unlink($resultFile);

	echo $backend, "\n";
	cache_clear($backend);

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		file_put_contents($resultFile, 'child locked: ' . (cache_lock($backend, $key, 2) ? 'true' : 'false') . "\n");
		exit(0);
	}

	pcntl_waitpid($pid, $status);
	echo file_get_contents($resultFile);

	var_dump(cache_lock($backend, $key));
	sleep(3);
	var_dump(cache_lock($backend, $key));
	var_dump(cache_unlock($backend, $key));

	@unlink($resultFile);
}

?>
--EXPECT--
volatile
child locked: true
bool(false)
bool(true)
bool(true)
pinned
child locked: true
bool(false)
bool(true)
bool(true)
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_explicit_cache_lock_lease_*') ?: [] as $path) {
	@unlink($path);
}
?>
