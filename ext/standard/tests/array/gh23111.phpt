--TEST--
GH-23111 (Stack overflow in array_walk_recursive with deeply nested arrays)
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
if (getenv('SKIP_ASAN')) {
    die('skip ASAN needs different stack limit setting due to more stack space usage');
}
?>
--INI--
zend.max_allowed_stack_size=256K
--FILE--
<?php
$a = [];
for ($i = 0; $i < 30000; $i++) {
    $a = [$a];
}
try {
    array_walk_recursive($a, function ($v) {});
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
