--TEST--
Bug #76136: stream_socket_get_name should enclose IPv6 in brackets
--SKIPIF--
<?php
@stream_socket_client('tcp://[::1]:0', $errno);
if ((PHP_OS_FAMILY === 'Windows' && $errno !== 10049) || (PHP_OS_FAMILY !== 'Windows' && $errno !== 111)) {
    die('skip IPv6 is not supported.');
}
?>
--FILE--
<?php
$server = stream_socket_server("tcp://[::1]:1337/");
echo stream_socket_get_name($server, false).PHP_EOL;
$server = stream_socket_server("tcp://127.0.0.1:1337/");
echo stream_socket_get_name($server, false);
?>
--EXPECT--
[::1]:1337
127.0.0.1:1337
