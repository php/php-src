--TEST--
A forked child closing a socket stream leaves the parent's descriptor non-blocking
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip not for Windows');
?>
--FILE--
<?php
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);

$pid = pcntl_fork();
if ($pid === 0) {
    fclose($a);
    exit(0);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status));

pcntl_signal(SIGALRM, function () { echo "blocked in recv\n"; exit(1); });
pcntl_async_signals(true);
pcntl_alarm(5);

stream_set_timeout($a, 0, 100000);
var_dump(fread($a, 10));
var_dump(stream_get_meta_data($a)['timed_out']);
?>
--EXPECT--
int(0)
bool(false)
bool(true)
