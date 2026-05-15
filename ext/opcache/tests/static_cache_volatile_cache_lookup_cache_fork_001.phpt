--TEST--
OPcache volatile cache request-local lookup cache sees cross-process updates on next fetch
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
--FILE--
<?php

function fetch_or_miss(string $key): string
{
	$value = OPcache\volatile_fetch($key, 'MISS');
	return is_string($value) ? $value : get_debug_type($value);
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

function run_lookup_cache_scenario(string $key, ?string $initialValue, string $action, ?string $updatedValue): void
{
	$prefix = sys_get_temp_dir() . '/opcache_lookup_cache_' . $key . '_' . getmypid();
	$readyFile = $prefix . '.ready';
	$doneFile = $prefix . '.done';
	$resultFile = $prefix . '.result';

	cleanup_files($readyFile, $doneFile, $resultFile);

	if ($initialValue === null) {
		OPcache\volatile_delete($key);
	} else {
		OPcache\volatile_store($key, $initialValue);
	}

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		$first = fetch_or_miss($key);
		file_put_contents($readyFile, 'ready');
		wait_for_file($doneFile);
		$second = fetch_or_miss($key);
		file_put_contents($resultFile, $first . "\n" . $second);
		exit(0);
	}

	wait_for_file($readyFile);
	switch ($action) {
		case 'store':
			OPcache\volatile_store($key, $updatedValue);
			break;
		case 'delete':
			OPcache\volatile_delete($key);
			break;
		case 'clear':
			OPcache\volatile_clear();
			break;
		default:
			throw new RuntimeException("unknown action {$action}");
	}
	file_put_contents($doneFile, 'done');
	pcntl_waitpid($pid, $status);

	echo trim((string) file_get_contents($resultFile)), "\n";
	cleanup_files($readyFile, $doneFile, $resultFile);
}

OPcache\volatile_clear();

run_lookup_cache_scenario('lookup_hit_store_key', 'old', 'store', 'new');
run_lookup_cache_scenario('lookup_miss_store_key', null, 'store', 'created');
run_lookup_cache_scenario('lookup_hit_delete_key', 'old', 'delete', null);
run_lookup_cache_scenario('lookup_hit_clear_key', 'old', 'clear', null);

?>
--EXPECT--
old
new
MISS
created
old
MISS
old
MISS
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_lookup_cache_*') ?: [] as $path) {
	@unlink($path);
}
?>
