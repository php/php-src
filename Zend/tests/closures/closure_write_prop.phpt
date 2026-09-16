--TEST--
Cannot write to closure properties
--FILE--
<?php

class A {
    function getFn() {
        return function() {
        };
    }
}

$a = new A;
try {
    $c = $a->getFn()->b = new stdClass;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot create dynamic property Closure::$b
