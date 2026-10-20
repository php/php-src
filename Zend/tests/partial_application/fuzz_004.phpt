--TEST--
Closure application fuzz 004
--FILE--
<?php
function foo($a, $b) {
    return $a + $b;
}

$foo = foo(b: 10, ...);

try {
    $foo->__invoke(UNDEFINED);
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Undefined constant "UNDEFINED"
