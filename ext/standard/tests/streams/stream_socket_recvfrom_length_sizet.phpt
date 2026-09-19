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

/* The receive buffer comes from zend_string_alloc($length), which adds the
 * header and the terminating NUL to the length without an overflow check, so
 * anything above PHP_STRING_MAX_LENGTH wraps to a tiny allocation that is then
 * handed to the transport as if it were $length bytes wide. The whole window
 * from the limit up to SIZE_MAX has to be refused, not just the values that
 * exceed SIZE_MAX itself. */
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;

$lengths = [
    'STR_MAX+1'  => PHP_STRING_MAX_LENGTH + 1,
    'STR_MAX+2'  => PHP_STRING_MAX_LENGTH + 2,
    'STR_MAX+3'  => PHP_STRING_MAX_LENGTH + 3,
    'SIZE_MAX-2' => $sizeMax - 2,
    'SIZE_MAX-1' => $sizeMax - 1,
    'SIZE_MAX'   => $sizeMax,
];

foreach ($lengths as $label => $length) {
    try {
        stream_socket_recvfrom($sock, $length);
        echo "$label: unexpected success\n";
    } catch (ValueError $e) {
        echo "$label: {$e->getMessage()}\n";
    }
}

fclose($sock);
?>
--EXPECTF--
stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+1: stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+2: stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+3: stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX-2: stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX-1: stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX: stream_socket_recvfrom(): Argument #2 ($length) must be less than or equal to %d
