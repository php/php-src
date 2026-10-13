--TEST--
socket_export_stream(): the Socket stays blocking on the descriptor the stream made non-blocking
--EXTENSIONS--
sockets
pcntl
posix
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no pcntl on Windows');
?>
--FILE--
<?php
socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $p);
$st = socket_export_stream($p[0]);
var_dump(stream_get_meta_data($st)['blocked']);

echo "read:\n";
$pid = pcntl_fork();
if ($pid == 0) {
    usleep(100000);
    socket_write($p[1], "world");
    posix_kill(posix_getpid(), SIGKILL);
}
var_dump(socket_read($p[0], 10));
pcntl_waitpid($pid, $status);

echo "write:\n";
$big = str_repeat("x", 1 << 20);
$pid = pcntl_fork();
if ($pid == 0) {
    usleep(100000);
    $n = 0;
    while ($n < strlen($big) && ($r = socket_read($p[1], 65536)) !== false && $r !== '') {
        $n += strlen($r);
    }
    socket_write($p[1], (string) $n);
    posix_kill(posix_getpid(), SIGKILL);
}
var_dump(socket_write($p[0], $big));
var_dump(socket_read($p[0], 10));
pcntl_waitpid($pid, $status);

echo "signal:\n";
pcntl_async_signals(true);
pcntl_signal(SIGUSR1, function () { echo "handler\n"; });
$pid = pcntl_fork();
if ($pid == 0) {
    usleep(100000);
    posix_kill(posix_getppid(), SIGUSR1);
    posix_kill(posix_getpid(), SIGKILL);
}
var_dump(@socket_read($p[0], 10), socket_last_error($p[0]) === SOCKET_EINTR);
pcntl_waitpid($pid, $status);

echo "non-blocking:\n";
socket_set_nonblock($p[0]);
var_dump(stream_get_meta_data($st)['blocked']);
var_dump(@socket_read($p[0], 10), socket_last_error($p[0]) === SOCKET_EAGAIN);
?>
--EXPECT--
bool(true)
read:
string(5) "world"
write:
int(1048576)
string(7) "1048576"
signal:
handler
bool(false)
bool(true)
non-blocking:
bool(false)
bool(false)
bool(true)
