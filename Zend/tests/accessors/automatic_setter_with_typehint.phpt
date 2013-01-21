--TEST--
Automatic setters can have typehints
--FILE--
<?php

class Test {
    public $test {
        get; set(stdClass $obj);
    }
}

$test = new Test;
$test->test = new stdClass;
var_dump($test->test);
$test->test = 42;

?>
--EXPECTF--
object(stdClass)#2 (0) {
}

Catchable fatal error: Argument 1 passed to Test::$test->set() must be an instance of stdClass, integer given in %s on line %d
