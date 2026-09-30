--TEST--
Stream can still be closed after a fatal error, despise being marked IN_USE
--EXTENSIONS--
pcntl
posix
zend_test
--SKIPIF--
<?php
$pair = @stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
if (!$pair) die("skip stream_socket_pair() not available");
?>
--FILE--
<?php

pcntl_signal(SIGUSR1, function (int $signo) use (&$read): void {
    echo "handler\n";
    zend_trigger_bailout();
});

register_shutdown_function(function() use (&$read) {
    echo "Accessing the stream from a shutdown function should fail:\n";
    try {
        fclose($read);
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    echo "End of shutdown function\n";
});

[$read, $write] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_blocking($read, true);
stream_set_timeout($read, 0, 1);

// Queue signal
posix_kill(posix_getpid(), SIGUSR1);

// Signal handled here
$result = fread($read, 1024);

echo "unreachable\n";

/* We are testing that no leaks occur */

?>
--EXPECTF--
handler

Fatal error: Bailout in %s on line %d
Accessing the stream from a shutdown function should fail:
Error: Concurrent access to a stream
End of shutdown function
