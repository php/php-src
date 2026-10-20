--TEST--
socket_cmsg_space() always returns int, never null
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die('skip SCM_RIGHTS not available on Windows');
}
if (!defined('SCM_RIGHTS')) {
    die('skip SCM_RIGHTS not defined on this platform');
}
?>
--FILE--
<?php
// Happy path: returns int, never null
$r = socket_cmsg_space(SOL_SOCKET, SCM_RIGHTS, 1);
var_dump(get_debug_type($r));

// Unknown level/type pair
try {
    socket_cmsg_space(999999, 999999);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Negative $num
try {
    socket_cmsg_space(SOL_SOCKET, SCM_RIGHTS, -1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
string(3) "int"
ValueError: Pair level 999999 and/or type 999999 is not supported
ValueError: socket_cmsg_space(): Argument #3 ($num) must be greater than or equal to 0
