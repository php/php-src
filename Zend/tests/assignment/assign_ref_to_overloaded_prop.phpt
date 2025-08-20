--TEST--
Cannot assign by reference to overloaded object, even if __get() returns by-ref
--FILE--
<?php

class Test {
    private $x;
    public function &__get($name) {
        return $this->x;
    }
}

$test = new Test;
$y = 5;
$test->x =& $y;
var_dump($test->x);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
