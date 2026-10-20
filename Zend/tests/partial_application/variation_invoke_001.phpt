--TEST--
PFA variation: __invoke()
--FILE--
<?php
function foo($a, $b) {
    return $a + $b;
}

$foo = foo(b: 10, ...);

var_dump($foo->__invoke(32));

try {
    $foo->nothing();
} catch (Error $ex) {
    echo $ex::class, ": ", $ex->getMessage(), "\n";
}
?>
--EXPECT--
int(42)
Error: Call to undefined method Closure::nothing()
