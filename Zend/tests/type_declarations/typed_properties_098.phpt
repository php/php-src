--TEST--
Make sure uninitialized property is initialized to null when taken by reference
--FILE--
<?php

class Test {
    public $prop;
}

$test = new Test;
unset($test->prop);
$ref =& $test->prop;
var_dump($ref);

?>
--EXPECT--
NULL
