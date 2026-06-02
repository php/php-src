--TEST--
OPcache volatile cache shared graph survives destructive mutators and reuse
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

class DestructiveMutatorPayload
{
	public function __construct(public array $rows) {}
}

function build_rows(string $prefix, int $multiplier): array
{
	$rows = [];

	for ($i = 0; $i < 6000; $i++) {
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

function run_destructive_mutator(string $operation): void
{
	$key = 'destructive_mutator_retire_' . $operation . '_' . getmypid();
	$overwriteKey = 'destructive_mutator_retire_overwrite_' . $operation . '_' . getmypid();
	$prefix = sys_get_temp_dir() . '/opcache_destructive_mutator_retire_' . getmypid() . '_' . $operation;
	$readyFile = $prefix . '.ready';
	$doneFile = $prefix . '.done';
	$resultFile = $prefix . '.result';
	@unlink($readyFile);
	@unlink($doneFile);
	@unlink($resultFile);

	OPcache\VolatileCache::clear();
	if (!OPcache\VolatileCache::set($key, new DestructiveMutatorPayload(build_rows('T', 3)))) {
		throw new RuntimeException('store failed');
	}

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		$fetched = OPcache\VolatileCache::get($key);
		file_put_contents($readyFile, 'ready');
		wait_for_file($doneFile);

		$before = $fetched->rows[123]['text'];
		$fetched->rows[123]['text'] = 'changed after ' . $operation;
		$after = $fetched->rows[123]['text'];
		$nested = $fetched->rows[123]['nested']['value'];
		$refetch = OPcache\VolatileCache::get($key, 'MISS') === 'MISS' ? 'MISS' : 'HIT';

		file_put_contents($resultFile, $before . "\n" . $after . "\n" . $nested . "\n" . $refetch);
		exit(0);
	}

	wait_for_file($readyFile);
	switch ($operation) {
		case 'delete':
			OPcache\VolatileCache::delete($key);
			break;
		case 'clear':
			OPcache\VolatileCache::clear();
			break;
		case 'reset':
			opcache_reset();
			break;
	}

	if (!OPcache\VolatileCache::set($overwriteKey, new DestructiveMutatorPayload(build_rows('X', 7)))) {
		throw new RuntimeException('overwrite store failed');
	}
	file_put_contents($doneFile, 'done');
	pcntl_waitpid($pid, $status);

	echo $operation, "\n", file_get_contents($resultFile), "\n";

	@unlink($readyFile);
	@unlink($doneFile);
	@unlink($resultFile);
}

run_destructive_mutator('delete');
run_destructive_mutator('clear');
run_destructive_mutator('reset');

?>
--EXPECT--
delete
TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
changed after delete
369
MISS
clear
TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
changed after clear
369
MISS
reset
TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
changed after reset
369
MISS
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_destructive_mutator_retire_*') ?: [] as $path) {
	@unlink($path);
}
?>
