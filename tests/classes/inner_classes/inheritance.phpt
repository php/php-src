--TEST--
inheritance
--FILE--
<?php

class Outer {
    abstract class Other {}
    class Middle extends Outer:>Other {
        class Inner1 extends Outer:>Other {}
        class Inner2 extends Outer:>Middle {}
    }
}
?>
--EXPECT--
