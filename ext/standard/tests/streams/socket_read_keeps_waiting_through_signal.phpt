--TEST--
Without a provider a blocking socket read keeps waiting through a handled signal, as on master
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_timeout($a, 3600);
$parent = posix_getpid();

$handled = 0;
pcntl_async_signals(true);
pcntl_signal(SIGUSR1, function () use (&$handled) { $handled++; });

$pid = pcntl_fork();
if ($pid === 0) {
    fclose($a);
    usleep(200000);
    posix_kill($parent, SIGUSR1);
    usleep(200000);
    fwrite($b, "data\n");
    exit(0);
}
fclose($b);

var_dump(fgets($a));
pcntl_signal_dispatch();
var_dump($handled);
pcntl_waitpid($pid, $status);
?>
--EXPECT--
string(5) "data
"
int(1)
