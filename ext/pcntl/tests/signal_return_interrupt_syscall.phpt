--TEST--
SignalReturn::Default with restart_syscalls=false interrupts a blocking socket read
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
// Handler returns null (Default); because restart_syscalls=false, the dispatch
// function returns ZEND_SIGNAL_INTERRUPT and the poll loop in
// php_sock_stream_wait_for_data exits, causing fread() to return "".
$signaled = false;
pcntl_signal(SIGUSR1, function (int $signo) use (&$signaled): void {
    echo "handler\n";
    $signaled = true;
}, restart_syscalls: false);

[$read, $write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($read, true);
// Ensure we have a timeout, so that fread() will poll()
stream_set_timeout($read, 3600);

$pid = pcntl_fork();
if ($pid === 0) {
    fwrite($write, 1);
    usleep(200_000); // let parent block in poll()
    posix_kill(posix_getppid(), SIGUSR1);
    exit(0);
}

// Wait for child to start
fread($read, 1);

$result = fread($read, 1024);
var_dump($result);
var_dump($signaled);

pcntl_waitpid($pid, $status);
fclose($read);
fclose($write);
?>
--EXPECT--
handler
bool(false)
bool(true)
