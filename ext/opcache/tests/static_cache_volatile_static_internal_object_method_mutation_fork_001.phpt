--TEST--
OPcache VolatileStatic tracks internal object method mutations across fork
--EXTENSIONS--
opcache
pcntl
spl
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
opcache.file_update_protection=0
opcache.jit=0
--FILE--
<?php

class VolatileStaticForkDateMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): DateTime
	{
		static $value = null;

		$value ??= new DateTime('2026-01-01 00:00:00', new DateTimeZone('UTC'));
		return $value;
	}
}

class VolatileStaticForkArrayObjectMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): ArrayObject
	{
		static $value = null;

		$value ??= new ArrayObject(['count' => 0]);
		return $value;
	}
}

class VolatileStaticForkFixedArrayMethodState
{
	#[OPcache\VolatileStatic(strategy: OPcache\CacheStrategy::Tracking)]
	public static function value(): SplFixedArray
	{
		static $value = null;

		$value ??= SplFixedArray::fromArray(['seed']);
		return $value;
	}
}

function static_cache_internal_object_wait_for_file(string $path): void
{
	$deadline = microtime(true) + 5.0;

	while (!file_exists($path)) {
		if (microtime(true) >= $deadline) {
			throw new RuntimeException("timed out waiting for {$path}");
		}

		usleep(1000);
	}
}

function static_cache_internal_object_reset(): void
{
	OPcache\volatile_clear();
	opcache_reset();
}

function static_cache_internal_object_fork_case(string $state): void
{
	$prefix = sys_get_temp_dir() . '/opcache_volatile_static_internal_object_method_mutation_fork_' . getmypid() . '_' . $state;
	$readyFile = $prefix . '.ready';
	$resultFile = $prefix . '.result';
	@unlink($readyFile);
	@unlink($resultFile);

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		if ($state === 'date') {
			$value = VolatileStaticForkDateMethodState::value();
			$value->modify('+1 day');
		} elseif ($state === 'array') {
			$value = VolatileStaticForkArrayObjectMethodState::value();
			$value->offsetSet('count', $value->offsetGet('count') + 1);
			$value->append('tail');
		} else {
			$value = VolatileStaticForkFixedArrayMethodState::value();
			$value->offsetSet(0, 'changed');
			$value->setSize(2);
			$value->offsetSet(1, 'tail');
		}

		file_put_contents($readyFile, 'ready');
		exit(0);
	}

	static_cache_internal_object_wait_for_file($readyFile);
	pcntl_waitpid($pid, $status);
	@unlink($readyFile);

	if (!pcntl_wifexited($status) || pcntl_wexitstatus($status) !== 0) {
		throw new RuntimeException('child process failed');
	}

	$pid = pcntl_fork();
	if ($pid === -1) {
		throw new RuntimeException('pcntl_fork() failed');
	}

	if ($pid === 0) {
		ob_start();
		static_cache_internal_object_dump_case($state);
		file_put_contents($resultFile, ob_get_clean());
		exit(0);
	}

	pcntl_waitpid($pid, $status);
	if (!pcntl_wifexited($status) || pcntl_wexitstatus($status) !== 0) {
		throw new RuntimeException('reader process failed');
	}

	echo file_get_contents($resultFile);
	@unlink($resultFile);
}

function static_cache_internal_object_dump_case(string $state): void
{
	if ($state === 'date') {
		$value = VolatileStaticForkDateMethodState::value();
		echo "date=", $value->format('Y-m-d'), "\n";
		return;
	}

	if ($state === 'array') {
		$value = VolatileStaticForkArrayObjectMethodState::value();
		echo "array=", $value->offsetGet('count'), ",", count($value), "\n";
		return;
	}

	$value = VolatileStaticForkFixedArrayMethodState::value();
	$second = $value->count() > 1 ? $value->offsetGet(1) : 'none';
	echo "fixed=", $value->count(), ",", $value->offsetGet(0), ",", $second, "\n";
}

foreach (['date', 'array', 'fixed'] as $state) {
	static_cache_internal_object_fork_case($state);
}

static_cache_internal_object_reset();

?>
--EXPECT--
date=2026-01-02
array=1,2
fixed=2,changed,tail
