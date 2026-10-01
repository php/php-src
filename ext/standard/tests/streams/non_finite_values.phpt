--TEST--
Non-finite timeout values in stream functions
--FILE--
<?php
$socket = stream_socket_server("tcp://0.0.0.0:14781", $errno, $errstr);
foreach ([NAN, -NAN, INF, -INF] as $value) {
    try {
        stream_socket_accept($socket, $value);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
fclose($socket);

foreach ([NAN, -NAN, INF, -INF] as $value) {
    try {
        stream_socket_client("tcp://0.0.0.0:14781", timeout: $value);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
ValueError: stream_socket_accept(): Argument #2 ($timeout) must be a finite value
ValueError: stream_socket_accept(): Argument #2 ($timeout) must be a finite value
ValueError: stream_socket_accept(): Argument #2 ($timeout) must be a finite value
ValueError: stream_socket_accept(): Argument #2 ($timeout) must be a finite value
ValueError: stream_socket_client(): Argument #4 ($timeout) must be a finite value
ValueError: stream_socket_client(): Argument #4 ($timeout) must be a finite value
ValueError: stream_socket_client(): Argument #4 ($timeout) must be a finite value
ValueError: stream_socket_client(): Argument #4 ($timeout) must be a finite value
