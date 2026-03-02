--TEST--
GH-16041 002: Stack overflow in phpdbg
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
?>
--INI--
zend.max_allowed_stack_size=512K
--PHPDBG--
set pagination off
run
quit
--FILE--
<?php

function map() {
    array_map('map', [1]);
}

try {
    map();
} catch (\Throwable $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

?>
--EXPECTF--
[Successful compilation of %sgh16041_002.php]
prompt> prompt> Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
[Script ended normally]
prompt>
