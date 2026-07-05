--TEST--
stream_socket_recvfrom() throws a catchable MemoryError when the requested length cannot fit in the memory limit
--SKIPIF--
<?php
if (!function_exists('stream_socket_pair')) die('skip stream_socket_pair() not available');
?>
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
fwrite($b, 'ping');

try {
    stream_socket_recvfrom($a, PHP_INT_MAX);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(stream_socket_recvfrom($a, 4));
fclose($a);
fclose($b);

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
string(4) "ping"
