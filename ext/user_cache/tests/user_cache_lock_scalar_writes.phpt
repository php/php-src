--TEST--
UserCache\Cache: scalar writes skip other keys' locks but still wait for a lock on their own key
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair')) die('skip requires stream_socket_pair');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
const WAIT_NS = 30000000000;

/* The child holds $key until told to release it, then writes $value right before unlocking */
function hold_lock(UserCache\Cache $cache, string $key, int $value): array
{
    [$parent, $child] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    $pid = pcntl_fork();
    if ($pid < 0) die("fork failed\n");
    if ($pid === 0) {
        fclose($parent);
        if (!$cache->lock($key)) exit(1);
        fwrite($child, "locked\n");
        if (fgets($child) !== "release\n") exit(2);
        /* Only widens the window for a write that ignores the lock; passing runs do not depend on it. */
        usleep(100000);
        if (!$cache->store($key, $value)) exit(3);
        exit($cache->unlock($key) ? 0 : 4);
    }
    fclose($child);

    return [$pid, $parent];
}

function read_line($socket, int $pid): string|false
{
    $deadline = hrtime(true) + WAIT_NS;
    do {
        $read = [$socket];
        $write = $except = null;
        if (stream_select($read, $write, $except, 0, 10000) > 0) {
            return fgets($socket);
        }
        if (pcntl_waitpid($pid, $status, WNOHANG) !== 0) {
            return false;
        }
    } while (hrtime(true) < $deadline);

    return false;
}

function reap(int $pid, $socket): bool
{
    $deadline = hrtime(true) + WAIT_NS;
    fclose($socket);
    do {
        $ret = pcntl_waitpid($pid, $status, WNOHANG);
        if ($ret !== 0) {
            return $ret === $pid && pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0;
        }
        usleep(1000);
    } while (hrtime(true) < $deadline);

    if (function_exists('posix_kill')) {
        posix_kill($pid, SIGKILL);
    }

    return false;
}

$cache = UserCache\Cache::getPool('scalar-locks');
$cache->clear();
var_dump($cache->store('counter', 0), $cache->store('other', 0));
var_dump($cache->fetch('counter'), $cache->fetch('other'));

/* The lock stays held until "release", so a write that waited for it would time out and fail */
[$pid, $socket] = hold_lock($cache, 'counter', 100);
var_dump(read_line($socket, $pid) === "locked\n");
var_dump($cache->store('other', 1));
fwrite($socket, "release\n");
var_dump($cache->store('counter', 1));
var_dump(reap($pid, $socket));
/* The overwrite lands after the value the child wrote right before unlocking */
var_dump($cache->fetch('counter'), $cache->fetch('other'));

[$pid, $socket] = hold_lock($cache, 'counter', 200);
var_dump(read_line($socket, $pid) === "locked\n");
var_dump($cache->increment('other'));
fwrite($socket, "release\n");
var_dump($cache->increment('counter'));
var_dump(reap($pid, $socket));
var_dump($cache->fetch('counter'), $cache->fetch('other'));
?>
--EXPECT--
bool(true)
bool(true)
int(0)
int(0)
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
int(1)
bool(true)
int(2)
int(201)
bool(true)
int(201)
int(2)
