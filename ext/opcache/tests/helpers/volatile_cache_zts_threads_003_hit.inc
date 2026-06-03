<?php

function fetch_or_miss(string $key): string
{
	return OPcache\VolatileCache::getInstance('default')->fetch($key, 'MISS');
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

function cleanup_files(string ...$paths): void
{
	foreach ($paths as $path) {
		@unlink($path);
	}
}

$key = 'zts_lookup_cache_hit_key';
$readyFile = sys_get_temp_dir() . '/opcache_zts_lookup_cache_hit.ready';
$doneFile = sys_get_temp_dir() . '/opcache_zts_lookup_cache_hit.done';

if ($mode === 'init') {
	cleanup_files($readyFile, $doneFile);
	opcache_static_cache_volatile_reset();
	return OPcache\VolatileCache::getInstance('default')->store($key, 'old');
}

if ($mode === 'writer') {
	wait_for_file($readyFile);
	$result = OPcache\VolatileCache::getInstance('default')->store($key, 'new');
	file_put_contents($doneFile, 'done');
	return $result;
}

$first = fetch_or_miss($key);
file_put_contents($readyFile, 'ready');
wait_for_file($doneFile);
$second = fetch_or_miss($key);
cleanup_files($readyFile, $doneFile);

return $first === 'old' && $second === 'new';
