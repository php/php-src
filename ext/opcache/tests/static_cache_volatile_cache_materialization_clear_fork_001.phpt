--TEST--
OPcache volatile cache fetched shared graph survives cross-process VolatileCache::clear()
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

class ClearPayload
{
	public function __construct(public array $rows) {}
}

function build_rows(): array
{
	$rows = [];

	for ($i = 0; $i < 8000; $i++) {
		$rows[] = [
			'id' => $i,
			'text' => str_repeat(chr(65 + ($i % 26)), 64),
			'nested' => ['value' => $i * 3],
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

$key = 'materialized_clear_payload';
$prefix = sys_get_temp_dir() . '/opcache_volatile_cache_materialized_clear_' . getmypid();
$readyFile = $prefix . '.ready';
$doneFile = $prefix . '.done';
$resultFile = $prefix . '.result';
@unlink($readyFile);
@unlink($doneFile);
@unlink($resultFile);

OPcache\VolatileCache::clear();
if (!OPcache\VolatileCache::set($key, new ClearPayload(build_rows()))) {
	throw new RuntimeException('store failed');
}

$pid = pcntl_fork();
if ($pid === -1) {
	throw new RuntimeException('pcntl_fork() failed');
}

if ($pid === 0) {
	$fetched = OPcache\VolatileCache::get($key);
	$before = $fetched->rows[123]['text'];
	file_put_contents($readyFile, 'ready');
	wait_for_file($doneFile);
	$fetched->rows[123]['text'] = 'changed after clear';
	$after = $fetched->rows[123]['text'];

	$refetch = OPcache\VolatileCache::get($key, 'MISS') === 'MISS' ? 'MISS' : 'HIT';

	file_put_contents($resultFile, $before . "\n" . $after . "\n" . $refetch);
	exit(0);
}

wait_for_file($readyFile);
OPcache\VolatileCache::clear();
file_put_contents($doneFile, 'done');
pcntl_waitpid($pid, $status);

echo file_get_contents($resultFile), "\n";
@unlink($readyFile);
@unlink($doneFile);
@unlink($resultFile);

?>
--EXPECT--
TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
changed after clear
MISS
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_volatile_cache_materialized_clear_[0-9]*.*') ?: [] as $path) {
	@unlink($path);
}
?>
