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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Closure object cannot have properties
