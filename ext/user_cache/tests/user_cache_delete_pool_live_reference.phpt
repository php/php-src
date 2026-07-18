--TEST--
UserCache\Cache: deletePool() keeps values and objects alive until the last reference drops
--EXTENSIONS--
user_cache
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_fork') || !function_exists('posix_kill')) {
	die('skip requires pcntl and posix');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
/* 1) A live Cache object survives deletePool(): the request-local pool object
 *    is only unregistered, not destroyed while userland still references it. */
$a = UserCache\Cache::getPool('live-ref');
$a->store('k', 'v');
var_dump(UserCache\Cache::deletePool('live-ref'));
var_dump($a->has('k'));                 /* value deleted */
var_dump($a->store('again', 1));        /* object still usable (recreates pool) */
var_dump($a->fetch('again', 'MISS'));

/* 2) A zero-copy value another process still holds is retired, not freed:
 *    the payload stays valid until that process releases its pin. */
$cache = UserCache\Cache::getPool('del-pin');
$original = ['list' => range(1, 64), 'label' => str_repeat('x', 100)];
$cache->store('graph', $original);

$ready = tempnam(sys_get_temp_dir(), 'uc_dp_ready');
$deleted = tempnam(sys_get_temp_dir(), 'uc_dp_done');

$pid = pcntl_fork();
if ($pid === 0) {
	/* Child: fetch and hold the value (this pins the shared payload), then wait
	 * for the parent to deletePool, then confirm the held value is intact. */
	$held = $cache->fetch('graph');
	file_put_contents($ready, 'r');

	while (filesize($deleted) === 0) {
		usleep(10000);
		clearstatcache();
	}

	echo "child: ", ($held === ['list' => range(1, 64), 'label' => str_repeat('x', 100)])
		? "intact\n" : "CORRUPTED\n";
	exit(0);
}

/* Parent: wait until the child has pinned the value, then delete the pool. */
while (filesize($ready) === 0) {
	usleep(10000);
	clearstatcache();
}

var_dump(UserCache\Cache::deletePool('del-pin'));
file_put_contents($deleted, 'd');
pcntl_waitpid($pid, $status);

/* After the child released its pin, the pool and its values are fully gone. */
var_dump(UserCache\Cache::hasPool('del-pin'));
var_dump(UserCache\Cache::getPool('del-pin')->fetch('graph', 'MISS'));

@unlink($ready);
@unlink($deleted);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
int(1)
bool(true)
child: intact
bool(false)
string(4) "MISS"
