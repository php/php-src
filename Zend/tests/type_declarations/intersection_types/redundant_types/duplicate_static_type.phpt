--TEST--
Duplicate static type
--FILE--
<?php

class A {
    public function test(): static&A&static {
    }
}

?>
--EXPECTF--
Fatal error: Duplicate type static is redundant in %s on line %d
