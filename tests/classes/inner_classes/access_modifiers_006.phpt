--TEST--
private(set) inner class
--FILE--
<?php

class Outer {
    private(set) class Inner {
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the private(set) modifier on a inner class in %s on line %d
