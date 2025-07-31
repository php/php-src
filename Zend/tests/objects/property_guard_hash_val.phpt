--TEST--
Test property guard hash value assumption
--FILE--
<?php
class Test {
    function __get($var) {
        return $this->{$var.''};
    }
}

$test = new Test;
var_dump($test->x);
?>
--EXPECTF--
Warning: Undefined property: Test::$x in %s on line %d
NULL
