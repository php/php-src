--TEST--
OPcache volatile cache destructive mutators allow same-key replacement while old shared graph is referenced
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

function build_same_key_rows(string $prefix, int $multiplier): array
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

function build_same_key_payload(string $prefix, int $multiplier): array
{
	return [
		'name' => 'payload-' . $prefix,
		'rows' => build_same_key_rows($prefix, $multiplier),
	];
}

function wait_for_same_key_file(string $path): void
{
	$deadline = microtime(true) + 5.0;
	while (!file_exists($path)) {
		if (microtime(true) >= $deadline) {
			throw new RuntimeException("timed out waiting for {$path}");
		}
		usleep(1000);
	}
}

function run_same_key_refetch_case(string $operation): void
{
	$key = 'destructive_mutator_same_key_refetch_' . $operation . '_' . getmypid();
	$prefix = sys_get_temp_dir() . '/opcache_destructive_mutator_same_key_refetch_' . getmypid() . '_' . $operation;
	$readyFile = $prefix . '.ready';
	$doneFile = $prefix . '.done';
	$resultFile = $prefix . '.result';
	@unlink($readyFile);
	@unlink($doneFile);
	@unlink($resultFile);

	OPcache\volatile_clear();
	if (!OPcache\volatile_store($key, build_same_key_payload('A', 3))) {
		throw new RuntimeException('initial store failed');
	}

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		$fetched = OPcache\volatile_fetch($key);
		file_put_contents($readyFile, 'ready');
		wait_for_same_key_file($doneFile);

		$before = $fetched['rows'][123]['text'];
		$fetched['rows'][123]['text'] = 'changed old after ' . $operation;
		$after = $fetched['rows'][123]['text'];
		$oldNested = $fetched['rows'][123]['nested']['value'];
		$refetched = OPcache\volatile_fetch($key, 'MISS');

		file_put_contents(
			$resultFile,
			$before . "\n" .
			$after . "\n" .
			$oldNested . "\n" .
			$refetched['rows'][123]['text'] . "\n" .
			$refetched['rows'][123]['nested']['value']
		);
		exit(0);
	}

	wait_for_same_key_file($readyFile);
	switch ($operation) {
		case 'delete':
			if (!OPcache\volatile_delete($key)) {
				throw new RuntimeException('delete failed');
			}
			break;
		case 'clear':
			if (!OPcache\volatile_clear()) {
				throw new RuntimeException('clear failed');
			}
			break;
		default:
			throw new RuntimeException('unknown operation');
	}

	if (!OPcache\volatile_store($key, build_same_key_payload('B', 7))) {
		throw new RuntimeException('replacement store failed');
	}
	$parentRefetched = OPcache\volatile_fetch($key);

	file_put_contents($doneFile, 'done');
	pcntl_waitpid($pid, $status);

	echo $operation, "\n";
	echo $parentRefetched['rows'][123]['text'], "\n";
	echo $parentRefetched['rows'][123]['nested']['value'], "\n";
	echo file_get_contents($resultFile), "\n";

	@unlink($readyFile);
	@unlink($doneFile);
	@unlink($resultFile);
}

run_same_key_refetch_case('delete');
run_same_key_refetch_case('clear');

?>
--EXPECT--
delete
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
861
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
changed old after delete
369
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
861
clear
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
861
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
changed old after clear
369
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
861
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_destructive_mutator_same_key_refetch_*') ?: [] as $path) {
	@unlink($path);
}
?>
