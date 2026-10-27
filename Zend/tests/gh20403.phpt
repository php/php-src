--TEST--
GH-20403 (Heap buffer overflow in zval_undefined_cv() when unsetting through an uninitialized typed static property)
--FILE--
<?php

class Test {
    public static X $instances;
}

$test = new Test;
unset($test::$instances[$x]->x);

?>
===DONE===
--EXPECTF--
Warning: Undefined variable $x in %s on line %d
===DONE===
