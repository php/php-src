--TEST--
GH-20583 (Stack overflow in http_build_query via deep structures)
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
$a = null;
for ($i = 0; $i < 3000; $i++) {
 $a = [$i => $a];
}
try {
    http_build_query($a, 'p');
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Maximum call stack size reached.
