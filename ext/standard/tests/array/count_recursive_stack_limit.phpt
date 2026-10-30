--TEST--
Stack overflow in count() with COUNT_RECURSIVE and deeply nested arrays
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
/* Two elements per nesting level: the sibling must not be visited once the
 * stack limit error has been thrown, so only one Error is thrown. */
$a = [];
for ($i = 0; $i < 30000; $i++) {
    $a = [$a, []];
}

try {
    count($a, COUNT_RECURSIVE);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
    var_dump($e->getPrevious());
}
?>
--EXPECTF--
Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
NULL
