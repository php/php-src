--TEST--
Stream is closed while marked in_use after a fatal error
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

$pid = pcntl_fork();
if ($pid === 0) {
    fwrite($write, "1");
    fclose($read);
    usleep(200_000); // let parent block in fread()
    posix_kill(posix_getppid(), SIGUSR1);
    exit(0);
}

pcntl_signal(SIGUSR1, function (int $signo) use ($read): void {
    echo "handler\n";
    eval('class bool {}');
}, restart_syscalls: true);

register_shutdown_function(function() use ($read) {
    try {
        fclose($read);
    } catch (Error $e) {
        echo "shutdown function still cannot access stream: ", $e::class, ": ", $e->getMessage(), "\n";
        return;
    }
    echo "unreachable (shutdown function)\n";
});

// Wait for child to start
fread($read, 1);

$result = fread($read, 1024);

echo "unreachable";
?>
--EXPECTF--
handler

Fatal error: Cannot use "bool" as a class name as it is reserved in %s : eval()'d code on line 1
shutdown function still cannot access stream: Error: Concurrent access to a stream
