--TEST--
Bug #79679: Constructor can not return any value (always void).
--FILE--
<?php

class A {
    public function __construct() {
        return 5;
    }
}

?>
--EXPECTF--
Fatal error: A void function must not return a value in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
