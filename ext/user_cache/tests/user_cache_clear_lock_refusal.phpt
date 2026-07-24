--TEST--
UserCache\Cache: clear() and deletePool() refuse while another process holds a lock
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('clear-lock-refusal');
$cache->clear();

var_dump($cache->lock('own'));
var_dump($cache->clear());
var_dump($cache->unlock('own'));

$pid = pcntl_fork();
if ($pid === 0) {
    $cache->lock('held-by-child');
    $cache->store('child-ready', true);
    while ($cache->fetch('parent-done') === null) {
        usleep(10000);
    }
    $cache->unlock('held-by-child');
    exit(0);
}

if ($pid > 0) {
    while ($cache->fetch('child-ready') === null) {
        usleep(10000);
    }

    var_dump($cache->clear());
    var_dump(UserCache\Cache::deletePool('clear-lock-refusal'));
    var_dump($cache->has('child-ready'));

    $cache->store('parent-done', true);
    pcntl_waitpid($pid, $status);

    var_dump($cache->clear());
    var_dump($cache->has('child-ready'));
} else {
    echo "pcntl_fork() failed\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
