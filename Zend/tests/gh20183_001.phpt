--TEST--
GH-20183: Stale EG(opline_before_exception) pointer through eval
--CREDITS--
Viet Hoang Luu (@vi3tL0u1s)
--FILE--
<?php

class A {
    function __destruct() {
        eval('try { throw new Error(); } catch (Error $e) {}');
        debug_print_backtrace();
    }
}

B::$b = new A;

?>
--EXPECTF--
#0 %s(10): A->__destruct()

Fatal error: Uncaught Error: Class "B" not found in %s:10
Stack trace:
#0 {main}
  thrown in %s on line 10
