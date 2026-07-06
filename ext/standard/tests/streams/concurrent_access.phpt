--TEST--
Stream cannot be accessed concurrently via signal handlers
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

[$read, $write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($read, true);
stream_set_timeout($read, 3600);

pcntl_signal(SIGUSR1, function (int $signo) use ($read): void {
    echo "handler\n";
    try {
        fread($read, 1);
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    try {
        fclose($read);
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}, restart_syscalls: true);

$pid = pcntl_fork();
if ($pid === 0) {
    fwrite($write, "1");
    fclose($read);
    usleep(200_000); // let parent block in fread()
    posix_kill(posix_getppid(), SIGUSR1);
    usleep(200_000); // let parent block in fread()
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
Error: Concurrent access to a stream
Error: Concurrent access to a stream
string(6) "hello
"
