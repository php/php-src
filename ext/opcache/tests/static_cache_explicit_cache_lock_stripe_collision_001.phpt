--TEST--
OPcache explicit cache lock stripes conservatively serialize colliding keys
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

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		opcache_static_cache_volatile_reset();
	} else {
		OPcache\StableCache::getInstance('default')->clear();
	}
}

function cache_lock(string $backend, string $key): bool
{
	return $backend === 'volatile'
		? OPcache\VolatileCache::getInstance('default')->lock($key)
		: OPcache\StableCache::getInstance('default')->lock($key);
}

function cache_store(string $backend, string $key, mixed $value): void
{
	if ($backend === 'volatile') {
		if (!OPcache\VolatileCache::getInstance('default')->store($key, $value)) {
			throw new RuntimeException('volatile_store failed');
		}
	} else {
		OPcache\StableCache::getInstance('default')->store($key, $value);
	}
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

function run_collision_case(string $backend): void
{
	$prefix = sys_get_temp_dir() . '/opcache_lock_stripe_collision_' . getmypid() . '_' . $backend;
	$resultFile = $prefix . '.result';
	$releaseFile = $prefix . '.release';
	@unlink($resultFile);
	@unlink($releaseFile);

	cache_clear($backend);

	$baseKey = $backend . '_lock_stripe_base_' . getmypid();
	var_dump(cache_lock($backend, $baseKey));

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		$blockedKey = null;
		for ($i = 0; $i < 4096; $i++) {
			$key = $backend . '_lock_stripe_candidate_' . getmypid() . '_' . $i;
			if (cache_lock($backend, $key)) {
				cache_store($backend, $key, 'release');
				continue;
			}
			$blockedKey = $key;
			break;
		}

		if ($blockedKey === null) {
			file_put_contents($resultFile, "no-collision\n");
			exit(1);
		}

		file_put_contents($resultFile, "blocked\n");
		wait_for_file($releaseFile);
		file_put_contents($resultFile, (cache_lock($backend, $blockedKey) ? "released\n" : "still-blocked\n"), FILE_APPEND);
		cache_store($backend, $blockedKey, 'released');
		exit(0);
	}

	wait_for_file($resultFile);
	cache_store($backend, $baseKey, 'parent-release');
	file_put_contents($releaseFile, 'release');
	pcntl_waitpid($pid, $status);

	echo $backend, "\n";
	echo file_get_contents($resultFile);
	@unlink($resultFile);
	@unlink($releaseFile);
}

run_collision_case('volatile');
run_collision_case('stable');

?>
--EXPECT--
bool(true)
volatile
blocked
released
bool(true)
stable
blocked
released
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_lock_stripe_collision_*') ?: [] as $path) {
	@unlink($path);
}
?>
