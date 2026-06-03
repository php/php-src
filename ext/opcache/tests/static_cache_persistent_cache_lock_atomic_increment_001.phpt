--TEST--
OPcache StableCache::lock is released by StableCache::increment
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
opcache.static_cache.stable_size_mb=32
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

$prefix = sys_get_temp_dir() . '/opcache_stable_exists_lock_' . getmypid();
$readyFile = $prefix . '.ready';
$resultFile = $prefix . '.result';
@unlink($readyFile);
@unlink($resultFile);

$key = 'stable_exists_lock_' . getmypid();
OPcache\StableCache::getInstance('default')->clear();

var_dump(OPcache\StableCache::getInstance('default')->lock($key));
var_dump(OPcache\StableCache::getInstance('default')->increment($key, 9));

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
	$exists = OPcache\StableCache::getInstance('default')->lock($key);
	$value = OPcache\StableCache::getInstance('default')->fetch($key);
	pcntl_alarm(0);
	file_put_contents($resultFile, ($exists ? 'true' : 'false') . "\n" . $value . "\n");
	exit(0);
}

wait_for_file($readyFile);
pcntl_waitpid($pid, $status);
echo file_get_contents($resultFile);
@unlink($readyFile);
@unlink($resultFile);

?>
--EXPECT--
bool(true)
int(9)
true
9
--CLEAN--
<?php
foreach (glob(sys_get_temp_dir() . '/opcache_stable_exists_lock_*') ?: [] as $path) {
	@unlink($path);
}
?>
