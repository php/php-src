--TEST--
OPcache explicit cache parent locks survive forked child shutdown
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

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\volatile_lock($key)
		: OPcache\pinned_lock($key);
}

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_store($key, $value);
	} else {
		OPcache\pinned_store($key, $value);
	}
}

function fork_lock_result(string $backend, string $key, string $label): string
{
	$resultFile = sys_get_temp_dir() . '/opcache_explicit_lock_parent_survives_' . getmypid() . '_' . $backend . '_' . $label . '.result';
	@unlink($resultFile);

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		file_put_contents($resultFile, cache_lock($backend, $key) ? "locked\n" : "not locked\n");
		exit(0);
	}

	pcntl_waitpid($pid, $status);
	$result = file_get_contents($resultFile);
	@unlink($resultFile);

	return $result;
}

foreach (['volatile', 'pinned'] as $backend) {
	$key = $backend . '_parent_lock_survives_child_shutdown_' . getmypid();

	echo $backend, "\n";
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
	echo fork_lock_result($backend, $key, 'first_child');
	echo fork_lock_result($backend, $key, 'second_child');
	cache_store($backend, $key, 'release');
	echo fork_lock_result($backend, $key, 'after_parent_store');
}

?>
--EXPECT--
volatile
bool(true)
not locked
not locked
locked
pinned
bool(true)
not locked
not locked
locked
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_explicit_lock_parent_survives_*') ?: [] as $path) {
	@unlink($path);
}
?>
