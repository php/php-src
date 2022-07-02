--TEST--
Bug #74764 IPv6 bindto fails with stream_socket_client()
--SKIPIF--
<?php
/* following copied straight from the tcp6loop.phpt */
@stream_socket_client('tcp://[::1]:0', $errno);
if ($errno != 111) die('skip IPv6 not supported.');
?>
--FILE--
<?php
$context = stream_context_create(
    ['socket' => array('bindto' => "[::]:0")]
    );
    $socket = stream_socket_client('tcp://localhost:1443', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $context);

$context = stream_context_create(
    array('socket' => array('bindto' => "0.0.0.0:0"))
    );
    $socket = stream_socket_client('tcp://localhost:1443', $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $context);
?>
--EXPECTF--
Warning: stream_socket_client(): Unable to connect to tcp://localhost:1443 (%s) in %s on line %d

Warning: stream_socket_client(): Unable to connect to tcp://localhost:1443 (%s) in %s on line %d
