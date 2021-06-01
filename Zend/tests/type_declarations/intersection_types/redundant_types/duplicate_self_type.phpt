--TEST--
Duplicate self type
--FILE--
<?php

class A {
    public function test(): self&A&self {
    }
}

?>
--EXPECTF--
Fatal error: Duplicate type self is redundant in %s on line %d
