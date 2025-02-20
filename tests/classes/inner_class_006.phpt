--TEST--
multi-level
--FILE--
<?php

class A {
    public class B {
        public class C {}
    }
}

var_dump(new A::B::C());
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "C", expecting variable or "$" in %s on line %d
