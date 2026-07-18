--TEST--
UserCache\Cache: shared-graph pins of SIGKILLed workers are stripped and their payloads reclaimed
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
user_cache.shm_size=8M
--FILE--
<?php
const KEYS = 20;

$cache = UserCache\Cache::getPool('dead-pin');

function free_memory(): int {
	return UserCache\Cache::getStatus()->getFreeMemory();
}

function seed(UserCache\Cache $cache): void {
	$pad = str_repeat('x', 2000);
	for ($i = 0; $i < KEYS; $i++) {
		$cache->store("g:$i", ['i' => $i, 'pad' => $pad]); /* array => shared graph */
	}
}

function forget(UserCache\Cache $cache): void {
	for ($i = 0; $i < KEYS; $i++) {
		$cache->delete("g:$i");
	}
}

/* Fork a child that pins every payload via fetch. 'clean' exits normally
 * (request shutdown releases the pins); 'kill' blocks until SIGKILLed. */
function run_pinner(UserCache\Cache $cache, string $mode): void {
	$ready = tempnam(sys_get_temp_dir(), 'uc_pin');
	$pid = pcntl_fork();
	if ($pid === 0) {
		for ($i = 0; $i < KEYS; $i++) {
			$cache->fetch("g:$i");
		}
		file_put_contents($ready, 'r');
		if ($mode === 'kill') {
			sleep(30); /* SIGKILL window */
		}
		exit(0);
	}
	while (filesize($ready) === 0) {
		usleep(10000);
		clearstatcache();
	}
	if ($mode === 'kill') {
		posix_kill($pid, SIGKILL);
	}
	pcntl_waitpid($pid, $status);
	@unlink($ready);
}

$baseline = free_memory();
$status = UserCache\Cache::getStatus();
$owners0 = $status->getDeadPinOwnersReclaimed();
$stripped0 = $status->getDeadPinsStripped();

/* Control: a clean-exit pinner releases its pins on request shutdown, so
 * deleting the entries frees every payload back to the baseline. */
seed($cache);
run_pinner($cache, 'clean');
forget($cache);
var_dump(free_memory() === $baseline);

/* Kill: the dead owner keeps its pins, so a sibling request's shutdown
 * sweep must strip them before the retired payloads can be freed. */
seed($cache);
run_pinner($cache, 'kill');
run_pinner($cache, 'clean'); /* fetches + clean exit -> request-end sweep */
forget($cache);
var_dump(free_memory() === $baseline);

$status = UserCache\Cache::getStatus();
var_dump($status->getDeadPinOwnersReclaimed() - $owners0);
var_dump($status->getDeadPinsStripped() - $stripped0 === KEYS);

/* No pins remain in any owner slot. */
var_dump($status->getGraphPinnedReferences());
?>
--EXPECT--
bool(true)
bool(true)
int(1)
bool(true)
int(0)
