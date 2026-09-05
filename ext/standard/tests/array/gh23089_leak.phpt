--TEST--
GH-23089 (Memory leak in array_merge_recursive on stack overflow with object)
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
$obj = new stdClass();
$deep = [];
for ($i = 0; $i < 30000; $i++) {
    $deep = ['k' => $deep];
}
$obj->val = $deep;
$a = ['obj' => $obj];
try {
    array_merge_recursive($a, $a);
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
echo "done\n";
?>
--EXPECTF--
Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
done
