--TEST--
GH-8810: Fix reported line number of multi-line method call
--FILE--
<?php

class A {
    public function b() {
        throw new Exception();
    }
}

(new A())
    ->
    b
    ();

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:5
Stack trace:
#0 %s(11): A->b()
#1 {main}
  thrown in %s on line 5
