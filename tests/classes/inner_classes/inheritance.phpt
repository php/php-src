--TEST--
inheritance
--FILE--
<?php

class Outer {
    abstract class Other {}
    class Middle extends Other {
        class Inner1 {} // extends Outer:>Middle
        class Inner2 extends Outer:>Middle {} // extends Outer
    }
}
?>
--EXPECT--
