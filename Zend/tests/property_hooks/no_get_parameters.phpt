--TEST--
No get property hook parameters
--FILE--
<?php

class Test {
    public $prop {
        get() {
            var_dump($customName);
        }
    }
}

$test = new Test();
$test->prop = 42;

?>
--EXPECTF--
Fatal error: get hook of property Test::$prop must not have a parameter list in %s on line %d
