--TEST--
OPcache volatile cache fetched shared graph survives cross-process clear and reuse
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

class ClearReusePayload
{
	public function __construct(public array $rows) {}
}

function build_rows(string $prefix, int $multiplier): array
{
	$rows = [];

	for ($i = 0; $i < 8000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat($prefix, 64),
			'nested' => ['value' => $i * $multiplier],
		];
	}

	return $rows;
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

$key = 'materialized_clear_reuse_payload';
$overwriteKey = 'materialized_clear_reuse_overwrite';
$prefix = sys_get_temp_dir() . '/opcache_volatile_cache_materialized_clear_reuse_' . getmypid();
$readyFile = $prefix . '.ready';
$doneFile = $prefix . '.done';
$resultFile = $prefix . '.result';
@unlink($readyFile);
@unlink($doneFile);
@unlink($resultFile);

opcache_static_cache_volatile_reset();
if (!OPcache\VolatileCache::getInstance('default')->store($key, new ClearReusePayload(build_rows('T', 3)))) {
	throw new RuntimeException('store failed');
}

$pid = pcntl_fork();
if ($pid === -1) {
	throw new RuntimeException('pcntl_fork() failed');
}

if ($pid === 0) {
	$fetched = OPcache\VolatileCache::getInstance('default')->fetch($key);
	file_put_contents($readyFile, 'ready');
	wait_for_file($doneFile);

	$before = $fetched->rows[123]['text'];
	$fetched->rows[123]['text'] = 'changed after clear and reuse';
	$after = $fetched->rows[123]['text'];
	$nested = $fetched->rows[123]['nested']['value'];

	$refetch = OPcache\VolatileCache::getInstance('default')->fetch($key, 'MISS') === 'MISS' ? 'MISS' : 'HIT';

	file_put_contents($resultFile, $before . "\n" . $after . "\n" . $nested . "\n" . $refetch);
	exit(0);
}

wait_for_file($readyFile);
opcache_static_cache_volatile_reset();
if (!OPcache\VolatileCache::getInstance('default')->store($overwriteKey, new ClearReusePayload(build_rows('X', 7)))) {
	throw new RuntimeException('overwrite store failed');
}
file_put_contents($doneFile, 'done');
pcntl_waitpid($pid, $status);

echo file_get_contents($resultFile), "\n";
@unlink($readyFile);
@unlink($doneFile);
@unlink($resultFile);

?>
--EXPECT--
TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
changed after clear and reuse
369
MISS
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_volatile_cache_materialized_clear_reuse_[0-9]*.*') ?: [] as $path) {
	@unlink($path);
}
?>
