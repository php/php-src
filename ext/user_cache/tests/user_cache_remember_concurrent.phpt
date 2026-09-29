--TEST--
UserCache\Cache: concurrent remember waits for the first callback and reuses its value
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair')) die('skip requires stream_socket_pair');
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
--FILE--
<?php
$cache = UserCache\Cache::getPool('remember-concurrent');
$cache->store('calls', 0);
$sockets = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$pid = pcntl_fork();
if ($pid < 0) die('fork failed');
if ($pid === 0) {
    fclose($sockets[0]);
    $value = $cache->remember('value', function () use ($cache, $sockets) {
        $cache->increment('calls');
        fwrite($sockets[1], "ready\n");
        if (fgets($sockets[1]) !== "fetch\n") exit(1);
        usleep(300000);
        return 'first';
    });
    exit($value === 'first' ? 0 : 2);
}
fclose($sockets[1]);
if (fgets($sockets[0]) !== "ready\n") die('handshake failed');
fwrite($sockets[0], "fetch\n");
var_dump($cache->remember('value', function () use ($cache) {
    $cache->increment('calls');
    return 'second';
}));
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);
var_dump($cache->fetch('calls'), $cache->fetch('value'));
fclose($sockets[0]);
?>
--EXPECT--
string(5) "first"
bool(true)
int(1)
string(5) "first"
