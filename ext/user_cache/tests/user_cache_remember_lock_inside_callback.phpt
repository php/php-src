--TEST--
UserCache\Cache: a lock() of the key inside the remember() callback stays held after remember() returns
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
use UserCache\Cache;

$cache = Cache::getPool('remember-lock-inside-callback');
$cache->clear();

var_dump($cache->remember('k', static function () use ($cache): int {
    var_dump($cache->lock('k', 30));

    return 1;
}));

$pid = pcntl_fork();
if ($pid === 0) {
    exit(Cache::getPool('remember-lock-inside-callback')->lock('k') ? 1 : 0);
}
pcntl_waitpid($pid, $status);
echo 'another process took the lock: ', var_export(pcntl_wexitstatus($status) === 1, true), "\n";
var_dump($cache->unlock('k'), $cache->unlock('k'));
?>
--EXPECT--
bool(true)
int(1)
another process took the lock: false
bool(true)
bool(false)
