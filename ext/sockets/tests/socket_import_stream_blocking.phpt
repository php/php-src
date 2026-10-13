--TEST--
socket_import_stream(): a blocking Socket waits on the non-blocking descriptor of the stream
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
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$s = socket_import_stream($a);

echo "non-blocking:\n";
socket_set_nonblock($s);
var_dump(@socket_read($s, 10), socket_last_error($s) === SOCKET_EAGAIN);
var_dump(@socket_recv($s, $buf, 10, 0), socket_last_error($s) === SOCKET_EAGAIN);

echo "SO_RCVTIMEO:\n";
socket_set_block($s);
socket_set_option($s, SOL_SOCKET, SO_RCVTIMEO, ['sec' => 0, 'usec' => 100000]);
var_dump(@socket_read($s, 10), socket_last_error($s) === SOCKET_EAGAIN);
socket_set_option($s, SOL_SOCKET, SO_RCVTIMEO, ['sec' => 0, 'usec' => 0]);

echo "blocking:\n";
$pid = pcntl_fork();
if ($pid == 0) {
    usleep(100000);
    fwrite($b, "hello\n");
    usleep(100000);
    fwrite($b, "abc");
    usleep(100000);
    fwrite($b, "def");
    posix_kill(posix_getpid(), SIGKILL);
}
var_dump(socket_read($s, 10));
var_dump(socket_recv($s, $buf, 6, MSG_WAITALL), $buf);
pcntl_waitpid($pid, $status);

echo "accept:\n";
$srv = stream_socket_server("tcp://127.0.0.1:0");
$ls = socket_import_stream($srv);
socket_set_block($ls);
$addr = stream_socket_get_name($srv, false);
$pid = pcntl_fork();
if ($pid == 0) {
    usleep(100000);
    $c = stream_socket_client("tcp://$addr");
    fwrite($c, "client");
    fclose($c);
    posix_kill(posix_getpid(), SIGKILL);
}
$conn = socket_accept($ls);
var_dump($conn instanceof Socket, socket_read($conn, 10));
pcntl_waitpid($pid, $status);
?>
--EXPECT--
non-blocking:
bool(false)
bool(true)
bool(false)
bool(true)
SO_RCVTIMEO:
bool(false)
bool(true)
blocking:
string(6) "hello
"
int(6)
string(6) "abcdef"
accept:
bool(true)
string(6) "client"
