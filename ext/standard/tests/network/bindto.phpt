--TEST--
Test invalid bindto
--FILE--
<?php
$ctx = stream_context_create([
    'socket' => [
        'bindto' => 'invalid',
    ],
]);
$fp = stream_socket_client(
    'tcp://www.' . str_repeat('x', 100) . '.com:80',
    $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx
);
?>
--EXPECTF--
Warning: stream_socket_client(): unable to connect to tcp://%s:80 (Failed to parse address "invalid") in %s on line %d
