--TEST--
OPcache explicit cache locks make builders wait while destructive mutators bypass reservations
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

function result_to_string(mixed $value): string
{
	return match (true) {
		$value === null => 'null',
		is_bool($value) => $value ? 'true' : 'false',
		default => (string) $value,
	};
}

function cache_clear(string $backend): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_clear();
	} else {
		OPcache\pinned_clear();
	}
}

function cache_store(string $backend, string $key, mixed $value): mixed
{
	if ($backend === 'volatile') {
		return OPcache\volatile_store($key, $value);
	}

	OPcache\pinned_store($key, $value);
	return null;
}

function cache_store_array(string $backend, array $values): mixed
{
	if ($backend === 'volatile') {
		return OPcache\volatile_store_array($values);
	}

	OPcache\pinned_store_array($values);
	return null;
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

function cache_delete(string $backend, string $key): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_delete($key);
	} else {
		OPcache\pinned_delete($key);
	}
}

function cache_delete_array(string $backend, array $keys): void
{
	if ($backend === 'volatile') {
		OPcache\volatile_delete_array($keys);
	} else {
		OPcache\pinned_delete_array($keys);
	}
}

function run_blocked_mutator(string $backend, string $label, callable $operation, callable $release, callable $after): void
{
	$prefix = sys_get_temp_dir() . '/opcache_explicit_exists_lock_public_mutators_' . getmypid() . '_' . $backend . '_' . $label;
	$readyFile = $prefix . '.ready';
	$resultFile = $prefix . '.result';
	@unlink($readyFile);
	@unlink($resultFile);

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		pcntl_async_signals(true);
		pcntl_signal(SIGALRM, static function () use ($resultFile): void {
			file_put_contents($resultFile, "timeout\n");
			exit(1);
		});
		pcntl_alarm(3);
		file_put_contents($readyFile, 'ready');
		$result = $operation();
		pcntl_alarm(0);
		file_put_contents($resultFile, $label . ' result: ' . result_to_string($result) . "\n");
		exit(0);
	}

	wait_for_file($readyFile);
	usleep(200000);
	echo $label, ' blocked: ', file_exists($resultFile) ? 'no' : 'yes', "\n";
	$release();
	pcntl_waitpid($pid, $status);
	echo file_get_contents($resultFile);
	$after();

	@unlink($readyFile);
	@unlink($resultFile);
}

function reserve_missing(string $backend, string $key): void
{
	cache_clear($backend);
	var_dump(cache_lock($backend, $key));
}

function run_backend_mutators(string $backend): void
{
	$baseKey = $backend . '_exists_lock_public_mutators_' . getmypid();

	echo $backend, "\n";

	$key = $baseKey . '_store';
	reserve_missing($backend, $key);
	run_blocked_mutator(
		$backend,
		'store',
		static fn () => cache_store($backend, $key, 'child'),
		static fn () => cache_store($backend, $key, 'owner'),
		static fn () => print 'store value: ' . cache_fetch($backend, $key, 'MISS') . "\n",
	);

	$key = $baseKey . '_store_array';
	reserve_missing($backend, $key);
	run_blocked_mutator(
		$backend,
		'store_array',
		static fn () => cache_store_array($backend, [$key => 'child']),
		static fn () => cache_store($backend, $key, 'owner'),
		static fn () => print 'store_array value: ' . cache_fetch($backend, $key, 'MISS') . "\n",
	);

	$key = $baseKey . '_delete';
	reserve_missing($backend, $key);
	run_blocked_mutator(
		$backend,
		'delete',
		static fn () => cache_delete($backend, $key),
		static fn () => cache_store($backend, $key, 'owner'),
		static fn () => print 'delete value: ' . cache_fetch($backend, $key, 'MISS') . "\n",
	);

	$key = $baseKey . '_delete_array';
	reserve_missing($backend, $key);
	run_blocked_mutator(
		$backend,
		'delete_array',
		static fn () => cache_delete_array($backend, [$key]),
		static fn () => cache_store($backend, $key, 'owner'),
		static fn () => print 'delete_array value: ' . cache_fetch($backend, $key, 'MISS') . "\n",
	);

	if ($backend === 'pinned') {
		$key = $baseKey . '_atomic';
		reserve_missing($backend, $key);
		run_blocked_mutator(
			$backend,
			'atomic',
			static fn () => OPcache\pinned_atomic_increment($key, 2),
			static fn () => cache_store($backend, $key, 10),
			static fn () => print 'atomic value: ' . cache_fetch($backend, $key, 'MISS') . "\n",
		);

		$key = $baseKey . '_atomic_decrement';
		reserve_missing($backend, $key);
		run_blocked_mutator(
			$backend,
			'atomic_decrement',
			static fn () => OPcache\pinned_atomic_decrement($key, 3),
			static fn () => cache_store($backend, $key, 10),
			static fn () => print 'atomic_decrement value: ' . cache_fetch($backend, $key, 'MISS') . "\n",
		);
	}

	$key = $baseKey . '_clear';
	$survivorKey = $baseKey . '_clear_survivor';
	cache_clear($backend);
	cache_store($backend, $survivorKey, 'survives');
	echo "stored survivor\n";
	var_dump(cache_lock($backend, $key));
	run_blocked_mutator(
		$backend,
		'clear',
		static fn () => cache_clear($backend),
		static fn () => cache_store($backend, $key, 'owner'),
		static fn () => print 'clear values: ' . cache_fetch($backend, $key, 'MISS') . ',' . cache_fetch($backend, $survivorKey, 'MISS') . "\n",
	);
}

run_backend_mutators('volatile');
run_backend_mutators('pinned');

?>
--EXPECT--
volatile
bool(true)
store blocked: yes
store result: true
store value: child
bool(true)
store_array blocked: yes
store_array result: true
store_array value: child
bool(true)
delete blocked: no
delete result: null
delete value: owner
bool(true)
delete_array blocked: no
delete_array result: null
delete_array value: owner
stored survivor
bool(true)
clear blocked: no
clear result: null
clear values: owner,MISS
pinned
bool(true)
store blocked: yes
store result: null
store value: child
bool(true)
store_array blocked: yes
store_array result: null
store_array value: child
bool(true)
delete blocked: no
delete result: null
delete value: owner
bool(true)
delete_array blocked: no
delete_array result: null
delete_array value: owner
bool(true)
atomic blocked: yes
atomic result: 12
atomic value: 12
bool(true)
atomic_decrement blocked: yes
atomic_decrement result: 7
atomic_decrement value: 7
stored survivor
bool(true)
clear blocked: no
clear result: null
clear values: owner,MISS
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_explicit_exists_lock_public_mutators_*') ?: [] as $path) {
	@unlink($path);
}
?>
