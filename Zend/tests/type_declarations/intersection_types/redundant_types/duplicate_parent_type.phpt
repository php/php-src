--TEST--
Duplicate parent type
--FILE--
<?php

class A {}
class B extends A {
    public function test(): parent&A&parent {
    }
}

?>
--EXPECTF--
Fatal error: Duplicate type parent is redundant in %s on line %d
