--TEST--
UserCache\Cache: end-to-end on the fcntl lock model used where robust process-shared mutexes are missing
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
--ENV--
USER_CACHE_DEBUG_FORCE_FCNTL_LOCK_MODEL=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
require __DIR__ . '/user_cache_memory_model_exercise.inc';

UserCache\Cache::getPool('lock-model')->store('probe', 1);
ob_start();
phpinfo(INFO_MODULES);
preg_match('/^Active lock model => (.+)$/m', ob_get_clean(), $m);
echo 'lock model: ', trim($m[1] ?? 'unknown'), "\n";

user_cache_memory_model_exercise('mmap');

/* Readers and writers in other processes contend on the fcntl lock. */
$cache = UserCache\Cache::getPool('lock-model-contention');
$cache->store('counter', 0);
$children = [];
for ($worker = 0; $worker < 3; $worker++) {
    $pid = pcntl_fork();
    if ($pid === 0) {
        for ($i = 0; $i < 300; $i++) {
            if ($cache->increment('counter') === null ||
                !$cache->store("w$worker", [$worker, $i, str_repeat('s', 300)]) ||
                $cache->fetch("w$worker")[1] !== $i
            ) {
                exit(1);
            }
        }
        exit(0);
    }
    $children[] = $pid;
}
$ok = true;
foreach ($children as $pid) {
    pcntl_waitpid($pid, $status);
    $ok = $ok && pcntl_wexitstatus($status) === 0;
}
var_dump($ok, $cache->fetch('counter'));
?>
--EXPECTF--
lock model: fcntl
%A
bool(true)
int(900)
