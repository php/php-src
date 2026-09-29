--TEST--
UserCache\Cache: entry lock ownership, release at exit, pool-scoped clear refusal and a waiting delete across processes
--EXTENSIONS--
pcntl
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

/* Lock ownership is per process; a child can neither release nor take the parent's lock. */
$cache = UserCache\Cache::getPool('locks-fork-ownership');
$cache->clear();

$shared = range(1, 4);
$value = [$shared, $shared, ['nested' => $shared]];

var_dump($cache->store('graph', $value));
var_dump($cache->lock('lock'));

$pid = fork_child();
if ($pid === 0) {
    echo "child unlock\n";
    var_dump($cache->unlock('lock'));
    echo "child lock\n";
    var_dump($cache->lock('lock', 1));
    echo "child fetch\n";
    var_dump($cache->fetch('graph') === $value);
    exit(0);
}

pcntl_waitpid($pid, $status);
echo "parent unlock\n";
var_dump($cache->unlock('lock'));

/* A child that only exits does not release the parent's lock at shutdown. */
$cache = UserCache\Cache::getPool('locks-fork-shutdown');
$cache->clear();

var_dump($cache->lock('held'));

$pid = fork_child();
if ($pid === 0) {
    exit(0);
}
pcntl_waitpid($pid, $status);

$pid = fork_child();
if ($pid === 0) {
    echo "child lock\n";
    var_dump($cache->lock('held', 1));
    exit(0);
}
pcntl_waitpid($pid, $status);

echo "parent unlock\n";
var_dump($cache->unlock('held'));

/* Locks still held at process exit are released. */
$cache = UserCache\Cache::getPool('locks-exit-release');
$cache->clear();

$pid = fork_child();
if ($pid === 0) {
    $cache->lock('exit-plain');
    exit(0);
}
pcntl_waitpid($pid, $status);

echo "lock released at child exit\n";
var_dump($cache->lock('exit-plain'));
var_dump($cache->unlock('exit-plain'));

/* clear() and deletePool() refuse while another process holds a key lock in their own pool only. */
$cache = UserCache\Cache::getPool('locks-clear-refusal');
$cleared = UserCache\Cache::getPool('locks-clear-other');
$deleted = UserCache\Cache::getPool('locks-delete-other');
$cache->clear();
$cleared->store('value', 'cleared');
$deleted->store('value', 'deleted');

$pid = fork_child();
if ($pid === 0) {
    $cache->lock('held-by-child');
    $cache->store('child-ready', true);
    while ($cache->fetch('parent-done') === null) {
        usleep(10000);
    }
    exit($cache->unlock('held-by-child') ? 0 : 1);
}

while ($cache->fetch('child-ready') === null) {
    usleep(10000);
}

var_dump($cache->clear());
var_dump(UserCache\Cache::deletePool('locks-clear-refusal'));
var_dump($cache->has('child-ready'));

echo "other pools\n";
var_dump($cleared->clear());
var_dump(UserCache\Cache::deletePool('locks-delete-other'));
var_dump($cleared->has('value'), $deleted->has('value'));

$cache->store('parent-done', true);
pcntl_waitpid($pid, $status);

echo "child unlock after the refusals\n";
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
var_dump($cache->clear());
var_dump($cache->has('child-ready'));

/* delete() of a foreign-locked key waits until the holder's last write before unlocking. */
$cache = UserCache\Cache::getPool('locks-waiting-delete');
$cache->store('key', 10);
$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$pid = fork_child();
if ($pid === 0) {
    fclose($sockets[0]);
    if (!$cache->lock('key')) {
        exit(1);
    }
    fwrite($sockets[1], 'L');
    if (fread($sockets[1], 1) !== 'R') {
        exit(2);
    }
    usleep(100000);
    if (!$cache->store('key', 11)) {
        exit(3);
    }
    if (!$cache->unlock('key')) {
        exit(4);
    }
    fclose($sockets[1]);
    exit(0);
}
fclose($sockets[1]);
if (fread($sockets[0], 1) !== 'L') {
    die('lock handshake failed');
}
var_dump($cache->lock('key'));
fwrite($sockets[0], 'R');
var_dump($cache->delete('key'));
pcntl_waitpid($pid, $status);
fclose($sockets[0]);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
var_dump($cache->fetch('key', 'missing'));
?>
--EXPECT--
bool(true)
bool(true)
child unlock
bool(false)
child lock
bool(false)
child fetch
bool(true)
parent unlock
bool(true)
bool(true)
child lock
bool(false)
parent unlock
bool(true)
lock released at child exit
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
other pools
bool(true)
bool(true)
bool(false)
bool(false)
child unlock after the refusals
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
string(7) "missing"
