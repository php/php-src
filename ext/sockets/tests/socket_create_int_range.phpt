--TEST--
socket_create() and socket_create_pair(): $type and $protocol outside of the int range are rejected
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die('skip 64-bit only');
}
if (!function_exists('socket_create_pair')) {
    die('skip socket_create_pair() is not available');
}
?>
--FILE--
<?php
$calls = [
    'type 2**32 + 1' => fn() => socket_create(AF_INET, 2 ** 32 + 1, 0),
    'type -2**32 + 1' => fn() => socket_create(AF_INET, -(2 ** 32) + 1, 0),
    'type PHP_INT_MIN' => fn() => socket_create(AF_INET, PHP_INT_MIN, 0),
    'protocol 2**32' => fn() => socket_create(AF_INET, SOCK_STREAM, 2 ** 32),
    'protocol 2**32 + 6' => fn() => socket_create(AF_INET, SOCK_STREAM, 2 ** 32 + 6),
    'protocol PHP_INT_MIN' => fn() => socket_create(AF_INET, SOCK_STREAM, PHP_INT_MIN),
    'pair type 2**32 + 1' => fn() => socket_create_pair(AF_UNIX, 2 ** 32 + 1, 0, $pair),
    'pair protocol 2**32' => fn() => socket_create_pair(AF_UNIX, SOCK_STREAM, 2 ** 32, $pair),
];

foreach ($calls as $label => $call) {
    try {
        $call();
    } catch (ValueError $e) {
        echo $label, ': ', $e->getMessage(), "\n";
    }
}

// Values inside of the range still work
var_dump(socket_create(AF_INET, SOCK_STREAM, SOL_TCP) instanceof Socket);
var_dump(socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair));
?>
--EXPECTF--
type 2**32 + 1: socket_create(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM optionally OR'ed with SOCK_CLOEXEC, SOCK_NONBLOCK
type -2**32 + 1: socket_create(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM optionally OR'ed with SOCK_CLOEXEC, SOCK_NONBLOCK
type PHP_INT_MIN: socket_create(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM optionally OR'ed with SOCK_CLOEXEC, SOCK_NONBLOCK
protocol 2**32: socket_create(): Argument #3 ($protocol) must be between -2147483648 and 2147483647
protocol 2**32 + 6: socket_create(): Argument #3 ($protocol) must be between -2147483648 and 2147483647
protocol PHP_INT_MIN: socket_create(): Argument #3 ($protocol) must be between -2147483648 and 2147483647
pair type 2**32 + 1: socket_create_pair(): Argument #2 ($type) must be one of SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, or SOCK_RDM optionally OR'ed with SOCK_CLOEXEC, SOCK_NONBLOCK
pair protocol 2**32: socket_create_pair(): Argument #3 ($protocol) must be between -2147483648 and 2147483647
bool(true)
bool(true)
