--TEST--
GH-23089 (Stack overflow in array_merge_recursive with deeply nested arrays)
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
?>
--INI--
zend.max_allowed_stack_size=1M
--FILE--
<?php
$a = [];
for ($i = 0; $i < 30000; $i++) {
    $a = ['k' => $a];
}
try {
    array_merge_recursive($a, $a);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* Avoid the separate stack overflow during array destruction (GH-15869). */
unset($e);
while ($a) {
    $a = $a['k'];
}
?>
--EXPECTF--
Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
