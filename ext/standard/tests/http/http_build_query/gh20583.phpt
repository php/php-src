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
zend.max_allowed_stack_size=128K
--FILE--
<?php
$a = null;
for ($i = 0; $i < 1000; $i++) {
 $a = [$i => $a];
}
try {
    http_build_query($a, 'p');
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
Fatal error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached during compilation. Try splitting expression in %s on line %d