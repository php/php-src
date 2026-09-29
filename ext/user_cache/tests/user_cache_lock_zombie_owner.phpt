--TEST--
UserCache\Cache: an entry lock held by a killed but not yet reaped process can be taken over
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') {
    die('skip Linux only');
}
if (!is_readable('/proc/self/stat')) {
    die('skip requires /proc');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('zombie-owner');
$cache->clear();

$pid = pcntl_fork();
if ($pid === 0) {
    $cache->lock('held');
    $cache->store('ready', true);
    sleep(30);
    exit(0);
}
while ($cache->fetch('ready') === null) {
    usleep(10000);
}

/* WNOWAIT leaves the killed owner as a zombie: kill(pid, 0) still succeeds. */
posix_kill($pid, SIGKILL);
$info = [];
var_dump(pcntl_waitid(P_PID, $pid, $info, WEXITED | WNOWAIT));

var_dump($cache->lock('held'));
var_dump($cache->unlock('held'));

pcntl_waitpid($pid, $status);
var_dump(pcntl_wifsignaled($status));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
