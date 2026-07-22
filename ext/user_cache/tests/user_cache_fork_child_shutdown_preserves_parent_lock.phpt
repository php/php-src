--TEST--
UserCache\Cache: forked child shutdown does not release the parent's entry lock
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('fork-shutdown-lock');
$cache->clear();

var_dump($cache->lock('held'));

$pid = pcntl_fork();
if ($pid === 0) {
    exit(0);
}

if ($pid < 0) {
    echo "pcntl_fork() failed\n";
    exit(1);
}

pcntl_waitpid($pid, $status);

$pid = pcntl_fork();
if ($pid === 0) {
    echo "child lock\n";
    var_dump($cache->lock('held', 1));

    exit(0);
}

if ($pid < 0) {
    echo "pcntl_fork() failed\n";

    exit(1);
}

pcntl_waitpid($pid, $status);

echo "parent unlock\n";
var_dump($cache->unlock('held'));
?>
--EXPECT--
bool(true)
child lock
bool(false)
parent unlock
bool(true)
