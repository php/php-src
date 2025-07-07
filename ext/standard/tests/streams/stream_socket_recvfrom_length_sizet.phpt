--TEST--
stream_socket_recvfrom() $length overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
$sock = stream_socket_server('tcp://127.0.0.1:0');
try {
    stream_socket_recvfrom($sock, PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fclose($sock);
?>
--EXPECTF--
stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
