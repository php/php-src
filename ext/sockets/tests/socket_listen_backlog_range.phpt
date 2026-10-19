--TEST--
socket_create_listen() and socket_listen(): $backlog outside of the int range is rejected
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die('skip 64-bit only');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_bind($socket, '127.0.0.1', 0);

foreach ([2 ** 32, 2 ** 31, PHP_INT_MAX, PHP_INT_MIN, -(2 ** 31) - 1] as $backlog) {
    try {
        socket_create_listen(0, $backlog);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        socket_listen($socket, $backlog);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

// Values inside of the range are still accepted
foreach ([0, 5, -1, 2 ** 31 - 1, -(2 ** 31)] as $backlog) {
    var_dump(socket_create_listen(0, $backlog) instanceof Socket);
    var_dump(socket_listen($socket, $backlog));
}
?>
--EXPECT--
socket_create_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_create_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_create_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_create_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_create_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
socket_listen(): Argument #2 ($backlog) must be between -2147483648 and 2147483647
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
