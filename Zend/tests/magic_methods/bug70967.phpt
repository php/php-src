--TEST--
Bug #70967 (Weird error handling for __toString when Error is thrown)
--FILE--
<?php
class A {
    public function __toString() {
        undefined_function();
    }
}

echo (new A);
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function undefined_function() in %s:%d
Stack trace:
#0 %s(%d): A->__toString()
#1 {main}
  thrown in %s on line %d
