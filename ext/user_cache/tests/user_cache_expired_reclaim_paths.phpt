--TEST--
UserCache\Cache: expired entries are reclaimed by read-only request shutdown and by write traffic alone
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
	die('skip requires pcntl');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=1M
--FILE--
<?php
const KEYS = 100;

$cache = UserCache\Cache::getPool('expired-reclaim');

function free_memory(): int {
	return UserCache\Cache::getStatus()->getFreeMemory();
}

function seed(UserCache\Cache $cache): void {
	$pad = str_repeat('x', 2000);
	for ($i = 0; $i < KEYS; $i++) {
		$cache->store("e:$i", $pad, 1);
	}
}

/* One bounded expiry scan must cover the whole table for this test. */
var_dump(UserCache\Cache::getStatus()->getEntryCapacity() <= 4096);

$baseline = free_memory();

/* Read-only reclaim: a request that only observes expired misses must free
 * them at its shutdown. */
seed($cache);
sleep(2);
var_dump(free_memory() < $baseline); /* still occupied */

$pid = pcntl_fork();
if ($pid === 0) {
	for ($i = 0; $i < KEYS; $i++) {
		$cache->fetch("e:$i"); /* expired miss observations, no writes */
	}
	exit(0);
}
pcntl_waitpid($pid, $status);
var_dump(free_memory() === $baseline);

/* Write-driven reclaim: write traffic alone (no expired reads) must advance
 * the bounded scan. */
seed($cache);
sleep(2);
for ($w = 0; $w < 70; $w++) {
	$cache->store('w', $w);
}
$cache->delete('w');
var_dump(free_memory() === $baseline);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
