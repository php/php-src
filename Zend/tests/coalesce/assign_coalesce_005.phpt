--TEST--
Coalesce assign (??=): Cannot reassign $this
--FILE--
<?php

class Test {
    public function foobar() {
        $this ??= 123;
    }
}

?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %s on line %d
