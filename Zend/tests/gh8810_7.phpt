--TEST--
GH-8810: Fix reported line number of multi-line new call
--FILE--
<?php

class A {
    public function __construct() {
        throw new Exception();
    }
}

new
    A
    ();

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:5
Stack trace:
#0 %s(10): A->__construct()
#1 {main}
  thrown in %s on line 5
