--TEST--
Partial application variation __invoke
--FILE--
<?php
function foo($a, $b) {
    return $a + $b;
}

$foo = foo(..., b: 10);

if ($foo->__invoke(32) == 42) {
    echo "OK\n";
}

try {
    $foo->nothing();
} catch (Error $ex) {
    echo "OK";
}
?>
--EXPECT--
OK
OK
