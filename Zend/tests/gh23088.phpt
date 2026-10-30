--TEST--
GH-23088 (Stack overflow when comparing deeply nested arrays)
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
$b = [];

for ($i = 0; $i < 20000; $i++) {
    $a = [$a];
    $b = [$b];
}

try {
    var_dump($a == $b);
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    var_dump($a === $b);
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Maximum call stack size reached during comparison
Maximum call stack size reached during comparison
