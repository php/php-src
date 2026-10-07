--TEST--
UserCache\Cache: locks and graph pins of owners that exited are reclaimed by probes made within the same second, and a live owner's lock keeps refusing a clear
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair')) {
    die('skip requires stream_socket_pair');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
function fork_child(): int
{
    $pid = pcntl_fork();
    if ($pid < 0) {
        die("pcntl_fork() failed\n");
    }

    return $pid;
}

$cache = UserCache\Cache::getPool('dead-owner-probe');
$cache->clear();
$cache->store('graph', range(1, 8));

/* Several children are killed holding the same lock and a pin in quick succession; each takeover re-probes the owner. */
$taken = 0;
for ($round = 0; $round < 5; $round++) {
    $pid = fork_child();
    if ($pid === 0) {
        $child = UserCache\Cache::getPool('dead-owner-probe');
        $child->lock('held');
        $child->fetch('graph');
        posix_kill(posix_getpid(), SIGKILL);
    }
    pcntl_waitpid($pid, $status);
    if ($cache->lock('held')) {
        $taken++;
        $cache->unlock('held');
    }
}
var_dump($taken);

/* A live child's lock keeps refusing clear() across repeated probes in the same second. */
[$parent, $child] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$pid = fork_child();
if ($pid === 0) {
    fclose($parent);
    $holder = UserCache\Cache::getPool('dead-owner-probe');
    fwrite($child, $holder->lock('live') ? 'L' : 'F');
    fread($child, 1);
    exit($holder->unlock('live') ? 0 : 1);
}
fclose($child);
var_dump(fread($parent, 1));
$refused = 0;
for ($i = 0; $i < 3; $i++) {
    if (!$cache->clear()) {
        $refused++;
    }
}
var_dump($refused, $cache->has('graph'));
fwrite($parent, 'R');
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

/* The dead readers' pins never block the payload and the lock is free again. */
var_dump($cache->fetch('graph') === range(1, 8));
var_dump($cache->lock('held'), $cache->unlock('held'), $cache->lock('live'), $cache->unlock('live'));
var_dump($cache->clear(), $cache->has('graph'));
var_dump(UserCache\Cache::getStatus()->getAvailability() === UserCache\CacheAvailability::Available);
?>
--EXPECT--
int(5)
string(1) "L"
int(3)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
