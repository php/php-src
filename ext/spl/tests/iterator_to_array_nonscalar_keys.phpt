--TEST--
Tests iterator_to_array() with non-scalar keys
--FILE--
<?php

function gen() {
    yield "foo" => 0;
    yield 1     => 1;
    yield 2.5   => 2;
    yield null  => 3;
    yield []    => 4;
    yield new stdClass => 5;
}

try {
    var_dump(iterator_to_array(gen()));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Deprecated: Implicit conversion from float 2.5 to int loses precision in %s on line %d
Cannot access offset of type array on array
