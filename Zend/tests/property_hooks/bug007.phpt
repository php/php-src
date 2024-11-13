--TEST--
Assign by reference to backed property is forbidden for &get-only
--FILE--
<?php

class Test {
    public $prop = 0 {
        &get {
            echo __METHOD__, "\n";
            return $this->prop;
        }
    }
}

$test = new Test();
$test->prop = &$ref;

?>
--EXPECTF--
Test::$prop::get

Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
