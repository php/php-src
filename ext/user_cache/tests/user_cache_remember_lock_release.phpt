--TEST--
UserCache\Cache: remember() releases its entry lock when the post-lock fetch throws and when the callback bails out
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
class RememberThrowingWakeup
{
    public int $value = 1;

    public function __wakeup(): void
    {
        throw new RuntimeException('__wakeup failed');
    }
}

/* The value published while remember() waits for the lock fails to restore */
$cache = UserCache\Cache::getPool('remember-lock-release');
$cache->clear();

$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$pid = pcntl_fork();
if ($pid < 0) die('fork failed');
if ($pid === 0) {
    fclose($sockets[0]);
    if (!$cache->lock('published-while-waiting')) exit(1);
    fwrite($sockets[1], "locked\n");
    if (fgets($sockets[1]) !== "waiting\n") exit(2);
    usleep(300000);
    if (!$cache->store('published-while-waiting', new RememberThrowingWakeup())) exit(3);
    exit($cache->unlock('published-while-waiting') ? 0 : 4);
}
fclose($sockets[1]);
if (fgets($sockets[0]) !== "locked\n") die('handshake failed');
fwrite($sockets[0], "waiting\n");

$calls = 0;
try {
    $cache->remember('published-while-waiting', function () use (&$calls) {
        $calls++;

        return 'computed';
    });
    echo "no exception\n";
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
pcntl_waitpid($pid, $status);
fclose($sockets[0]);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
var_dump($calls);
var_dump($cache->unlock('published-while-waiting'));
var_dump($cache->has('published-while-waiting'));

/* A fatal error inside the callback releases the lock before shutdown functions run */
$cache = UserCache\Cache::getPool('remember-bailout');
$cache->clear();

register_shutdown_function(function () use ($cache) {
    echo "shutdown\n";
    var_dump($cache->unlock('fatal'));
    var_dump($cache->has('fatal'));
    var_dump($cache->lock('fatal'), $cache->unlock('fatal'));
});

$cache->remember('fatal', function () {
    eval('function remember_lock_release_dup() {} function remember_lock_release_dup() {}');

    return 'never stored';
});
echo "not reached\n";
?>
--EXPECTF--
__wakeup failed
bool(true)
int(0)
bool(false)
bool(true)

Fatal error: Cannot redeclare function remember_lock_release_dup() (previously declared in %s) in %s on line %d
%Ashutdown
bool(false)
bool(false)
bool(true)
bool(true)
