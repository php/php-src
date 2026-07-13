--TEST--
UserCache\Cache: lock lease expires and can be taken over by another process
--EXTENSIONS--
user_cache
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('lock-lease');
$cache->clear();

var_dump($cache->lock('leased', 1));

$pid = pcntl_fork();
if ($pid === 0) {
    echo "child lock before expiry\n";
    var_dump($cache->lock('leased'));
    sleep(2);
    echo "child lock after expiry\n";
    var_dump($cache->lock('leased'));
    var_dump($cache->unlock('leased'));
    exit(0);
}

if ($pid > 0) {
    pcntl_waitpid($pid, $status);
} else {
    echo "pcntl_fork() failed\n";
}
?>
--EXPECT--
bool(true)
child lock before expiry
bool(false)
child lock after expiry
bool(true)
bool(true)
