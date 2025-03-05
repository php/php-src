--TEST--
multi-level
--FILE--
<?php

class A {
    public class B {
        public class C {}
    }
}
?>
--EXPECTF--
Fatal error: Cannot nest classes more than 1 level deep in %s on line %d
