--TEST--
Stream cannot be accessed concurrently via signal handlers
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php
$pair = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$pair) die("skip stream_socket_pair() not available");
?>
--FILE--
<?php

pcntl_signal(SIGUSR1, function (int $signo) use (&$read): void {
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
});

[$read, $write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($read, true);
stream_set_timeout($read, 0, 1);

// Queue signal
posix_kill(posix_getpid(), SIGUSR1);

// Signal handled here
$result = fread($read, 1024);
var_dump($result);

fclose($read);
fclose($write);
?>
--EXPECT--
handler
Error: Concurrent access to a stream
Error: Concurrent access to a stream
bool(false)
