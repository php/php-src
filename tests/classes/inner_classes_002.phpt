--TEST--
invalid inner class
--FILE--
<?php

class Outer {
    int class Inner {
    }
}

?>
--EXPECTF--
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
