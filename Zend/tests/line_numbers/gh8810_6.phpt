--TEST--
GH-8810: Fix reported line number of multi-line static call
--FILE--
<?php

class A {
    public static function b() {
        throw new Exception();
    }
}

A
    ::
    b
    ();

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:5
Stack trace:
#0 %s(11): A::b()
#1 {main}
  thrown in %s on line 5
