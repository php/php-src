--TEST--
SignalReturn::Default with restart_syscalls=true restarts a blocking socket read
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
$pair = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$pair) die("skip stream_socket_pair() not available");
if (!function_exists('pcntl_fork')) die("skip pcntl_fork() not available");
?>
--FILE--
<?php
// Handler returns null (Default); because restart_syscalls=true, the dispatch
// function returns ZEND_SIGNAL_RESTART and the poll loop retries. The child
// then writes data, poll() becomes readable, and fread() returns the data.
pcntl_signal(SIGUSR1, function (int $signo): void {
    echo "handler\n";
}, restart_syscalls: true);

[$read, $write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($read, true);
stream_set_timeout($read, 3600);

$pid = pcntl_fork();
if ($pid === 0) {
    fwrite($write, "1");
    fclose($read);
    usleep(200_000); // let parent block in poll()
    posix_kill(posix_getppid(), SIGUSR1);
    usleep(200_000); // let parent restart and re-enter poll()
    fwrite($write, "hello\n");
    fclose($write);
    exit(0);
}

// Wait for child to start
fread($read, 1);

$result = fread($read, 1024);
var_dump($result);

pcntl_waitpid($pid, $status);
fclose($read);
fclose($write);
?>
--EXPECT--
handler
string(6) "hello
"
